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


#ifndef __WILDCAT_GEOMETRIC_LIBRARY_H__
#define __WILDCAT_GEOMETRIC_LIBRARY_H__


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
#include "Utility/vector.h"
#include "Utility/visual_object.h"


//Type definitions
typedef std::pair<WCVector4, std::pair<WPFloat,WPFloat> >	WPIntersectRec;


//Global Variables
//None


/***********************************************~***************************************************/


inline WPUInt NextPowerOfTwo(const WPUInt in) {
	WPUInt i = 1;
	while (i < in) i *= 2;
	return i;
}


/***********************************************~***************************************************/


//NURBS Knot Modes
#define NURBS_MODE_DEFAULT		0
//#define NURBS_MODE_UNIFORM	1
#define NURBS_MODE_BEZIER		2
#define NURBS_MODE_CUSTOM		3


class WCNurbsMode {
private:
	unsigned int								_mode;												//!< Mode ID
	WCNurbsMode()								{ }													//!< Deny access to default constructor
	WCNurbsMode(const unsigned int &mode) : _mode(mode)			{ }									//!< Primary constructor
public:
	//Constructors and Destructors
	WCNurbsMode(const WCNurbsMode &mode) : _mode(mode._mode)	{ }									//!< Copy constructor
	~WCNurbsMode()								{ }													//!< Default destructor
	//Static Creation Methods
	static WCNurbsMode Default(void)			{ return WCNurbsMode(NURBS_MODE_DEFAULT); }			//!< Create a default knot mode object
	static WCNurbsMode Bezier(void)				{ return WCNurbsMode(NURBS_MODE_BEZIER); }			//!< Create a bezier knot mode object
	static WCNurbsMode Custom(void)				{ return WCNurbsMode(NURBS_MODE_CUSTOM); }			//!< Create a custom knot mode object
	//Overridden Operators
	WCNurbsMode& operator=(const WCNurbsMode &mode) { this->_mode = mode._mode; return *this; }		//!< Equals operator
	bool operator==(const WCNurbsMode &mode)	{ return this->_mode == mode._mode; }				//!< Equality operator
	bool operator!=(const WCNurbsMode &mode)	{ return this->_mode != mode._mode; }				//!< Inequality operator
	//Serialization Methods
	void ToElement(xercesc::DOMElement *element)	{ WCSerializeableObject::AddFloatAttrib(element, "mode", this->_mode); }
	void FromElement(xercesc::DOMElement *element)	{ this->_mode = WCSerializeableObject::GetFloatAttrib(element, "mode"); }
};


/***********************************************~***************************************************/


#endif //__WILDCAT_GEOMETRIC_LIBRARY_H__

