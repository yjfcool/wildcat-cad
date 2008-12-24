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


#ifndef __VISUAL_OBJECT_H__
#define __VISUAL_OBJECT_H__


/*** Included Headers ***/
#include <Utility/object.h>
#include <Utility/bounding_object.h>


/*** Locally Defined Values ***/
#define VISUALOBJECT_DEFAULT_COLOR				1.0, 1.0, 1.0, 1.0


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


class WCVisualObject : virtual public WCObject {
private:
	bool										_isVisualDirty;										//!< Dirty flag
protected:
	bool										_isVisible;											//!< Visible flag
	WCAlignedBoundingBox						*_bounds;											//!< Bounding box
	GLuint										_renderProg;										//!< Program for rendering the object
	WCColor										_color;												//!< Colors for the object
public:
	//Constructors and Destructors
	WCVisualObject() : _isVisualDirty(true), _isVisible(true), _bounds(NULL),						//!< Default constructor
												_renderProg(0), _color(VISUALOBJECT_DEFAULT_COLOR) { }	
	WCVisualObject(const WCVisualObject &obj) : _isVisualDirty(true),	_isVisible(obj._isVisible),	//!< Copy constructor
												_bounds(NULL), _renderProg(obj._renderProg), _color(obj._color) { }
	virtual ~WCVisualObject()					{ if (this->_bounds != NULL) delete this->_bounds; }//!< Default destructor
		
	//Member Access Functions
	virtual inline void IsVisualDirty(const bool &status){ this->_isVisualDirty = status; }			//!< Set the dirty flag
	virtual inline bool IsVisualDirty(void) const		{ return this->_isVisualDirty; }			//!< Get the dirty flag
	virtual inline void IsVisible(const bool &status)	{ this->_isVisible = status; }				//!< Set the visible flag
	virtual inline bool IsVisible(void) const			{ return this->_isVisible; }				//!< Get the visible flag
	virtual inline WCAlignedBoundingBox BoundingBox(void) {											//!< Get the current bounding box
														if (this->_bounds == NULL) return WCAlignedBoundingBox();
														else return *this->_bounds; }
	virtual inline void RenderProgram(const GLuint &prog){ this->_renderProg = prog; }				//!< Set the rendering prog
	virtual inline GLuint RenderProgram(void) const		{ return this->_renderProg; }				//!< Get the rendering prog
	virtual inline void Color(const WCColor &color)		{ this->_color = color; }					//!< Set the object color
	virtual inline WCColor Color(void) const			{ return this->_color; }					//!< Get the object color
	
	//Required Member Overloads
	virtual void Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom)=0;	//!< Render the object
};


/***********************************************~****************************************************/


}	   // End Wildcat Namespace
#endif //__VISUAL_OBJECT_H__

