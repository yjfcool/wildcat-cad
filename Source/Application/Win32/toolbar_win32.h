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


#ifndef __TOOLBAR_WIN32_H__
#define __TOOLBAR_WIN32_H__


/*** Included Header Files ***/
#include "Application/Win32/toolbar_bridge.h"


/*** Locally Defined Values ***/
#define TOOLBAR_ICON_SMALLSIZE					32
#define TOOLBAR_ICON_LARGESIZE					64
#define TOOLBAR_HEADER_HEIGHT					24
#define TOOLBAR_FOOTER_HEIGHT					26
#define TOOLBAR_ICON_PADDING					8


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


class WCToolbarButton_Win32 {
private:
	__WILDCAT_NAMESPACE__::WCToolbarButton_Bridge	*_bridge;										//!< Pointer to bridge button
public:
	//Constructors and Destructors
	WCToolbarButton_Win32(WCToolbarButton_Bridge* bridge) : _bridge(bridge) { }						//!< Primar constructor
	~WCToolbarButton_Win32()					{ }													//!< Default destructor

	//Member Access Methods
	inline __WILDCAT_NAMESPACE__::WCToolbarButton_Bridge* Bridge(void)	{ return this->_bridge; }	//!< Get associated bridge
};


/***********************************************~***************************************************/


class WCToolbar_Win32 {
private:
	__WILDCAT_NAMESPACE__::WCToolbar_Bridge		*_bridge;											//!< Pointer to toolbar bridge
	std::string									_title;												//!< Toolbar title
	std::vector<WCToolbarButton_Win32*>			_buttons;											//!< Vector of toolbar elements
	//Hidden Constructors
	WCToolbar_Win32();																				//!< Deny access to default constructor
	WCToolbar_Win32(const WCToolbar_Win32&);														//!< Deny access to copy constructor
public:
	WCToolbar_Win32(__WILDCAT_NAMESPACE__::WCToolbar_Bridge *bridge, const std::string &title);		//!< Primary constructor
	~WCToolbar_Win32()							{ }													//!< Default destructor

	//Event Methods
	void OnButtonPress(void);																		//!< Called when a button is pressed
	void OnCloseButtonPress(void);																	//!< Called when close button is pressed

	//Member Access Methods
	void AddButton(__WILDCAT_NAMESPACE__::WCToolbarButton_Bridge* bridge);							//!< Add an button to the toolbar
	void IsVisible(const bool &state);																//!< Set the visible state
};


/***********************************************~***************************************************/


#endif //__TOOLBAR_WIN32_H__

