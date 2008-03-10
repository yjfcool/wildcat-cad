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


#ifndef __RAY_H__
#define __RAY_H__


/*** Included Header Files ***/
#include "Geometry/wgeol.h"
#include "Geometry/geometric_types.h"


/*** Local Defines ***/
//None


/***********************************************~***************************************************/


class WCRay : public WCGeometricObject {
protected:
	WCVector4					_base;																//!< Originating point
	WCVector4					_direction;															//!< Direction of the ray

	//Deny Access
	WCRay();																						//!< Deny access to default constructor
public:
	//Constructors and destructors
	WCRay(const WCVector4 &base, const WCVector4 &direction);										//!< Primary constructor
	WCRay(const WCRay &ray);																		//!< Copy constructor
	WCRay(xercesc::DOMElement *element, WCSerialDictionary *dictionary);							//!< Persistance constructor
	~WCRay();																						//!< Default destructor
	
	//Member Access Functions
	void Base(const WCVector4 &base);																//!< Set the base vector of the ray
	WCVector4 Base(void) const					{ return this->_base; }								//!< Get the base vector of the ray
	void Direction(const WCVector4 &direction);														//!< Set the direction vector of the ray
	WCVector4 Direction(void) const				{ return this->_direction; }						//!< Get the direction vector of the ray

	//Inherited Member Functions
	WCVisualObject* HitTest(const WCRay &ray, const WPFloat &tolerance)	{ return NULL; }			//!< Hit test with a ray	
	void ApplyTransform(const WCMatrix4 &transform);												//!< Apply a transform to the ray
	void ApplyTranslation(const WCVector4 &translation);											//!< Apply a linear translation to the object
	void Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom);				//!< Render the object
	
	//Original Member Functions
	static WCRay FromScreenCoords(const int &x, const int &y, const int &width, const int &height,	//!< Generate ray from onscreen location
												const GLfloat &frontZ, const GLfloat &backZ);								
	
	//Operator Overloads
	WCRay& operator=(const WCRay &ray);																//!< Equals operator
	bool operator==(const WCRay &ray);																//!< Equivalence operator
	
	//Serialization and Object Methods
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
	
	/*** Non-Member Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCRay &ray);							//!< Overloaded output operator
};


/***********************************************~***************************************************/


#endif //__RAY_H__

