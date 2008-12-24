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


#ifndef __SKETCH_POINT_MODES_H__
#define __SKETCH_POINT_MODES_H__


/*** Included Header Files ***/
#include <Kernel/wftrl.h>
#include <Kernel/drawing_mode.h>
#include <Sketcher/sketch_align_suggestion.h>


/*** Locally Defined Values ***/
#define SKETCHPOINTMODE_CREATE_NAME					"SketchPoint Create Mode"
#define SKETCHPOINTMODE_EDIT_NAME					"SketchPoint Edit Mode"


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCSketchWorkbench;
class WCSketchPoint;


/***********************************************~***************************************************/


class WCModeSketchPointCreate : public WCDrawingMode {
private:
	WCSketchWorkbench							*_workbench;										//!< Parent sketch workbench
	WPFloat										_xSuggest, _ySuggest;								//!< Location of suggestion
	WCSketchAlignmentSuggestion					_suggestionType;									//!< Type of suggestion
	std::list<WCSketchAlignmentSuggestion>		_alignRules;										//!< List of alignment suggestion rules

	//Deny Access
	WCModeSketchPointCreate();																		//!< Default constructor
public:
	//Constructors and Destructors
	WCModeSketchPointCreate(WCSketchWorkbench *wb);													//!< Primary constructor
	~WCModeSketchPointCreate()				{ }														//!< Default destructor
	
	//Virtual Methods
	void OnEntry(void);																				//!< Handle entry into mode
	void OnExit(void);																				//!< Handle exit from mode
	void OnMouseMove(const WPFloat &x, const WPFloat &y);											//!< Handle mouse moves
	void OnMouseUp(const WCMouseButton &button);													//!< Handle mouse button release
	void Render(void);																				//!< Render the mode
};


/***********************************************~***************************************************/


class WCModeSketchPointEdit : public WCDrawingMode {
private:
	WCSketchPoint								*_point;											//!< Point being edited
	WCVector4									_position;											//!< Initial position
	
	WPFloat										_xSuggest, _ySuggest;								//!< Location of suggestion
	WCSketchAlignmentSuggestion					_suggestionType;									//!< Type of suggestion
	std::list<WCSketchAlignmentSuggestion>		_alignRules;										//!< List of alignment suggestion rules

	//Deny Access
	WCModeSketchPointEdit();																		//!< Default constructor
public:
	//Constructors and Destructors
	WCModeSketchPointEdit(WCSketchPoint *point);													//!< Primary constructor
	~WCModeSketchPointEdit()				{ }														//!< Default destructor
	
	//Virtual Methods
	void OnEntry(void);																				//!< Handle entry into mode
	void OnExit(void);																				//!< Handle exit from mode
	void OnMouseMove(const WPFloat &x, const WPFloat &y);											//!< Handle mouse moves
	void OnMouseUp(const WCMouseButton &button);													//!< Handle mouse button release
	void OnArrowKeyPress(const WCArrowKey &key);													//!< Handle arrow key press
	void Render(void);																				//!< Render the mode
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__SKETCH_POINT_MODES_H__

