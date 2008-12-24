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
#include <Kernel/OSX/toolbar_bridge.h>
#include <Utility/log_manager.h>
#include <Kernel/toolbar_manager.h>
#include <Kernel/document.h>
#include <Kernel/workbench.h>


/*** Imported Header Files ***/
#import <Kernel/OSX/toolbar_osx.h>



/***********************************************~***************************************************/


WCToolbarButton_Bridge::WCToolbarButton_Bridge(WCToolbarButton *button, const WCUserMessage &message, const std::string &tooltip, 
	const unsigned int &type, const std::string &stdIcon, const std::string &activeIcon, const bool &active, const bool &enabled) :
	_core(button), _platforms(), _message(message), _tooltip(nil), _type(type), _stdIcon(nil), _activeIcon(nil), 
	_isActive(active), _isEnabled(enabled) {

	//Setup tooltip NSString
	const char *cString = tooltip.c_str();
	this->_tooltip = [[NSString stringWithCString:cString] retain];

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
}


WCToolbarButton_Bridge::~WCToolbarButton_Bridge() {
	//Release the images
	[this->_stdIcon release];
	[this->_activeIcon release];
	//Release the tooltip
	[this->_tooltip release];
}


void WCToolbarButton_Bridge::AddInstance(WCToolbarButton_OSX* instance) {
	//Add to instance list
	this->_platforms.push_back(instance);
}


void WCToolbarButton_Bridge::IsActive(const bool &state) {
	//Set the active flag (if needed)
	if (this->_isActive != state) {
		this->_isActive = state;
		//Loop through all instances
		std::list<WCToolbarButton_OSX*>::iterator iter;
		for (iter = this->_platforms.begin(); iter != this->_platforms.end(); iter++) {
			//Mark button as needing to be redrawing
			WCToolbarButton_OSX *button = *iter;
			[button setNeedsDisplay:YES];
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
	_document(doc),	_isVisible(false), _core(toolbar), _platform(nil) {

	//Setup title string
	const char *cTitle = title.c_str();
	NSString* titleString = [NSString stringWithCString:cTitle];

	//Setup position rect
	NSRect positionRect = NSMakeRect(position.I(), position.J(), position.K(), position.L());

	//Create platform toolbar
	this->_platform = [[WCToolbar_OSX alloc] initWithBridge:this title:titleString position:positionRect];
	//Make sure is not nil
	if (this->_platform == nil) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCToolbar_Bridge::WCToolbar_Bridge - Not able to create platform toolbar");
	}
}


WCToolbar_Bridge::~WCToolbar_Bridge() {
	//Release platform
	if (this->_platform != nil) {
		//Close the toolbar
		[[this->_platform window] close];
		[this->_platform release];
	}
}


void WCToolbar_Bridge::OnButtonPress(WCToolbarButton_OSX *button) {
	//Get bridge button
	WCToolbarButton_Bridge* bridge = [button bridge];
	//If button is enabled, pass along
	if (bridge->IsEnabled()) {
		//Get user message and pass to active workbench
		this->_document->ActiveWorkbench()->OnUserMessage( [button bridge]->Message() );
	}
}


void WCToolbar_Bridge::OnCloseButtonPress(void) {
	//Pass to the platform
	[this->_platform setVisible:false];
	//Update the flag
	this->_isVisible = false;
}


void WCToolbar_Bridge::AddButton(WCToolbarButton_Bridge *button) {
	//Call to platform
	[this->_platform addButton:button];
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
		[this->_platform setVisible:state];
		//Update the flag
		this->_isVisible = state;
	}
}


/***********************************************~***************************************************/

