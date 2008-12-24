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


#ifndef __RENDER_WINDOW_OSX_H__
#define __RENDER_WINDOW_OSX_H__


/*** Imported Header Files ***/
#import <Cocoa/Cocoa.h>
#import <Kernel/OSX/opengl_view.h>


/*** Objective-C Class Predefines ***/
@class WCDocument_OSX;


/***********************************************~***************************************************/


@interface WCRenderWindow : NSWindowController {
	WCDocument_OSX								*_doc;												//!< Document associated with render view
	IBOutlet WCOpenGLView						*_renderView;										//!< Actual OpenGL rendering view
	IBOutlet NSTextField						*_statusText;										//!< Status text at bottom of view
}

/*** Class Mehthods ***/
- (id)initWithDocument:(WCDocument_OSX*)document;
- (void)windowDidLoad;																				//!< Called when the window has loaded
- (void)windowDidBecomeKey:(NSNotification *)notification;											//!< Called when window becomes key
- (void)windowDidResignKey:(NSNotification *)notification;											//!< Called when window is no longer key

/*** Member Access Methods ***/
- (NSTextField*)statusText;																			//!< Get the status text view
- (WCOpenGLView*)renderView;																		//!< Get the OpenGL view
- (WCDocument_OSX*)document;																		//!< Get the parent document


@end


/***********************************************~***************************************************/



#endif //__RENDER_WINDOW_OSX_H__

