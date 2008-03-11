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
#include "Utility/bounding_object.h"
#include "Utility/vector.h"
#include "Utility/adapter.h"
#include "Geometry/geometric_point.h"


/***********************************************~***************************************************/


void WCAlignedBoundingBox::GenerateVBO(void) {
	GLfloat p0[4] = { this->_xMin, this->_yMin, this->_zMin, 1.0 };
	GLfloat p1[4] = { this->_xMin, this->_yMax, this->_zMin, 1.0 };
	GLfloat p2[4] = { this->_xMax, this->_yMax, this->_zMin, 1.0 };
	GLfloat p3[4] = { this->_xMax, this->_yMin, this->_zMin, 1.0 };
	GLfloat p4[4] = { this->_xMin, this->_yMin, this->_zMax, 1.0 };
	GLfloat p5[4] = { this->_xMin, this->_yMax, this->_zMax, 1.0 };
	GLfloat p6[4] = { this->_xMax, this->_yMax, this->_zMax, 1.0 };
	GLfloat p7[4] = { this->_xMax, this->_yMin, this->_zMax, 1.0 };
	//Create data array
	GLfloat data[96];
	unsigned int size = 4 * sizeof(GLfloat);
	//Quad 0 - Front
	memcpy(data, p0, size);
	memcpy(data+4, p4, size);
	memcpy(data+8, p7, size);
	memcpy(data+12, p3, size);
	//Quad 1 - Left
	memcpy(data+16, p1, size);
	memcpy(data+20, p5, size);
	memcpy(data+24, p4, size);
	memcpy(data+28, p0, size);
	//Quad 2 - Back
	memcpy(data+32, p2, size);
	memcpy(data+36, p6, size);
	memcpy(data+40, p5, size);
	memcpy(data+44, p1, size);
	//Quad 3 - Right
	memcpy(data+48, p3, size);
	memcpy(data+52, p7, size);
	memcpy(data+56, p6, size);
	memcpy(data+60, p2, size);
	//Quad 4 - Top
	memcpy(data+64, p4, size);
	memcpy(data+68, p5, size);
	memcpy(data+72, p6, size);
	memcpy(data+76, p7, size);
	//Quad 5 - Bottom
	memcpy(data+80, p1, size);
	memcpy(data+84, p0, size);
	memcpy(data+88, p3, size);
	memcpy(data+92, p2, size);
	//Size and init buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffer);
	glBufferData(GL_ARRAY_BUFFER, 96*sizeof(GLfloat), data, GL_STATIC_DRAW);
/*** Debug ***
	GLfloat *data2 = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	for (int i=0; i<24; i++) printf("\t%d V: %f %f %f %f\n", i, data2[i*4], data2[i*4+1], data2[i*4+2], data2[i*4+3]);
	glUnmapBuffer(GL_ARRAY_BUFFER);	
/*** Debug ***/
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Mark as clean
	this->_isDirty = false;
	//Check for errors
	if (glGetError() != GL_NO_ERROR) std::cout << "WCAlignedBoundingBox::GenerateVBO Error - At Buffer Creation.\n";
}


/***********************************************~***************************************************/


WCAlignedBoundingBox::WCAlignedBoundingBox() : ::WCBoundingObject(), 
	_xMin(0.0), _xMax(0.0), _yMin(0.0), _yMax(0.0), _zMin(0.0), _zMax(0.0), _buffer(0) {
	//Set up the buffer
	glGenBuffers(1, &this->_buffer);
}


WCAlignedBoundingBox::WCAlignedBoundingBox(const WCAlignedBoundingBox &box) : ::WCBoundingObject(box),
	_xMin(box._xMin), _xMax(box._xMax), _yMin(box._yMin),
	_yMax(box._yMax), _zMin(box._zMin), _zMax(box._zMax) {
	//Set up the buffer
	glGenBuffers(1, &this->_buffer);
}


WCAlignedBoundingBox::WCAlignedBoundingBox(const GLfloat &xMin, const GLfloat &xMax, const GLfloat &yMin,
	const GLfloat &yMax, const GLfloat &zMin, const GLfloat &zMax) :  ::WCBoundingObject(), 
	_xMin(xMin), _xMax(xMax), _yMin(yMin),
	_yMax(yMax), _zMin(zMin), _zMax(zMax) {
	//Set up the buffer
	glGenBuffers(1, &this->_buffer);
}


