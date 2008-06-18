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


#ifndef __PART_PAD_TYPES_H__
#define __PART_PAD_TYPES_H__


/*** Included Header Files ***/
#include "Kernel/wftrl.h"


/*** Locally Defined Values ***/
#define PARTPAD_CLASSNAME						"Pad"


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {
	
	
/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


//Defines for profile types
enum PartPadType {
	PadDimension,
	PadUpToNext,
	PadUpToLast,
	PadUpToPlane,
	PadUpToSurface
};

class WCPartPadType {
private:
	PartPadType									_type;												//!< Type id
	WCPartPadType();																				//!< Deny access to default constructor 
	WCPartPadType(PartPadType type)				{ this->_type = type; }								//!< Hidden primary constructor
	friend class WCScene;
public:
	//Constructors and Destructors
	WCPartPadType(const WCPartPadType &type) : _type(type._type)	{ }								//!< Copy constructor
	~WCPartPadType()							{ }													//!< Default destructor
	static WCPartPadType Dimension(void)		{ return WCPartPadType(PadDimension); }				//!< Dimension type
	static WCPartPadType UpToNext(void)			{ return WCPartPadType(PadUpToNext); }				//!< Up-To-Next type
	static WCPartPadType UpToLast(void)			{ return WCPartPadType(PadUpToLast); }				//!< Uo-To-Last type
	static WCPartPadType UpToPlane(void)		{ return WCPartPadType(PadUpToPlane); }				//!< Up-To-Plane type
	static WCPartPadType UpToSurface(void)		{ return WCPartPadType(PadUpToSurface); }			//!< Up-To-Surface type
	
	//Overridden Operators
	WCPartPadType& operator=(const WCPartPadType &type)	{ this->_type = type._type; return *this; }	//!< Type equation				
	bool operator==(const WCPartPadType &type) const { return this->_type == type._type; }			//!< Equals operator
	bool operator!=(const WCPartPadType &type) const { return this->_type != type._type; }			//!< Inequality operator
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__PART_PAD_TYPES_H__

