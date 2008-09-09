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
#include "Scene/compass.h"
#include "Scene/user_interface_layer.h"
#include "Scene/scene.h"
#include "Scene/camera.h"
#include "Scene/text.h"


/***********************************************~***************************************************/


WCCompass::WCCompass(WCUserInterfaceLayer *layer) : ::WCOverlay(layer, false), _xText(NULL), _yText(NULL), _zText(NULL) {
	//Set the size of the compass
	this->WCWidget::SetSize(COMPASS_SIZE, COMPASS_SIZE);
	this->_layer->RegisterWidget(this);
	
	//Setup x, y, and z text labels
	WCScene *scene = this->_layer->Scene();
	this->_xText = new WCText(scene, "x", WCColor::Red(), WCTextFont::Palatino(), WCTextStyle::Roman(), 15.0);
	this->_yText = new WCText(scene, "y", WCColor::Green(), WCTextFont::Palatino(), WCTextStyle::Roman(), 15.0);
	this->_zText = new WCText(scene, "z", WCColor::Yellow(), WCTextFont::Palatino(), WCTextStyle::Roman(), 15.0);
}


WCCompass::~WCCompass() {
	//Unregister the widget
	this->_layer->UnregisterWidget(this);
	//Delete the text objects
	if (this->_xText != NULL) delete this->_xText;
	if (this->_yText != NULL) delete this->_yText;
	if (this->_zText != NULL) delete this->_zText;
}


void WCCompass::OnReshape(void) {
	this->WCOverlay::OnReshape();
}


void WCCompass::OnMouseDown(const WCMouseButton &button, const WPFloat x, const WPFloat y) {
	this->WCOverlay::OnMouseDown(button, x, y);
}


void WCCompass::OnMouseEnter(const WPFloat x, const WPFloat y) {
	this->WCOverlay::OnMouseEnter(x, y);
}


void WCCompass::OnMouseExit(const WPFloat x, const WPFloat y) {
	this->WCOverlay::OnMouseExit(x, y);
}


void WCCompass::OnMouseMove(const WPFloat x, const WPFloat y) {
	this->WCOverlay::OnMouseMove(x, y);
}


void WCCompass::Render(void) {
	//Check to make sure the compass is visible
	if (!this->_isVisible) return;

	//Find the center of the overlay
	WPFloat x = this->_xMin + (COMPASS_SIZE / 2.0) - 0.05;
	WPFloat y = this->_yMin + (COMPASS_SIZE / 2.0) - 0.05;
	WCVector4 center(x, y, 0.0, 0.0);

	//Just rotate, no zoom or pan
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	//Only apply rotation
	WCMatrix4 rotMat = this->_layer->Scene()->ActiveCamera()->Quaternion().ToDCM();
	
	//Setup and rotate the x Axis line end-point
	WCVector4 xAxis(1.0, 0.0, 0.0, 0.0);
	xAxis = (rotMat * xAxis).Normalize(true) * COMPASS_SCALE_FACTOR + center;
	//Setup and rotate the y Axis line end-point
	WCVector4 yAxis(0.0, 1.0, 0.0, 0.0);
	yAxis = (rotMat * yAxis).Normalize(true) * COMPASS_SCALE_FACTOR + center;
	//Setup and rotate the z Axis line end-point
	WCVector4 zAxis(0.0, 0.0, 1.0, 0.0);
	zAxis = (rotMat * zAxis).Normalize(true) * COMPASS_SCALE_FACTOR + center;
	
	GLfloat vData[18] = { (GLfloat)x, (GLfloat)y, 0.0f, (GLfloat)xAxis.I(), (GLfloat)xAxis.J(), (GLfloat)xAxis.K(),
						  (GLfloat)x, (GLfloat)y, 0.0f, (GLfloat)yAxis.I(), (GLfloat)yAxis.J(), (GLfloat)yAxis.K(),
						  (GLfloat)x, (GLfloat)y, 0.0f, (GLfloat)zAxis.I(), (GLfloat)zAxis.J(), (GLfloat)zAxis.K() };
	GLfloat cData[24] = { 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
						  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
						  1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f };
	//Set up render state
	glLineWidth(COMPASS_LINEWIDTH);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vData);
	glColorPointer(4, GL_FLOAT, 0, cData);
	//Draw the line
	glDrawArrays(GL_LINES, 0, 6);
	//Clean up state
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	
	//Finally write the axis labels
	this->_xText->DrawAtPoint(xAxis);
	this->_yText->DrawAtPoint(yAxis);
	this->_zText->DrawAtPoint(zAxis);

	//Reset the modelview matrix
	glPopMatrix();
}


/***********************************************~***************************************************/

