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
#include "Scene/background_layer.h"
#include "Scene/scene.h"


/***********************************************~***************************************************/


void WCBackgroundLayer::GenerateBuffers(void) {
	//Create the vertex data
	GLfloat data[16];
	//Lower left
	data[0] = (GLfloat)this->_scene->XMin();
	data[1] = (GLfloat)this->_scene->YMin();
	data[2] = (GLfloat)(SCENE_ZMIN_DEFAULT + 0.01);
	data[3] = (GLfloat)1.0;
	//Upper left
	data[4] = (GLfloat)this->_scene->XMin();
	data[5] = (GLfloat)this->_scene->YMax();
	data[6] = (GLfloat)(SCENE_ZMIN_DEFAULT + 0.01);
	data[7] = (GLfloat)1.0;
	//Upper right
	data[8] = (GLfloat)this->_scene->XMax();
	data[9] = (GLfloat)this->_scene->YMax();
	data[10] = (GLfloat)(SCENE_ZMIN_DEFAULT + 0.01);
	data[11] = (GLfloat)1.0;
	//Lower right
	data[12] = (GLfloat)this->_scene->XMax();
	data[13] = (GLfloat)this->_scene->YMin();
	data[14] = (GLfloat)(SCENE_ZMIN_DEFAULT + 0.01);
	data[15] = (GLfloat)1.0;
	
	//Create the color data
	GLfloat color[16];
	//Lower left
	color[0] = (GLfloat)this->_llColor.R();
	color[1] = (GLfloat)this->_llColor.G();
	color[2] = (GLfloat)this->_llColor.B();
	color[3] = (GLfloat)this->_llColor.A();
	//Upper left
	color[4] = (GLfloat)this->_ulColor.R();
	color[5] = (GLfloat)this->_ulColor.G();
	color[6] = (GLfloat)this->_ulColor.B();
	color[7] = (GLfloat)this->_ulColor.A();
	//Upper right
	color[8] = (GLfloat)this->_urColor.R();
	color[9] = (GLfloat)this->_urColor.G();
	color[10] = (GLfloat)this->_urColor.B();
	color[11] = (GLfloat)this->_urColor.A();
	//Lower right
	color[12] = (GLfloat)this->_lrColor.R();
	color[13] = (GLfloat)this->_lrColor.G();
	color[14] = (GLfloat)this->_lrColor.B();
	color[15] = (GLfloat)this->_lrColor.A();

	if(WCAdapter::HasGLEXTFramebufferObject()) {
		//Copy the data into the VBO
		glBindBuffer(GL_ARRAY_BUFFER, this->_vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, 16*sizeof(GLfloat), data, GL_STATIC_DRAW);

		//Copy the data into the VBO
		glBindBuffer(GL_ARRAY_BUFFER, this->_colorBuffer);
		glBufferData(GL_ARRAY_BUFFER, 16*sizeof(GLfloat), color, GL_STATIC_DRAW);
		/*** Debug ***
		std::cout << "Background Color:\n";
		glBindBuffer(GL_ARRAY_BUFFER, this->_colorBuffer);	
		GLfloat *data2= (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
		for (int i=0; i<4; i++) printf("\t%d V: %f %f %f %f\n", i, data2[i*4], data2[i*4+1], data2[i*4+2], data2[i*4+3]);
		glUnmapBuffer(GL_ARRAY_BUFFER);
		/*** Debug ***/
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	else {
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(4, GL_FLOAT, 0, data);

		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_FLOAT, 0, color);
		glDrawArrays(GL_QUADS, 0, 4);

		//Make sure that vertex and normal arrays are disabled
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
	}
}


/***********************************************~***************************************************/


WCBackgroundLayer::WCBackgroundLayer(WCScene *scene) : ::WCLayer(scene, "Background"),
	_vertexBuffer(0), _colorBuffer(0), _llColor(BACKGROUNDLAYER_BOTTOM_COLOR), _lrColor(BACKGROUNDLAYER_BOTTOM_COLOR), 
	_ulColor(BACKGROUNDLAYER_TOP_COLOR), _urColor(BACKGROUNDLAYER_TOP_COLOR) {
	//Set the buffer objects
	if(WCAdapter::HasGLEXTFramebufferObject()) {
		glGenBuffers(1, &this->_vertexBuffer);
		glGenBuffers(1, &this->_colorBuffer);
	}
	//Nothing to be done for now
	this->_isShadowPass = false;
	this->_isSelectionPass = false;
	//Mark as dirty
	this->_isDirty = true;
}


WCBackgroundLayer::~WCBackgroundLayer() {
	//Free the buffers
	if(WCAdapter::HasGLEXTFramebufferObject()) {
		glDeleteBuffers(1, &this->_vertexBuffer);
		glDeleteBuffers(1, &this->_colorBuffer);
	}
}


bool WCBackgroundLayer::OnReshape(const WPFloat width, const WPFloat height) {
	//Mark as dirty
	this->_isDirty = true;
	return false;
}


void WCBackgroundLayer::Render(WCRenderState *state) {
	//Only render if visible
	if (!this->_isVisible) return;

	//Check the polygon mode
	if(state->PolygonMode() != GL_FILL) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//Setup billboarding
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	//Turn off stuff
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	if(WCAdapter::HasGLEXTFramebufferObject()) {
		//Check if dirty
		if (this->_isDirty) {
			this->GenerateBuffers();
			//Mark as clean
			this->_isDirty = false;
		}	

		//Set up state
		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, this->_vertexBuffer);
		glVertexPointer(4, GL_FLOAT, 0, 0);

		glEnableClientState(GL_COLOR_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, this->_colorBuffer);
		glColorPointer(4, GL_FLOAT, 0, 0);
		glDrawArrays(GL_QUADS, 0, 4);

		//Bind back to nothing
		glBindBuffer(GL_ARRAY_BUFFER, 0);	
		//Make sure that vertex and normal arrays are disabled
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
	}
	else {
		GenerateBuffers();
	}

	//Restore the modelview matrix
	glPopMatrix();
	//Restore other stuff
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	//Restore polygon mode
	if(state->PolygonMode() != GL_FILL) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	
	//Check for errors
	if (glGetError() != GL_NO_ERROR)
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCBackgroundLayer::Render - GL error at Cleanup.");
}


/***********************************************~***************************************************/


