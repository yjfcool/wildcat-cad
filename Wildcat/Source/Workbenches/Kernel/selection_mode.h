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


#ifndef __SELECTION_MODE_H__
#define __SELECTION_MODE_H__


/*** Included Header Files ***/
#include "Kernel/wftrl.h"
#include "Kernel/drawing_mode.h"
#include "Kernel/workbench.h"


/*** Local Defines ***/
#define SELECTIONMODE_NAME						"SelectionMode"
#define SELECTIONMODE_COLOR						1.0, 1.0, 1.0, 0.1
#define SELECTIONMODE_COLOR_BORDER				0.5, 0.5, 0.5, 0.5


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


class WCSelectionMode : virtual public WCDrawingMode {
private:
	WCWorkbench									*_workbench;										//!< Parent workbench
	bool										_isSelecting;										//!< Selection state
	WPFloat										_markX, _markY;										//!< Initial location
	//Deny Access
	WCSelectionMode();																				//!< Default constructor
public:
	//Constructors and Destructors
	WCSelectionMode(WCWorkbench *wb);																//!< Primary constructor
	~WCSelectionMode()				{ }																//!< Default destructor
	
	//Virtual Methods
	void OnEntry(void);																				//!< Handle entry into mode
	void OnExit(void);																				//!< Handle exit from mode
	void OnMouseDown(const WCMouseButton &button);													//!< Handle mouse button press
	void OnMouseMove(const WPFloat &x, const WPFloat &y);											//!< Handle mouse moves
	void OnMouseUp(const WCMouseButton &button);													//!< Handle mouse button release
	void Render(void);																				//!< Render the mode
};


/***********************************************~***************************************************/


#endif //__SELECTION_MODE_H__

