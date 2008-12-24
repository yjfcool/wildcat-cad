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


#ifndef __LIGHT_SOURCE_H__
#define __LIGHT_SOURCE_H__


/*** Included Header Files ***/
#include <Scene/wscnl.h>


/*** Locally Defined Values ***/
#define LIGHTSOURCE_DEFAULT_AMBIENT				1.0, 1.0, 1.0, 1.0
#define LIGHTSOURCE_DEFAULT_DIFFUSE				1.0, 1.0, 1.0, 1.0
#define LIGHTSOURCE_DEFAULT_SPECULAR			1.0, 1.0, 1.0, 1.0
#define LIGHTSOURCE_DEFAULT_POSITION			0.0, 0.0, 2.0
#define LIGHTSOURCE_DEFAULT_DIRECTION			0.0, 0.0, -1.0


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


class WCLightSource : virtual public WCSerializeableObject {
protected:
	bool										_active;											//!< Active flag
	WCColor										_ambient, _diffuse, _specular;						//!< Light colors
	WCVector4									_position;											//!< Light position
	WCVector4									_direction;											//!< Light direction
	GLuint										*_locations;										//!< Uniform locations
	GLuint										_lastProg;											//!< Last program

public:
	//Constructors and Destructors
	WCLightSource();																				//!< Default constructor
	WCLightSource(const WCColor& ambient, const WCColor& diffuse, const WCColor& specular,			//!< Alternate constructor
												const WCVector4& position, const WCVector4& direction);
	WCLightSource(const WCLightSource& light);														//!< Copy constructor
	WCLightSource(xercesc::DOMNode* node, WCSerialDictionary *dictionary);							//!< Persistance constructor
	virtual ~WCLightSource();																		//!< Default destructor
	
	//Member Access Functions
	inline bool IsActive(void) const			{ return this->_active; }							//!< Get the active flag
	inline void IsActive(const bool status)		{ this->_active = status; }							//!< Set the active flag
	inline WCColor Ambient(void) const			{ return this->_ambient; }							//!< Get the ambient color
	inline void Ambient(const WCColor &color)	{ this->_ambient = color; }							//!< Set the ambient color
	inline WCColor Diffuse(void) const			{ return this->_diffuse; }							//!< Get the diffuse color
	inline void Diffuse(const WCColor &color)	{ this->_diffuse = color; }							//!< Set the diffuse color
	inline WCColor Specular(void) const			{ return this->_specular; }							//!< Get the specular color	
	inline void Specular(const WCColor &color)	{ this->_specular = color; }						//!< Set the specular color
	inline WCVector4 Position(void) const		{ return this->_position; }							//!< Get the position
	inline void Position(const WCVector4& pos)	{ this->_position = pos; }							//!< Set the position
	inline WCVector4 Direction(void) const		{ return this->_direction; }						//!< Get the direction	
	inline void Direction(const WCVector4& dir)	{ this->_direction = dir; }							//!< Set the direction
	
	//Overridden Operators
	virtual WCLightSource& operator=(const WCLightSource& light);									//!< Equals operator
	
	//Inherited Methods
	virtual void Render(GLuint currentProg);														//!< Enable the light
	
	//Serialization and Object Methods
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
};


/***********************************************~***************************************************/


class WCUberLight : virtual public WCLightSource {
protected:
	bool										_barnShaping;
	GLfloat										_seWidth, _seHeight, _seWidthEdge, _seHeightEdge;
	GLfloat										_seRoundness;
	GLfloat										_dsNear, _dsFar, _dsNearEdge, _dsFarEdge;

public:
	//Constructors and Destructors
	WCUberLight();																					//!< Default constructor
	WCUberLight(const WCUberLight& light);															//!< Copy constructor
	virtual ~WCUberLight();																			//!< Default destructor
	
	//Member Access Functions
	//...
	
	//Overridden Operators
	virtual WCUberLight& operator=(const WCUberLight& light);										//!< Equals operator
	
	//Inherited Methods
	virtual void Render(GLuint currentProg);														//!< Enable the light
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__LIGHT_SOURCE_H__

