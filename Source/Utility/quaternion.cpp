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
#include "Utility/quaternion.h"
#include "Utility/matrix.h"
#include "Utility/serializeable_object.h"
#include "Utility/log_manager.h"
#include "Utility/assert_exception.h"


/***********************************************~***************************************************/


WCQuaternion::WCQuaternion() {
	//Set the initial q values
	this->_q[0] = 0.0;
	this->_q[1] = 0.0;
	this->_q[2] = 0.0;
	this->_q[3] = 1.0;
}


WCQuaternion::WCQuaternion(const WPFloat &a, const WPFloat &b, const WPFloat &c, const WPFloat &d, const bool &euler) {
	//Are the 4 input parameters a Euler Axis and degree of rotation
	if (euler) {
		//If so, load them
		this->FromEulerAxis(a, b, c, d);
	}
	//Otherwise they are just Quaternion parameters
	else {
		this->_q[0] = a;
		this->_q[1] = b;
		this->_q[2] = c;
		this->_q[3] = d;
	}
}


WCQuaternion::WCQuaternion(const WCQuaternion& quat) {
	//Copy the parameters
	this->_q[0] = quat._q[0];
	this->_q[1] = quat._q[1];
	this->_q[2] = quat._q[2];
	this->_q[3] = quat._q[3];
}


WCQuaternion::WCQuaternion(const WCQuaternion *quat) {
	//Make sure quat is present
	ASSERT(quat);
	//Copy the parameters
	memcpy(this->_q, quat->_q, 4 * sizeof(WPFloat));
}


WCQuaternion::WCQuaternion(const WCMatrix4 &mat) {
	//Calculate the four components (might need possible alternative methods)
	this->_q[3] = 0.5 * sqrt( 1.0 + mat.Get(0,0) + mat.Get(1,1) + mat.Get(2,2) );
	WPFloat div = 1.0 / (4.0 * this->_q[3]);
	this->_q[0] = div * (mat.Get(1,2) - mat.Get(2,1));
	this->_q[1] = div * (mat.Get(2,0) - mat.Get(0,2));
	this->_q[2] = div * (mat.Get(0,1) - mat.Get(1,0));
}


WCQuaternion::~WCQuaternion() {
	//Nothing to do for now
}


void WCQuaternion::FromEulerZXZ(const WPFloat &phi, const WPFloat &theta, const WPFloat &psi) {
	//Make sure to convert from degrees (phi, theta, psi) to radians
	WPFloat negPP = (phi - psi) / 2.0 * D2R;
	WPFloat posPP = (phi + psi) / 2.0 * D2R;
	WPFloat halfT = theta / 2.0 * D2R;
	WPFloat tmp = sin(halfT);
	
	//Calculate the new values of the quaternion
	this->_q[0] = -cos(negPP) * tmp;
	this->_q[1] = sin(negPP) * tmp;
	tmp = cos(halfT);
	this->_q[2] = -sin(posPP) * tmp;
	this->_q[3] = cos(posPP) * tmp;	
}


void WCQuaternion::FromEulerXYZ(const WPFloat &phi, const WPFloat &theta, const WPFloat &psi) {
}


void WCQuaternion::FromEulerAxis(const WPFloat &x, const WPFloat &y, const WPFloat &z, const WPFloat &deg) {
	//Make sure to convert from degrees to radians
	WPFloat rad = deg * D2R / 2.0;
	WPFloat stHalf = sin(rad);
	//Calculate the new values of the quaternion
	this->_q[0] = x * stHalf;
	this->_q[1] = y * stHalf;
	this->_q[2] = z * stHalf;
	this->_q[3] = cos(rad);	
}


