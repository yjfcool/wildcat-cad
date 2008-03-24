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
#include "Kernel/workbench_layer.h"
#include "Kernel/drawing_mode.h"


/***********************************************~***************************************************/


WCWorkbenchLayer::WCWorkbenchLayer(WCScene *scene, std::string name) : ::WCVisualLayer(scene, name), 
	_isActive(false), _mode(NULL) {
	//Nothing else for now
}


WCWorkbenchLayer::~WCWorkbenchLayer() {
}


bool WCWorkbenchLayer::OnMouseMove(const WPFloat x, const WPFloat y) {
	if ((!this->_isVisible) || (!this->_isActive)) return false;
	//Call MouseMove on drawMode
	if (this->_mode != NULL) this->_mode->OnMouseMove(x, y);
	//Absorb the event
	return true;
}


bool WCWorkbenchLayer::OnMouseDown(const WCMouseButton &button) {
	if ((!this->_isVisible) || (!this->_isActive)) return false;
	//Set this layer as first responder
	this->_scene->FirstResponder(this);
	//Call MouseDown on drawMode
	if (this->_mode != NULL) this->_mode->OnMouseDown(button);
	//Absorb the event
	return true;
}


bool WCWorkbenchLayer::OnMouseUp(const WCMouseButton &button) {
	if ((!this->_isVisible) || (!this->_isActive)) return false;
	//Clear this layer as first responder
	this->_scene->FirstResponder(NULL);
	//Call MouseUp on drawMode
	if (this->_mode != NULL) this->_mode->OnMouseUp(button);
	//Absorb the event
	return true;
}


bool WCWorkbenchLayer::OnArrowKeyPress(const WCArrowKey &key) {
	if ((!this->_isVisible) || (!this->_isActive)) return false;
	//Call ArrowKeyPress on drawMode
	if (this->_mode != NULL) this->_mode->OnArrowKeyPress(key);
	//Absorb the event
	return true;
}


bool WCWorkbenchLayer::OnReshape(const WPFloat width, const WPFloat height) {
	return false;
}


void WCWorkbenchLayer::Render(WCRenderState *state) {
	//Ignore if not visible or not active
	if ((!this->_isVisible) || (!this->_isActive)) return;
	//Render all children
	this->WCVisualLayer::Render(state);
	//Render drawMode
	if (this->_mode != NULL) this->_mode->Render();
}


/***********************************************~***************************************************/

