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
#include "Application/Win32/dialog_win32.h"


/***********************************************~***************************************************/


void WCDialog::Controller(WCDialogController *controller) {
	//Create the window if needed
	if (this->_controller == NULL) {
		//Set the controller
		this->_controller = controller;
		//See if cached
		if (this->_mode == WCDialogMode::Cached()) {
			//Just need to display the window
			WCDialog_Win32 *window = (WCDialog_Win32*)this->_window;
			//Order front the window
//			[[[window WebView] window] orderFront:window];
		}
		//Otherwise...
		else {
			//See if dynamic content needs to be generated
			if (this->_mode == WCDialogMode::Dynamic()) {
				//Generate the content...
			}
			//Create the window
//			this->_window = (void*)[[WCDialog_OSX alloc] initWithDialog:this];
		}
		//Mark as open
		this->_isOpen = true;
	}
	//Check error case
	else {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCDialog::Controller - Shouldn't set controller when already open.");
		//throw error
	}
}


/***********************************************~***************************************************/


WCDialog::WCDialog(const std::string &name, const unsigned int &width, const unsigned int &height,
	const bool &modal, const bool &boundary, const WCDialogMode &mode) :
	_name(name), _winWidth(width), _winHeight(height), _modal(modal), _boundary(boundary), _isOpen(false), _mode(mode), _controller(NULL), _window(NULL) {
	//Create window here (if cached)
	if (this->_mode == WCDialogMode::Cached()) {
		//Create the window
//		this->_window = (void*)[[WCDialog_OSX alloc] initWithDialog:this];
		//Mark as open
		this->_isOpen = true;
	}
}

WCDialog::~WCDialog() {
	//Make sure window is closed
	if (this->_isOpen) {
		//Cast the pointer
		WCDialog_Win32 *window = (WCDialog_Win32*)this->_window;
		//Send the close message
//		[window close];
	}
}


void WCDialog::CloseWindow(const bool &fromWindow) {
	//If not from window, then close the window
	if (!fromWindow) {
		//Cast the pointer
		WCDialog_Win32 *window = (WCDialog_Win32*)this->_window;
		//If modal
		if (this->_mode == WCDialogMode::Cached()) {
			//Just order out the window
			WCDialog_Win32 *window = (WCDialog_Win32*)this->_window;
			//Break modal if needed
//			if (this->_modal) [NSApp stopModal];
			//Reset the controller
			this->_controller = NULL;
			//Order front the window
//			[[[window WebView] window] orderOut:window];
		}
		//Otherwise...
		else {
			//Send the close message
//			[window close];
		}
	}
	//Otherwise...
	else {
		//Mark as closed
		this->_isOpen = false;
		//For now just set controller to NULL
		this->_controller = NULL;
	}
}


std::string WCDialog::StringFromScript(const std::string &var) {
	//Make sure window is open
	if (!this->_isOpen) return "";
	//Cast to the Win32 class
	WCDialog_Win32 *dialog = (WCDialog_Win32*)this->_window;
	//Call to underlying method
	std::string value = dialog->StringFromScript(var);
	//Return the value
	return value;
}


WPFloat WCDialog::FloatFromScript(const std::string &var) {
	//Make sure window is open
	if (!this->_isOpen) return 0.0;
	//Cast to the Win32 class
	WCDialog_Win32 *dialog = (WCDialog_Win32*)this->_window;
	//Call to underlying method
	WPFloat value = dialog->FloatFromScript(var);
	//Return the value
	return value;
}


WPInt WCDialog::IntFromScript(const std::string &var) {
	//Make sure window is open
	if (!this->_isOpen) return 0;
	//Cast to the Win32 class
	WCDialog_Win32 *dialog = (WCDialog_Win32*)this->_window;
	//Call to underlying method
	WPInt value = dialog->IntFromScript(var);
	//Clean up a bit
//	[variableName release];
	//Return the value
	return value;
}


WPUInt WCDialog::UnsignedIntFromScript(const std::string &var) {
	//Make sure window is open
	if (!this->_isOpen) return 0;
	//Cast to the Win32 class
	WCDialog_Win32 *dialog = (WCDialog_Win32*)this->_window;
	//Call to underlying method
	WPUInt value = dialog->UnsignedIntFromScript(var);
	//Return the value
	return value;
}


bool WCDialog::BoolFromScript(const std::string &var) {
	//Make sure window is open
	if (!this->_isOpen) return false;
	//Cast to the Win32 class
	WCDialog_Win32 *dialog = (WCDialog_Win32*)this->_window;
	//Call to underlying method
	bool value = dialog->BoolFromScript(var);
	//Return the value
	return value;
}


void WCDialog::StringFromScript(const std::string &var, const std::string &value) {
	//Make sure window is open
	if (!this->_isOpen) return;
	//Cast to the Win32 class
	WCDialog_Win32 *dialog = (WCDialog_Win32*)this->_window;
	//Call to underlying method
	dialog->StringFromScript(var, value);
}


void WCDialog::FloatFromScript(const std::string &var, const WPFloat &value) {
	//Make sure window is open
	if (!this->_isOpen) return;
	//Cast to the Win32 class
	WCDialog_Win32 *dialog = (WCDialog_Win32*)this->_window;
	//Call to underlying method
	dialog->FloatFromScript(var, value);
}


void WCDialog::IntFromScript(const std::string &var, const WPInt &value) {
	//Make sure window is open
	if (!this->_isOpen) return;
	//Cast to the Win32 class
	WCDialog_Win32 *dialog = (WCDialog_Win32*)this->_window;
	//Call to underlying method
	dialog->IntFromScript(var, value);
}


void WCDialog::UnsignedIntFromScript(const std::string &var, const WPUInt &value) {
	//Make sure window is open
	if (!this->_isOpen) return;
	//Cast to the Win32 class
	WCDialog_Win32 *dialog = (WCDialog_Win32*)this->_window;
	//Call to underlying method
	dialog->UnsignedIntFromScript(var, value);
}


void WCDialog::BoolFromScript(const std::string &var, const bool &value) {
	//Make sure window is open
	if (!this->_isOpen) return;
	//Cast to the Win32 class
	WCDialog_Win32 *dialog = (WCDialog_Win32*)this->_window;
	//Call to underlying method
	dialog->BoolFromScript(var, value);
}


/***********************************************~***************************************************/