WCMatrix4 WCQuaternion::ToDCM(void) {
	WCMatrix4 matrix;
	
	//Set up all of the needed variables
	WPFloat q12 = this->_q[0] * this->_q[0];
	WPFloat q22 = this->_q[1] * this->_q[1];
	WPFloat q32 = this->_q[2] * this->_q[2];
	WPFloat q42 = this->_q[3] * this->_q[3];
	WPFloat v = q42 - q12 - q22 - q32;
	WPFloat q1q2 = this->_q[0] * this->_q[1];
	WPFloat q1q3 = this->_q[0] * this->_q[2];
	WPFloat q1q4 = this->_q[0] * this->_q[3];
	WPFloat q2q3 = this->_q[1] * this->_q[2];
	WPFloat q2q4 = this->_q[1] * this->_q[3];
	WPFloat q3q4 = this->_q[2] * this->_q[3];
	
	//Calculate matrix values
	matrix.Set(0,0, v + 2.0 * q12		);
	matrix.Set(1,0, 2.0 * (q1q2 + q3q4)	);
	matrix.Set(2,0, 2.0 * (q1q3 - q2q4)	);
	matrix.Set(3,0, 0.0					);
	matrix.Set(0,1, 2.0 * (q1q2 - q3q4)	);
	matrix.Set(1,1, v + 2.0 * q22		);
	matrix.Set(2,1, 2.0 * (q2q3 + q1q4)	);
	matrix.Set(3,1, 0.0					);
	matrix.Set(0,2, 2.0 * (q1q3 + q2q4)	);
	matrix.Set(1,2, 2.0 * (q2q3 - q1q4)	);
	matrix.Set(2,2, v + 2.0 * q32		);
	matrix.Set(3,2, 0.0					);
	matrix.Set(0,3, 0.0					);
	matrix.Set(1,3, 0.0					);
	matrix.Set(2,3, 0.0					);	
	matrix.Set(3,3, 1.0 );
	//Return the new matrix
	return matrix;
}


void WCQuaternion::ToGLRotation(void) {
	//Calculate the norm of the imaginary parts
	WPFloat norm = sqrt( (this->_q[0] * this->_q[0]) + (this->_q[1] * this->_q[1]) + (this->_q[2] * this->_q[2]) );
	//If the norm == 0 then default to a non-rotation
	if (norm != 0.0) {
		//Call glRotated to implement the rotation
		glRotated(2.0 * acos(this->_q[3])*R2D,			// Make sure to convert from radians to degrees
			  this->_q[0] / norm,						// X-axis value
			  this->_q[1] / norm,						// Y-axis value
			  this->_q[2] / norm);						// Z-axis value
	}
}


xercesc::DOMElement* WCQuaternion::ToElement(xercesc::DOMNode *parent, const std::string &name) {
	//Make sure parent is present
	ASSERT(parent);
	//Name the node
	XMLCh* xmlString = xmlString = xercesc::XMLString::transcode(name.c_str());
	//Create the node in the document
	xercesc::DOMElement* child = parent->getOwnerDocument()->createElement(xmlString);
	//Make sure to release the name
	xercesc::XMLString::release(&xmlString);

	//Add i element
	WCSerializeableObject::AddFloatAttrib(child, "i", this->_q[0]);
	//Add j element
	WCSerializeableObject::AddFloatAttrib(child, "j", this->_q[1]);
	//Add k element
	WCSerializeableObject::AddFloatAttrib(child, "k", this->_q[2]);
	//Add l element
	WCSerializeableObject::AddFloatAttrib(child, "l", this->_q[3]);

	//Append child to parent
	parent->appendChild(child);
	//Return the child
	return child;
}


void WCQuaternion::FromElement(xercesc::DOMElement *element) {
	//Make sure element is present
	ASSERT(element);
	//Get i element
	this->_q[0] = WCSerializeableObject::GetFloatAttrib(element, "i");
	//Get j element
	this->_q[1] = WCSerializeableObject::GetFloatAttrib(element, "j");
	//Get k element
	this->_q[2] = WCSerializeableObject::GetFloatAttrib(element, "k");
	//Get l element
	this->_q[3] = WCSerializeableObject::GetFloatAttrib(element, "l");
}


WPFloat WCQuaternion::InnerProduct(const WCQuaternion &quat) {
	//Calculate the inner product
	return (this->_q[0] * quat._q[0]) + (this->_q[1] * quat._q[1]) + 
		   (this->_q[2] * quat._q[2]) + (this->_q[3] * quat._q[3]);
}


WCQuaternion WCQuaternion::Slerp(const WCQuaternion &quat, const WPFloat &scalar) {
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCQuaternion::Slerp Error - Not yet implemented.");
	WCQuaternion a(quat);
	return a;
}


