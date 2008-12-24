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
#include <Kernel/wftrl.h>
#include <Kernel/drawing_mode.h>
#include <Kernel/workbench.h>


/*** Locally Defined Values ***/
#define SELECTIONMODE_NAME						"SelectionMode"
#define SELECTIONMODE_COLOR						1.0, 1.0, 1.0, 0.1
#define SELECTIONMODE_COLOR_BORDER				0.5, 0.5, 0.5, 0.5


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


class WCSelectionMode : public WCDrawingMode {
private:
	WCWorkbench									*_workbench;										//!< Parent workbench
	bool										_isSelecting;										//!< Selection state
	WPFloat										_markX, _markY;										//!< Initial location
	//Hidden Constructors
	WCSelectionMode();																				//!< Deny access to default constructor
	WCSelectionMode(const WCSelectionMode &mode);													//!< Deny access to copy constructor
public:
	//Constructors and Destructors
	WCSelectionMode(WCWorkbench *wb);																//!< Primary constructor
	virtual ~WCSelectionMode()					{ }													//!< Default destructor
	
	//Virtual Methods
	virtual void OnEntry(void);																		//!< Handle entry into mode
	virtual void OnExit(void);																		//!< Handle exit from mode
	virtual void OnMouseDown(const WCMouseButton &button);											//!< Handle mouse button press
	virtual void OnMouseUp(const WCMouseButton &button);											//!< Handle mouse button release
	virtual void Render(void);																		//!< Render the mode
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__SELECTION_MODE_H__

