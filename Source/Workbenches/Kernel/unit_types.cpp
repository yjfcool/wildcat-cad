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


/*** Included Header Files ***/
#include "Kernel/unit_types.h"


/***********************************************~***************************************************/


#define UNIT_MM_CM								0.1000000000
#define UNIT_MM_DM								0.0100000000
#define UNIT_MM_M								0.0010000000
#define UNIT_MM_DAM								0.0001000000
#define UNIT_MM_HM								0.0000100000
#define UNIT_MM_IN								0.0393700787
#define UNIT_MM_FT								0.0032808399

#define UNIT_CM_MM								10.000000000
#define UNIT_DM_MM								100.00000000
#define UNIT_M_MM								1000.0000000
#define UNIT_DAM_MM								10000.000000
#define UNIT_HM_MM								100000.00000
#define UNIT_IN_MM								25.4
#define UNIT_FT_MM								304.8

#define UNIT_DEG_RAD							0.0174532925
#define UNIT_RAD_DEG							57.2957795


/***********************************************~***************************************************/


std::string UnitDisplay(WCUnitType *type, const WPFloat &value, const WPUInt &precision) {
	//Convert newValue to a string
	std::ostringstream os;
	os.precision(precision);
	os << (value * type->FromBase());
	std::string label = os.str() + type->Symbol();
	//Return the label
	return label;
}


std::string FootDisplay(WCUnitType *type, const WPFloat &value, const WPUInt &precision) {
	//Get feet and inches
	WPFloat feet = value * type->FromBase();
	WPFloat inches = (feet - floor(feet)) * 12.0;
	feet = floor(feet);
	//Convert newValue to a string
	std::ostringstream osFeet, osInches;
	osFeet.precision(1);
	osInches.precision(precision);
	osFeet << floor(feet);
	osInches << inches;
	//If there are feet, display them
	std::string label;
	if (feet > 0.0) label = osFeet.str() + "\' - " + osInches.str() + "\"";
	else label = osInches.str() + "\"";
	//Return the label
	return label;
}



/***********************************************~***************************************************/


struct WCUnitDefinition {
	std::string									_name;
	std::string									_symbol;
	unsigned int								_category;
	WPFloat										_toBaseFactor;
	WPFloat										_fromBaseFactor;
	std::string (*_displayFunc)(WCUnitType *type, const WPFloat &value, const WPUInt &precision);
};


#define UNITTYPE_COUNT							10

WCUnitDefinition _unitTypes[UNITTYPE_COUNT] = {
	/*** Name		Symbol	Category				ToBase			FromBase		DisplayFunc ***/
	"Millimeter",	"mm",	UNITCATEGORY_LENGTH,	1.0,			1.0,			UnitDisplay,		//SI Length
	"Centimeter",	"cm",	UNITCATEGORY_LENGTH,	UNIT_CM_MM,		UNIT_MM_CM,		UnitDisplay,
	"Decimeter",	"dm",	UNITCATEGORY_LENGTH,	UNIT_DM_MM,		UNIT_MM_DM,		UnitDisplay,
	"Meter",		"m",	UNITCATEGORY_LENGTH,	UNIT_M_MM,		UNIT_MM_M,		UnitDisplay,
	"Decameter",	"dam",	UNITCATEGORY_LENGTH,	UNIT_DAM_MM,	UNIT_MM_DAM,	UnitDisplay,
	"Hectometer",	"hm",	UNITCATEGORY_LENGTH,	UNIT_HM_MM,		UNIT_MM_HM,		UnitDisplay,
	"Inch",			"in",	UNITCATEGORY_LENGTH,	UNIT_IN_MM,		UNIT_MM_IN,		UnitDisplay,		//Non-SI Length
	"Foot",			"ft",	UNITCATEGORY_LENGTH,	UNIT_FT_MM,		UNIT_MM_FT,		FootDisplay,
	"Radians",		"rad",	UNITCATEGORY_ANGLE,		1.0,			1.0,			UnitDisplay,		//Angle
	"Degrees",		"deg",	UNITCATEGORY_ANGLE,		UNIT_DEG_RAD,	UNIT_RAD_DEG,	UnitDisplay,
};



/***********************************************~***************************************************/


WCUnitType::WCUnitType(const std::string &name, const std::string &symbol, const WCUnitCategory &category,
	const WPFloat &toBase, const WPFloat &fromBase, 
	std::string (*displayFunc)(WCUnitType *type, const WPFloat &value, const WPUInt &precision)) :
	_name(name), _symbol(symbol), _category(category), _toBase(toBase), _fromBase(fromBase), _displayFunc(displayFunc) {

	//Nothing else for now
}


std::string WCUnitType::DisplayString(const WPFloat &value, const WPUInt &precision) {
	//Make sure display func in not null
	if (this->_displayFunc != NULL)
		return this->_displayFunc(this, value, precision);
	//Otherwise, return empty
	return "";
}


/***********************************************~***************************************************/


WCUnitType* WCUnitType::TypeFromName(const std::string &name) {
	//Look through _unitTypes for name
	for (int index=0; index<UNITTYPE_COUNT; index++) {
		//If there is a name match
		if (_unitTypes[index]._name == name) {
			//Create object
			WCUnitType *type = new WCUnitType(_unitTypes[index]._name, 
											  _unitTypes[index]._symbol, 
											  _unitTypes[index]._category, 
											  _unitTypes[index]._toBaseFactor, 
											  _unitTypes[index]._fromBaseFactor, 
											  _unitTypes[index]._displayFunc);
			//Return it
			return type;
		}
	}
	//Otherwise, no type - return NULL
	return NULL;
}


/***********************************************~***************************************************/

