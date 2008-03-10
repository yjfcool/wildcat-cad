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


#ifndef __SKETCH_CONSTRAINT_MODES_H__
#define __SKETCH_CONSTRAINT_MODES_H__


/*** Included Header Files ***/
#include "Constraint/wgcsl.h"
#include "Kernel/wftrl.h"
#include "Kernel/drawing_mode.h"
#include "Sketcher/sketch_workbench.h"


/*** Local Defines ***/
#define SKETCHCONSTRAINT_DIMCREATE_NAME				"SketchConstraint Dimensional Create Mode"
#define SKETCHCONSTRAINT_DIMEDIT_NAME				"SketchConstraint Dimensional Edit Mode"
#define SKETCHCONSTRAINT_GEOCREATE_NAME				"SketchConstraint Geometric Create Mode"
#define SKETCHCONSTRAINT_GEOEDIT_NAME				"SketchConstraint Geometric Edit Mode"


/*** Class Predefines ***/
class WCSketchConstraint;
class WCConstraintMeasure;


/***********************************************~***************************************************/


class WCModeSketchConstraintDimensionalCreate : virtual public WCDrawingMode {
private:
	WCSketchWorkbench							*_workbench;										//!< Parent sketch workbench
	unsigned int								_stage;												//!< Stage of creating contraint
	unsigned int								_type;												//!< Current type of constraint
	WCVector4									_position;											//!< Position mark
	WPFloat										_offset, _labelOffset;								//!< Offsets mark
	WCSketchFeature								*_first, *_second;									//!< First and second selected objects
	WCMeasurePosition							_posA, _posB;										//!< Measure positions
	WCConstraintMeasure							*_measure;											//!< Visualization measure
	WCGeometricLine								*_displayLine;										//!< Visualization line

	//Deny Access
	WCModeSketchConstraintDimensionalCreate();														//!< Default constructor
public:
	//Constructors and Destructors
	WCModeSketchConstraintDimensionalCreate(WCSketchWorkbench *wb);									//!< Primary constructor
	~WCModeSketchConstraintDimensionalCreate()	{ }													//!< Default destructor
	
	//Virtual Methods
	void OnEntry(void);																				//!< Handle entry into mode
	void OnExit(void);																				//!< Handle exit from mode
	void OnMouseDown(const WCMouseButton &button);													//!< Handle mouse button press
	void OnMouseMove(const WPFloat &x, const WPFloat &y);											//!< Handle mouse movement
	void OnMouseUp(const WCMouseButton &button);													//!< Handle mouse button release
	void Render(void);																				//!< Render the mode
};


/***********************************************~***************************************************/


class WCModeSketchConstraintGeometricCreate : virtual public WCDrawingMode {
private:
	WCSketchWorkbench							*_workbench;										//!< Parent sketch workbench

	//Deny Access
	WCModeSketchConstraintGeometricCreate();														//!< Default constructor
public:
	//Constructors and Destructors
	WCModeSketchConstraintGeometricCreate(WCSketchWorkbench *wb);									//!< Primary constructor
	~WCModeSketchConstraintGeometricCreate()	{ }													//!< Default destructor
	
	//Virtual Methods
	void OnEntry(void);																				//!< Handle entry into mode
	void OnExit(void);																				//!< Handle exit from mode
	void OnMouseDown(const WCMouseButton &button);													//!< Handle mouse button press
	void OnMouseMove(const WPFloat &x, const WPFloat &y);											//!< Handle mouse movement
	void OnMouseUp(const WCMouseButton &button);													//!< Handle mouse button release
	void Render(void);																				//!< Render the mode
};


/***********************************************~***************************************************/


#endif //__SKETCH_CONSTRAINT_MODES_H__

