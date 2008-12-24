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


#ifndef __SKETCH_CIRCLE_ACTION_H__
#define __SKETCH_CIRCLE_ACTION_H__


/*** Included Header Files ***/
#include <Kernel/wftrl.h>
#include <Kernel/action.h>


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCSketch;
class WCSketchCircle;


/***********************************************~***************************************************/


class WCActionSketchCircleCreate : public WCAction {
private:
	WCGUID										_sketchGUID;										//!< GUID for associated objects
	WCSketchCircle								*_circle;											//!< Associated sketch object
	std::string									_circleName;										//!< Name of new circle
	WCSketch									*_sketch;											//!< Creating sketch
	WCVector4									_center;											//!< Circle center
	WPFloat										_radius;											//!< Circle radius
	//Hidden Constructors
	WCActionSketchCircleCreate();																	//!< Deny access to default constructor
	WCActionSketchCircleCreate(WCSketch *sketch, const std::string &circleName,						//!< Primary constructor
												const WCVector4 &center, const WPFloat &radius);
	//Friend Declaration
	friend class WCSketchCircle;																	//Make SketchCircle a friend
public:
	//Constructors and Destructors
	WCActionSketchCircleCreate(xercesc::DOMElement *element, WCSerialDictionary *dictionary);		//!< Persistance constructor
	~WCActionSketchCircleCreate()				{ }													//!< Default destructor
	
	//Member Access Methods
	inline WCSketchCircle* Circle(void)			{ return this->_circle; }							//!< Get the sketchcircle

	//Inherited Methods
	WCFeature* Execute(void);																		//!< Execute the action
	bool Merge(WCAction *action)				{ return false; }									//!< Try to merge two actions
	bool Rollback(void);																			//!< Try to rollback the action
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary);	//!< Serialize the object
};


/***********************************************~***************************************************/


class WCActionSketchCircleMove : public WCAction {
private:
	WCGUID										_circleGUID;										//!< GUID for associated objects
	WCSketchCircle								*_circle;											//!< Associated feature
	WCVector4									_center, _oldCenter;								//!< Pre and Post-move coords
	//Hidden Constructors
	WCActionSketchCircleMove();																		//!< Deny access to default constructor
	WCActionSketchCircleMove(WCSketchCircle *circle, const WCVector4 &center);						//!< Primary constructor
	//Friend Declarations
	friend class WCSketchCircle;																	//!< Make SketchCircle a friend
public:
	//Constructors and Destructors
	WCActionSketchCircleMove(xercesc::DOMElement *element, WCSerialDictionary *dictionary);			//!< Persistance constructor
	~WCActionSketchCircleMove()					{ }													//!< Default destructor
	
	//Member Access Methods
	inline WCVector4 OldCenter(void) const		{ return this->_oldCenter; }						//!< Get the pre-move center value
		
	//Inherited Methods
	WCFeature* Execute(void);																		//!< Execute the action
	bool Merge(WCAction *action)				{ return false; }									//!< Try to merge two actions
	bool Rollback(void);																			//!< Try to rollback the action
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
};


/***********************************************~***************************************************/


class WCActionSketchCircleRadius : public WCAction {
private:
	WCGUID										_circleGUID;										//!< GUID for associated objects
	WCSketchCircle								*_circle;											//!< Associated feature
	WPFloat										_radius, _oldRadius;								//!< Pre and Post-move values
	//Hidden Constructors
	WCActionSketchCircleRadius();																	//!< Deny access to default constructor
	WCActionSketchCircleRadius(WCSketchCircle *circle, const WPFloat &radius);						//!< Primary constructor
	//Friend Declarations
	friend class WCSketchCircle;																	//!< Make SketchCircle a friend
public:
	//Constructors and Destructors
	WCActionSketchCircleRadius(xercesc::DOMElement *element, WCSerialDictionary *dictionary);		//!< Persistance constructor
	~WCActionSketchCircleRadius()				{ }													//!< Default destructor
	
	//Member Access Methods
	inline WPFloat OldRadius(void) const		{ return this->_oldRadius; }						//!< Get the pre-move radius value
		
	//Inherited Methods
	WCFeature* Execute(void);																		//!< Execute the action
	bool Merge(WCAction *action)				{ return false; }									//!< Try to merge two actions
	bool Rollback(void);																			//!< Try to rollback the action
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__SKETCH_CIRCLE_ACTION_H__

