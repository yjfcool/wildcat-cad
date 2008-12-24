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


#ifndef __SKETCH_ACTIONS_H__
#define __SKETCH_ACTIONS_H__


/*** Included Header Files ***/
#include <Kernel/wftrl.h>
#include <Kernel/action.h>


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCSketch;
class WCPartPlane;
class WCSketchFeature;


/***********************************************~***************************************************/


class WCActionSketchCreate : public WCAction {
private:
	WCSketch									*_sketch;											//!< Associated sketch
	std::string									_sketchName;										//!< Sketch name
	WCPartPlane										*_refPlane;											//!< Reference plane

	//Constructors
	WCActionSketchCreate();																			//!< Deny access to default constructor
	WCActionSketchCreate(const WCActionSketchCreate &action);										//!< Deny access to copy constructor
	WCActionSketchCreate& operator=(const WCActionSketchCreate& action);							//!< Deny access to equals operator
	WCActionSketchCreate(WCFeature *creator, const std::string &sketchName, WCPartPlane *refPlane);		//!< Primary constructor
							 
	//Friend Declarations
	friend class WCSketch;																			//!< Sketch is a friend
public:
	//Constructors and Destructors
	WCActionSketchCreate(xercesc::DOMElement *element, WCSerialDictionary *dictionary);					//!< Persistance constructor
	~WCActionSketchCreate()						{ }													//!< Default destructor

	//Inherited Methods
	WCFeature* Execute(void);																		//!< Execute the action
	bool Merge(WCAction *action)				{ return false; }									//!< Try to merge two actions
	bool Rollback(void)							{ return false; }									//!< Try to rollback the action
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
};


/***********************************************~***************************************************/


class WCActionSketchMarkConstruction : public WCAction {
private:
	WCSketch									*_sketch;											//!< Associated sketch
	std::list<WCSketchFeature*>					_features;											//!< List of features
	std::list<bool>								_history;											//!< History of features

	//Constructors
	WCActionSketchMarkConstruction();																//!< Deny access to default constructor
	WCActionSketchMarkConstruction(const WCActionSketchMarkConstruction &action);					//!< Deny access to copy constructor
	WCActionSketchMarkConstruction& operator=(const WCActionSketchMarkConstruction& action);		//!< Deny access to equals operator
	WCActionSketchMarkConstruction(WCSketch *sketch, std::list<WCSketchFeature*> features);			//!< Primary constructor

	//Friend Declarations
	friend class WCSketch;																			//!< Sketch is a friend
public:
	//Constructors and Destructors
	WCActionSketchMarkConstruction(xercesc::DOMElement *element, WCSerialDictionary *dictionary);			//!< Persistance constructor
	~WCActionSketchMarkConstruction()			{ }													//!< Default destructor

	//Inherited Methods
	WCFeature* Execute(void);																		//!< Execute the action
	bool Merge(WCAction *action)				{ return false; }									//!< Try to merge two actions
	bool Rollback(void)							{ return false; }									//!< Try to rollback the action
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
};


class WCActionSketchMarkNonConstruction : public WCAction {
private:
	WCSketch									*_sketch;											//!< Associated sketch
	std::list<WCSketchFeature*>					_features;											//!< List of features
	std::list<bool>								_history;											//!< History of features

	//Constructors
	WCActionSketchMarkNonConstruction();															//!< Deny access to default constructor
	WCActionSketchMarkNonConstruction(const WCActionSketchMarkNonConstruction &action);				//!< Deny access to copy constructor
	WCActionSketchMarkNonConstruction& operator=(const WCActionSketchMarkNonConstruction& action);	//!< Deny access to equals operator
	WCActionSketchMarkNonConstruction(WCSketch *sketch, std::list<WCSketchFeature*> features);		//!< Primary constructor

	//Friend Declarations
	friend class WCSketch;																			//!< Sketch is a friend
public:
	//Constructors and Destructors
	WCActionSketchMarkNonConstruction(xercesc::DOMElement *element, WCSerialDictionary *dictionary);		//!< Persistance constructor
	~WCActionSketchMarkNonConstruction()		{ }													//!< Default destructor

	//Inherited Methods
	WCFeature* Execute(void);																		//!< Execute the action
	bool Merge(WCAction *action)				{ return false; }									//!< Try to merge two actions
	bool Rollback(void)							{ return false; }									//!< Try to rollback the action
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
};


/***********************************************~***************************************************/


class WCActionSketchDelete : public WCAction {
private:
	WCSketch									*_sketch;											//!< Associated sketch
	std::list<WCSketchFeature*>					_features;											//!< List of features

	//Constructors
	WCActionSketchDelete();																			//!< Deny access to default constructor
	WCActionSketchDelete(const WCActionSketchDelete &action);										//!< Deny access to copy constructor
	WCActionSketchDelete& operator=(const WCActionSketchDelete& action);							//!< Deny access to equals operator
	WCActionSketchDelete(WCSketch *sketch, std::list<WCSketchFeature*> features);					//!< Primary constructor

	//Friend Declarations
	friend class WCSketch;																			//!< Sketch is a friend
public:
	//Constructors and Destructors
	WCActionSketchDelete(xercesc::DOMElement *element, WCSerialDictionary *dictionary);					//!< Persistance constructor
	~WCActionSketchDelete()						{ }													//!< Default destructor
	//Inherited Methods
	WCFeature* Execute(void);																		//!< Execute the action
	bool Merge(WCAction *action)				{ return false; }									//!< Try to merge two actions
	bool Rollback(void)							{ return false; }									//!< Try to rollback the action
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__SKETCH_ACTIONS_H__

