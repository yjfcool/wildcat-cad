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


/*** Included Header Files ***/
#include "Utility/wutil.h"
#include "Utility/vector.h"


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** C++ Class Predefines ***/
class WCToolbarButton;
class WCToolbar;
class WCMainFrame;
class WCToolbar_Bridge;


/***********************************************~***************************************************/


class WCToolbarButton_Bridge {
public:
	wxToolBarToolBase							*_button;											//!< Underlying wx button object
	std::string									_message;											//!< Message
	std::string									_tooltip;											//!< Tooltip for the button
	int											_type;												//!< Type of button
	std::string									_stdIcon, _activeIcon;								//!< Icon pointers
	bool										_isActive;											//!< Is button active
	bool										_isEnabled;											//!< Is button enabled
public:
	//Constructors and Destructors
	WCToolbarButton_Bridge(const WCUserMessage &message, const std::string &tooltip,				//!< Primary constructor
												const unsigned int &type, const std::string &stdIcon, 
												const std::string &activeIcon, const bool &active, const bool &enabled);
	~WCToolbarButton_Bridge();																		//!< Default destructor

	//Member Access Methods
	inline WCUserMessage Message(void) const	{ return this->_message; }							//!< Get the user message
	inline std::string Tooltip(void)			{ return this->_tooltip; }							//!< Get the tooltip
	inline std::string StandardIcon(void)		{ return this->_stdIcon; }							//!< Get the standard icon
	inline std::string ActiveIcon(void)			{ return this->_activeIcon; }						//!< Get the active icon
	inline int Type(void) const					{ return this->_type; }								//!< Get the button type
	inline bool IsActive(void) const			{ return this->_isActive; }							//!< Get the active state
	void IsActive(const bool &state);																//!< Set the active state
	inline bool IsEnabled(void) const			{ return this->_isEnabled; }						//!< Get the enabled state
	void IsEnabled(const bool &state);																//!< Set the enabled state
};


/***********************************************~***************************************************/


class WCToolbar_Bridge {
private:
	WCMainFrame									*_frame;											//!< Associated main frame
	bool										_isVisible;											//!< Visibility flag
	std::string									_title;												//!< Title of the toolbar
	WCToolbar									*_core;												//!< Core toolbar pointer
	wxToolBar									*_platform;											//!< Platform toolbar pointer
	static int									_nextToolID;										//!< Counter for toolbar button placement
public:
	//Constructors and Destructors
	WCToolbar_Bridge(WCMainFrame *frame, WCToolbar* toolbar, const std::string &title,				//!< Primary constructor
												 const WCVector4 &position);
	~WCToolbar_Bridge();																			//!< Default destructor

	//Event Methods
	void OnButtonPress(WCToolbarButton_Bridge *button);												//!< Triggered when an element is pressed
	void OnCloseButtonPress(void);																	//!< Called when close button is pressed

	//Member Access Methods
	void AddButton(WCToolbarButton_Bridge *button);													//!< Add a button to the toolbar
	WCToolbarButton* ButtonFromName(const std::string &name);										//!< Get a button from a name
	inline bool IsVisible(void) const			{ return this->_isVisible; }						//!< Get the visible state
	void IsVisible(const bool &state);																//!< Set the visible state
};



/***********************************************~***************************************************/


}	   //End Wildcat Namespace
#endif //__TOOLBAR_BRIDGE_H__

