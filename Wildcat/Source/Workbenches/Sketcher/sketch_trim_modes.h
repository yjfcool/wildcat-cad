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


#ifndef __SKETCH_TRIM_MODES_H__
#define __SKETCH_TRIM_MODES_H__


/*** Included Header Files ***/
#include "Kernel/wftrl.h"
#include "Kernel/drawing_mode.h"
#include "Sketcher/sketch_workbench.h"


/*** Local Defines ***/
#define SKETCHTRIMMODE_CREATE_NAME				"SketchTrim Create Mode"
#define SKETCHTRIM_MIN_LENGTH					0.001


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


class WCModeSketchTrimCreate : virtual public WCDrawingMode {
private:
	WCSketchWorkbench							*_workbench;										//!< Parent sketch workbench
	WCGeometricCurve*							_curve;

	//Deny Access
	WCModeSketchTrimCreate();																		//!< Default constructor
public:
	//Constructors and Destructors
	WCModeSketchTrimCreate(WCSketchWorkbench *wb);													//!< Primary constructor
	~WCModeSketchTrimCreate()				{ }														//!< Default destructor
	
	//Virtual Methods
	void OnEntry(void);																				//!< Handle entry into mode
	void OnExit(void);																				//!< Handle exit from mode
	void OnMouseDown(const WCMouseButton &button);													//!< Handle mouse button press
	void OnMouseMove(const WPFloat &x, const WPFloat &y);											//!< Handle mouse movement
	void Render(void);																				//!< Render the mode
};


/***********************************************~***************************************************/


#endif //__SKETCH_TRIM_MODES_H__

