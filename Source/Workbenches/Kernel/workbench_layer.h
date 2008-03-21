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


#ifndef __WORKBENCH_LAYER_H__
#define __WORKBENCH_LAYER_H__


/*** Included Headers ***/
#include "Kernel/wftrl.h"


/*** Local Defines ***/
//None


/*** Class Predefines ***/
class WCDrawingMode;


/***********************************************~***************************************************/


class WCWorkbenchLayer : public WCVisualLayer {
private:
	bool										_isActive;											//!< Intercept mouse messages
	WCDrawingMode								*_mode;												//!< DrawingMode object
	//Hidden Constructors
	WCWorkbenchLayer();																				//!< Deny access to default constructor
	WCWorkbenchLayer(const WCWorkbenchLayer &layer);												//!< Deny access to copy constructor
	WCWorkbenchLayer& operator=(const WCWorkbenchLayer &layer);										//!< Deny access to equals operator
	bool operator==(const WCWorkbenchLayer &layer);													//!< Deny access to equality operator
public:
	//Constructors and Destructors
	WCWorkbenchLayer(WCScene *scene, std::string name);												//!< Primary constructor
	virtual ~WCWorkbenchLayer();																	//!< Default destructor

	inline void IsActive(const bool state)		{ this->_isActive = state; }						//!< Set active state
	inline bool IsActive(void) const			{ return this->_isActive; }							//!< Get active state
	inline void DrawingMode(WCDrawingMode *mode){ this->_mode = mode; }								//!< Set the drawingMode

	bool OnMouseMove(const WPFloat x, const WPFloat y);												//!< Track the mouse x and y position
	bool OnMouseDown(const WCMouseButton &button);													//!< Mouse button down event
	bool OnMouseUp(const WCMouseButton &button);													//!< Mouse button up event
	bool OnArrowKeyPress(const WCArrowKey &key);													//!< Arrow key press event
	bool OnReshape(const WPFloat width, const WPFloat height);										//!< Window reshape info
	void Render(WCRenderState *state);																//!< Render the layer
};


/***********************************************~***************************************************/


#endif //__WORKBENCH_LAYER_H__

