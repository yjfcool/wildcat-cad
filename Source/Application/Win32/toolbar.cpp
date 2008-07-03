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
#include "Application/toolbar.h"
#include "Application/Win32/toolbar_bridge.h"


/***********************************************~***************************************************/


WCToolbarButton::WCToolbarButton(const WCUserMessage &message, const std::string &tooltip, const unsigned int &type,
	const std::string &stdIcon, const std::string &activeIcon, const bool &active, const bool &enabled) : _bridge(NULL) {

	//Create toolbar button bridge
	this->_bridge = new WCToolbarButton_Bridge(this, message, tooltip, type, stdIcon, activeIcon, active, enabled);
	//Make sure is non-null
	if (this->_bridge == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCToolbarButton::WCToolbarButton - Not able to create toolbar button bridge");
	}
}


WCToolbarButton::~WCToolbarButton() {
	//Try to delete the bridge
	if (this->_bridge != NULL) delete this->_bridge;
	this->_bridge = NULL;
}


bool WCToolbarButton::IsActive(void) const {
	//Just call to the underlying bridge
	if (this->_bridge != NULL) return this->_bridge->IsActive();
	//Otehrwise, return false
	return false;
}


void WCToolbarButton::IsActive(const bool &state) {
	//Just call to the underlying bridge
	if (this->_bridge != NULL) return this->_bridge->IsActive(state);
}


bool WCToolbarButton::IsEnabled(void) const {
	//Just call to the underlying bridge
	if (this->_bridge != NULL) return this->_bridge->IsEnabled();
	//Otehrwise, return false
	return false;

}


void WCToolbarButton::IsEnabled(const bool &state) {
	//Just call to the underlying bridge
	if (this->_bridge != NULL) this->_bridge->IsEnabled(state);
}


/***********************************************~***************************************************/


WCToolbar::WCToolbar(WCDocument *doc, const std::string &name, const WCVector4 &position) : _bridge(NULL) {
	//Create toolbar bridge
	this->_bridge = new WCToolbar_Bridge(doc, this, name, position);
	//Make sure is non-null
	if (this->_bridge == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCToolbar::WCToolbar - Not able to create toolbar bridge");
	}	
}


WCToolbar::~WCToolbar() {
	//Try to delete the bridge
	if (this->_bridge != NULL) delete this->_bridge;
	this->_bridge = NULL;
}


void WCToolbar::AddButton(WCToolbarButton *button) {
	//Just call to the underlying bridge
	if (this->_bridge != NULL) return this->_bridge->AddButton(button->_bridge);
}


WCToolbarButton* WCToolbar::ButtonFromName(const std::string &name) {
	//Just call to the underlying bridge
	if (this->_bridge != NULL) return this->_bridge->ButtonFromName(name);
	//Otherwise, just return NULL
	return NULL;
}


bool WCToolbar::IsVisible(void) const {
	//Just call to the underlying bridge
	if (this->_bridge != NULL) return this->_bridge->IsVisible();
	//Otehrwise, return false
	return false;
}


void WCToolbar::IsVisible(const bool &state) {
	//Just call to the underlying bridge
	if (this->_bridge != NULL) return this->_bridge->IsVisible(state);
}


/***********************************************~***************************************************/

