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
#include "Application/Win32/toolbar_bridge.h"
#include "Utility/log_manager.h"
#include "Application/toolbar_manager.h"
#include "Kernel/document.h"
#include "Kernel/workbench.h"


/*** Platform Header Files ***/
#include "Application/Win32/toolbar_win32.h"



/***********************************************~***************************************************/


WCToolbarButton_Bridge::WCToolbarButton_Bridge(WCToolbarButton *button, const WCUserMessage &message, const std::string &tooltip, 
	const unsigned int &type, const std::string &stdIcon, const std::string &activeIcon, const bool &active, const bool &enabled) :
	_core(button), _platforms(), _message(message), _tooltip(tooltip), _type(type), _stdIcon(NULL), _activeIcon(NULL), 
	_isActive(active), _isEnabled(enabled) {
/*
	//Get the bundle
	NSBundle* bundle = [NSBundle mainBundle];

	//Setup standard icon
	cString = stdIcon.c_str();
	NSString* imageName = [NSString stringWithCString:cString];
	this->_stdIcon = [[NSImage alloc] initWithContentsOfFile:[bundle pathForImageResource:imageName]];
	//Check that image is valid
	if (this->_stdIcon == nil) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCToolbarButton_Bridge::WCToolbarButton_Bridge - Not able to load icon: " << stdIcon);
	}

	//Setup active icon
	cString = activeIcon.c_str();
	imageName = [NSString stringWithCString:cString];
	this->_activeIcon = [[NSImage alloc] initWithContentsOfFile:[bundle pathForImageResource:imageName]];
	//Check that image is valid
	if (this->_activeIcon == nil) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCToolbarButton_Bridge::WCToolbarButton_Bridge - Not able to load icon: " << activeIcon);
	}
*/
}


WCToolbarButton_Bridge::~WCToolbarButton_Bridge() {
	//Release the images
	if (this->_stdIcon != NULL) delete this->_stdIcon;
	if (this->_activeIcon != NULL) delete this->_activeIcon;
}


void WCToolbarButton_Bridge::AddInstance(WCToolbarButton_Win32* instance) {
	//Add to instance list
	this->_platforms.push_back(instance);
}


void WCToolbarButton_Bridge::IsActive(const bool &state) {
	//Set the active flag (if needed)
	if (this->_isActive != state) {
		this->_isActive = state;
		//Loop through all instances
		std::list<WCToolbarButton_Win32*>::iterator iter;
		for (iter = this->_platforms.begin(); iter != this->_platforms.end(); iter++) {
			//Mark button as needing to be redrawing
			WCToolbarButton_Win32 *button = *iter;
//			[button setNeedsDisplay:YES];
		}
	}
}


void WCToolbarButton_Bridge::IsEnabled(const bool &state) {
	//Set the enabled flag (if needed)
	if (this->_isEnabled != state) {
		this->_isEnabled = state;
	}
}


/***********************************************~***************************************************/


WCToolbar_Bridge::WCToolbar_Bridge(WCDocument *doc, WCToolbar* toolbar, const std::string &title, const WCVector4 &position) : 
	_document(doc),	_isVisible(false), _core(toolbar), _platform(NULL) {
	//Setup position rect
//	NSRect positionRect = NSMakeRect(position.I(), position.J(), position.K(), position.L());

	//Create platform toolbar
	this->_platform = new WCToolbar_Win32(this, title);
	//Make sure is not nil
	if (this->_platform == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCToolbar_Bridge::WCToolbar_Bridge - Not able to create platform toolbar");
	}
}


WCToolbar_Bridge::~WCToolbar_Bridge() {
	//Release platform
	if (this->_platform != NULL) {
		//Close the toolbar
//		[[this->_platform window] close];
		delete this->_platform;
	}
}


void WCToolbar_Bridge::OnButtonPress(WCToolbarButton_Win32 *button) {
	//Get bridge button
	WCToolbarButton_Bridge* bridge = button->Bridge();
	//If button is enabled, pass along
	if (bridge->IsEnabled()) {
		//Get user message and pass to active workbench
		this->_document->ActiveWorkbench()->OnUserMessage( bridge->Message() );
	}
}


void WCToolbar_Bridge::OnCloseButtonPress(void) {
	//Pass to the platform
	this->_platform->IsVisible(false);
	//Update the flag
	this->_isVisible = false;
}


void WCToolbar_Bridge::AddButton(WCToolbarButton_Bridge *button) {
	//Call to platform
	this->_platform->AddButton(button);
}


WCToolbarButton* WCToolbar_Bridge::ButtonFromName(const std::string &name) {
	//Get the toolbar manager from the document
	WCToolbarManager *manager = this->_document->ToolbarManager();
	//Request button from manager
	return manager->ButtonFromName(name);
}


void WCToolbar_Bridge::IsVisible(const bool &state) {
	//Work with visCount
	if (state != this->_isVisible) {
		//Pass to the platform
		this->_platform->IsVisible(state);
		//Update the flag
		this->_isVisible = state;
	}
}


/***********************************************~***************************************************/

