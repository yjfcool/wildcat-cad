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


#ifndef __SKETCH_RECT_ACTION_H__
#define __SKETCH_RECT_ACTION_H__


/*** Included Header Files ***/
#include "Kernel/wftrl.h"
#include "Kernel/action.h"
#include "Sketcher/sketch_rect_mode.h"


/*** Local Defines ***/
//None


/*** Class Predefines ***/
class WCSketch;
class WCSketchLine;



/***********************************************~***************************************************/


class WCActionSketchRectangleCreate : public WCAction {
private:
	WCSketch									*_sketch;											//!< Creating sketch
	WCVector4									_lowerLeft, _upperRight;							//!< Corner points
	WCSketchLine								*_lines[4];											//!< Associated sketch lines

	//Deny Access
	WCActionSketchRectangleCreate(WCSketch *sketch, const WCVector4 &lowerLeft,						//!< Primary constructor
												const WCVector4 &upperRight);
	
	//Friend Declarations
	friend class WCSketchRectangle;																	//!< Make SketchRectangle a friend
public:
	//Constructors and Destructors
	WCActionSketchRectangleCreate(xercesc::DOMElement *element, WCSerialDictionary *dictionary);	//!< Persistance constructor
	~WCActionSketchRectangleCreate()			{ }													//!< Default destructor
	
	inline WCSketchLine* LeftLine(void)			{ return this->_lines[0]; }							//!< Get the left line
	inline WCSketchLine* TopLine(void)			{ return this->_lines[1]; }							//!< Get the top line
	inline WCSketchLine* RightLine(void)		{ return this->_lines[2]; }							//!< Get the right line
	inline WCSketchLine* BottomLine(void)		{ return this->_lines[3]; }							//!< Get the bottom line
	
	//Inherited Methods
	WCFeature* Execute(void);																		//!< Execute the action
	bool Merge(WCAction *action);																	//!< Try to merge two actions
	bool Rollback(void);																			//!< Try to rollback the action
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
};


/***********************************************~***************************************************/


class WCSketchRectangle {
public:
	static WCDrawingMode* ModeCreate(WCSketchWorkbench *wb);										//!< Return a rectangle creation mode controller
	static WCActionSketchRectangleCreate* ActionCreate(WCSketch *sketch, const WCVector4 &lowerLeft,//!< Primary constructor
												 const WCVector4 &upperRight);
};


/***********************************************~***************************************************/


#endif //__SKETCH_RECT_ACTION_H__

