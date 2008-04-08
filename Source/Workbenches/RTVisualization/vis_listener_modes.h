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


#ifndef __VIS_LISTENER_MODES_H__
#define __VIS_LISTENER_MODES_H__


/*** Included Header Files ***/
#include "RTVisualization/wrtv.h"
#include "Kernel/drawing_mode.h"


/*** Local Defines ***/
#define VISLISTENERMODE_CREATE_NAME				"VisListener Create Mode"
#define VISLISTENERMODE_EDIT_NAME				"VisListener Edit Mode"


/*** Class Predefines ***/
class WCVisWorkbench;
class WCVisListener;


/***********************************************~***************************************************/


class WCModeVisListenerCreate : public WCDrawingMode {
private:
	WCVisWorkbench								*_workbench;										//!< Parent sketch workbench
	WCDialog									*_dialog;											//!< Pointer to a dialog object
	WCDialogController							*_controller;										//!< Dialog controller
	//Deny Access
	WCModeVisListenerCreate();																		//!< Default constructor
public:
	//Constructors and Destructors
	WCModeVisListenerCreate(WCVisWorkbench *wb);													//!< Primary constructor
	~WCModeVisListenerCreate()					{ }													//!< Default destructor
	
	//Virtual Methods
	void OnEntry(void);																				//!< Handle entry into mode
	void OnExit(void);																				//!< Handle exit from mode
};


/***********************************************~***************************************************/


class WCModeVisListenerEdit : public WCDrawingMode {
private:
	WCVisListener								*_listener;											//!< Listener being edited
	//Deny Access
	WCModeVisListenerEdit();																		//!< Default constructor
public:
	//Constructors and Destructors
	WCModeVisListenerEdit(WCVisListener *listener);													//!< Primary constructor
	~WCModeVisListenerEdit()					{ }													//!< Default destructor
	
	//Virtual Methods
	void OnEntry(void);																				//!< Handle entry into mode
	void OnExit(void);																				//!< Handle exit from mode
	void OnMouseMove(const WPFloat &x, const WPFloat &y);											//!< Handle mouse moves
	void OnMouseUp(const WCMouseButton &button);													//!< Handle mouse button release
	void OnArrowKeyPress(const WCArrowKey &key);													//!< Handle arrow key press
	void Render(void);																				//!< Render the mode
};


/***********************************************~***************************************************/


#endif //__VIS_LISTENER_MODES_H__

