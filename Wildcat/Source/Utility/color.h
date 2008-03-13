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


#ifndef __WUTIL_COLOR_H__
#define __WUTIL_COLOR_H__


/*** Included Header Files ***/
#include "Utility/wutil.h"


/*** Locally Defined Values ***/
#define COLOR_DEFAULT							0.0, 0.0, 0.0, 0.0


/***********************************************~***************************************************/



class WCColor {
private:
	GLfloat										_rgba[4];											//!< Four consecutive GLfloat elements
public:
	//Constructors and Destructors
	WCColor();																						//!< Default constructor
	WCColor(const GLfloat &r, const GLfloat &g, const GLfloat &b, const GLfloat &a=1.0);			//!< Alternate constructor
	WCColor(const WPFloat &r, const WPFloat &g, const WPFloat &b, const WPFloat &a=1.0);			//!< Alternate constructor
	WCColor(const WCColor& color);																	//!< Copy constructor
	~WCColor()									{ }													//!< Default destructor
	
	//General Access Methods
	void Set(const GLfloat &r, const GLfloat &g, const GLfloat &b, const GLfloat &a=1.0);			//!< Set the color values
	void Set(const WCColor &color);																	//!< Set using existing color
	void Enable(void) const						{ glColor4fv(this->_rgba); }						//!< Enable this color
	void R(const GLfloat &red)					{ this->_rgba[0] = red; }							//!< Set red channel
	GLfloat R(void) const						{ return this->_rgba[0]; }							//!< Get red channel
	void G(const GLfloat &green)				{ this->_rgba[1] = green; }							//!< Set green channel
	GLfloat G(void) const						{ return this->_rgba[1]; }							//!< Get green channel
	void B(const GLfloat &blue)					{ this->_rgba[2] = blue; }							//!< Set blue channel	
	GLfloat B(void) const						{ return this->_rgba[2]; }							//!< Get blue channel
	void A(const GLfloat &alpha)				{ this->_rgba[3] = alpha; }							//!< Set alpha channel
	GLfloat A(void) const						{ return this->_rgba[3]; }							//!< Get alpha channel			
	
	//Operator overloads
	WCColor& operator=(const WCColor &col);															//!< Equals operator
	bool operator==(const WCColor &col) const;														//!< Equality operator
	bool operator!=(const WCColor &col) const;														//!< Non-equality operator
	
	//Conversion Methods
	void ToElement(xercesc::DOMElement *element);													//!< Serialize the object
	void FromElement(xercesc::DOMElement *element);													//!< Deserialize the object
	
	//Static Creation Methods
	static WCColor Default(void)				{ return WCColor(COLOR_DEFAULT); }					//!< Default - nothing - color
	static WCColor White(void)					{ return WCColor(1.0, 1.0, 1.0, 1.0); }				//!< Return a white color
	static WCColor Red(void)					{ return WCColor(1.0, 0.0, 0.0, 1.0); }				//!< Return a red color
	static WCColor Green(void)					{ return WCColor(0.0, 1.0, 0.0, 1.0); }				//!< Return a green color
	static WCColor Blue(void)					{ return WCColor(0.0, 0.0, 1.0, 1.0); }				//!< Return a blue color
	static WCColor Yellow(void)					{ return WCColor(1.0, 1.0, 0.0, 1.0); }				//!< Return a blue color
	
	//Friend classes
	friend std::ostream& operator<<(std::ostream& out, const WCColor &color);						//!< Overloaded output operator	
};


/***********************************************~***************************************************/


#endif //__WUTIL_COLOR_H__

