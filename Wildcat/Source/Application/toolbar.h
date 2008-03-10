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


#ifndef __TOOLBAR_H__
#define __TOOLBAR_H__


/*** Included Headers ***/
#include "Application/wildcat.h"


/*** Local Defines ***/
//None


/*** C++ Class Predefines ***/
class WCToolbarButton_Bridge;
class WCDocument;
class WCToolbar;
class WCToolbar_Bridge;
class WCToolbarManager;


/***********************************************~***************************************************/


//Button types
#define TOOLBARELEMENT_TYPE_TOGGLE				0
#define TOOLBARELEMENT_TYPE_MOMENTARY			1
#define TOOLBARELEMENT_TYPE_MODE				2


struct WCToolbarButton {
private:
	WCToolbarButton_Bridge						*_bridge;											//!< Bridge to platform indep.
	//Deny Acces
	WCToolbarButton();																				//!< Deny access to default constructor

	//Friend Classes
	friend class WCToolbar;																			//!< Declare toolbar to be a friend
public:
	//Constructors and Destructors
	WCToolbarButton(const WCUserMessage &message, const std::string &tooltip, const unsigned int &type,	//!< Primary constructor
												const std::string &stdIcon, const std::string &activeIcon,
												const bool &active, const bool &enabled);
	~WCToolbarButton();																				//!< Default destructor

	//Member Access Methods
	bool IsActive(void) const;																		//!< Get the active state
	void IsActive(const bool &state);																//!< Set the active state
	bool IsEnabled(void) const;																		//!< Get the enabled state
	void IsEnabled(const bool &state);																//!< Set the enabled state
};


/***********************************************~***************************************************/


class WCToolbar {
private:
	WCToolbar_Bridge							*_bridge;											//!< Bridge to platform dependent implementation

	//Private Methods
	void AddButton(WCToolbarButton *button);														//!< Add an element to the toolbar

	//Deny Access
	WCToolbar();																					//!< Deny access to default constructor
	WCToolbar(const WCToolbar& toolbar);															//!< Deny access to copy constructor
	WCToolbar& operator=(const WCToolbar& toolbar);													//!< Deny access to equals operator
	
	//Friend Classes
	friend class WCToolbarManager;																	//!< Toolbar manager is a friend
public:
	//Constructors and Destructors
	WCToolbar(WCDocument *doc, const std::string &name, const WCVector4 &position);					//!< Default constructor
	~WCToolbar();																					//!< Default constructor

	//Member Access Methods
	WCToolbarButton* ButtonFromName(const std::string &name);										//!< Get a button from a name
	bool IsVisible(void) const;																		//!< Get the visible state
	void IsVisible(const bool &state);																//!< Set the visible state
};


/***********************************************~***************************************************/


#endif //__TOOLBAR_H__

