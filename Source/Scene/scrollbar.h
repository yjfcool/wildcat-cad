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


#ifndef __SCROLLBAR_H__
#define __SCROLLBAR_H__


/*** Included Header Files ***/
#include "Scene/wscnl.h"
#include "Scene/layer.h"
#include "Scene/widget.h"


/*** Local Defines ***/
//None


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


class WCScrollbar : public WCWidget {
protected:
	WCWidget									*_parent;											//!< Parent widget
	WPFloat										_extent;											//!< How much can it scroll
	WPFloat										_position;											//!< Current scroll location
	bool										_isScrolling;										//!< Is the slider selected
	GLuint										_vertBuffers[9], _texBuffers[9];					//!< Buffers for vertex and texCoord info
	GLuint										_textures[9];										//!< Texture buffers
	WCVector4									_texSizes[9];										//!< Texture sizes

private:
	//Deny Access
	WCScrollbar();																					//!< Deny access to the default constructor
public:
	WCScrollbar(WCWidget *parent, WPFloat extent, WPFloat position);								//!< Primary constructor
	virtual ~WCScrollbar();																			//!< Default destructor
	
	//Member Access Methods
	virtual inline WPFloat Extent(void) const	{ return this->_extent; }							//!< Get the extent
	virtual void Extent(const WPFloat extent);														//!< Set the extent
	virtual inline WPFloat Position(void) const	{ return this->_position; }							//!< Get the position
	
	//Inherited Member Functions
	virtual void OnMouseDown(const WCMouseButton &button, const WPFloat x, const WPFloat y)=0;		//!< Receive message about mouse button being pressed
	virtual void OnMouseUp(const WCMouseButton &button, const WPFloat x, const WPFloat y)=0;		//!< Receive message about mouse button being released
	virtual void OnMouseMove(const WPFloat x, const WPFloat y)=0;									//!< Receive message about mouse being moved
	virtual void OnScroll(const WPFloat &x, const WPFloat &y)=0;									//!< Receive message about scroll-wheel being moved
	virtual void Render(void)=0;																	//!< Primary render entry point
};


/***********************************************~***************************************************/


class WCVerticalScrollbar : public WCScrollbar {
protected:
	bool										_isSliderSelected, _isDownSelected, _isUpSelected;	//!< Interaction booleans
	WCVector4									_mark;												//!< Slider mark
	
	//Private Methods
	void GenerateBuffers(void);																		//!< Generate the buffers
public:
	WCVerticalScrollbar(WCWidget *parent, const WPFloat &extent, const WPFloat &position);
	~WCVerticalScrollbar();
	
	//Member Access Methods
	inline bool IsSliderSelected(void) const	{ return this->_isSliderSelected; }					//!< Get slider selection status
	
	//Virtual Methods
	virtual void IsVisible(const bool state);														//!< Set visible state
	virtual bool IsVisible(void) const			{ return this->_isVisible; }						//!< Get visible state
	virtual void OnReshape(void);																	//!< Modify placement on resize
	virtual void OnMouseDown(const WCMouseButton &button, const WPFloat x, const WPFloat y);		//!< Receive message about mouse button being pressed
	virtual void OnMouseUp(const WCMouseButton &button, const WPFloat x, const WPFloat y);			//!< Receive message about mouse button being released
	virtual void OnMouseMove(const WPFloat x, const WPFloat y);										//!< Receive message about mouse being moved
	virtual void OnScroll(const WPFloat &x, const WPFloat &y);										//!< Receive message about scroll-wheel being moved
	virtual void Render(void);																		//!< Primary render entry point
};


/***********************************************~***************************************************/


#endif //__SCROLLBAR_H__

