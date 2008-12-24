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


#ifndef __DIALOG_OSX_H__
#define __DIALOG_OSX_H__


/*** Imported Header Files ***/
#import <Cocoa/Cocoa.h>
#import <WebKit/WebView.h>


/*** Included Header Files ***/
#include <Kernel/dialog.h>


/*** C++ Class Predefines ***/
//None


/*** Objective-C Class Predefines ***/
//None


/***********************************************~***************************************************/


@interface WCDialog_OSX : NSWindowController {
	IBOutlet WebView							*webView;											//!< Associated webview object
	NSURL										*location;											//!< Current location
	__WILDCAT_NAMESPACE__::WCDialog				*_dialog;											//!< Associated dialog object
}

/*** Primary Methods ***/
- (id)initWithDialog:(WCDialog*)dialog;																//!< Initialize with a specific dialog
- (WebView*)WebView;																				//!< Get the embedded webview
- (void)showWindow;
- (void)hideWindow;

/*** Delegate Methods ***/
- (void)windowWillClose:(NSNotification *)notification;												//!< Make sure close of window is captured
- (void)webView:(WebView *)sender didFinishLoadForFrame:(WebFrame *)frame;							//!< Act as delegate for webframe and get load status

/*** Script Object Get Methods ***/
- (std::string)getStringFromScript:(NSString*)name;													//!< Deletegate calls to get script value
- (WPFloat)getFloatFromScript:(NSString*)name;														//!< Deletegate calls to get script value
- (WPInt)getIntFromScript:(NSString*)name;															//!< Deletegate calls to get script value
- (WPUInt)getUnsignedIntFromScript:(NSString*)name;													//!< Deletegate calls to get script value
- (bool)getBoolFromScript:(NSString*)name;															//!< Deletegate calls to get script value

/*** Script Object Set Methods ***/
- (void)setStringFromScript:(std::string)var withValue:(std::string)value;							//!< Deletegate calls to set script value
- (void)setFloatFromScript:(std::string)var withValue:(WPFloat)value;								//!< Deletegate calls to set script value
- (void)setIntFromScript:(std::string)var withValue:(WPInt)value;									//!< Deletegate calls to set script value
- (void)setUnsignedIntFromScript:(std::string)var withValue:(WPUInt)value;							//!< Deletegate calls to set script value
- (void)setBoolFromScript:(std::string)var withValue:(bool)value;									//!< Deletegate calls to set script value

@end


/***********************************************~***************************************************/


#endif //__DIALOG_OSX_H__

