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


#ifndef __TOOLBAR_MANAGER_H__
#define __TOOLBAR_MANAGER_H__


/*** Included Headers ***/
#include "Application/wildcat.h"


/*** Locally Defined Values ***/
#define TOOLBAR_ICON_SMALLSIZE					32
#define TOOLBAR_ICON_LARGESIZE					64
#define TOOLBAR_HEADER_HEIGHT					24
#define TOOLBAR_FOOTER_HEIGHT					26
#define TOOLBAR_ICON_PADDING					8


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCToolbar;
class WCToolbarButton;
class WCDocument;


/***********************************************~***************************************************/


class WCToolbarManager {
private:
	WCDocument									*_document;											//!< Parent document
	std::map<std::string, WCToolbarButton*>		_buttonMap;											//!< Map of all toolbar buttons
	std::map<std::string, WCToolbar*>			_toolbarMap;										//!< Map of names to toolbar objects
	std::stack< std::list< std::pair<bool,WCToolbar*> > > _stateStack;								//!< Push/pop stack

	//Private Methods
	void ParseManifest(const std::string &manifest, const std::string &directory, const bool &verbose);		//!< Parse the given manifest
	void ParseUserConfig(const std::string &prefFile, const std::string &directory, const bool &verbose);	//!< Parse the given user preferences
	
	//Deny Access
	WCToolbarManager();																				//!< Deny access to default constructor
	WCToolbarManager(const WCToolbarManager& mgr);													//!< Deny access to copy constructor
	WCToolbarManager& operator=(const WCToolbarManager& mgr);										//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCToolbarManager(WCDocument *doc, const std::string &manifest, const std::string &directory,	//!< Initialize the manager with a manifest
												const bool &verbose=false);
	~WCToolbarManager();																			//!< Default destructor
	
	//Shader access info
	void HideAll(void);																				//!< Hide all toolbars
	void PushState(void);																			//!< Push toolbar visibility state
	void PopState(void);																			//!< Pop toolbar visibility state
	WCToolbar* ToolbarFromName(const std::string &name);											//!< Get a toolbar from a name
	WCToolbarButton* ButtonFromName(const std::string &name);										//!< Get an element from a name
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__TOOLBAR_MANAGER_H__

