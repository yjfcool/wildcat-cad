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


#ifndef __TOOLBAR_BRIDGE_H__
#define __TOOLBAR_BRIDGE_H__


/*** Imported Header Files ***/
//None


/*** Included Header Files ***/
#include "Utility/wutil.h"
#include "Utility/vector.h"


/*** C++ Class Predefines ***/
class WCToolbarButton;
class WCToolbar;
class WCDocument;
class WCToolbar_Bridge;


/*** Objective-C Class Predefines ***/
@class NSString;
@class NSImage;
@class WCToolbarButton_OSX;
@class WCToolbar_OSX;


/***********************************************~***************************************************/

class WCToolbarButton_Bridge {
private:
	WCToolbarButton								*_core;												//!< Pointer to core button
	std::list<WCToolbarButton_OSX*>				_platforms;											//!< Pointer to platform buttons
	WCUserMessage								_message;											//!< Message that is passed on button press
	NSString									*_tooltip;											//!< Tooltip for element
	int											_type;												//!< Button type
	NSImage										*_stdIcon, *_activeIcon;							//!< Icon images for element
	bool										_isActive;											//!< Is the element active
	bool										_isEnabled;											//!< Is the element selected

	//Friend Classes
	friend class WCToolbar_Bridge;																	//!< Declare toolbar bridge a friend
public:
	//Constructors and Destructors
	WCToolbarButton_Bridge(WCToolbarButton *button, const WCUserMessage &message,					//!< Primary constructor
												const std::string &tooltip, const unsigned int &type,
												const std::string &stdIcon, const std::string &activeIcon,
												const bool &active, const bool &enabled);
	~WCToolbarButton_Bridge();																		//!< Default destructor

	//Addition Method
	void AddInstance(WCToolbarButton_OSX* instance);												//!< Add an instance of the button

	//Member Access Methods
	inline WCUserMessage Message(void) const	{ return this->_message; }							//!< Get the user message
	inline NSString* Tooltip(void)				{ return this->_tooltip; }							//!< Get the tooltip
	inline NSImage* StandardIcon(void)			{ return this->_stdIcon; }							//!< Get the standard icon
	inline NSImage* ActiveIcon(void)			{ return this->_activeIcon; }						//!< Get the active icon
	inline int Type(void) const					{ return this->_type; }								//!< Get the button type
	inline bool IsActive(void) const			{ return this->_isActive; }							//!< Get the active state
	void IsActive(const bool &state);																//!< Set the active state
	inline bool IsEnabled(void) const			{ return this->_isEnabled; }						//!< Get the enabled state
	void IsEnabled(const bool &state);																//!< Set the enabled state
};


/***********************************************~***************************************************/


class WCToolbar_Bridge {
private:
	WCDocument									*_document;											//!< Associated document
	bool										_isVisible;											//!< Visibility flag
	WCToolbar									*_core;												//!< Core toolbar pointer
	WCToolbar_OSX								*_platform;											//!< Platform toolbar pointer
public:
	//Constructors and Destructors
	WCToolbar_Bridge(WCDocument *doc, WCToolbar* toolbar, const std::string &title,					//!< Primary constructor
												 const WCVector4 &position);
	~WCToolbar_Bridge();																			//!< Default destructor

	//Event Methods
	void OnButtonPress(WCToolbarButton_OSX *button);												//!< Triggered when an element is pressed
	void OnCloseButtonPress(void);																	//!< Called when close button is pressed

	//Member Access Methods
	void AddButton(WCToolbarButton_Bridge *button);													//!< Add a button to the toolbar
	WCToolbarButton* ButtonFromName(const std::string &name);										//!< Get a button from a name
	inline bool IsVisible(void) const			{ return this->_isVisible; }						//!< Get the visible state
	void IsVisible(const bool &state);																//!< Set the visible state
};


/***********************************************~***************************************************/


#endif //__TOOLBAR_BRIDGE_H__

