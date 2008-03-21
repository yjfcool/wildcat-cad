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


#ifndef __QUATERNION_H__
#define __QUATERNION_H__


/*** Included Header Files ***/
#include "Utility/wutil.h"


/*** Local Defines ***/
#define DPI 3.141592653589793238462643
#define D2R 0.01745329251994329576923691
#define R2D 57.29577951308232087679815


/*** Class Predefines ***/
class WCMatrix4;


/***********************************************~***************************************************/


class WCQuaternion {
private:
	WPFloat										_q[4];												//!< Quaternion data array
public:
	//Constructors and Destructors
	WCQuaternion();																					//!< Default constructor
	WCQuaternion(const WPFloat &a, const WPFloat &b, const WPFloat &c, const WPFloat &d,			//!< Primary constructor
												 const bool &euler=false);
	WCQuaternion(const WCQuaternion& quat);															//!< Copy constructor	
	WCQuaternion(const WCQuaternion *quat);															//!< Alternate copy constructor
	WCQuaternion(const WCMatrix4 &mat);																//!< DCM (9 elements) constructor
	~WCQuaternion();																				//!< Default destructor

	//General Access Functions
	WPFloat X(void)	const						{ return this->_q[0]; }								//!< Return the X value
	WPFloat Y(void)	const						{ return this->_q[1]; }								//!< Return the Y value
	WPFloat Z(void)	const						{ return this->_q[2]; }								//!< Return the Z value
	WPFloat T(void)	const						{ return this->_q[3]; }								//!< Return the T value
	inline WCQuaternion& Set(const WPFloat &x, const WPFloat &y, const WPFloat &z, const WPFloat &t) { //!< Set all the elements
												this->_q[0] = x; this->_q[1] = y;
												this->_q[2] = z; this->_q[3] = t; return *this; }	
	
	//Conversion Algorithms
	void FromEulerZXZ(const WPFloat &phi, const WPFloat &theta, const WPFloat &psi);				//!< Load using ZXZ convention
	void FromEulerXYZ(const WPFloat &phi, const WPFloat &theta, const WPFloat &psi);				//!< Load using XYZ convention	
	void FromEulerAxis(const WPFloat &x, const WPFloat &y, const WPFloat &z, const WPFloat &deg);	//!< Load using Euler axis
	WCMatrix4 ToDCM(void);																			//!< Output the DCM matrix (9 elements)
	void ToGLRotation(void);																		//!< Make call to glRotated with values
	void ToElement(xercesc::DOMNode *parent, const std::string &name);								//!< Serialize the object
	void FromElement(xercesc::DOMElement *element);													//!< Deserialize the object


	//Member Functions
	WPFloat InnerProduct(const WCQuaternion &quat);													//!< Inner product between quaternions
	WCQuaternion Slerp(const WCQuaternion &quat, const WPFloat &scalar);							//!< Spherical linear interpolation
	WCQuaternion& Normalize(void);																	//!< Normalize the quaternion

	//Operator Overloads
	WCQuaternion operator+(const WCQuaternion &quat);												//!< Quaternion addition
	WCQuaternion operator-(const WCQuaternion &quat);												//!< Quaternion subtraction
	WCQuaternion operator*(const WPFloat &scalar);													//!< Scalar multiplication
	WCQuaternion operator/(const WPFloat &scalar);													//!< Scalar division
	WCQuaternion operator*(const WCQuaternion &quat);												//!< Quaternion multiplication
	WCQuaternion& operator=(const WCQuaternion &quat);												//!< Quaternion equation
	WCQuaternion& operator+=(const WCQuaternion &quat);												//!< Quaternion plus equals
	bool operator==(const WCQuaternion &quat);														//!< Quaternion equals?
	
	//Friend classes
	friend std::ostream& operator<<(std::ostream& out, const WCQuaternion &quat);					//!< Overloaded output operator	
};


/***********************************************~***************************************************/


#endif //__QUATERNION_H__

