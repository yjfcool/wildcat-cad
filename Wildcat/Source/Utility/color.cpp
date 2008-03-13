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
#include "Utility/color.h"
#include "Utility/serializeable_object.h"


/***********************************************~***************************************************/


WCColor::WCColor() {
	//Default is white
	this->_rgba[0] = 1.0;
	this->_rgba[1] = 1.0;
	this->_rgba[2] = 1.0;
	this->_rgba[3] = 0.0;
}


WCColor::WCColor(const GLfloat &r, const GLfloat &g, const GLfloat &b, const GLfloat &a) {
	//Set the colors
	this->Set(r, g, b, a);
}

WCColor::WCColor(const WPFloat &r, const WPFloat &g, const WPFloat &b, const WPFloat &a) {
	//Set the colors
	this->Set((GLfloat)r, (GLfloat)g, (GLfloat)b, (GLfloat)a);
}


WCColor::WCColor(const WCColor& color) {
	//Copy all of the components
	memcpy(this->_rgba, color._rgba, 4*sizeof(GLfloat));
}


void WCColor::Set(const GLfloat &r, const GLfloat &g, const GLfloat &b, const GLfloat &a) {
	//Default is white
	this->_rgba[0] = r;
	this->_rgba[1] = g;
	this->_rgba[2] = b;
	this->_rgba[3] = a;
}


void WCColor::Set(const WCColor &color) {
	//Copy the components
	memcpy(this->_rgba, color._rgba, 4*sizeof(GLfloat));
}


WCColor& WCColor::operator=(const WCColor &color) {
	//Copy all color components
	memcpy(this->_rgba, color._rgba, 4*sizeof(GLfloat));
	return *this;
}


bool WCColor::operator==(const WCColor &color) const {
	//Return true if all components are same
	if ((this->_rgba[0] == color._rgba[0]) && 
		(this->_rgba[1] == color._rgba[1]) && 
		(this->_rgba[2] == color._rgba[2]) && 
		(this->_rgba[3] == color._rgba[3])) 
			return true;
	return false;
}


bool WCColor::operator!=(const WCColor &color) const {
	return !(*this == color);
}


void WCColor::ToElement(xercesc::DOMElement *element) {
	//Add r element
	WCSerializeableObject::AddFloatAttrib(element, "r", this->_rgba[0]);
	//Add g element
	WCSerializeableObject::AddFloatAttrib(element, "g", this->_rgba[1]);
	//Add b element
	WCSerializeableObject::AddFloatAttrib(element, "b", this->_rgba[2]);
	//Add a element
	WCSerializeableObject::AddFloatAttrib(element, "a", this->_rgba[3]);
}


void WCColor::FromElement(xercesc::DOMElement *element) {
	//Get r element
	this->_rgba[0] = WCSerializeableObject::GetFloatAttrib(element, "r");
	//Get g element
	this->_rgba[1] = WCSerializeableObject::GetFloatAttrib(element, "g");
	//Get b element
	this->_rgba[2] = WCSerializeableObject::GetFloatAttrib(element, "b");
	//Get a element
	this->_rgba[3] = WCSerializeableObject::GetFloatAttrib(element, "a");
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCColor &color) {
	//Print out basic color info
	out << "WCColor(" << &color << ") ";	
	for(int i=0; i < 4; i++)
		std::cout << color._rgba[i] << "\t";
	//Return the output stream
	return out;
}


/***********************************************~***************************************************/

