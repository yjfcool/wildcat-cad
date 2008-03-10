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


#ifndef __SKETCH_CIRCLE_MODES_H__
#define __SKETCH_CIRCLE_MODES_H__


/*** Included Header Files ***/
#include "Kernel/wftrl.h"
#include "Kernel/drawing_mode.h"
#include "Sketcher/sketch_workbench.h"


/*** Local Defines ***/
#define SKETCHCIRCLEMODE_CREATE_NAME			"SketchCircle Create Mode"
#define SKETCHCIRCLEMODE_EDIT_NAME				"SketchCircle Edit Mode"
#define SKETCHCIRCLE_MIN_RADIUS					0.001


/*** Class Predefines ***/
class WCSketchWorkbench;
class WCSketchCircle;


/***********************************************~***************************************************/


class WCModeSketchCircleCreate : virtual public WCDrawingMode {
private:
	WCSketchWorkbench							*_workbench;										//!< Parent sketch workbench
	bool										_isDrawing;											//!< Has the first point been drawn
	WPFloat										_beginX, _beginY, _radius;							//!< Initial point coords
	WCGeometricPoint							*_center;											//!< Center point
	WCNurbsCurve								*_circle;											//!< Current circle being drawn
	WPFloat										_xSuggest, _ySuggest;								//!< Location of suggestion
	WCSketchAlignmentSuggestion					_suggestionType;									//!< Type of suggestion
	std::list<WCSketchAlignmentSuggestion>		_alignRules;										//!< List of alignment suggestion rules
	//Deny Access
	WCModeSketchCircleCreate();																		//!< Default constructor
public:
	//Constructors and Destructors
	WCModeSketchCircleCreate(WCSketchWorkbench *wb);												//!< Primary constructor
	~WCModeSketchCircleCreate()					{ }													//!< Default destructor
	
	//Virtual Methods
	void OnEntry(void);																				//!< Handle entry into mode
	void OnExit(void);																				//!< Handle exit from mode
	void OnMouseDown(const WCMouseButton &button);													//!< Handle mouse button press
	void OnMouseMove(const WPFloat &x, const WPFloat &y);											//!< Handle mouse movement
	void Render(void);																				//!< Render the mode
};


/***********************************************~***************************************************/


class WCModeSketchCircleEdit : virtual public WCDrawingMode {
private:
	WCSketchCircle								*_circle;											//!< Parent feature
	WCVector4									_center, _position;									//!< Position information
	WPFloat										_radius, _xSuggest, _ySuggest;						//!< Radius and suggestion location
	WCSketchAlignmentSuggestion					_suggestionType;									//!< Type of suggestion
	std::list<WCSketchAlignmentSuggestion>		_alignRules;										//!< List of alignment suggestion rules
	WPUInt										_editMode;											//!< End position or radius
	//Deny Access
	WCModeSketchCircleEdit();																		//!< Default constructor
public:
	//Constructors and Destructors
	WCModeSketchCircleEdit(WCSketchCircle *circle);													//!< Primary constructor
	~WCModeSketchCircleEdit()					{ }													//!< Default destructor
	
	//Virtual Methods
	void OnEntry(void);																				//!< Handle entry into mode
	void OnExit(void);																				//!< Handle exit from mode
	void OnMouseMove(const WPFloat &x, const WPFloat &y);											//!< Handle mouse movement
	void OnMouseUp(const WCMouseButton &button);													//!< Handle mouse button press
	void OnArrowKeyPress(const WCArrowKey &key);													//!< Handle arrow key press
	void Render(void);																				//!< Render the mode
};


/***********************************************~***************************************************/


#endif //__SKETCH_CIRCLE_MODES_H__

