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
#include "Scene/user_interface_layer.h"
#include "Scene/widget.h"


/***********************************************~***************************************************/


WCUserInterfaceLayer::WCUserInterfaceLayer(WCScene *scene) : ::WCLayer(scene, "UserInterface"), _firstResponder(NULL) {
	//Set some layer parameters
	this->_isShadowPass = false;
	this->_isSelectionPass = false;	
}


WCUserInterfaceLayer::~WCUserInterfaceLayer() {
	//Check to make sure all widgets have been unregistered
	if (this->_widgets.size() > 0) 
		CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCUserInterfaceLayer::~WCUserInterfaceLayer - " << this->_widgets.size() << " widgets remain");
}

	
void WCUserInterfaceLayer::RegisterWidget(WCWidget* widget) {
	//Make sure widget is not NULL
	if (widget == NULL) { std::cout << "WCUserInterfaceLayer::RegisterWidget Error - NULL widget passed.\n"; return; }
	//Look through the list to see if widget already exists
	//Otherwise, look for a new widget
	std::list<WCWidget*>::iterator iter = this->_widgets.begin();		
	while(iter != this->_widgets.end()) {
		if ((*iter) == widget) { std::cout << "WCUserInterfaceLayer::RegisterWidget Error - Widget already registered.\n"; return; }
		iter++;
	}	
	//Add the widget into the list
	this->_widgets.push_back(widget);
	widget->Retain(*this);
}


void WCUserInterfaceLayer::UnregisterWidget(WCWidget* widget) {
	//Make sure widget is not NULL
	if (widget == NULL) { std::cout << "WCUserInterfaceLayer::UnregisterWidget Error - NULL widget passed.\n"; return; }
	//Remove the widget from the list
	this->_widgets.remove(widget);
	widget->Release(*this);
}


bool WCUserInterfaceLayer::OnIdle(void) {
	//Pass OnIdle on to all widgets
	std::list<WCWidget*>::iterator iter = this->_widgets.begin();		
	while(iter != this->_widgets.end()) {
		(*iter)->OnIdle();
		iter++;
	}
	return false;
}


bool WCUserInterfaceLayer::OnMouseMove(const WPFloat x, const WPFloat y) {
	//See if the first responder is still valid
	if (this->_firstResponder != NULL) {
		if (this->_firstResponder->HitTest(x, y)) {
			this->_firstResponder->OnMouseMove(x, y);
			//Set this layer as first responder
			this->_scene->FirstResponder(this);
			return true;
		}
		//Otherwise need to change first responder
		else {
			this->_firstResponder->OnMouseExit(x, y);
			this->_firstResponder = NULL;
		}
	}
	//Otherwise, look for a new widget
	std::list<WCWidget*>::iterator iter = this->_widgets.begin();		
	while(iter != this->_widgets.end()) {
		if ((*iter)->HitTest(x, y)) {
			this->_firstResponder = *iter;
			this->_firstResponder->OnMouseEnter(x, y);
			//Set this layer as first responder
			this->_scene->FirstResponder(this);
			return true;
		}
		iter++;
	}
	//UI Layer didn't need event
	return false;
}


bool WCUserInterfaceLayer::OnMouseDown(const WCMouseButton &button) {
	//See if the first responder is still valid
	if (this->_firstResponder != NULL) {
		if (this->_firstResponder->HitTest(this->_scene->MouseX(), this->_scene->MouseY())) {
			this->_firstResponder->OnMouseDown(button, this->_scene->MouseX(), this->_scene->MouseY());
			return true;
		}
	}
	return false;
}


bool WCUserInterfaceLayer::OnMouseUp(const WCMouseButton &button) {
	//See if the first responder is still valid
	if (this->_firstResponder != NULL) {
		this->_firstResponder->OnMouseUp(button, this->_scene->MouseX(), this->_scene->MouseY());
		return true;
	}
	return false;
}


bool WCUserInterfaceLayer::OnScroll(const WPFloat &x, const WPFloat &y) {
	//Update all widgets that the scroll-wheel was used
	std::list<WCWidget*>::iterator iter = this->_widgets.begin();		
	while(iter != this->_widgets.end()) {
		//Pass onScroll to widget
		(*iter)->OnScroll(x, y);
		//Go to next widget
		iter++;
	}
	return false;
}


bool WCUserInterfaceLayer::OnArrowKeyPress(const WCArrowKey &key) {
	//Update all widgets that an arrow key was pressed
	std::list<WCWidget*>::iterator iter = this->_widgets.begin();		
	while(iter != this->_widgets.end()) {
		//Pass onArrowKeyPress to widget
		(*iter)->OnArrowKeyPress(key);
		//Go to next widget
		iter++;
	}
	return false;
}


bool WCUserInterfaceLayer::OnReshape(const WPFloat width, const WPFloat height) {
	//Update all widgets that the layer was reshaped
	std::list<WCWidget*>::iterator iter = this->_widgets.begin();		
	while(iter != this->_widgets.end()) {
		(*iter)->OnReshape();
		iter++;
	}
	return false;
}


void WCUserInterfaceLayer::Render(WCRenderState *state) {
	//Only render if visible
	if (!this->_isVisible) return;
	//Set the program to zero
	if(glUseProgram)glUseProgram(0);
	//Setup billboarding
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
//	state->DepthTest(false);
	glDisable(GL_DEPTH_TEST);
	//Render UI widgets
	std::list<WCWidget*>::iterator iter = this->_widgets.begin();		
	while(iter != this->_widgets.end()) {
		(*iter)->Render();
		iter++;
	}
	//Restore the render state
	glEnable(GL_DEPTH_TEST);
	state->DepthTest(true);
	glPopMatrix();
	//Restore the program
//	glUseProgram(state->Program());
	//Check for errors
	if (glGetError() != GL_NO_ERROR) std::cout << "WCUserInterfaceLayer::Render Error - Unspecified Errors.\n";
}


/***********************************************~***************************************************/


