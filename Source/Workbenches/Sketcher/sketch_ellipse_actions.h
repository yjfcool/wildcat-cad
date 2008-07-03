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


#ifndef __SKETCH_ELLIPSE_ACTIONS_H__
#define __SKETCH_ELLIPSE_ACTIONS_H__


/*** Included Header Files ***/
#include "Kernel/wftrl.h"
#include "Kernel/action.h"


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCSketch;
class WCSketchEllipse;


/***********************************************~***************************************************/


class WCActionSketchEllipseCreate : public WCAction {
private:
	WCGUID										_sketchGUID;										//!< GUID for associated objects
	WCSketchEllipse								*_ellipse;											//!< Associated sketch ellipse
	std::string									_ellipseName;										//!< Name of new object
	WCSketch									*_sketch;											//!< Creating sketch
	WCVector4									_center, _majorAxis;								//!< Center of the ellipse, major axis
	WPFloat										_semiMinor, _semiMajor;								//!< Radii values
	//Hidden Constructors
	WCActionSketchEllipseCreate();																	//!< Deny access to default constructor
	WCActionSketchEllipseCreate(WCSketch *sketch, const std::string &ellipseName,					//!< Primary constructor
												const WCVector4 &center, const WCVector4 &majorAxis,
												const WPFloat &semiMinor, const WPFloat &semiMajor);
	//Friend Declarations
	friend class WCSketchEllipse;																	//!< Make SketchEllipse a friend
public:
	//Constructors and Destructors
	WCActionSketchEllipseCreate(xercesc::DOMElement *element, WCSerialDictionary *dictionary);		//!< Persistance constructor
	~WCActionSketchEllipseCreate()				{ }													//!< Default destructor

	//Member Access Methods
	inline WCSketchEllipse* Ellipse(void)		{ return this->_ellipse; }							//!< Get the sketchellipse

	//Inherited Methods
	WCFeature* Execute(void);																		//!< Execute the action
	bool Merge(WCAction *action)				{ return false; }									//!< Try to merge two actions
	bool Rollback(void);																			//!< Try to rollback the action
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary);	//!< Serialize the object
};


/***********************************************~***************************************************/


class WCActionSketchEllipseMove : public WCAction {
private:
	WCGUID										_ellipseGUID;										//!< GUID for associated objects
	WCSketchEllipse								*_ellipse;											//!< Associated feature
	WCVector4									_center, _oldCenter;								//!< Pre and Post-move coords
	//Hidden Constructors
	WCActionSketchEllipseMove();																	//!< Deny access to default constructor
	WCActionSketchEllipseMove(WCSketchEllipse *ellipse, const WCVector4 &center);					//!< Primary constructor
	//Friend Declarations
	friend class WCSketchEllipse;																	//!< Make SketchEllipse a friend
public:
	//Constructors and Destructors
	WCActionSketchEllipseMove(xercesc::DOMElement *element, WCSerialDictionary *dictionary);		//!< Persistance constructor
	~WCActionSketchEllipseMove()				{ }													//!< Default destructor
	
	//Member Access Methods
	inline WCVector4 OldCenter(void) const		{ return this->_oldCenter; }						//!< Get the pre-move center value
		
	//Inherited Methods
	WCFeature* Execute(void);																		//!< Execute the action
	bool Merge(WCAction *action)				{ return false; }									//!< Try to merge two actions
	bool Rollback(void);																			//!< Try to rollback the action
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
};


/***********************************************~***************************************************/


class WCActionSketchEllipseMajor : public WCAction {
private:
	WCGUID										_ellipseGUID;										//!< GUID for associated objects
	WCSketchEllipse								*_ellipse;											//!< Associated feature
	WCVector4									_majorAxis, _oldMajorAxis;							//!< Pre and Post-move axis
	WPFloat										_major, _oldMajor;									//!< Per and post move radius
	//Hidden Constructors
	WCActionSketchEllipseMajor();																	//!< Deny access to default constructor
	WCActionSketchEllipseMajor(WCSketchEllipse *ellipse, const WCVector4 &majorAxis,				//!< Primary constructor
												const WPFloat &major);
	//Friend Declarations
	friend class WCSketchEllipse;																	//!< Make SketchEllipse a friend
public:
	//Constructors and Destructors
	WCActionSketchEllipseMajor(xercesc::DOMElement *element, WCSerialDictionary *dictionary);		//!< Persistance constructor
	~WCActionSketchEllipseMajor()				{ }													//!< Default destructor
	
	//Member Access Methods
	inline WCVector4 OldMajorAxis(void) const	{ return this->_oldMajorAxis; }						//!< Get the pre-move major axis value
	inline WCVector4 OldMajor(void) const		{ return this->_oldMajor; }							//!< Get the pre-move major value
		
	//Inherited Methods
	WCFeature* Execute(void);																		//!< Execute the action
	bool Merge(WCAction *action)				{ return false; }									//!< Try to merge two actions
	bool Rollback(void);																			//!< Try to rollback the action
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
};


/***********************************************~***************************************************/


class WCActionSketchEllipseMinor : public WCAction {
private:
	WCGUID										_ellipseGUID;										//!< GUID for associated objects
	WCSketchEllipse								*_ellipse;											//!< Associated feature
	WPFloat										_minor, _oldMinor;									//!< Pre and Post-move values
	//Hidden Constructors
	WCActionSketchEllipseMinor();																	//!< Deny access to default constructor
	WCActionSketchEllipseMinor(WCSketchEllipse *ellipse, const WPFloat &minor);						//!< Primary constructor
	//Friend Declarations
	friend class WCSketchEllipse;																	//!< Make SketchEllipse a friend
public:
	//Constructors and Destructors
	WCActionSketchEllipseMinor(xercesc::DOMElement *element, WCSerialDictionary *dictionary);		//!< Persistance constructor
	~WCActionSketchEllipseMinor()				{ }													//!< Default destructor
	
	//Member Access Methods
	inline WPFloat OldMinor(void) const			{ return this->_oldMinor; }							//!< Get the pre-move minor value
		
	//Inherited Methods
	WCFeature* Execute(void);																		//!< Execute the action
	bool Merge(WCAction *action)				{ return false; }									//!< Try to merge two actions
	bool Rollback(void);																			//!< Try to rollback the action
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
};


/***********************************************~***************************************************/

}	   // End Wildcat Namespace
#endif //__SKETCH_ELLIPSE_ACTIONS_H__

