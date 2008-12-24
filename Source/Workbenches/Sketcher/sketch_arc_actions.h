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


#ifndef __SKETCH_ARC_ACTIONS_H__
#define __SKETCH_ARC_ACTIONS_H__


/*** Included Header Files ***/
#include <Kernel/wftrl.h>
#include <Kernel/action.h>
#include <Sketcher/sketch_arc_type.h>


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCSketch;
class WCSketchArc;


/***********************************************~***************************************************/


class WCActionSketchArcCreate : public WCAction {
private:
	WCGUID										_sketchGUID;										//!< GUID for associated objects
	WCSketchArcType								_type;												//!< Arc type
	WCSketchArc									*_arc;												//!< Associated sketch object
	std::string									_arcName;											//!< Name of new arc
	WCSketch									*_sketch;											//!< Creating sketch
	WCVector4									_center, _threePoint;								//!< Arc position, and three-point
	WPFloat										_radius, _startAngle, _endAngle;					//!< Radius, initial and final angles
	//Hidden Constructors
	WCActionSketchArcCreate();																		//!< Deny access to default constructor
	WCActionSketchArcCreate(WCSketch *sketch, const std::string &arcName, const WCVector4 &center,	//!< Primary standard constructor
												const WPFloat &radius, const WPFloat &startAngle, const WPFloat &endAngle);
	WCActionSketchArcCreate(WCSketch *sketch, const std::string &arcName, const WCVector4 &center,	//!< Primary three-point constructor
												const WPFloat &radius, const WPFloat &startAngle, const WPFloat &endAngle,
												const WCVector4 &threePoint);
	//Friend Declaration
	friend class WCSketchArc;																		//Make SketchArc a friend
public:
	//Constructors and Destructors
	WCActionSketchArcCreate(xercesc::DOMElement *element, WCSerialDictionary *dictionary);			//!< Persistance constructor
	~WCActionSketchArcCreate()					{ }													//!< Default destructor

	//Member Access Methods
	inline WCSketchArc* Arc(void)				{ return this->_arc; }								//!< Get the sketcharc

	//Inherited Methods
	WCFeature* Execute(void);																		//!< Execute the action
	bool Merge(WCAction *action)				{ return false; }									//!< Try to merge two actions
	bool Rollback(void);																			//!< Try to rollback the action
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
};


/***********************************************~***************************************************/


class WCActionSketchArcMove : public WCAction {
private:
	WCGUID										_arcGUID;											//!< GUID for associated objects
	WCSketchArc									*_arc;												//!< Associated feature
	WCVector4									_center, _oldCenter;								//!< Pre and Post-move coords
	//Hidden Constructors
	WCActionSketchArcMove();																		//!< Deny access to default constructor
	WCActionSketchArcMove(WCSketchArc *arc, const WCVector4 &center);								//!< Primary constructor
	//Friend Declarations
	friend class WCSketchArc;																		//!< Make SketchArc a friend
public:
	//Constructors and Destructors
	WCActionSketchArcMove(xercesc::DOMElement *element, WCSerialDictionary *dictionary);			//!< Persistance constructor
	~WCActionSketchArcMove()					{ }													//!< Default destructor
	
	//Member Access Methods
	inline WCVector4 OldCenter(void) const		{ return this->_oldCenter; }						//!< Get the pre-move center value
		
	//Inherited Methods
	WCFeature* Execute(void);																		//!< Execute the action
	bool Merge(WCAction *action)				{ return false; }									//!< Try to merge two actions
	bool Rollback(void);																			//!< Try to rollback the action
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
};


/***********************************************~***************************************************/


class WCActionSketchArcRadius : public WCAction {
private:
	WCGUID										_arcGUID;											//!< GUID for associated objects
	WCSketchArc									*_arc;												//!< Associated feature
	WPFloat										_radius, _oldRadius;								//!< Pre and Post-move values
	//Hidden Constructors
	WCActionSketchArcRadius();																		//!< Deny access to default constructor
	WCActionSketchArcRadius(WCSketchArc *arc, const WPFloat &radius);								//!< Primary constructor
	//Friend Declarations
	friend class WCSketchArc;																		//!< Make SketchArc a friend
public:
	//Constructors and Destructors
	WCActionSketchArcRadius(xercesc::DOMElement *element, WCSerialDictionary *dictionary);			//!< Persistance constructor
	~WCActionSketchArcRadius()					{ }													//!< Default destructor
	
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
#endif //__SKETCH_ARC_ACTIONS_H__

