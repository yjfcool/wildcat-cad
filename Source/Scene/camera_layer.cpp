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
#include "Scene/camera_layer.h"
#include "Scene/camera.h"


/***********************************************~***************************************************/


WCCameraLayer::WCCameraLayer(WCScene *scene) : 
	::WCLayer(scene, "CameraLayer"), 
	_isMarked(false), _xMark(0), _yMark(0), _zoom(1), _panX(0), _panY(0), 
	_mode(WCCameraMode::Rotate()),
	_rotSensitivity(CAMERALAYER_DEFAULT_ROTSENS), 
	_zoomSensitivity(CAMERALAYER_DEFAULT_ZOOMSENS), 
	_panSensitivity(CAMERALAYER_DEFAULT_PANSENS) {
	//Nothing else to do for now
}


WCCameraLayer::~WCCameraLayer() {
	//Nothing to do for now
}
	

bool WCCameraLayer::OnMouseMove(const WPFloat x, const WPFloat y) {
	if (this->_isMarked) {
		//Handle the movement based on camera layer mode
		if (this->_mode == WCCameraMode::Rotate()) {
			WPFloat min = this->_scene->Width() < this->_scene->Height() ? this->_scene->Width() : this->_scene->Height();
			min *= 0.5f;
			WCVector4 a(this->_xMark, this->_yMark, 0.0, 0.0);
			WCVector4 b(x, y, 0.0, 0.0);
			a /= min;
			b /= min;
			a.K( pow(2.0, -0.5 * a.Magnitude()) );
			a.Normalize(true);
			b.K( pow(2.0, -0.5 * b.Magnitude()) );
			b.Normalize(true);
			WCVector4 axis = a.CrossProduct(b).Normalize(true);
			WPFloat angle = acos( a.DotProduct(b) );
			WCQuaternion incr(axis.I(), axis.J(), axis.K(), angle * this->_rotSensitivity, true);
//			incr = incr * this->_scene->ActiveCamera()->Quaternion();
//			incr.Normalize();
//			this->_scene->ActiveCamera()->Quaternion( incr );
			this->_scene->ActiveCamera()->Quaternion( incr * this->_scene->ActiveCamera()->Quaternion() );
			this->_xMark = x;
			this->_yMark = y;
		}
		else if (this->_mode == WCCameraMode::Pan()) {
			WPFloat xDiff = (x - this->_xMark) * this->_panSensitivity;
			WPFloat yDiff = (y - this->_yMark) * this->_panSensitivity;		
			this->_scene->ActiveCamera()->Pan(this->_panX + xDiff, this->_panY + yDiff);
		}
		else if (this->_mode == WCCameraMode::Zoom()) {
			//Calculate new zo0m factor
//			WPFloat oldZoom = this->_scene->ActiveCamera()->Zoom();
			WPFloat newZoom = this->_zoom - (this->_yMark - y) * this->_zoomSensitivity;
			if (newZoom > CAMERALAYER_ZOOM_MAX) newZoom = CAMERALAYER_ZOOM_MAX;
			if (newZoom < CAMERALAYER_ZOOM_MIN) newZoom = CAMERALAYER_ZOOM_MIN;
			this->_scene->ActiveCamera()->Zoom(newZoom);
			//Determine pan offset to maintain center
//			WPFloat panX = this->_scene->ActiveCamera()->PanX();
//			WPFloat panY = this->_scene->ActiveCamera()->PanY();
//			std::cout << "X: " << panX << ", Y: " << panY << std::endl;
//			std::cout << "Diff: " << oldZoom-newZoom << std::endl;
//			panX = panX + (newZoom - oldZoom);
//			panY = panY + (newZoom - oldZoom);
//			std::cout << "X: " << panX << ", Y: " << panY << std::endl;
//			this->_scene->ActiveCamera()->Pan(panX, panY);
		}
		else {
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCCameraLayer::OnMouseMove - Invalid Camera Mode.");
		}
		//Mark the layer as dirty
		this->_isDirty = true;
		return true;
	}
	return false;
}


bool WCCameraLayer::OnMouseDown(const WCMouseButton &button) {
	//Mark the layer as active
	this->_isMarked = true;
	//Gather current information
	this->_xMark = this->_scene->MouseX();
	this->_yMark = this->_scene->MouseY();
	this->_panX = this->_scene->ActiveCamera()->PanX();
	this->_panY = this->_scene->ActiveCamera()->PanY();
	this->_zoom = this->_scene->ActiveCamera()->Zoom();
	//Set this layer as FirstResponder
	this->_scene->FirstResponder(this);
	return true;
}


bool WCCameraLayer::OnMouseUp(const WCMouseButton &button) {
	//Mark the layer as no longer active
	this->_isMarked = false;
	return true;
}


/***********************************************~***************************************************/


