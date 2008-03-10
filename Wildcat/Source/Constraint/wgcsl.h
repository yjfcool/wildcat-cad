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


#ifndef __WILDCAT_GEOMETRIC_CONSTRAINT_LIBRARY_H__
#define __WILDCAT_GEOMETRIC_CONSTRAINT_LIBRARY_H__


/***********************************************~***************************************************/


//Utility Headers
#include "Utility/wutil.h"
#include "Utility/adapter.h"
#include "Utility/bounding_object.h"
#include "Utility/color.h"
#include "Utility/log_manager.h"
#include "Utility/matrix.h"
#include "Utility/object.h"
#include "Utility/quaternion.h"
#include "Utility/serializeable_object.h"
#include "Utility/shader_manager.h"
#include "Utility/texture_manager.h"
#include "Utility/vector.h"
#include "Utility/visual_object.h"


//Geometry Headers
#include "Geometry/geometric_point.h"
#include "Geometry/geometric_line.h"
#include "Geometry/nurbs_curve.h"


//Scene Headers
#include "Scene/event_controller.h"


/***********************************************~***************************************************/


//Locall Defined Values
#define MEASUREPOSITION_BEGIN					0
#define MEASUREPOSITION_CLOSEST					1
#define MEASUREPOSITION_END						2
#define MEASUREPOSITION_CENTER					3

class WCMeasurePosition {
private:
	WPUInt										_pos;												//!< Type value
	//Deny Access
	WCMeasurePosition();																			//!< Deny access to default constructor
	WCMeasurePosition(const WPUInt &pos) : _pos(pos) { }											//!< Primary hidden constructor
public:
	//Constructors and Destructors
	WCMeasurePosition(const WCMeasurePosition &pos) : _pos(pos._pos) { }							//!< Copy constructor
	~WCMeasurePosition()						{ }													//!< Default destructor
	//Static Constructors
	static WCMeasurePosition Begin(void)	{ return WCMeasurePosition(MEASUREPOSITION_BEGIN); }	//!< Static begin constructor
	static WCMeasurePosition Closest(void)	{ return WCMeasurePosition(MEASUREPOSITION_CLOSEST);}	//!< Static closest const.
	static WCMeasurePosition End(void)		{ return WCMeasurePosition(MEASUREPOSITION_END); }		//!< Static end constructor
	static WCMeasurePosition Center(void)	{ return WCMeasurePosition(MEASUREPOSITION_CENTER); }	//!< Static center constructor

	//Overridden Operators
	bool operator==(const WCMeasurePosition &pos) { return this->_pos == pos._pos; }				//!< Equality operator
	bool operator!=(const WCMeasurePosition &pos) { return this->_pos != pos._pos; }				//!< Inequality operator
	WCMeasurePosition& operator=(const WCMeasurePosition &pos) { this->_pos = pos._pos; return *this; } //!< Equals operator

	//Persistance Methods
	void ToElement(xercesc::DOMElement *element)	{ WCSerializeableObject::AddFloatAttrib(element, "pos", this->_pos); }
	void FromElement(xercesc::DOMElement *element)	{ this->_pos = WCSerializeableObject::GetFloatAttrib(element, "pos"); }
};


/***********************************************~***************************************************/


//Values for Distance Type
#define MEASURETYPE_HORIZONTAL					0
#define MEASURETYPE_VERTICAL					1
#define MEASURETYPE_ABSOLUTE					2

class WCMeasureType {
private:
	int											_type;												//!< Type value
	WCMeasureType();																				//!< Deny access to default constructor 
	WCMeasureType(int type)						{ this->_type = type; }								//!< Hidden primary constructor
public:
	//Constructors and Destructors
	WCMeasureType(const WCMeasureType &type) : _type(type._type)	{ }								//!< Copy constructor
	~WCMeasureType()							{ }													//!< Default destructor
	//Static Constructors
	static WCMeasureType Horizontal(void)		{ return WCMeasureType(MEASURETYPE_HORIZONTAL);}	//!< Horizontal type
	static WCMeasureType Vertical(void)			{ return WCMeasureType(MEASURETYPE_VERTICAL); }		//!< Vertical type
	static WCMeasureType Absolute(void)			{ return WCMeasureType(MEASURETYPE_ABSOLUTE); }		//!< Absolute type
	
	//Overridden Operators
	WCMeasureType& operator=(const WCMeasureType &type) { this->_type = type._type; return *this;}	//!< Type equation
	bool operator==(const WCMeasureType &type) const { return this->_type == type._type; }			//!< Equals operator
	bool operator!=(const WCMeasureType &type) const { return this->_type != type._type; }			//!< Inequality operator

	//Persistance Methods
	void ToElement(xercesc::DOMElement *element)	{ WCSerializeableObject::AddFloatAttrib(element, "type", this->_type); }
	void FromElement(xercesc::DOMElement *element)	{ this->_type = WCSerializeableObject::GetFloatAttrib(element, "type"); }
};


/***********************************************~***************************************************/


#endif //__WILDCAT_GEOMETRIC_CONSTRAINT_LIBRARY_H__

