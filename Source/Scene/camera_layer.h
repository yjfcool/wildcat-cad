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


#ifndef __CAMERA_LAYER_H__
#define __CAMERA_LAYER_H__


/*** Included Header Files ***/
#include "Scene/wscnl.h"
#include "Scene/layer.h"


/*** Local Defines ***/
#define CAMERALAYER_DEFAULT_ROTSENS				200
#define CAMERALAYER_DEFAULT_PANSENS				1
#define CAMERALAYER_DEFAULT_ZOOMSENS			1.5
#define CAMERALAYER_ZOOM_MIN					0.05
#define CAMERALAYER_ZOOM_MAX					100


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


//Camera Modes
#define CAMERALAYER_MODE_ROTATE					0
#define CAMERALAYER_MODE_PAN					1
#define CAMERALAYER_MODE_ZOOM					2


class WCCameraMode {
private:
	unsigned int								_mode;												//!< Mode ID
	WCCameraMode()								{ }													//!< Deny access to default constructor
	WCCameraMode(const unsigned int mode) : _mode(mode)			{ }									//!< Primary constructor
public:
	//Public constructor and destructor
	WCCameraMode(const WCCameraMode &mode) : _mode(mode._mode)	{ }									//!< Copy constructor
	~WCCameraMode()								{ }													//!< Default destructor
	//Static creation methods
	static WCCameraMode Rotate(void)			{ WCCameraMode m(CAMERALAYER_MODE_ROTATE); return m; }//!< Create a default rotate mode object
	static WCCameraMode Pan(void)				{ WCCameraMode m(CAMERALAYER_MODE_PAN);  return m; }//!< Create a pan mode object
	static WCCameraMode Zoom(void)				{ WCCameraMode m(CAMERALAYER_MODE_ZOOM);  return m; }//!< Create a zoom mode object
	
	//Overridden operators
	WCCameraMode& operator=(const WCCameraMode &mode) { this->_mode = mode._mode; return *this; }	//!< Equals operator
	bool operator==(const WCCameraMode &mode)	{ return this->_mode == mode._mode; }				//!< Equality operator
	bool operator!=(const WCCameraMode &mode)	{ return this->_mode != mode._mode; }				//!< Inequality operator
};


/***********************************************~***************************************************/


class WCCameraLayer : public WCLayer {
protected:
	bool										_isMarked;											//!< Is the camera layer active
	WPFloat										_xMark, _yMark;										//!< Location of mouse down event
	WPFloat										_zoom;												//!< Zoom factor
	WPFloat										_panX, _panY;										//!< Pan factors
	WCCameraMode								_mode;												//!< Rotate, pan, or zoom mode
	WPFloat										_rotSensitivity, _zoomSensitivity, _panSensitivity;	//!< Sensitivity factors for each motion
	
private:
	WCCameraLayer();																				//!< Deny access to default constructor
	WCCameraLayer(const WCCameraLayer &layer);														//!< Deny access to copy constructor
	WCCameraLayer& operator=(const WCCameraLayer &layer);											//!< Deny access to equals operator
	
public:
	//Constructors and Destructors
	WCCameraLayer(WCScene *scene);																	//!< Primary constructor
	~WCCameraLayer();																				//!< Default destructor
	
	//General Access Methods
	inline WCCameraMode Mode(void) const		{ return this->_mode; }								//!< Get the camera mode
	inline void Mode(const WCCameraMode mode)	{ this->_mode = mode; }								//!< Set the camera mode
	
	//Inherited Methods
	bool OnMouseMove(const WPFloat x, const WPFloat y);												//!< Track the mouse x and y position
	bool OnMouseDown(const WCMouseButton &button);													//!< Mouse button down event
	bool OnMouseUp(const WCMouseButton &button);													//!< Mouse button up event
	
	//Required Methods
	void Render(WCRenderState *state)			{ }													//!< No camera layer rendering
};


/***********************************************~***************************************************/


#endif //__CAMERA_LAYER_H__