WCAlignedBoundingBox::WCAlignedBoundingBox(const std::vector<WCGeometricPoint*> &points) : ::WCBoundingObject(),
	_xMin(0.0), _xMax(0.0), _yMin(0.0), _yMax(0.0), _zMin(0.0), _zMax(0.0) {
	//Set up the buffer
	glGenBuffers(1, &this->_buffer);
	//Just set the data
	this->Set(points);
}


WCAlignedBoundingBox::WCAlignedBoundingBox(const std::vector<WCVector4> &vectors) : ::WCBoundingObject(),
	_xMin(0.0), _xMax(0.0), _yMin(0.0), _yMax(0.0), _zMin(0.0), _zMax(0.0) {
	//Set up the buffer
	glGenBuffers(1, &this->_buffer);
	//Just set the data
	this->Set(vectors);
}


WCAlignedBoundingBox::WCAlignedBoundingBox(WCVector4 *vectors, const WPUInt &count) : ::WCBoundingObject(),
	_xMin(0.0), _xMax(0.0), _yMin(0.0), _yMax(0.0), _zMin(0.0), _zMax(0.0) {
	//Set up the buffer
	glGenBuffers(1, &this->_buffer);
	//Just set the data
	this->Set(vectors, count);
}


WCAlignedBoundingBox::WCAlignedBoundingBox(const GLuint &buffer, const WPUInt &size) : ::WCBoundingObject(),
	_xMin(0.0), _xMax(0.0), _yMin(0.0), _yMax(0.0), _zMin(0.0), _zMax(0.0) {
	//Set up the buffer
	glGenBuffers(1, &this->_buffer);
	//Just set the data
	this->Set(buffer, size);
}


WCMatrix WCAlignedBoundingBox::ToMatrix(void) {
	//Marshal the bounds data
	WPFloat data[] = { this->_xMin, this->_xMin, this->_xMin, this->_xMin, this->_xMax, this->_xMax, this->_xMax, this->_xMax,
					   this->_yMin, this->_yMin, this->_yMax, this->_yMax, this->_yMin, this->_yMin, this->_yMax, this->_yMax,
					   this->_zMin, this->_zMax, this->_zMin, this->_zMax, this->_zMin, this->_zMax, this->_zMin, this->_zMax,
					   1.0		  , 1.0		   , 1.0		, 1.0		 , 1.0		  , 1.0		   , 1.0		, 1.0		  };
	//Generate 4x8 matrix
	return WCMatrix(4, 8, data);
}


void WCAlignedBoundingBox::Render(void) {
	//Only render if visible
	if (!this->_isVisible) return;
	//Generate VBO if dirty
	if (this->_isDirty) this->GenerateVBO();

	//Get current program
//	GLuint prog = (GLuint)glGetHandleARB(GL_PROGRAM_OBJECT_ARB);
	//Get the current polymode
	GLint polyMode[2];
	glGetIntegerv(GL_POLYGON_MODE, polyMode);
	//Set polygon mode to lines
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//Set program to 0
	glUseProgram(0);
	//Do the drawing
	glColor4f(1.0, 1.0, 1.0, 0.5);
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffer);
	glVertexPointer(4, GL_FLOAT, 0, 0);
	glDrawArrays(GL_QUADS, 0, 24);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	
	//Reset the polygon mode
	if (polyMode[0] == GL_LINE)	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//Reset the program
//	glUseProgram(prog);
	//Check for errors
	if (glGetError() != GL_NO_ERROR) std::cout << "WCAlignedBoundingBox::Render Error - Unknown error.\n";
}


