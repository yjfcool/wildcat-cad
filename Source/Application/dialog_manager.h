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


#ifndef __DIALOG_MANAGER_H__
#define __DIALOG_MANAGER_H__


/*** Included Headers ***/
#include "Application/wildcat.h"


/*** Local Defines ***/
//None


/*** C++ Class Predefines ***/
//None


/***********************************************~***************************************************/


class WCDialog {
private:
	std::string									_name;												//!< Both the reference name and filename
	unsigned int								_winWidth, _winHeight;								//!< Window width and height
	bool										_modal;												//!< Is dialog modal flag
	//Hidden Constructors
	WCDialog();																						//!< Deny access to default constructor
	WCDialog(const WCDialog&);																		//!< Deny access to copy constructor
	WCDialog& operator=(const WCDialog&);															//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCDialog(const std::string &name, const unsigned int &width, const unsigned int &height, const bool &modal) :	//!< Primary constructor
												_name(name), _winWidth(width), _winHeight(height), _modal(modal) { }
	~WCDialog()									{}
	//Member Access Methods
	inline std::string Name(void)				{ return this->_name; }								//!< Get the dialog name
	inline unsigned int Width(void)				{ return this->_winWidth; }							//!< Get the window width
	inline unsigned int Height(void)			{ return this->_winHeight; }						//!< Get the window height
	inline bool Modal(void)						{ return this->_modal; }							//!< Get the modal flag
};


/***********************************************~***************************************************/

class WCDialogManager {
private:
	static std::map<std::string, WCDialog*>*	_dialogMap;											//!< Map of all dialog objects
	//Private Methods
	static void ParseManifest(const std::string &manifest, const bool &verbose);					//!< Parse the given manifest
	//Hidden Constructors
	WCDialogManager();																				//!< Deny access to default constructor
	WCDialogManager(const WCDialogManager&);														//!< Deny access to copy constructor
	WCDialogManager& operator=(const WCDialogManager&);												//!< Deny access to equals operator
public:
	//Constructors and Destructors
	static bool Initialize(const std::string &manifest, const bool &verbose=false);					//!< Initialize the manager with a manifest
	static bool Terminate();																		//!< Terminate the manager
	//General Access Methods
	static WCDialog* DialogFromName(const std::string &name);										//!< Get a toolbar from a name
};


/***********************************************~***************************************************/


#endif //__DIALOG_MANAGER_H__

