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


#ifndef __SKETCH_ARC_MODES_H__
#define __SKETCH_ARC_MODES_H__


/*** Included Header Files ***/
#include "Kernel/wftrl.h"
#include "Kernel/drawing_mode.h"
#include "Sketcher/sketch_workbench.h"
#include "Sketcher/sketch_arc_type.h"


/*** Local Defines ***/
#define SKETCHARCMODE_STANDARDCREATE_NAME		"SketchArc Standard Create Mode"
#define SKETCHARCMODE_THREEPOINTCREATE_NAME		"SketchArc Three-Point Create Mode"
#define SKETCHARCMODE_EDIT_NAME					"SketchArc Edit Mode"


/*** Class Predefines ***/
class WCSketchWorkbench;
class WCSketchArc;
class WCGeometricPoint;
class WCNurbsCurve;


/***********************************************~***************************************************/


class WCModeSketchArcStandardCreate : public WCDrawingMode {
private:
	WCSketchWorkbench							*_workbench;										//!< Parent sketch workbench
	WPUInt										_stage;												//!< Creation stage
	WCVector4									_first, _second;									//!< First and second click locations
	bool										_peepRight;											//!< First move to the right?
	WCGeometricPoint							*_centerPoint, *_secondPoint, *_thirdPoint;			//!< We three points
	WCNurbsCurve								*_arc;												//!< Current arc being drawn
	WCGeometricLine								*_firstLine, *_secondLine;							//!< Two lines for fanciness
	
	WPFloat										_xSuggest, _ySuggest;								//!< Location of suggestion
	WCSketchAlignmentSuggestion					_suggestionType;									//!< Type of suggestion
	std::list<WCSketchAlignmentSuggestion>		_alignRules;										//!< List of alignment suggestion rules
	
	//Deny Access
	WCModeSketchArcStandardCreate();																//!< Default constructor
public:
	//Constructors and Destructors
	WCModeSketchArcStandardCreate(WCSketchWorkbench *wb);											//!< Primary constructor
	~WCModeSketchArcStandardCreate()							{ }									//!< Default destructor
	
	//Virtual Methods
	void OnEntry(void);																				//!< Handle entry into mode
	void OnExit(void);																				//!< Handle exit from mode
	void OnMouseDown(const WCMouseButton &button);													//!< Handle mouse button press
	void OnMouseMove(const WPFloat &x, const WPFloat &y);											//!< Handle mouse movement
	void Render(void);																				//!< Render the mode
};


/***********************************************~***************************************************/


class WCModeSketchArcThreePointCreate : public WCDrawingMode {
private:
	WCSketchWorkbench							*_workbench;										//!< Parent sketch workbench
	WPUInt										_stage;												//!< Creation stage
	WCVector4									_first, _second;									//!< First and second click locations
	
	WCGeometricPoint							*_firstPoint, *_secondPoint, *_thirdPoint;			//!< We three points
	WCNurbsCurve								*_arc;												//!< Current arc being drawn
	WCGeometricLine								*_firstLine;										//!< One line for fanciness
	
	WPFloat										_xSuggest, _ySuggest;								//!< Location of suggestion
	WCSketchAlignmentSuggestion					_suggestionType;									//!< Type of suggestion
	std::list<WCSketchAlignmentSuggestion>		_alignRules;										//!< List of alignment suggestion rules
	
	//Deny Access
	WCModeSketchArcThreePointCreate();																//!< Default constructor
public:
	//Constructors and Destructors
	WCModeSketchArcThreePointCreate(WCSketchWorkbench *wb);											//!< Primary constructor
	~WCModeSketchArcThreePointCreate()							{ }									//!< Default destructor
	
	//Virtual Methods
	void OnEntry(void);																				//!< Handle entry into mode
	void OnExit(void);																				//!< Handle exit from mode
	void OnMouseDown(const WCMouseButton &button);													//!< Handle mouse button press
	void OnMouseMove(const WPFloat &x, const WPFloat &y);											//!< Handle mouse movement
	void Render(void);																				//!< Render the mode
};


/***********************************************~***************************************************/


class WCModeSketchArcEdit : public WCDrawingMode {
private:
	WCSketchArc									*_arc;												//!< Parent feature
	WCVector4									_center, _position;									//!< Position information
	WPFloat										_radius, _xSuggest, _ySuggest;						//!< Radius and suggestion location
	WCSketchAlignmentSuggestion					_suggestionType;									//!< Type of suggestion
	std::list<WCSketchAlignmentSuggestion>		_alignRules;										//!< List of alignment suggestion rules
	WPUInt										_editMode;											//!< End position or radius
	//Deny Access
	WCModeSketchArcEdit();																			//!< Default constructor
public:
	//Constructors and Destructors
	WCModeSketchArcEdit(WCSketchArc *arc);															//!< Primary constructor
	~WCModeSketchArcEdit()						{ }													//!< Default destructor
	
	//Virtual Methods
	void OnEntry(void);																				//!< Handle entry into mode
	void OnExit(void);																				//!< Handle exit from mode
	void OnMouseMove(const WPFloat &x, const WPFloat &y);											//!< Handle mouse movement
	void OnMouseUp(const WCMouseButton &button);													//!< Handle mouse button press
	void OnArrowKeyPress(const WCArrowKey &key);													//!< Handle arrow key press
	void Render(void);																				//!< Render the mode
};



/***********************************************~***************************************************/


#endif //__SKETCH_ARC_MODES_H__