void WCAlignedBoundingBox::Set(const std::vector<WCGeometricPoint*> &points) {
	//Check for zero sized collections
	if (points.size() == 0) return;
	//Set the initial values
	this->_xMin = (GLfloat)points.at(0)->X();
	this->_xMax = (GLfloat)this->_xMin;
	this->_yMin = (GLfloat)points.at(0)->Y();
	this->_yMax = (GLfloat)this->_yMin;
	this->_zMin = (GLfloat)points.at(0)->Z();
	this->_zMax = (GLfloat)this->_zMin;
	//Find the bounds of the data set
	for (unsigned int i=1; i<points.size(); i++) {
		if (points.at(i)->X() > this->_xMax) this->_xMax = (GLfloat)points.at(i)->X();
		if (points.at(i)->X() < this->_xMin) this->_xMin = (GLfloat)points.at(i)->X();
		if (points.at(i)->Y() > this->_yMax) this->_yMax = (GLfloat)points.at(i)->Y();
		if (points.at(i)->Y() < this->_yMin) this->_yMin = (GLfloat)points.at(i)->Y();
		if (points.at(i)->Z() > this->_zMax) this->_zMax = (GLfloat)points.at(i)->Z();
		if (points.at(i)->Z() < this->_zMin) this->_zMin = (GLfloat)points.at(i)->Z();
	}
	//Mark as dirty
	this->_isDirty = true;
}


void WCAlignedBoundingBox::Set(const std::vector<WCVector4> &vectors) {
	//Check for zero sized collections
	if (vectors.size() == 0) return;
	//Set the initial values
	this->_xMin = (GLfloat)vectors.at(0).I();
	this->_xMax = (GLfloat)this->_xMin;
	this->_yMin = (GLfloat)vectors.at(0).J();
	this->_yMax = (GLfloat)this->_yMin;
	this->_zMin = (GLfloat)vectors.at(0).K();
	this->_zMax = (GLfloat)this->_zMin;
	//Find the bounds of the data set
	for (unsigned int i=1; i<vectors.size(); i++) {
		if (vectors.at(i).I() > this->_xMax) this->_xMax = (GLfloat)vectors.at(i).I();
		if (vectors.at(i).I() < this->_xMin) this->_xMin = (GLfloat)vectors.at(i).I();
		if (vectors.at(i).J() > this->_yMax) this->_yMax = (GLfloat)vectors.at(i).J();
		if (vectors.at(i).J() < this->_yMin) this->_yMin = (GLfloat)vectors.at(i).J();
		if (vectors.at(i).K() > this->_zMax) this->_zMax = (GLfloat)vectors.at(i).K();
		if (vectors.at(i).K() < this->_zMin) this->_zMin = (GLfloat)vectors.at(i).K();
	}
	//Mark as dirty
	this->_isDirty = true;
}


void WCAlignedBoundingBox::Set(WCVector4 *vectors, const WPUInt &size) {
	//Check to make sure size is greater than zero
	if (size < 1) return;
	//Bind and map the buffer
	if (vectors == NULL) { std::cout << "WCAlignedBoundingBox::Set Error - Bad MapBuffer.n"; return; }
	//Set the initial values
	this->_xMin = (GLfloat)vectors[0].I();
	this->_xMax = (GLfloat)this->_xMin;
	this->_yMin = (GLfloat)vectors[0].J();
	this->_yMax = (GLfloat)this->_yMin;
	this->_zMin = (GLfloat)vectors[0].K();
	this->_zMax = (GLfloat)this->_zMin;
	//Find the bounds of the data set
	for (unsigned int i=1; i<size; i++) {
		if (vectors[i].I() > this->_xMax) this->_xMax = (GLfloat)vectors[i].I();
		if (vectors[i].I() < this->_xMin) this->_xMin = (GLfloat)vectors[i].I();
		if (vectors[i].J() > this->_yMax) this->_yMax = (GLfloat)vectors[i].J();
		if (vectors[i].J() < this->_yMin) this->_yMin = (GLfloat)vectors[i].J();
		if (vectors[i].K() > this->_zMax) this->_zMax = (GLfloat)vectors[i].K();
		if (vectors[i].K() < this->_zMin) this->_zMin = (GLfloat)vectors[i].K();
	}
	//Mark as dirty
	this->_isDirty = true;
}


