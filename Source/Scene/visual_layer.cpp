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
#include "Scene/visual_layer.h"
#include "Scene/camera.h"


/***********************************************~***************************************************/


WCVisualLayer::WCVisualLayer(WCScene *scene, std::string name) : ::WCLayer(scene, name),
	_objectList(), _visibleList(), _renderProg(0) {
	//Nothing else for now
}


WCVisualLayer::~WCVisualLayer() {
	//Release all geometric objects
	for (std::list<WCVisualObject*>::iterator iter=this->_objectList.begin(); iter!=this->_objectList.end(); ++iter)
		(*iter)->Release(*this);
	//Clear both lists
	this->_objectList.clear();
	this->_visibleList.clear();
}


void WCVisualLayer::AddObject(WCVisualObject *object) {
	//Make sure the object is not NULL
	if (object == NULL) { 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometryLayer::AddObject Error - NULL value passed."); 
		return; 
	}
	//Add the object into the master list
	this->_objectList.push_back(object);
	//Retain the object (should be item 0 in retain list)
	object->Retain(*this);
}


void WCVisualLayer::RemoveObject(WCVisualObject *object) {
	//Make sure the object is not NULL
	if (object == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometryLayer::RemoveObject Error - NULL value passed.");
		return;
	}
	//Remove the object into the master list
	this->_objectList.remove(object);
	//Release the object (should be item 0 in retain list)
	object->Release(*this);
}


WCAlignedBoundingBox WCVisualLayer::BoundingBox(void) {
	//Setup initial box
	WCAlignedBoundingBox box;
	//Check visibility
	if (!this->_isVisible) return box;
	WCVisualObject *obj;
	//Add contributions for all geometric objects
	for (std::list<WCVisualObject*>::iterator iter=this->_objectList.begin(); iter!=this->_objectList.end(); iter++) {
		obj = *iter;
		box += obj->BoundingBox();
	}
	//Return the box
	return box;
}


void WCVisualLayer::Render(WCRenderState *state) {
	//Only render if visible
	if (!this->_isVisible) return;
	
	//Set lighting params if needed
	if (this->_renderProg != 0) {
		glUseProgram(this->_renderProg);
		this->_scene->ActiveCamera()->EnableLighting(this->_renderProg);
	}

	//Render geometric objects in visible list
	for (std::list<WCVisualObject*>::iterator iter=this->_objectList.begin(); iter!=this->_objectList.end(); ++iter) {
		//Set the lighting parameters (if appropriate)
		if ( (*iter)->RenderProgram() != 0 ) {
			glUseProgram( (*iter)->RenderProgram() );
			this->_scene->ActiveCamera()->EnableLighting( (*iter)->RenderProgram() );
		}
		//Render the object
		(*iter)->Render(this->_renderProg, WCColor::Default(), this->_scene->ActiveCamera()->Zoom());
	}
	//Return to no program
	if(glUseProgram)glUseProgram(0);

	//Mark layer as clean
	this->_isDirty = false;
	//Check for errors
	if (glGetError() != GL_NO_ERROR) 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometryLayer::Render Error - Unspecified Errors.");	
}


/***********************************************~***************************************************/

