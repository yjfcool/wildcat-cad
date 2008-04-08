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


//Included Header Files
#include "Application/dialog.h"
#import "Application/MacOS/dialog_osx.h"


/***********************************************~***************************************************/


void WCDialog::Controller(WCDialogController *controller) {
	//Set the controller
	this->_controller = controller;
	//Create the window if needed
	if (this->_window == NULL) {
		this->_window = (void*)[[WCDialog_OSX alloc] initWithDialog:this];
	}
}


/***********************************************~***************************************************/


WCDialog::WCDialog(const std::string &name, const unsigned int &width, const unsigned int &height,
	const bool &modal, const bool &boundary, const WCDialogMode &mode) :
	_name(name), _winWidth(width), _winHeight(height), _modal(modal), _boundary(boundary), _mode(mode), _controller(NULL), _window(NULL) {
	//Create window here (if cached)
	if (this->_mode == WCDialogMode::Cached()) {
		this->_window = (void*)[[WCDialog_OSX alloc] initWithDialog:this];
	}
}

WCDialog::~WCDialog() {
	//Make sure window is closed
	if (this->_window != NULL) {
		//Cast the pointer
		WCDialog_OSX *window = (WCDialog_OSX*)this->_window;
		//Send the close message
		[window close];
	}
}


void WCDialog::CloseWindow(const bool &fromWindow) {
	//If not from window, then close the window
	if (!fromWindow) {
		//Cast the pointer
		WCDialog_OSX *window = (WCDialog_OSX*)this->_window;
		//Send the close message
		[window close];
	}
	//Otherwise...
	else {
		//For now just set to NULL
		this->_window = NULL;
	}
}


std::string WCDialog::StringFromScript(const std::string &var) {
	//Cast to the OSX class
	WCDialog_OSX *dialog = (WCDialog_OSX*)this->_window;
	//Call to underlying method
	return [dialog getStringFromScript:var];
}


WPFloat WCDialog::FloatFromScript(const std::string &var) {
	//Cast to the OSX class
	WCDialog_OSX *dialog = (WCDialog_OSX*)this->_window;
	//Call to underlying method
	return [dialog getFloatFromScript:var];
}


WPInt WCDialog::IntFromScript(const std::string &var) {
	//Cast to the OSX class
	WCDialog_OSX *dialog = (WCDialog_OSX*)this->_window;
	//Call to underlying method
	return [dialog getIntFromScript:var];
}


WPUInt WCDialog::UnsignedIntFromScript(const std::string &var) {
	//Cast to the OSX class
	WCDialog_OSX *dialog = (WCDialog_OSX*)this->_window;
	//Call to underlying method
	return [dialog getUnsignedIntFromScript:var];
}


bool WCDialog::BoolFromScript(const std::string &var) {
	//Cast to the OSX class
	WCDialog_OSX *dialog = (WCDialog_OSX*)this->_window;
	//Call to underlying method
	return [dialog getBoolFromScript:var];
}


void WCDialog::StringFromScript(const std::string &var, const std::string &value) {
	//Cast to the OSX class
	WCDialog_OSX *dialog = (WCDialog_OSX*)this->_window;
	//Call to underlying method
	[dialog setStringFromScript:var withValue:value];
}


void WCDialog::FloatFromScript(const std::string &var, const WPFloat &value) {
	//Cast to the OSX class
	WCDialog_OSX *dialog = (WCDialog_OSX*)this->_window;
	//Call to underlying method
	[dialog setFloatFromScript:var withValue:value];
}


void WCDialog::IntFromScript(const std::string &var, const WPInt &value) {
	//Cast to the OSX class
	WCDialog_OSX *dialog = (WCDialog_OSX*)this->_window;
	//Call to underlying method
	[dialog setIntFromScript:var withValue:value];
}


void WCDialog::UnsignedIntFromScript(const std::string &var, const WPUInt &value) {
	//Cast to the OSX class
	WCDialog_OSX *dialog = (WCDialog_OSX*)this->_window;
	//Call to underlying method
	[dialog setUnsignedIntFromScript:var withValue:value];
}


void WCDialog::BoolFromScript(const std::string &var, const bool &value) {
	//Cast to the OSX class
	WCDialog_OSX *dialog = (WCDialog_OSX*)this->_window;
	//Call to underlying method
	[dialog setBoolFromScript:var withValue:value];
}


/***********************************************~***************************************************/

