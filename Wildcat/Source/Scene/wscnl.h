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


#ifndef __WILDCAT_SCENE_LIBRARY_H__
#define __WILDCAT_SCENE_LIBRARY_H__


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
#include "Geometry/geometry_context.h"


//Screen parameters (will need to adjust for res independent OS's)
#define SCREEN_RESOLUTION_DPI					72.0
#define SCREEN_PIXEL_WIDTH						0.013888888888888888888888


/***********************************************~***************************************************/


//Values for mouse buttons
#define MOUSE_BUTTON_LEFT						0
#define MOUSE_BUTTON_MIDDLE						1
#define MOUSE_BUTTON_RIGHT						2

class WCMouseButton {
private:
	unsigned int								_button;
	WCMouseButton();																				//!< Deny access to default constructor 
	WCMouseButton(int button)					{ this->_button = button; }							//!< Hidden primary constructor
	friend class WCScene;
public:
	//Constructors and Destructors
	WCMouseButton(const WCMouseButton &button) : _button(button._button)	{ }						//!< Copy constructor
	~WCMouseButton()							{ }													//!< Default destructor
	static WCMouseButton Left(void)				{ return WCMouseButton(MOUSE_BUTTON_LEFT); }		//!< Left mouse button
	static WCMouseButton Middle(void)			{ return WCMouseButton(MOUSE_BUTTON_MIDDLE); }		//!< Middle mouse button
	static WCMouseButton Right(void)			{ return WCMouseButton(MOUSE_BUTTON_RIGHT); }		//!< Right mouse button
	
	//Overridder Operators
	WCMouseButton& operator=(const WCMouseButton &button)											//!< Button equation				
												{ this->_button = button._button; return *this; }
	bool operator==(const WCMouseButton &button) const { return this->_button == button._button; }	//!< Equals operator
	bool operator!=(const WCMouseButton &button) const { return this->_button != button._button; }	//!< Inequality operator
};


/***********************************************~***************************************************/


//Values for arrow keys
#define ARROW_KEY_LEFT							0
#define ARROW_KEY_UP							1
#define ARROW_KEY_RIGHT							3
#define ARROW_KEY_DOWN							4

class WCArrowKey {
private:
	unsigned int								_key;
	WCArrowKey();																					//!< Deny access to default constructor
	WCArrowKey(int key)							{ this->_key = key; }								//!< Hidden primary constructor
	friend class WCScene;
public:
	//Constructors and Destructors
	WCArrowKey(const WCArrowKey &key) : _key(key._key)	{ }											//!< Copy constructor
	~WCArrowKey()								{ }													//!< Default destructor
	static WCArrowKey Left(void)				{ return WCArrowKey(ARROW_KEY_LEFT); }				//!< Left mouse button
	static WCArrowKey Up(void)					{ return WCArrowKey(ARROW_KEY_UP); }				//!< Middle mouse button
	static WCArrowKey Right(void)				{ return WCArrowKey(ARROW_KEY_RIGHT); }				//!< Right mouse button
	static WCArrowKey Down(void)				{ return WCArrowKey(ARROW_KEY_DOWN); }				//!< Right mouse button
	
	//Overridder Operators
	WCArrowKey& operator=(const WCArrowKey &key){ this->_key = key._key; return *this; }			//!< Equation
	bool operator==(const WCArrowKey &key) const { return this->_key == key._key; }					//!< Equals operator
	bool operator!=(const WCArrowKey &key) const { return this->_key != key._key; }					//!< Inequality operator
};


/***********************************************~***************************************************/


//Values for placements
#define PLACEMENT_LOWERLEFT						0
#define PLACEMENT_LOWERRIGHT					1
#define PLACEMENT_UPPERLEFT						2
#define PLACEMENT_UPPERRIGHT					3
#define PLACEMENT_CUSTOM						43

class WCPlacement {
private:
	unsigned int								_place;
	WCPlacement();																					//!< Deny access to default constructor 
	WCPlacement(int place)						{ this->_place = place; }							//!< Hidden primary constructor
public:
	//Public constructor and destructor
	WCPlacement(const WCPlacement &place) : _place(place._place)	{ }								//!< Copy constructor
	~WCPlacement()								{ }													//!< Default destructor
	//Static creation methods
	static WCPlacement LowerLeft(void)			{ WCPlacement p(PLACEMENT_LOWERLEFT); return p; }	//!< Create a lower-left placement object
	static WCPlacement LowerRight(void)			{ WCPlacement p(PLACEMENT_LOWERRIGHT); return p; }	//!< Create a lower-right placement object
	static WCPlacement UpperLeft(void)			{ WCPlacement p(PLACEMENT_UPPERLEFT);  return p; }	//!< Create a upper-left placement object
	static WCPlacement UpperRight(void)			{ WCPlacement p(PLACEMENT_UPPERRIGHT);  return p; }	//!< Create a upper-right placement object
	static WCPlacement Custom(void)				{ WCPlacement p(PLACEMENT_CUSTOM);  return p; }		//!< Create a custom placement object
	
	//Overridden operators
	WCPlacement& operator=(const WCPlacement &place) { this->_place = place._place; return *this; }	//!< Equals operator
	bool operator==(const WCPlacement &place) const	{ return this->_place == place._place; }		//!< Equality operator
	bool operator!=(const WCPlacement &place) const	{ return this->_place != place._place; }		//!< Inequality operator
};


/***********************************************~***************************************************/


#endif //__WILDCAT_SCENE_LIBRARY_H__

