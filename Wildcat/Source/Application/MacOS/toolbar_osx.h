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


#ifndef __TOOLBAR_OSX_H__
#define __TOOLBAR_OSX_H__


/*** Included Header Files ***/
#import <Cocoa/Cocoa.h>


/*** Included Header Files ***/
//None


/*** C++ Class Predefines ***/
class WCToolbarButton_Bridge;
class WCToolbar_Bridge;


/*** Objective-C Class Predefines ***/
//None


/*** Local Defines ***/
#define TOOLBAR_ICON_SMALLSIZE					32
#define TOOLBAR_ICON_LARGESIZE					64
#define TOOLBAR_HEADER_HEIGHT					24
#define TOOLBAR_FOOTER_HEIGHT					26
#define TOOLBAR_ICON_PADDING					8


/***********************************************~***************************************************/


@interface WCToolbarButton_OSX : NSButton {
	WCToolbarButton_Bridge						*_bridge;											//!< Pointer to bridge button
}

/*** Constructors ***/
- (id)initWithBridge:(WCToolbarButton_Bridge*)bridge frame:(NSRect)frame;							//!< Primary constructor

/*** Member Access Methods ***/
- (WCToolbarButton_Bridge*)bridge;																	//!< Get the associated message

@end


/***********************************************~***************************************************/


@interface WCToolbarButtonCell_OSX : NSButtonCell {
}
@end


/***********************************************~***************************************************/


@interface WCToolbar_OSX : NSWindowController {
	WCToolbar_Bridge							*_bridge;											//!< Pointer to toolbar bridge
	NSString									*_title;											//!< Toolbar title
	NSMutableArray								*_buttons;											//!< NSMutableArray of toolbar elements
}

/*** Constructors ***/
- (id)initWithBridge:(WCToolbar_Bridge*)bridge title:(NSString*)title position:(NSRect)position;	//!< Default initializer

/*** Event Methods ***/
- (void)onButtonPress:(id)sender;																	//!< Called when a button is pressed
- (void)onCloseButtonPress;																			//!< Called when close button is pressed

/*** Member Access Methods ***/
- (void)addButton:(WCToolbarButton_Bridge*)bridge;													//!< Add an button to the toolbar
- (void)setVisible:(bool)state;																		//!< Set the visible state

@end


/***********************************************~***************************************************/


#endif //__TOOLBAR_OSX_H__

