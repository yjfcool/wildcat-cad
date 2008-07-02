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
#include "Scene/line_layer.h"
#include "Scene/camera.h"
#include "Geometry/geometric_line.h"
#include "Geometry/geometric_point.h"


/***********************************************~***************************************************/

void WCLineLayer::GenerateBuffers(void) {
	//Create arrays for all possible lines
	GLfloat *vData = new GLfloat[6*this->_lineList.size()];
	GLfloat *cData = new GLfloat[8*this->_lineList.size()];
	//Reset the number of visible lines
	this->_numVisible = 0;
	WPUInt vIndex = 0;
	WPUInt cIndex = 0;
	//Go through all lines and add to buffers as needed
	WCGeometricLine *line;
	//Add contributions for all lines
	for (std::list<WCGeometricLine*>::iterator iter=this->_lineList.begin(); iter!=this->_lineList.end(); iter++) {
		line = *iter;
		if (line->IsVisible()) {
			//Set the first vertex data
			vData[vIndex++] = (GLfloat)line->Begin().I();
			vData[vIndex++] = (GLfloat)line->Begin().J();
			vData[vIndex++] = (GLfloat)line->Begin().K();
			//Set the second vertex data
			vData[vIndex++] = (GLfloat)line->End().I();
			vData[vIndex++] = (GLfloat)line->End().J();
			vData[vIndex++] = (GLfloat)line->End().K();
			//Set the color data for first vertex
			cData[cIndex++] = line->Color().R();
			cData[cIndex++] = line->Color().G();
			cData[cIndex++] = line->Color().B();
			cData[cIndex++] = line->Color().A();
			//Set the color data for second vertex
			cData[cIndex++] = line->Color().R();
			cData[cIndex++] = line->Color().G();
			cData[cIndex++] = line->Color().B();
			cData[cIndex++] = line->Color().A();			
			this->_numVisible+=2;
		}
	}
	
	//Copy the data into the vertex VBO
	glBindBuffer(GL_ARRAY_BUFFER, this->_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vIndex*sizeof(GLfloat), vData, GL_STATIC_DRAW);
	//Copy the data into the vertex VBO
	glBindBuffer(GL_ARRAY_BUFFER, this->_colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, cIndex*sizeof(GLfloat), cData, GL_STATIC_DRAW);
	//Delete arrays
	delete vData;
	delete cData;
/*** DEBUG ***
	//Show Vertex Data
	glBindBuffer(GL_ARRAY_BUFFER, this->_vertexBuffer);
	GLfloat *data2 = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	for (int i=0; i<vIndex; i+=3) printf("\tVertices - %d: %f %f %f\n", i, data2[i], data2[i+1], data2[i+2]);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	//Show Color Data
	glBindBuffer(GL_ARRAY_BUFFER, this->_colorBuffer);
	data2 = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	for (int i=0; i<cIndex; i+=4) printf("\tColors - %d: %f %f %f %f\n", i, data2[i], data2[i+1], data2[i+2], data2[i+3]);
	glUnmapBuffer(GL_ARRAY_BUFFER);
/*** DEBUG ***/
	//Clean up and check for errors
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	if (glGetError() != GL_NO_ERROR) 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCLineLayer::GenerateBuffers - At Cleanup and Exit.");
}


/***********************************************~***************************************************/


WCLineLayer::WCLineLayer(WCScene *scene, std::string name) : ::WCLayer(scene, name), _renderProg(0),
	_numVisible(0), _thickness(LINELAYER_THICKNESS), _vertexBuffer(0), _colorBuffer(0) {
	//Generate the two buffers
	glGenBuffers(1, &this->_vertexBuffer);
	glGenBuffers(1, &this->_colorBuffer);
}


WCLineLayer::~WCLineLayer() {
	//Delete the two buffers
	glDeleteBuffers(1, &this->_vertexBuffer);
	glDeleteBuffers(1, &this->_colorBuffer);
}


void WCLineLayer::AddLine(WCGeometricLine *line) {
	//Make sure the line is not NULL
	if (line == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCLineLayer::AddLine Error - NULL value passed.");
		return;
	}
	//Add the line into the master list
	this->_lineList.push_back(line);
	//Retain the line (should be item 0 in retain list)
	line->Retain(*this);
	//Set the layer for the line
//	line->Layer(this);
	//Mark the layer as dirty
	this->_isDirty = true;
}


void WCLineLayer::RemoveLine(WCGeometricLine *line) {
	//Make sure the object is not NULL
	if (line == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCLineLayer::RemoveLine Error - NULL value passed.");
		return;
	}
	//Remove the line into the master list
	this->_lineList.remove(line);
	//Release the line (should be item 0 in retain list)
	line->Release(*this);
	//Unset the layer for the object
//	line->Layer(NULL);
	//Mark the layer as dirty
	this->_isDirty = true;
}


void WCLineLayer::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
	//Mark the layer as dirty
	this->_isDirty = true;
}


WCAlignedBoundingBox WCLineLayer::BoundingBox(void) {
	//Setup initial box
	WCAlignedBoundingBox box;
	//Check visibility
	if (!this->_isVisible) return box;
	//Check dirty status
	if (this->_isDirty) {
		this->GenerateBuffers();
		this->_isDirty = false;
	}
	//Set the box using vertex data
	box.Set(this->_vertexBuffer, this->_numVisible);
	//Return the box
	return box;
}


void WCLineLayer::Render(WCRenderState *state) {
	//Only render if visible
	if (!this->_isVisible) return;
	
	//Rebuild buffers if dirty
	if (this->_isDirty) {
		//Generate the buffers
		this->GenerateBuffers();
		//Mark as clean
		this->_isDirty = false;
	}
	//Set lighting params if needed
//	if (this->_renderProg != 0) {
		glUseProgram(this->_renderProg);
//		this->_scene->ActiveCamera()->EnableLighting(this->_renderProg);
//	}

	//Set up rendering environment
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	//Bump z to move lines off of surfaces
	glTranslated(0.0, 0.0, 0.05);

	glEnable(GL_LINE_SMOOTH);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glLineWidth(LINELAYER_THICKNESS);
	//Render the lines from the buffers
	glBindBuffer(GL_ARRAY_BUFFER, this->_vertexBuffer);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, this->_colorBuffer);
	glColorPointer(4, GL_FLOAT, 0, 0);
	glLineWidth((GLfloat)this->_thickness);
	//Draw the lines
	glDrawArrays(GL_LINES, 0, this->_numVisible);
	//Clean up the environment
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisable(GL_LINE_SMOOTH);

	//Restore matrix and mode
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	//Check for errors
	if (glGetError() != GL_NO_ERROR)
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCLineLayer::Render Error - Unspecified Errors.");
}


/***********************************************~***************************************************/