WCQuaternion& WCQuaternion::Normalize(void) {
	//Find the size of the quaternion
	WPFloat magnitude = sqrt( (this->_q[0] * this->_q[0]) + (this->_q[0] * this->_q[0]) +
							  (this->_q[0] * this->_q[0]) + (this->_q[0] * this->_q[0]) );
	//Divide the quaternion
	this->_q[0] = this->_q[0] / magnitude;
	this->_q[1] = this->_q[1] / magnitude;
	this->_q[2] = this->_q[2] / magnitude;
	this->_q[3] = this->_q[3] / magnitude;
	return *this;
}


WCQuaternion WCQuaternion::operator+(const WCQuaternion &quat) {
	//Piecewise add the components together
	WCQuaternion newQuat(this->_q[0] + quat._q[0], this->_q[1] + quat._q[1], this->_q[2] + quat._q[2], this->_q[3] + quat._q[3]); 
	return newQuat;
}


WCQuaternion WCQuaternion::operator-(const WCQuaternion &quat) {
	//Piecewise subtract the components
	WCQuaternion newQuat(this->_q[0] - quat._q[0], this->_q[1] - quat._q[1], this->_q[2] - quat._q[2], this->_q[3] - quat._q[3]); 
	return newQuat;
}


WCQuaternion WCQuaternion::operator*(const WPFloat &scalar) {
	//Calculate each of the new components
	WPFloat a0 = this->_q[0] * scalar;
	WPFloat a1 = this->_q[1] * scalar;
	WPFloat a2 = this->_q[2] * scalar;
	WPFloat a3 = this->_q[3] * scalar;
	WCQuaternion newQuat(a0, a1, a2, a3);
	return newQuat;
}


WCQuaternion WCQuaternion::operator/(const WPFloat &scalar) {
	//Calculate each of the new components
	WPFloat a0 = this->_q[0] / scalar;
	WPFloat a1 = this->_q[1] / scalar;
	WPFloat a2 = this->_q[2] / scalar;
	WPFloat a3 = this->_q[3] / scalar;
	WCQuaternion newQuat(a0, a1, a2, a3);
	return newQuat;
}


WCQuaternion WCQuaternion::operator*(const WCQuaternion &quat) {
	//Calculate each of the new components
	WPFloat a1 = (this->_q[3] * quat._q[0]) + (this->_q[0] * quat._q[3]) + (this->_q[1] * quat._q[2]) - (this->_q[2] * quat._q[1]);
	WPFloat a2 = (this->_q[3] * quat._q[1]) - (this->_q[0] * quat._q[2]) + (this->_q[1] * quat._q[3]) + (this->_q[2] * quat._q[0]);
	WPFloat a3 = (this->_q[3] * quat._q[2]) + (this->_q[0] * quat._q[1]) - (this->_q[1] * quat._q[0]) + (this->_q[2] * quat._q[3]);
	WPFloat a4 = (this->_q[3] * quat._q[3]) - (this->_q[0] * quat._q[0]) - (this->_q[1] * quat._q[1]) - (this->_q[2] * quat._q[2]);
	WCQuaternion newQuat(a1, a2, a3, a4);
	return newQuat;
}


WCQuaternion& WCQuaternion::operator=(const WCQuaternion &quat) {
	//Copy all parameters
	this->_q[0] = quat._q[0];
	this->_q[1] = quat._q[1];
	this->_q[2] = quat._q[2];
	this->_q[3] = quat._q[3];
	//Return the current object
	return *this;
}


WCQuaternion& WCQuaternion::operator+=(const WCQuaternion &quat) {
	//Copy all parameters
	this->_q[0] += quat._q[0];
	this->_q[1] += quat._q[1];
	this->_q[2] += quat._q[2];
	this->_q[3] += quat._q[3];
	//Return the current object
	return *this;
}


bool WCQuaternion::operator==(const WCQuaternion &quat) {
	//If any parameters are not equal then return false
	if (this->_q[0] != quat._q[0]) return false;
	if (this->_q[1] != quat._q[1]) return false;
	if (this->_q[2] != quat._q[2]) return false;
	if (this->_q[3] != quat._q[3]) return false;
	//Otherwise return true
	return true;
}


/***********************************************~***************************************************/


std::ostream& __WILDCAT_NAMESPACE__::operator<<(std::ostream& out, const WCQuaternion &quat) {
	//Print out basic surface info
	out << "Quaternion(" << &quat << ") ";	
	for(int i=0; i < 4; i++) std::cout << quat._q[i] << "\t";
	//Return the output stream
	return out;
}


/***********************************************~***************************************************/

