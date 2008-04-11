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


#ifndef __SKETCH_POINT_ACTIONS_H__
#define __SKETCH_POINT_ACTIONS_H__


/*** Included Header Files ***/
#include "Kernel/wftrl.h"
#include "Kernel/action.h"


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCSketch;
class WCSketchPoint;


/***********************************************~***************************************************/


class WCActionSketchPointCreate : public WCAction {
private:
	WCGUID										_sketchGUID;										//!< GUID for associated objects
	WCSketchPoint								*_point;											//!< Associated sketch point
	std::string									_pointName;											//!< Name of new point
	WCSketch									*_sketch;											//!< Creating sketch
	WCVector4									_position;											//!< On-plane x-y coordinates
	//Hidden Constructors
	WCActionSketchPointCreate();																	//!< Deny access to default constructor
	WCActionSketchPointCreate(WCSketch *sketch, const std::string &pointName, const WCVector4 &pos);//!< Primary constructor
	//Friend Declaration
	friend class WCSketchPoint;																		//!< Make SketchPoint a friend
public:
	//Constructors and Destructors
	WCActionSketchPointCreate(xercesc::DOMElement *element, WCSerialDictionary *dictionary);		//!< Persistance constructor
	~WCActionSketchPointCreate()				{ }													//!< Default destructor
	
	//Member Access Methods
	inline WCSketchPoint* Point(void)			{ return this->_point; }							//!< Get the associated point
	
	//Inherited Methods
	WCFeature* Execute(void);																		//!< Execute the action
	bool Merge(WCAction *action)				{ return false; }									//!< Try to merge two actions
	bool Rollback(void);																			//!< Try to rollback the action
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary);	//!< Serialize the object
};


/***********************************************~***************************************************/


class WCActionSketchPointMove : public WCAction {
private:
	WCGUID										_pointGUID;											//!< GUID for associated objects
	WCSketchPoint								*_point;											//!< Associated sketch point
	WCVector4									_position, _oldPosition;							//!< Pre-move positions
	//Hidden Constructors
	WCActionSketchPointMove();																		//!< Deny access to default constructor
	WCActionSketchPointMove(WCSketchPoint *point, const WCVector4 &pos);							//!< Primary constructor
	//Friend Declaration
	friend class WCSketchPoint;																		//!< Make SketchPoint a friend
public:
	//Constructors and Destructors
	WCActionSketchPointMove(xercesc::DOMElement *element, WCSerialDictionary *dictionary);			//!< Persistance constructor
	~WCActionSketchPointMove()					{ }													//!< Default destructor
	
	//Member Access Methods
	inline WCVector4 OldPosition(void) const	{ return this->_oldPosition; }						//!< Get old position
	
	//Inherited Methods
	WCFeature* Execute(void);																		//!< Execute the action
	bool Merge(WCAction *action)				{ return false; }									//!< Try to merge two actions
	bool Rollback(void);																			//!< Try to rollback the action
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary);	//!< Serialize the object
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__SKETCH_POINT_ACTIONS_H__

