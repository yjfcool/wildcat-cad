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
#include "Scene/point_layer.h"
#include "Scene/camera.h"
#include "Geometry/geometric_line.h"
#include "Geometry/geometric_point.h"


/***********************************************~***************************************************/

void WCPointLayer::GenerateBuffers(void) {
	//Create arrays for all possible points
	GLfloat *vData = new GLfloat[3*this->_pointList.size()];
	GLfloat *cData = new GLfloat[4*this->_pointList.size()];
	//Reset the number of visible points
	this->_numVisible = 0;
	WPUInt vIndex = 0;
	WPUInt cIndex = 0;
	//Go through all points and add to buffers as needed
	WCGeometricPoint *point;
	WCColor color;
	//Add contributions for all point
	for (std::list<WCGeometricPoint*>::iterator iter=this->_pointList.begin(); iter!=this->_pointList.end(); iter++) {
		point = *iter;
		if (point->IsVisible()) {
			//Set the first vertex data
			vData[vIndex++] = (GLfloat)point->X();
			vData[vIndex++] = (GLfloat)point->Y();
			vData[vIndex++] = (GLfloat)point->Z();
			color = point->Color();
			//Set the color data
			cData[cIndex++] = color.R();
			cData[cIndex++] = color.G();
			cData[cIndex++] = color.B();
			cData[cIndex++] = color.A();
			this->_numVisible++;
		}
	}
	
	//Copy the data into the vertex VBO
	glBindBuffer(GL_ARRAY_BUFFER, this->_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vIndex*sizeof(GLfloat), vData, GL_STATIC_DRAW);
	//Copy the data into the vertex VBO
	glBindBuffer(GL_ARRAY_BUFFER, this->_colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, cIndex*sizeof(GLfloat), cData, GL_STATIC_DRAW);
	//Clean up and check for errors
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	if (glGetError() != GL_NO_ERROR) 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCPointLayer::GenerateBuffers - At Cleanup and Exit.");
	//Delete arrays
	delete vData;
	delete cData;
}


/***********************************************~***************************************************/


WCPointLayer::WCPointLayer(WCScene *scene, std::string name) : ::WCLayer(scene, name), _renderProg(0),
	_numVisible(0), _vertexBuffer(0), _colorBuffer(0) {
	//Generate the two buffers
	glGenBuffers(1, &this->_vertexBuffer);
	glGenBuffers(1, &this->_colorBuffer);
}


WCPointLayer::~WCPointLayer() {
	//Delete the two buffers
	glDeleteBuffers(1, &this->_vertexBuffer);
	glDeleteBuffers(1, &this->_colorBuffer);
}


void WCPointLayer::AddPoint(WCGeometricPoint *point) {
	//Make sure the point is not NULL
	if (point == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCPointLayer::AddLine Error - NULL value passed.");
		return;
	}
	//Add the point into the master list
	this->_pointList.push_back(point);
	//Retain the point (should be item 0 in retain list)
	point->Retain(*this);
	//Set the point for the line
//	point->Layer(this);
	//Mark the layer as dirty
	this->_isDirty = true;
}


void WCPointLayer::RemovePoint(WCGeometricPoint *point) {
	//Make sure the object is not NULL
	if (point == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCPointLayer::RemoveLine Error - NULL value passed.");
		return;
	}
	//Remove the point into the master list
	this->_pointList.remove(point);
	//Release the point (should be item 0 in retain list)
	point->Release(*this);
	//Unset the layer for the object
//	point->Layer(NULL);
	//Mark the layer as dirty
	this->_isDirty = true;
}


void WCPointLayer::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
	//Mark the layer as dirty
	this->_isDirty = true;
}


WCAlignedBoundingBox WCPointLayer::BoundingBox(void) {
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


void WCPointLayer::Render(WCRenderState *state) {
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
	if (this->_renderProg != 0) {
		glUseProgram(this->_renderProg);
		this->_scene->ActiveCamera()->EnableLighting(this->_renderProg);
	}

	//Set up rendering environment
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	//Bump z to move points off of surfaces
	glTranslated(0.0, 0.0, 0.05);

	//Set up rendering environment
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	//Render the points from the buffers
	glBindBuffer(GL_ARRAY_BUFFER, this->_vertexBuffer);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, this->_colorBuffer);
	glColorPointer(4, GL_FLOAT, 0, 0);
	//Draw the points
	glDrawArrays(GL_POINTS, 0, this->_numVisible);
	//Clean up the environment
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	//Restore matrix and mode
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	//Check for errors
	if (glGetError() != GL_NO_ERROR)
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCLineLayer::Render Error - Unspecified Errors.");
}


/***********************************************~***************************************************/

