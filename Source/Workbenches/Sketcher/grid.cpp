/*******************************************************************************
* Copyright (c) 2007, 2008, CerroKai Development
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of CerroKai Development nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY CerroKai Development ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL CerroKai Development BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************************/


/*** Included Header Files ***/
#include "Sketcher/grid.h"
#include "Sketcher/sketch.h"
#include "Kernel/document.h"
#include "PartDesign/part_plane.h"


/***********************************************~***************************************************/


void WCGrid::GenerateBuffers(void) {
	//Spacing
	WPFloat width = this->_xSpacing * this->_xGraduations;
	WPFloat height = this->_ySpacing * this->_yGraduations;
	WPFloat xMin, yMin, xMax, yMax;
	
	if (this->_sketch->Document()->Scene()->ActiveCamera()->IsOrthographic()) {
		//Calculate screen corners
		WCMatrix4 cameraInverse = this->_sketch->Document()->Scene()->ActiveCamera()->ViewInverseMatrix();
		WCMatrix4 planeInverse = this->_sketch->ReferencePlane()->InverseTransformMatrix();
		WCMatrix4 mat = planeInverse * cameraInverse;
		//Set up corners of the grid
		WCVector4 ll(this->_sketch->Document()->Scene()->XMin(), this->_sketch->Document()->Scene()->YMin(), 0.0, 1.0);
		WCVector4 ul(this->_sketch->Document()->Scene()->XMin(), this->_sketch->Document()->Scene()->YMax(), 0.0, 1.0);
		WCVector4 ur(this->_sketch->Document()->Scene()->XMax(), this->_sketch->Document()->Scene()->YMax(), 0.0, 1.0);
		WCVector4 lr(this->_sketch->Document()->Scene()->XMax(), this->_sketch->Document()->Scene()->YMin(), 0.0, 1.0);
		//Inverse project
		ll = mat * ll;
		ul = mat * ul;
		ur = mat * ur;
		lr = mat * lr;
		xMin = STDMIN( STDMIN(ll.I(), ul.I()), STDMIN(lr.I(), ur.I()) );
		xMax = STDMAX( STDMAX(ll.I(), ul.I()), STDMAX(lr.I(), ur.I()) );
		yMin = STDMIN( STDMIN(ll.J(), ul.J()), STDMIN(lr.J(), ur.J()) );
		yMax = STDMAX( STDMAX(ll.J(), ul.J()), STDMAX(lr.J(), ur.J()) );
		this->_xMin = xMin;
		this->_xMax = xMax;
		this->_yMin = yMin;
		this->_yMax = yMax;
	}
	//Projection view bounds estimate
	else {
		xMin = -10.0;
		xMax = 10.0;
		yMin = -10.0;
		yMax = 10.0;
	}

	//Calculate line count
	WPInt hStart, hStop, vStart, vStop;
	//Horizontal lines
	hStart = (WPInt)floor( xMin / width ) - 1;
	hStop = (WPInt)ceil( xMax / width ) + 1;
	//Vertical lines
	vStart = (WPInt)floor( yMin / height ) - 1;
	vStop = (WPInt)ceil( yMax / height ) + 1;
	this->_numLines = (vStop - vStart + 1) + (hStop - hStart + 1);
	GLfloat *data = new GLfloat[8 * this->_numLines];

	//Load data
	int index = 0;
	WCVector4 vec;
	WCMatrix4 planeMatrix = this->_sketch->ReferencePlane()->TransformMatrix();
	//Generate horizontal lines
	for (int i=vStart; i<=vStop; ++i) {
		//Set up the start of the line
		vec.Set(xMin - width, i * height, 0.0, 1.0);
		//Transform it to the plane
		vec = planeMatrix * vec;
		//Load data
		data[index++] = (GLfloat)vec.I();
		data[index++] = (GLfloat)vec.J();
		data[index++] = (GLfloat)vec.K();
		data[index++] = (GLfloat)vec.L();
		//Set up the end of the line
		vec.Set(xMax + width, i * height, 0.0, 1.0);
		//Transform it to the plane
		vec = planeMatrix * vec;
		//Load data
		data[index++] = (GLfloat)vec.I();
		data[index++] = (GLfloat)vec.J();
		data[index++] = (GLfloat)vec.K();
		data[index++] = (GLfloat)vec.L();
	}
	//Generate vertical lines
	for (int i=hStart; i<=hStop; ++i) {
		//Set up the start of the line
		vec.Set(i * width, yMin - height, 0.0, 1.0);
		//Transform it to the plane
		vec = planeMatrix * vec;
		//Load data
		data[index++] = (GLfloat)vec.I();
		data[index++] = (GLfloat)vec.J();
		data[index++] = (GLfloat)vec.K();
		data[index++] = (GLfloat)vec.L();
		//Set up the end of the line
		vec.Set(i * width, yMax + height, 0.0, 1.0);
		//Transform it to the plane
		vec = planeMatrix * vec;
		//Load data
		data[index++] = (GLfloat)vec.I();
		data[index++] = (GLfloat)vec.J();
		data[index++] = (GLfloat)vec.K();
		data[index++] = (GLfloat)vec.L();
	}
	//Put data into the buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffer);
	glBufferData(GL_ARRAY_BUFFER, 8*this->_numLines*sizeof(GLfloat), data, GL_STATIC_DRAW);
	//Delete the array
	delete data;
	
	//Create sublines if needed
	if (this->_numLines < GRID_SUBLINE_LIMIT) {
		//Determine number of sublines
		hStart = (WPInt)floor( xMin / this->_xSpacing ) - 1;
		hStop = (WPInt)ceil( xMax / this->_xSpacing ) + 1;
		//Vertical lines
		vStart = (WPInt)floor( yMin / this->_ySpacing ) - 1;
		vStop = (WPInt)ceil( yMax / this->_ySpacing ) + 1;
		this->_numSubLines = (vStop - vStart + 1) + (hStop - hStart + 1);
		//Size the buffer
		GLfloat *subData = new GLfloat[8 * this->_numSubLines];
		
		index = 0;
		//Generate horizontal lines
		for (int i=vStart; i<=vStop; ++i) {
			//Set up the start of the line
			vec.Set(xMin - this->_xSpacing, i * this->_ySpacing, 0.0, 1.0);
			//Transform it to the plane
			vec = planeMatrix * vec;
			//Load data
			subData[index++] = (GLfloat)vec.I();
			subData[index++] = (GLfloat)vec.J();
			subData[index++] = (GLfloat)vec.K();
			subData[index++] = (GLfloat)vec.L();
			//Set up the end of the line
			vec.Set(xMax + this->_xSpacing, i * this->_ySpacing, 0.0, 1.0);
			//Transform it to the plane
			vec = planeMatrix * vec;
			//Load data
			subData[index++] = (GLfloat)vec.I();
			subData[index++] = (GLfloat)vec.J();
			subData[index++] = (GLfloat)vec.K();
			subData[index++] = (GLfloat)vec.L();
		}
		//Generate vertical lines
		for (int i=hStart; i<=hStop; ++i) {
			//Set up the start of the line
			vec.Set(i * this->_xSpacing, yMin - this->_ySpacing, 0.0, 1.0);
			//Transform it to the plane
			vec = planeMatrix * vec;
			//Load data
			subData[index++] = (GLfloat)vec.I();
			subData[index++] = (GLfloat)vec.J();
			subData[index++] = (GLfloat)vec.K();
			subData[index++] = (GLfloat)vec.L();
			//Set up the end of the line
			vec.Set(i * this->_xSpacing, yMax + this->_ySpacing, 0.0, 1.0);
			//Transform it to the plane
			vec = planeMatrix * vec;
			//Load data
			subData[index++] = (GLfloat)vec.I();
			subData[index++] = (GLfloat)vec.J();
			subData[index++] = (GLfloat)vec.K();
			subData[index++] = (GLfloat)vec.L();
		}

		//Put data into the buffer
		glBindBuffer(GL_ARRAY_BUFFER, this->_subBuffer);
		glBufferData(GL_ARRAY_BUFFER, 8*this->_numSubLines*sizeof(GLfloat), subData, GL_STATIC_DRAW);	
		//Delete the array
		delete subData;
	}
	//Otherwise, set the number of lines to zero
	else this->_numSubLines = 0;
/*** Debug ***
	std::cout << "Grid VBO: " << this->_numLines << std::endl;
	GLfloat *data2 = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	for (int i=0; i<this->_numLines*2; i++) printf("\t%d: %f %f %f %f\n", i, data2[i*4], data2[i*4+1], data2[i*4+2], data2[i*4+3]);
	glUnmapBuffer(GL_ARRAY_BUFFER);
/*** Debug ***/
	//Clean up some
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


/***********************************************~***************************************************/


WCGrid::WCGrid(WCSketch &sketch, const WPFloat &xSpacing, const WPFloat &ySpacing, const WPUInt &xGrads, const WPUInt &yGrads) :
	::WCVisualObject(), _sketch(&sketch), _isSnap(true), _isDistortable(false),
	_xSpacing(xSpacing), _ySpacing(ySpacing), _xGraduations(xGrads), _yGraduations(yGrads), 
	_buffer(0), _numLines(0), _subBuffer(0), _numSubLines(0), _xMin(0.0), _yMin(0.0), _xMax(0.0), _yMax(0.0) {
	//Make sure sketch is not null
	if (this->_sketch == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGrid::WCGrid - NULL sketch passed.");
		//throw error
		return;
	}
	//Set the color
	this->_color.Set(GRID_DEFAULT_COLOR);
	//Create buffer object
	glGenBuffers(1, &this->_buffer);
	glGenBuffers(1, &this->_subBuffer);
	//Retain the the camera and scene (get ModelView updates)
	this->_sketch->Document()->Scene()->ActiveCamera()->Retain(*this);
	this->_sketch->Document()->Scene()->Retain(*this);
}


WCGrid::~WCGrid() {
	//Release the the camera and scene
	this->_sketch->Document()->Scene()->ActiveCamera()->Release(*this);
	this->_sketch->Document()->Scene()->Release(*this);
	//Destroy buffers
	glDeleteBuffers(1, &this->_buffer);
	glDeleteBuffers(1, &this->_subBuffer);
}


void WCGrid::Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom) {
	//Check visibility
	if (!this->_isVisible) return;
	//Check dirty flag
	if (this->_isVisualDirty) {
		//Generate VBO
		this->GenerateBuffers();
		//Mark as clean
		this->_isVisualDirty = false;
	}
	
	//Set the color
	this->_color.Enable();
	//Set up rendering environment
	glEnableClientState(GL_VERTEX_ARRAY);	
	
	//Draw the main lines
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffer);
	glVertexPointer(4, GL_FLOAT, 0, 0);	
	this->_color.Enable();
	glLineWidth(GRID_LINEWIDTH);
	glDrawArrays(GL_LINES, 0, this->_numLines*2);

	//See about drawing sublines
	if (this->_numSubLines != 0) {
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(GRID_LINESTIPPLE_FACTOR, GRID_LINESTIPPLE_PATTERN);
		glBindBuffer(GL_ARRAY_BUFFER, this->_subBuffer);
		glVertexPointer(4, GL_FLOAT, 0, 0);
		glDrawArrays(GL_LINES, 0, this->_numSubLines*2);
		glDisable(GL_LINE_STIPPLE);
	}

	//Clean up the environment
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
}


bool WCGrid::Snap(WPFloat &x, WPFloat &y) {
	//Make sure snapping is on
	if (!this->_isSnap) return false;
	//Find the closest on-grid x/y location
	x = round(x / this->_xSpacing) * this->_xSpacing;
	y = round(y / this->_ySpacing) * this->_ySpacing;
	//True, snapping happended
	return true;
}


void WCGrid::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
	//Mark this as dirty
	this->_isVisualDirty = true;
}


//WCAlignedBoundingBox WCGrid::BoundingBox(void) {
//	return WCAlignedBoundingBox(-2.0, 1.0, -1.0, 1.0, 0.0, 0.0);
//}

	
xercesc::DOMElement* WCGrid::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	return NULL;
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCGrid &grid) {
	return out;
}


/***********************************************~***************************************************/

