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


#ifndef __GEOMETRIC_TYPES_H__
#define __GEOMETRIC_TYPES_H__


/*** Included Headers ***/
#include "Geometry/wgeol.h"
#include "Geometry/geometric_intersection.h"


/*** Locally Defined Values ***/
#define GEOMETRICOBJECT_DEFAULT_EPSILON		0.001
#define GEOMETRICCURVE_THICKNESS			1.0


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCRay;
class WCGeometricPoint;


/***********************************************~***************************************************/


class WCGeometricObject : virtual public WCSerializeableObject, virtual public WCVisualObject {
protected:
	bool										_isConstruction;									//!< Is this a construction element
private:
	//Hidden Constructors
	WCGeometricObject& operator=(const WCGeometricObject &object);									//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCGeometricObject() : ::WCSerializeableObject(), ::WCVisualObject(), _isConstruction(false) { }	//!< Default constructor
	WCGeometricObject(xercesc::DOMElement *element, WCSerialDictionary *dictionary);				//!< Persistance constructor
	virtual ~WCGeometricObject()				{ }													//!< Default destructor
	
	//Member Access Methods
	inline bool IsConstruction(void) const		{ return this->_isConstruction; }					//!< Get construction element state
	inline void IsConstruction(const bool &state)	{ this->_isConstruction = state; }				//!< Set construction element state

	//Required Member Overloads
	virtual WCVisualObject* HitTest(const WCRay &ray, const WPFloat &tolerance)=0;					//!< Hit test on the object
	virtual void ApplyTransform(const WCMatrix4 &transform)=0;										//!< Apply a general transform to object
	virtual void ApplyTranslation(const WCVector4 &translation)=0;									//!< Apply a linear translation to object

	//Serialization Method
	virtual xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);//!< Serialize the object	
};


/***********************************************~***************************************************/


class WCGeometricCurve : public WCGeometricObject {
public:
	static WPFloat								DefaultThickness;									//!< Static default curve thickness
protected:
	bool										_isClosed;											//!< Curve closure flag
	bool										_isSelfIntersecting;								//!< Curve self-intersection flag
	WPFloat										_thickness;											//!< Curve thicknes
private:
	//Hidden Constructors
	WCGeometricCurve& operator=(const WCGeometricCurve &curve);										//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCGeometricCurve() : ::WCGeometricObject(), _isClosed(false), _isSelfIntersecting(false),		//!< Default constructor
												_thickness(WCGeometricCurve::DefaultThickness) { }
	WCGeometricCurve(const WCGeometricCurve &curve) : ::WCVisualObject(curve),						//!< Copy constructor
												_isClosed(curve._isClosed),
												_isSelfIntersecting(curve._isSelfIntersecting),
												_thickness(curve._thickness) { }
	WCGeometricCurve(xercesc::DOMElement *element, WCSerialDictionary *dictionary);					//!< Persistance constructor
	virtual ~WCGeometricCurve()					{ }													//!< Default destructor

	//Member Access Functions
	virtual bool IsClosed(void) const			{ return this->_isClosed; }							//!< Get the closed flag
	virtual bool IsSelfIntersecting(void) const { return this->_isSelfIntersecting; }				//!< Get the self-intersection flag
	virtual WPFloat Thickness(void) const		{ return this->_thickness; }						//!< Get the line thickness
	virtual void Thickness(const WPFloat &thick){ this->_thickness = thick; }						//!< Set the line thickness
	
	//Required Member Function
	virtual WPFloat Length(const WPFloat &tolerance=GEOMETRICOBJECT_DEFAULT_EPSILON)=0;				//!< Calculate the length of the curve
	virtual WCVector4 Evaluate(const WPFloat &u)=0;													//!< Evaluate curve for a parametric value
	virtual WCVector4 Derivative(const WPFloat &u, const WPUInt &der)=0;							//!< Evaluate derivative at specific point
	virtual WCRay Tangent(const WPFloat &u)=0;														//!< Get the tangent to the curve at U
	virtual std::pair<WCVector4,WPFloat> PointInversion(const WCVector4 &point)=0;					//!< Get closest point on curve from point

	//Serialization Method
	virtual xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);//!< Serialize the object
};


/***********************************************~***************************************************/


class WCGeometricSurface : public WCGeometricObject {
protected:
	bool										_isClosedU, _isClosedV;								//!< Curve closure flags
	bool										_isSelfIntersecting;								//!< Curve self-intersection flag
private:
	//Hidden Constructors
	WCGeometricSurface& operator=(const WCGeometricSurface &surface);								//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCGeometricSurface() : ::WCGeometricObject(), _isClosedU(false), _isClosedV(false), 			//!< Default constructor
												_isSelfIntersecting(false) { }						//!< Copy constructor
	WCGeometricSurface(const WCGeometricSurface &surface) : ::WCVisualObject(surface),
												_isClosedU(surface._isClosedU),
												_isClosedV(surface._isClosedV),
												_isSelfIntersecting(surface._isSelfIntersecting) { }
	WCGeometricSurface(xercesc::DOMElement *element, WCSerialDictionary *dictionary);				//!< Persistance constructor
	virtual ~WCGeometricSurface()				{ }													//!< Default destructor

	//Member Access Functions
	virtual bool IsClosedU(void) const			{ return this->_isClosedU; }						//!< Get the closed on U flag
	virtual bool IsClosedV(void) const			{ return this->_isClosedV; }						//!< Get the closed on V flag
	virtual bool IsSelfIntersecting(void) const { return this->_isSelfIntersecting; }				//!< Get the self-intersection flag

	//Required Member Functions
	virtual WPFloat Area(const WPFloat &tolerance=GEOMETRICOBJECT_DEFAULT_EPSILON)=0;				//!< Calculate the area of the surface
	virtual WCVector4 Evaluate(const WPFloat &u, const WPFloat &v)=0;								//!< Evaluate the suface at parametric values
	virtual WCVector4 Derivative(const WPFloat &u, const WPUInt &uDer,								//!< Evaluate derivative at specific point
												const WPFloat &v, const WPUInt &vDer)=0;
	virtual WCRay Tangent(const WPFloat &u, const WPFloat &v)=0;									//!< Get the tangent to the curve at U
	virtual std::pair<WCVector4,WCVector4> PointInversion(const WCVector4 &point)=0;				//!< Get closest point on curve from point

	//Serialization Method
	virtual xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);//!< Serialize the object
};


/***********************************************~****************************************************/


}	   // End Wildcat Namespace
#endif //__GEOMETRIC_TYPES_H__

