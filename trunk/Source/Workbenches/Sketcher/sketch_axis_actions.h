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


#ifndef __SKETCH_AXIS_ACTIONS_H__
#define __SKETCH_AXIS_ACTIONS_H__


/*** Included Header Files ***/
#include <Kernel/wftrl.h>
#include <Kernel/action.h>


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCSketch;
class WCSketchAxis;


/***********************************************~***************************************************/


class WCActionSketchAxisCreate : public WCAction {
private:
	WCGUID										_sketchGUID;										//!< GUID for associated objects
	WCSketchAxis								*_axis;												//!< Associated sketch object
	std::string									_axisName;											//!< Name of new point
	WCSketch									*_sketch;											//!< Creating sketch
	WCVector4									_begin, _end;										//!< Sketch coords
	//Hidden Constructors
	WCActionSketchAxisCreate();																		//!< Deny access to default constructor
	WCActionSketchAxisCreate(WCSketch *sketch, const std::string &axisName, const WCVector4 &begin,	//!< Primary constructor
												const WCVector4 &end);
	//Friend Declarations
	friend class WCSketchAxis;																		//!< Make SketchAxis a friend
public:
	//Constructors and Destructors
	WCActionSketchAxisCreate(xercesc::DOMElement *element, WCSerialDictionary *dictionary);			//!< Persistance constructor
	~WCActionSketchAxisCreate()					{ }													//!< Default destructor
	
	//Member Access Methods
	WCSketchAxis* Axis(void)					{ return this->_axis; }								//!< Get the sketchline
	
	//Inherited Methods
	WCFeature* Execute(void);																		//!< Execute the action
	bool Merge(WCAction *action)				{ return false; }									//!< Try to merge two actions
	bool Rollback(void);																			//!< Try to rollback the action
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary);	//!< Serialize the object
};


/***********************************************~***************************************************/


class WCActionSketchAxisMove : public WCAction {
private:
	WCGUID										_axisGUID;											//!< GUID for associated objects
	WCSketchAxis								*_axis;												//!< Associated sketch line
	WCVector4									_begin, _end;										//!< Post-move line coords
	WCVector4									_oldBegin, _oldEnd;									//!< Pre-move line coords
	//Hidden Constructors
	WCActionSketchAxisMove();																		//!< Deny access to default constructor
	WCActionSketchAxisMove(WCSketchAxis *axis, const WCVector4 &begin, const WCVector4 &end);		//!< Primary constructor
	//Friend Declarations
	friend class WCSketchAxis;																		//!< Make SketchLine a friend
public:
	//Constructors and Destructors
	WCActionSketchAxisMove(xercesc::DOMElement *element, WCSerialDictionary *dictionary);			//!< Persistance constructor
	~WCActionSketchAxisMove()					{ }													//!< Default destructor
	
	//Member Access Methods
	inline WCVector4 OldBegin(void) const		{ return this->_oldBegin; }							//!< Get the pre-move begin value
	inline WCVector4 OldEnd(void) const			{ return this->_oldEnd; }							//!< Get the pre-move end value
		
	//Inherited Methods
	WCFeature* Execute(void);																		//!< Execute the action
	bool Merge(WCAction *action)				{ return false; }									//!< Try to merge two actions
	bool Rollback(void);																			//!< Try to rollback the action
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary);	//!< Serialize the object
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__SKETCH_AXIS_ACTIONS_H__

