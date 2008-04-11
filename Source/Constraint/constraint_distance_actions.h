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


#ifndef __CONSTRAINT_DISTANCE_ACTIONS_H__
#define __CONSTRAINT_DISTANCE_ACTIONS_H__


/*** Included Header Files ***/
#include "Constraint/wgcsl.h"
#include "Constraint/sketch_constraint.h"
#include "Kernel/action.h"


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCSketch;
class WCSketchFeature;
class WCConstraintDistance;


/***********************************************~***************************************************/


class WCActionConstraintDistanceCreate : public WCAction {
private:
	WCGUID										_firstGUID, _secondGUID, _sketchGUID;					//!< GUID for associated objects
	WCSketch									*_sketch;											//!< Associated sketch
	std::string									_constraintName;									//!< Name of new object
	WCMeasureType								_type;												//!< Type of measure
	WCSketchFeature								*_first, *_second;									//!< Associated objects
	WCMeasurePosition							_posA, _posB;										//!< Position types
	WCConstraintDistance						*_constraint;										//!< Post-creation pointer
	//Hidden Constructors
	WCActionConstraintDistanceCreate();																//!< Deny access to default constructor
	WCActionConstraintDistanceCreate(WCSketch *sketch, const std::string &constraintName,			//!< Primary constructor
												const WCMeasureType &type, 
												WCSketchFeature *first, WCSketchFeature *second,
												const WCMeasurePosition &posA=WCMeasurePosition::Closest(),
												const WCMeasurePosition &posB=WCMeasurePosition::Closest());
	//Friend Declarations
	friend class WCConstraintDistance;																//!< Make WCConstraintDistance a friend
public:
	//Constructors and Destructors
	WCActionConstraintDistanceCreate(xercesc::DOMElement *element, WCSerialDictionary *dict);		//!< Persistance constructor
	~WCActionConstraintDistanceCreate()			{ }													//!< Default destructor

	//Member Access Methods
	inline WCConstraintDistance* Constraint(void) { return this->_constraint; }						//!< Get the post-creation constraint

	//Inherited Methods
	WCFeature* Execute(void);																		//!< Execute the action
	bool Merge(WCAction *action)				{ return false; }									//!< Try to merge two actions
	bool Rollback(void);																			//!< Try to rollback the action
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
};


/***********************************************~***************************************************/


class WCActionConstraintDistanceMove : public WCAction {
private:
	WCGUID										_constraintGUID;									//!< GUID for associated object
	WCConstraintDistance						*_constraint;										//!< Associated object
	WPFloat										_oldOffset, _oldLabelOffset;						//!< Old data
	WPFloat										_offset, _labelOffset;								//!< Post-move data
	//Hidden Constructors
	WCActionConstraintDistanceMove();																//!< Deny access to default constructor
	WCActionConstraintDistanceMove(WCConstraintDistance *constraint, const WPFloat &offset,			//!< Primary constructor
												const WPFloat &labelOffset);
	//Friend Declaration
	friend class WCConstraintDistance;																//!< Make constraint distance a friend
public:
	//Constructors and Destructors
	WCActionConstraintDistanceMove(xercesc::DOMElement *element, WCSerialDictionary *dictionary);	//!< Persistance constructor
	~WCActionConstraintDistanceMove()			{ }													//!< Default destructor
	
	//Member Access Methods
	inline WPFloat OldOffset(void) const		{ return this->_oldOffset; }						//!< Get old position
	inline WPFloat OldLabelOffset(void) const	{ return this->_oldLabelOffset; }					//!< Get old label position
	
	//Inherited Methods
	WCFeature* Execute(void);																		//!< Execute the action
	bool Merge(WCAction *action)				{ return false; }									//!< Try to merge two actions
	bool Rollback(void);																			//!< Try to rollback the action
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__CONSTRAINT_DISTANCE_ACTIONS_H__

