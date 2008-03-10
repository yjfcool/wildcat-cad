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


#ifndef __UNIT_TYPES_H__
#define __UNIT_TYPES_H__


/*** Included Header Files ***/
#include "Kernel/wftrl.h"


/*** Local Defines ***/
//None


/*** Class Predefines ***/
class WCUnitType;


/***********************************************~***************************************************/


//Local Defines
#define UNITCATEGORY_LENGTH						1
#define UNITCATEGORY_ANGLE						2
#define UNITCATEGORY_TIME						3
#define UNITCATEGORY_MASS						4
#define UNITCATEGORY_DENSITY					5
#define UNITCATEGORY_AREA						6
#define UNITCATEGORY_VOLUME						7

class WCUnitCategory {
private:
	unsigned int								_cat;												//!< Category itself
	//Deny Access
	WCUnitCategory();																				//!< Deny access to default constructor 
	WCUnitCategory(const unsigned int &cat)		{ this->_cat = cat; }								//!< Hidden primary constructor
	//Friend Declaration
	friend class WCUnitType;																		//!< Declare unit type a friend
public:
	//Constructors and Destructors
	WCUnitCategory(const WCUnitCategory &cat) : _cat(cat._cat) { }									//!< Copy constructor
	~WCUnitCategory()							{ }													//!< Default destructor

	//Static Constructors
	static WCUnitCategory Length(void)			{ return WCUnitCategory(UNITCATEGORY_LENGTH); }		//!< Length unit type
	static WCUnitCategory Angle(void)			{ return WCUnitCategory(UNITCATEGORY_ANGLE); }		//!< Angle unit type
	static WCUnitCategory Time(void)			{ return WCUnitCategory(UNITCATEGORY_TIME); }		//!< Time unit type
	static WCUnitCategory Mass(void)			{ return WCUnitCategory(UNITCATEGORY_MASS); }		//!< Mass unit type
	static WCUnitCategory Density(void)			{ return WCUnitCategory(UNITCATEGORY_DENSITY); }	//!< Density unit type
	static WCUnitCategory Area(void)			{ return WCUnitCategory(UNITCATEGORY_AREA); }		//!< Area unit type
	static WCUnitCategory Volume(void)			{ return WCUnitCategory(UNITCATEGORY_VOLUME); }		//!< Volume unit type

	//Overridder Operators
	WCUnitCategory& operator=(const WCUnitCategory &cat)	{ this->_cat = cat._cat; return *this; }//!< Button equation				
	bool operator==(const WCUnitCategory &cat) const	{ return this->_cat == cat._cat; }			//!< Equals operator
	bool operator!=(const WCUnitCategory &cat) const	{ return this->_cat != cat._cat; }			//!< Inequality operator
};


/***********************************************~***************************************************/


class WCUnitType {
protected:
	std::string									_name;												//!< Unit type name
	std::string									_symbol;											//!< Unit type symbol
	WCUnitCategory								_category;											//!< Unit type category
	WPFloat										_toBase, _fromBase;									//!< Conversion values
	std::string (*_displayFunc)(WCUnitType *type, const WPFloat &value, const WPUInt &precision);	//!< Display function pointer
	
	//Deny Access
	WCUnitType();																					//!< Deny access to default constructor
	WCUnitType(const std::string &name, const std::string &symbol, const WCUnitCategory &category,	//!< Hidden primary constructor
												const WPFloat &toBase, const WPFloat &fromBase,
												std::string (*displayFunc)(WCUnitType *type, const WPFloat &value, const WPUInt &precision));
public:
	//Constructors and Destructors
	virtual ~WCUnitType()						{ }													//!< Default destructor

	//Member Access Methods
	inline std::string Name(void)				{ return this->_name; }								//!<
	inline std::string Symbol(void)				{ return this->_symbol; }							//!<
	inline WCUnitCategory Category(void)		{ return this->_category; }							//!<
	inline WPFloat ToBase(void)					{ return this->_toBase; }							//!<
	inline WPFloat FromBase(void)				{ return this->_fromBase; }							//!<

	//Primary Methods
	std::string DisplayString(const WPFloat &value, const WPUInt &precision);						//!< Mandatory method to display value
	inline WPFloat ToBase(const WPFloat &value)		{ return value * this->_toBase; }				//!< Convert from this unit to base unit
	inline WPFloat FromBase(const WPFloat &value)	{ return value * this->_fromBase; }				//!< Convert from base unit to this unit

	/*** Static Creation Method ***/
	static WCUnitType* TypeFromName(const std::string &name);
};


/***********************************************~***************************************************/


#endif //__UNIT_TYPES_H__

