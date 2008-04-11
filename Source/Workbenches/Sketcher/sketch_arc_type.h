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


#ifndef __SKETCH_ARC_TYPE_H__
#define __SKETCH_ARC_TYPE_H__


/*** Included Header Files ***/
#include "Kernel/wftrl.h"


/*** Locally Defined Values ***/
#define SKETCHARC_TYPE_STANDARD					1
#define SKETCHARC_TYPE_THREEPOINT				2


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


class WCSketchArcType {
private:
	WPUInt										_type;												//!< Type value
	WCSketchArcType();																				//!< Deny access to the 
	WCSketchArcType(WPUInt type)				{ this->_type = type; }								//!< Hidden primary constructor
public:
	//Public constructor and destructor
	WCSketchArcType(const WCSketchArcType &type) : _type(type._type)	{ }							//!< Copy constructor
	~WCSketchArcType()								{ }												//!< Default destructor

	//Static creation methods
	inline static WCSketchArcType Standard(void)	{ WCSketchArcType t(SKETCHARC_TYPE_STANDARD);	//!< Create a lower-left placement object
												return t; }		
	inline static WCSketchArcType ThreePoint(void)	{ WCSketchArcType t(SKETCHARC_TYPE_THREEPOINT);	//!< Create a lower-right placement object
												return t; }

	
	//Overridden operators
	inline WCSketchArcType& operator=(const WCSketchArcType &type) { this->_type = type._type;		//!< Equals operator
												return *this; }
	inline bool operator==(const WCSketchArcType &type) const { return this->_type == type._type; }	//!< Equality operator
	inline bool operator!=(const WCSketchArcType &type) const { return this->_type != type._type; }	//!< Inequality operator
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__SKETCH_ARC_TYPE_H__

