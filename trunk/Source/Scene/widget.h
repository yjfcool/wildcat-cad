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


#ifndef __WIDGET_H__
#define __WIDGET_H__


/*** Included Header Files ***/
#include <Scene/wscnl.h>
#include <Scene/user_interface_layer.h>


/*** Locally Defined Values ***/
#define WIDGET_RADIUS				6
#define WIDGET_RADIUS_SEGMENTS		8


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


class WCWidget : virtual public WCObject {
protected:
	WPFloat										_xMin, _yMin, _xMax, _yMax;							//!< 4 corners parameters	
	WPFloat										_width, _height;									//!< Width and height
	bool										_isVisible;											//!< Is the widget visible
	bool										_isDirty;											//!< Is the widget dirty
	bool										_isFocus;											//!< Is the widget the focus of the parent
	WCWidget									*_firstResponder;									//!< The first responder
	WCWidget									*_mouseOver;										//!< The mouse over widget
	WCUserInterfaceLayer						*_layer;											//!< Parent layer
	
private:
	//Deny Access
	WCWidget();																						//!< Deny access to default constructor
	WCWidget(const WCWidget& widget);																//!< Deny access to the copy constructor
	WCWidget(const WCWidget* widget);																//!< Deny access to the other copy constructor
	WCWidget& operator=(const WCWidget& widget);													//!< Deny access to equals operator
public:
	WCWidget(WCUserInterfaceLayer *layer) : ::WCObject(), _xMin(0.0), _yMin(0.0),					//!< Primary constructor
												_xMax(1.0), _yMax(1.0),
												_width(1.0), _height(1.0),
												_isVisible(true), _isDirty(true), 
												_firstResponder(NULL) , _mouseOver(NULL),
												_layer(layer) { }		
	virtual ~WCWidget()							{ }													//!< Default destructor
	
	//Member Access Methods	
	virtual inline WCUserInterfaceLayer* Layer(void){ return this->_layer; }						//!< Get the associated user interface
	virtual inline void IsVisible(const bool state)	{ this->_isVisible = state; }					//!< Set the visibility of the object
	virtual inline bool IsVisible(void) const	{ return this->_isVisible; }						//!< Get the visibility of the object
	virtual inline void MarkDirty(void)			{ this->_isDirty = true; 							//!< Mark the object as dirty
												  this->_layer->MarkDirty(); }
	virtual inline WPFloat XMin(void) const			{ return this->_xMin; }							//!< Return the XMin
	virtual inline WPFloat YMin(void) const			{ return this->_yMin; }							//!< Return the YMin
	virtual inline WPFloat XMax(void) const			{ return this->_xMax; }							//!< Return the XMax
	virtual inline WPFloat YMax(void) const			{ return this->_yMax; }							//!< Return the YMax
	virtual inline WPFloat Width(void) const			{ return this->_width; }					//!< Return the Width
	virtual inline WPFloat Height(void) const			{ return this->_height; }					//!< Return the Height
	virtual inline void SetPlacement(const WPFloat lowerLeft, const WPFloat lowerRight) {			//!< Set the placement of the widget
												  this->_xMin = lowerLeft;
												  this->_yMin = lowerRight;
												  this->_xMax = this->_xMin + this->_width;
												  this->_yMax = this->_yMin + this->_height; }
	virtual inline void SetSize(const WPFloat width, const WPFloat height) {						//!< Set the size of the widget
												  this->_width = width;
												  this->_height = height;
												  this->_xMax = this->_xMin + this->_width;
												  this->_yMax = this->_yMin + this->_height; }
	
	//First Responder Methods
	inline void FirstResponder(WCWidget* resp)	{ this->_firstResponder = resp;}					//!< Set the next responder
	inline WCWidget* FirstResponder(void)		{ return this->_firstResponder; }					//!< Return the next responder
	
	//Hit Testing Method		
	virtual inline bool HitTest(WPFloat x, WPFloat y)	{ if (!this->_isVisible) return false;		//!< Default hit test
												  if ((x >= this->_xMin) && (x <= this->_xMax) &&
													  (y >= this->_yMin) && (y <= this->_yMax)) 
														return true;
												   else return false; }
		
	//Environment Event Calls
	virtual void OnIdle(void)					{ }													//!< Receive message about idle time
	virtual void OnReshape(void)				{ }													//!< Receive message about parent object reshaping
	virtual void OnMouseEnter(const WPFloat x, const WPFloat y)		{ }								//!< Receive message about mouse being over overlay
	virtual void OnMouseExit(const WPFloat x, const WPFloat y)		{ }								//!< Receive message about mouse no longer being over
	virtual void OnMouseDown(const WCMouseButton &button, const WPFloat x, const WPFloat y)	{ }		//!< Receive message about mouse button being pressed
	virtual void OnMouseUp(const WCMouseButton &button, const WPFloat x, const WPFloat y)	{ }		//!< Receive message about mouse button being released
	virtual void OnMouseMove(const WPFloat x, const WPFloat y)		{ }								//!< Receive mouse dragging data
	virtual void OnScroll(const WPFloat &x, const WPFloat &y)		{ }								//!< Receive scroll-wheel movement event
	virtual void OnArrowKeyPress(const WCArrowKey &key)				{ }								//!< Arrow key event
	virtual bool AcceptsFirstResponder(void) const { return true; }									//!< Checks if widget will become first responder
	virtual void OnBecomeFirstResponder(void)	{ }													//!< Receive message notifying of becoming the first responder
	virtual void OnResignFirstResponder(void)	{ }													//!< Receive message about losing first responder status
	
	virtual void Render(void)=0;																	//!< Render the cursor
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__WIDGET_H__