void WCAlignedBoundingBox::Set(const GLuint &buffer, const WPUInt &size) {
	//Check to make sure size is greater than zero
	if (size < 1) return;
	//Bind and map the buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	GLfloat *data = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	if (data == NULL) { std::cout << "WCAlignedBoundingBox::Set Error - Bad MapBuffer.\n"; return; }
	int index;
	//Set the initial values
	this->_xMin = data[0];
	this->_xMax = this->_xMin;
	this->_yMin = data[1];
	this->_yMax = this->_yMin;
	this->_zMin = data[2];
	this->_zMax = this->_zMin;
	//Find the bounds of the data set
	for (WPUInt i=1; i<size; i++) {
		index = i*4;
		if (data[index] >   this->_xMax) this->_xMax = data[index];
		if (data[index] <   this->_xMin) this->_xMin = data[index];
		if (data[index+1] > this->_yMax) this->_yMax = data[index+1];
		if (data[index+1] < this->_yMin) this->_yMin = data[index+1];
		if (data[index+2] > this->_zMax) this->_zMax = data[index+2];
		if (data[index+2] < this->_zMin) this->_zMin = data[index+2];
	}
	//Unbind and unmap the buffer
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Mark as dirty
	this->_isDirty = true;
}


WCAlignedBoundingBox& WCAlignedBoundingBox::operator+=(const WCAlignedBoundingBox &obj) {
	//Compare each min/max dimension
	this->_xMin = STDMIN(this->_xMin, obj._xMin);
	this->_yMin = STDMIN(this->_yMin, obj._yMin);
	this->_zMin = STDMIN(this->_zMin, obj._zMin);
	this->_xMax = STDMAX(this->_xMax, obj._xMax);
	this->_yMax = STDMAX(this->_yMax, obj._yMax);
	this->_zMax = STDMAX(this->_zMax, obj._zMax);
	//Mark as dirty
	this->_isDirty = true;
	//Return this object
	return *this;
}


WCAlignedBoundingBox& WCAlignedBoundingBox::operator=(const WCAlignedBoundingBox &obj) {
	//Copy min/max values
	this->_xMin = obj._xMin;
	this->_yMin = obj._yMin;
	this->_zMin = obj._zMin;
	this->_xMax = obj._xMax;
	this->_yMax = obj._yMax;
	this->_zMax = obj._zMax;
	//Mark as dirty
	this->_isDirty = true;
	//Return this object
	return *this;
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCAlignedBoundingBox &box) {
	//Print out basic info
	out << "AlignedBoundingBox(" << &box << ")";	
	std::cout << " xMin: " << box._xMin;
	std::cout << " xMax: " << box._xMax;
	std::cout << " yMin: " << box._yMin;
	std::cout << " yMax: " << box._yMax;
	std::cout << " zMin: " << box._zMin;
	std::cout << " zMax: " << box._zMax;
	//Return the output stream
	return out;
}


/***********************************************~***************************************************/


WCOrientedBoundingBox::WCOrientedBoundingBox() : _bounds(true) {
}


WCOrientedBoundingBox::WCOrientedBoundingBox(const std::vector<WCGeometricPoint*> &points) : _bounds() {
	//Just set the data
	this->Set(points);
}


WCOrientedBoundingBox::WCOrientedBoundingBox(const std::vector<WCVector4> &vectors) : _bounds() {
	//Just set the data
	this->Set(vectors);
}


WCOrientedBoundingBox::WCOrientedBoundingBox(WCVector4 *vectors, const WPUInt &count) : _bounds() {
	//Just set the data
	this->Set(vectors, count);
}
	

WCOrientedBoundingBox::WCOrientedBoundingBox(const GLuint &buffer, const WPUInt &size) : _bounds() {
	//Just set the data
	this->Set(buffer, size);
}

	
void WCOrientedBoundingBox::Render(void) {
	std::cout << "WCOrientedBoundingBox::Render Error - Not yet implemented\n";
}


void WCOrientedBoundingBox::Set(const std::vector<WCGeometricPoint*> &points) {
	std::cout << "WCOrientedBoundingBox::Set Error - Not yet implemented\n";
}


void WCOrientedBoundingBox::Set(const std::vector<WCVector4> &vectors) {
	std::cout << "WCOrientedBoundingBox::Set Error - Not yet implemented\n";
}


void WCOrientedBoundingBox::Set(WCVector4 *vectors, const WPUInt &size) {
	std::cout << "WCOrientedBoundingBox::Set Error - Not yet implemented\n";
}


void WCOrientedBoundingBox::Set(const GLuint &buffer, const WPUInt &size) {
	std::cout << "WCOrientedBoundingBox::Set Error - Not yet implemented\n";
}

/***********************************************~***************************************************/

