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


#ifndef __CONSTRAINT_DISTANCE_MODES_H__
#define __CONSTRAINT_DISTANCE_MODES_H__


/*** Included Header Files ***/
#include "Constraint/wgcsl.h"
#include "Kernel/drawing_mode.h"


/*** Local Defines ***/
#define CONSTRAINTDISTANCEMODE_EDIT_NAME		"Distance Constraint Edit Mode"


/*** Class Predefines ***/
class WCSketchWorkbench;
class WCConstraintDistance;


/***********************************************~***************************************************/


class WCModeConstraintDistanceEdit : virtual public WCDrawingMode {
private:
	WCConstraintDistance						*_constraint;										//!< Object being edited
	WCVector4									_position;											//!< Initial position
	WPFloat										_offset, _labelOffset;								//!< Offset values
	//Deny Access
	WCModeConstraintDistanceEdit();																	//!< Default constructor
public:
	//Constructors and Destructors
	WCModeConstraintDistanceEdit(WCConstraintDistance *constraint);									//!< Primary constructor
	~WCModeConstraintDistanceEdit()				{ }													//!< Default destructor
	
	//Virtual Methods
	void OnEntry(void);																				//!< Handle entry into mode
	void OnMouseMove(const WPFloat &x, const WPFloat &y);											//!< Handle mouse moves
	void OnMouseUp(const WCMouseButton &button);													//!< Handle mouse button release
};


/***********************************************~***************************************************/


#endif //__CONSTRAINT_DISTANCE_MODES_H__

