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


#ifndef __GEOMETRIC_LINE_H__
#define __GEOMETRIC_LINE_H__


/*** Included Headers ***/
#include "Geometry/wgeol.h"
#include "Geometry/geometric_types.h"


/*** Locally Defined Values ***/
#define GEOMETRICLINE_DEFAULT_EPSILON			0.001


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


class WCGeometricLine : public WCGeometricCurve {
protected:
	WCVector4									_p0, _p1;											//!< Beginning and ending points
	bool										_isInfinite;										//!< Is this intended to be an infinite line?

	//Deny Access
	WCGeometricLine();																				//!< Deny access to default constructor
	WCGeometricLine& operator=(const WCGeometricLine &line);										//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCGeometricLine(const WCVector4 &p0, const WCVector4 &p1);										//!< Primary constructor
	WCGeometricLine(const WCGeometricLine &line);													//!< Copy constructor
	WCGeometricLine(xercesc::DOMElement *element, WCSerialDictionary *dictionary);					//!< Persistance constructor
	~WCGeometricLine();																				//!< Default destructor

	//Member Access Functions
	inline bool IsClosed(void) const			{ return false; }									//!< Get the closed flag
	inline bool IsSelfIntersecting(void) const	{ return false; }									//!< Get the self-intersection flag
	inline bool IsInfinite(void) const			{ return this->_isInfinite; }						//!< Get the infinite flag
	inline void IsInfinite(const bool &state)	{ this->_isInfinite = state; }						//!< Get the infinite state
	inline WCVector4 Begin(void) const			{ return this->_p0; }								//!< Get the beginning point
	void Begin(const WCVector4 &begin);																//!< Set the beginning point
	inline WCVector4 End(void) const			{ return this->_p1; }								//!< Get the ending point
	void End(const WCVector4 &end);																	//!< Set the ending point
	void Set(const WCVector4 &begin, const WCVector4 &end);											//!< Set both points
	inline WCColor Color(void) const			{ return this->_color; }							//!< Get the object color
	void Color(const WCColor &color);																//!< Set the object color
	
	//Required Geometric Methods
	virtual std::list<WPFloat> Intersect(WCGeometricPoint *point, const WPFloat &tol);				//!< Check point intersection
	virtual std::list<WPIntersectRec> Intersect(WCGeometricCurve *curve, const WPFloat &tol);		//!< Check for intersection with curve
	WPFloat Length(const WPFloat &tolerance=GEOMETRICOBJECT_DEFAULT_EPSILON);						//!< Calculate the length of the curve
	WCVector4 Evaluate(const WPFloat &u);															//!< Evaluate curve for a parametric value
	WCVector4 Derivative(const WPFloat &u, const WPUInt &der);										//!< Evaluate derivative at a specific point
	WCRay Tangent(const WPFloat &u);																//!< Get tangent to the curve at U
	std::pair<WCVector4,WPFloat> PointInversion(const WCVector4 &point);							//!< Get closest point on curve

	//Required Inherited Methods
	WCVisualObject* HitTest(const WCRay &ray, const WPFloat &tolerance);							//!< Hit test on the object
	void ApplyTransform(const WCMatrix4 &transform);												//!< Apply a general transform
	void ApplyTranslation(const WCVector4 &translation);											//!< Apply a linear translation
	void Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom);				//!< Render the object
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object

	/*** Non-Member Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCGeometricLine &line);				//!< Overloaded output operator
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__GEOMETRIC_LINE_H__

