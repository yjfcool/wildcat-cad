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


#ifndef __BOUNDING_OBJECT_H__
#define __BOUNDING_OBJECT_H__


/*** Included Header Files ***/
#include "Utility/wutil.h"
#include "Utility/vector.h"
#include "Utility/matrix.h"


/*** Locally Defined Values ***/
#define BOUNDINGOBJECT_MIN_DIM					0.25
#define BOUNDINGOBJECT_MIN_DIM2					0.125		// MIN_DIM / 2.0


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCVector4;
class WCGeometricPoint;
class WCControlPoint;


/***********************************************~***************************************************/


class WCBoundingObject {
protected:
	bool										_isVisible;											//!< Visibility flag
	bool										_isDirty;											//!< Dirty flag
public:
	//Constructors and Destructors
	WCBoundingObject() : _isVisible(false), _isDirty(false)	{ }										//!< Default constructor
	WCBoundingObject(const WCBoundingObject&obj) : _isVisible(false), _isDirty(true) { }			//!< Copy constructor
	virtual ~WCBoundingObject()					{ }													//!< Default destructor
	
	//Member Access Methods
	virtual void IsVisible(const bool status)	{ this->_isVisible = status; }						//!< Set the visible flag
	virtual bool IsVisible(void) const			{ return this->_isVisible; }						//!< Get the visible flag
	
	//Required Member Overloads	
	virtual void Render(void)=0;																	//!< Required render method
	virtual void Set(const std::vector<WCGeometricPoint*> &points)=0;								//!< Required point set
	virtual void Set(const std::vector<WCVector4> &vectors)=0;										//!< Required vector set
	virtual void Set(WCVector4 *vectors, const WPUInt &size)=0;										//!< Required array set
	virtual void Set(const GLuint &buffer, const WPUInt &size)=0;									//!< Required VBO set
	
	//Operator Overloads
	WCBoundingObject& operator=(const WCBoundingObject &obj) {										//!< Equals operator
												  this->_isVisible = obj._isVisible;
												  this->_isDirty = true; return *this; }
};


/***********************************************~***************************************************/


class WCAlignedBoundingBox: public WCBoundingObject {
protected:
	GLfloat										_xMin, _xMax;										//!< Bounds values for x dimension
	GLfloat										_yMin, _yMax;										//!< Bounds values for y dimension
	GLfloat										_zMin, _zMax;										//!< Bounds values for y dimension
	GLuint										_buffer;											//!< Vertex buffer
	void GenerateVBO(void);																			//!< Generate the vertex buffer
	
public:
	//Constructors and Destructors
	WCAlignedBoundingBox();																			//!< Default constructor
	WCAlignedBoundingBox(const WCAlignedBoundingBox &box);											//!< Copy constructor	
	WCAlignedBoundingBox(const GLfloat &xMin, const GLfloat &xMax, const GLfloat &yMin,				//!< Default constructor
												const GLfloat &yMax, const GLfloat &zMin, const GLfloat &zMax);
	WCAlignedBoundingBox(const std::vector<WCGeometricPoint*> &points);								//!< Alternate point collection constructor
	WCAlignedBoundingBox(const std::vector<WCVector4> &vectors);									//!< Alternate vector of vectors constructor
	WCAlignedBoundingBox(WCVector4 *vectors, const WPUInt &count);									//!< Alternate vector array constructor
	WCAlignedBoundingBox(const GLuint &buffer, const WPUInt &size);									//!< Alternate VBO constructor
	~WCAlignedBoundingBox()						{ glDeleteBuffers(1, &this->_buffer); }				//!< Default destructor
	
	//Axis Max/Min Values
	inline WPFloat XMin(void) const				{ return this->_xMin; }								//!< X-axis min value
	inline WPFloat XMax(void) const				{ return this->_xMax; }								//!< X-axis max value
	inline WPFloat YMin(void) const				{ return this->_yMin; }								//!< Y-axis min value
	inline WPFloat YMax(void) const				{ return this->_yMax; }								//!< Y-axis max value
	inline WPFloat ZMin(void) const				{ return this->_zMin; }								//!< Z-axis min value
	inline WPFloat ZMax(void) const				{ return this->_zMax; }								//!< Z-axis max value
	
	//Conversion Routines
	WCMatrix ToMatrix(void);																		//!< Convert to a 4x8 matrix
		
	//Inherited Methods
	void Render(void);																				//!< Render the ABB
	void Set(const std::vector<WCControlPoint*> &controlPoints);									//!< Required cp set
	void Set(const std::vector<WCGeometricPoint*> &points);											//!< Required point set
	void Set(const std::vector<WCVector4> &vectors);												//!< Required vector set
	void Set(WCVector4 *vectors, const WPUInt &size);												//!< Required array set
	void Set(const GLuint &buffer, const WPUInt &size);												//!< Required VBO set

	//Operator Overloads
	WCAlignedBoundingBox& operator+=(const WCAlignedBoundingBox &obj);								//!< Operator plus equals
	WCAlignedBoundingBox& operator=(const WCAlignedBoundingBox &obj);								//!< Operator equality

	//Friend classes
	friend std::ostream& operator<<(std::ostream& out, const WCAlignedBoundingBox &box);			//!< Overloaded output operator	
};


/***********************************************~***************************************************/


class WCOrientedBoundingBox: public WCBoundingObject {
protected:
	WCMatrix4									_bounds;											//!< Bounds matrix
public:
	//Constructors and Destructors
	WCOrientedBoundingBox();																		//!< Default constructor
	WCOrientedBoundingBox(const std::vector<WCGeometricPoint*> &points);							//!< Alternate point collection constructor
	WCOrientedBoundingBox(const std::vector<WCVector4> &vectors);									//!< Alternate vector of vectors constructor
	WCOrientedBoundingBox(WCVector4 *vectors, const WPUInt &count);									//!< Alternate vector array constructor
	WCOrientedBoundingBox(const GLuint &buffer, const WPUInt &size);								//!< Alternate VBO constructor
	~WCOrientedBoundingBox()					{ }													//!< Default destructor

	//Inherited Methods
	void Render(void);																				//!< Render the OBB
	void Set(const std::vector<WCGeometricPoint*> &points);											//!< Required point set
	void Set(const std::vector<WCVector4> &vectors);												//!< Required vector set
	void Set(WCVector4 *vectors, const WPUInt &size);												//!< Required array set
	void Set(const GLuint &buffer, const WPUInt &size);												//!< Required VBO set
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__BOUNDING_OBJECT_H__

