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


#ifndef __POINT_H__
#define __POINT_H__


/*** Included Header Files ***/
#include "Geometry/wgeol.h"
#include "Geometry/geometric_types.h"


/*** Locally Defined Values ***/
#define POINT_POINTSIZE							2.0
#define POINT_FLOATS_PER_VERTEX					4
#define POINT_HITTEST_RAYCAST					100
#define POINT_EQUALITY_EPSILON					0.001
#define POINT_BOUNDS_BOUNDARY					0.1


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCAlignedBoundingBox; 


/***********************************************~***************************************************/


class WCGeometricPoint : public WCGeometricObject {
protected:
	WPFloat										_size;												//!< Size of the point
	WCVector4									_data;												//!< Data vector for the point
private:
	//Hidden Constructors
	WCGeometricPoint();																				//!< Deny access to default constructor
public:
	//Constructors and Destructors
	WCGeometricPoint(const WPFloat &x, const WPFloat &y, const WPFloat &z, const WPFloat &w=1.0);	//!< Primary constructor
	WCGeometricPoint(const WCVector4 &vec);															//!< Vector constructor
	WCGeometricPoint(const WCGeometricPoint &point);												//!< Copy constructor
	WCGeometricPoint(const WPUInt &size, const WPFloat *data);										//!< Pointer constructor
	WCGeometricPoint(xercesc::DOMElement *element, WCSerialDictionary *dictionary);					//!< Persistance constructor
	~WCGeometricPoint();																			//!< Default destructor
	
	//Member Access Functions
	void Set(const WPFloat &x, const WPFloat &y, const WPFloat &z, const WPFloat &w=1.0);			//!< Set the point's values
	void Set(const WCVector4 &vector);																//!< Set the point's values using a vector
	inline WCVector4 Data(void) const			{ return this->_data; }								//!< Return the data vector
	inline WPFloat X(void)	const				{ return this->_data.I(); }							//!< Return the value of X
	void X(const WPFloat &x);																		//!< Set the value of X
	inline WPFloat Y(void)	const 				{ return this->_data.J(); }							//!< Return the value of Y
	void Y(const WPFloat &y);																		//!< Set the value of Y
	inline WPFloat Z(void)	const 				{ return this->_data.K(); }							//!< Return the value of Z
	void Z(const WPFloat &z);																		//!< Set the value of Z
	inline WPFloat W(void)	const 				{ return this->_data.L(); }							//!< Return the value of W
	void W(const WPFloat &w);																		//!< Set the value of W
	inline void Size(const WPFloat &size)		{ this->_size = size; }								//!< Set the rendering size
	inline WPFloat Size(void) const				{ return this->_size; }								//!< Get the rendering size
	inline WCColor Color(void) const			{ return this->_color; }							//!< Get the object color
	void Color(const WCColor &color);																//!< Set the object color
	
	//Inherited Member Functions
	WCVisualObject* HitTest(const WCRay &ray, const WPFloat &tolerance);							//!< Hit test with a ray	
	void ApplyTransform(const WCMatrix4 &transform);												//!< Apply transform to the point
	void ApplyTranslation(const WCVector4 &translation);											//!< Apply a linear translation to the object
	void Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom);				//!< Render the object
	//Original Member Functions
	bool Intersect(WCGeometricPoint &point, const WPFloat &tolerance=POINT_EQUALITY_EPSILON);		//!< Check for intersection with point

	//Overridden Operators
	WCGeometricPoint operator+(const WCGeometricPoint &point);										//!< Point addition
	WCGeometricPoint operator-(const WCGeometricPoint &point);										//!< Point subtraction
	WCGeometricPoint operator*(const WPFloat &scalar);												//!< Scalar multiplication
	WCGeometricPoint operator/(const WPFloat &scalar);												//!< Scalar division
	WCGeometricPoint& operator=(const WCGeometricPoint &point);										//!< Point equation
	bool operator==(const WCGeometricPoint &point);													//!< Point equivalence
	
	//Serialization and Object Methods
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object

	/*** Static Methods ***/
	static std::vector<WCGeometricPoint*> VectorFromArray(const WPUInt &count, const WPFloat *data);//!< Create a vector from an array of points
	
	/*** Non-Member Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCGeometricPoint &point);				//!< Overloaded output operator
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__POINT_H__

