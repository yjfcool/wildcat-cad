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


#ifndef __NAMED_VIEW_H__
#define __NAMED_VIEW_H__


/*** Included Header Files ***/
#include "Scene/wscnl.h"
#include "Scene/layer.h"


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


class WCNamedView : public WCSerializeableObject {
private:
	std::string									_name;												//!< View name
	bool										_isOrtho;											//!< View type
	bool										_isAuto;											//!< Is the view auto scaling
	WCVector4									_origin;											//!< View origin
	WCVector4									_target;											//!< View target
	WPFloat										_angle;												//!< Projection angle
	WPFloat										_zoom;												//!< Zoom factor
	WPFloat										_panX, _panY;										//!< Pan factors
	WCQuaternion								_quaternion;										//!< Rotation quaternion
	
	//Deny Access
	WCNamedView();																					//!< Deny access to default constructor
public:
	//Constructors and Destructors
	WCNamedView(const std::string name, const WCVector4 &origin, const WCVector4 &target,			//!< Alternative constructor
												bool isOrtho=true, WPFloat angle=0.65);
	WCNamedView(const std::string name, const WCQuaternion &quat, bool isOrtho=true, WPFloat angle=0.65);//!< Other constructor
	WCNamedView(const WCNamedView &view);															//!< Copy constructor
	WCNamedView(xercesc::DOMElement* element, WCSerialDictionary *dictionary);						//!< Persistance constructor
	~WCNamedView();																					//!< Default destructor
	
	//Member Access Methods
	inline std::string Name(void) const			{ return this->_name; }								//!< Get the view name
	inline bool IsAuto(void) const				{ return this->_isAuto; }							//!< Get the auto state
	inline void IsOrtho(const bool type)		{ this->_isOrtho = type; }							//!< Set the view type
	inline bool IsOrtho(void) const				{ return this->_isOrtho; }							//!< Get the view type
	inline void Origin(const WCVector4 &origin)	{ this->_origin = origin; }							//!< Set the view origin
	inline WCVector4 Origin(void) const			{ return this->_origin; }							//!< Get the view origin
	inline void Target(const WCVector4 &target)	{ this->_target = target; }							//!< Set the view target
	inline WCVector4 Target(void) const			{ return this->_target; }							//!< Get the view target
	inline void Angle(const WPFloat &angle)		{ if (!this->_isOrtho) {							//!< Set the view angle
												  this->_angle = angle; this->_isAuto = false; } }
	inline WPFloat Angle(void) const			{ return this->_angle; }							//!< Get the view angle
	inline void Zoom(const WPFloat &zoom)		{ this->_zoom = zoom; this->_isAuto = false; }		//!< Set the view zoom
	inline WPFloat Zoom(void) const				{ return this->_zoom; }								//!< Get the view zoom
	inline void PanX(const WPFloat &x)			{ this->_panX = x; this->_isAuto = false; }			//!< Set the view panX
	inline WPFloat PanX(void) const				{ return this->_panX; }								//!< Get the view panX
	inline void PanY(const WPFloat &y)			{ this->_panY = y; this->_isAuto = false; }			//!< Set the view panY
	inline WPFloat PanY(void) const				{ return this->_panY; }								//!< Get the view panY
	inline void Quaternion(const WCQuaternion &quat){ this->_quaternion = quat; }					//!< Set the view quaternion
	inline WCQuaternion Quaternion(void) const	{ return this->_quaternion; }						//!< Get the view quaternion

	//Overloaded Operators
	WCNamedView& operator=(const WCNamedView &view);												//!< Equals operator

	//Serialization and Object Methods
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object

	/*** Friend Methods ***/
	friend std::ostream& operator<<(std::ostream& out, const WCNamedView &view);					//!< Overloaded output operator		
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__NAMED_VIEW_H__

