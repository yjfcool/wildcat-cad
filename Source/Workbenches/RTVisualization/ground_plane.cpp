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
#include "RTVisualization/ground_plane.h"
#include "RTVisualization/visualization.h"
#include "Kernel/document.h"


/***********************************************~***************************************************/


void WCGroundPlane::GenerateBuffers(void) {
	//Set the render program
	this->_renderProg = this->_visualization->Scene()->ShaderManager()->ProgramFromName("scn_basiclight");
	GLfloat vertData[12] = {
							-5.0, -5.0, 0.0,		// Lower left corner
							-5.0, 5.0, 0.0,		// Upper left corner
							5.0, 5.0, 0.0,		// Upper right corner
							5.0, -5.0, 0.0		// Lower right corner
						   };
	//Copy the data into the VBO
	glBindBuffer(GL_ARRAY_BUFFER, this->_vertBuffer);
	glBufferData(GL_ARRAY_BUFFER, 12*sizeof(GLfloat), vertData, GL_STATIC_DRAW);
	GLfloat normData[12] = {
							0.0, 0.0, 1.0,		// Lower left corner
							0.0, 0.0, 1.0,		// Upper left corner
							0.0, 0.0, 1.0,		// Upper right corner
							0.0, 0.0, 1.0		// Lower right corner
						   };

	//Copy the data into the VBO
	glBindBuffer(GL_ARRAY_BUFFER, this->_normBuffer);
	glBufferData(GL_ARRAY_BUFFER, 12*sizeof(GLfloat), normData, GL_STATIC_DRAW);
	//Clean up a bit
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Create a bounding box around the ground plane
	this->_bounds = new WCAlignedBoundingBox(this->_vertBuffer, 4);
}


/***********************************************~***************************************************/


WCGroundPlane::WCGroundPlane(WCVisualization &vis, const WPFloat &xSpacing, const WPFloat &ySpacing) :
	::WCVisualObject(), _visualization(&vis), _xSpacing(xSpacing), _ySpacing(ySpacing),
	_vertBuffer(0), _normBuffer(0), _numLines(0) {
	//Make sure visualization is not null
	if (this->_visualization == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGroundPlane::WCGroundPlane - NULL sketch passed.");
		//throw error
		return;
	}
	//Set the color
	this->_color.Set(GROUNDPLANE_DEFAULT_COLOR);
	//Create buffer objects
	glGenBuffers(1, &this->_vertBuffer);
	glGenBuffers(1, &this->_normBuffer);

	//Retain the the camera and scene (get ModelView updates)
	this->_visualization->Document()->Scene()->ActiveCamera()->Retain(*this);
	this->_visualization->Document()->Scene()->Retain(*this);
}


WCGroundPlane::~WCGroundPlane() {
	//Release the the camera and scene
	this->_visualization->Document()->Scene()->ActiveCamera()->Release(*this);
	this->_visualization->Document()->Scene()->Release(*this);
	//Destroy buffer
	glDeleteBuffers(1, &this->_vertBuffer);
	glDeleteBuffers(1, &this->_normBuffer);
}


void WCGroundPlane::Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom) {
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
	
	//Set the shader
	if (this->_renderProg != 0)	glUseProgram(this->_renderProg);

	//Set up vertex buffer state
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, this->_vertBuffer);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	//Set up normal buffer state
	glEnableClientState(GL_NORMAL_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, this->_normBuffer);
	glNormalPointer(GL_FLOAT, 0, 0);

	//Draw the mesh
	glDrawArrays(GL_QUADS, 0, 24);

	//Bind back to nothing
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Make sure that vertex and normal arrays are disabled
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}


void WCGroundPlane::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
	//Mark this as dirty
	this->_isVisualDirty = true;
}

	
xercesc::DOMElement* WCGroundPlane::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	return NULL;
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCGroundPlane &ground) {
	return out;
}


/***********************************************~***************************************************/

