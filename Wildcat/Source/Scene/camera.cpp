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
#include "Scene/camera.h"
#include "Scene/render_state.h"



/*** Platform Included Header Files ***/
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif
#ifdef __WIN32__
//...
#endif


/***********************************************~***************************************************/


void WCCamera::GenerateMatrices(void) {
	//Determine view matrix
	WCMatrix4 panZoom(this->_zoom, 0.0, 0.0, this->_panX,
					  0.0, this->_zoom, 0.0, this->_panY,
					  0.0, 0.0, this->_zoom, 0.0,
					  0.0, 0.0, 0.0, 1.0);
	this->_matrix = panZoom * this->_quaternion.ToDCM();
	//Calculate the inverse
	this->_inverseMatrix = this->_matrix.Inverse();
	//Let dependent objects know about change
	this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);
}


/***********************************************~***************************************************/


WCCamera::WCCamera(WCScene *scene, std::string name, const bool moveable) : ::WCSerializeableObject(), 
	_scene(scene), _zoom(1.0), _panX(0.0), _panY(0.0), 
	_isOrtho(true), _quaternion(0.0, 0.0, 0.0, 1.0), _matrix(false), _inverseMatrix(false), 
	_name(name), _isMoveable(moveable), _renderState(NULL), _isAnimating(false), 
	_start(), _stop(), 
	_aniZoom(1.0), _aniPanX(0.0), _aniPanY(0.0), _aniAngle(0.0), 
	_markZoom(1.0), _markPanX(0.0), _markPanY(0.0), _markAngle(0.0),
	_aniQuaternion(), _markQuaternion() {

	//Create a render state for this camera
	this->_renderState = new WCRenderState();
	//Retain the render state
	this->_renderState->Retain(*this);
}


WCCamera::WCCamera(const WCCamera &camera) : ::WCObject(), 
	_scene(camera._scene), _zoom(camera._zoom), _panX(camera._panX), _panY(camera._panY), 
	_isOrtho(camera._isOrtho), _quaternion(camera._quaternion), _name(camera._name), _isMoveable(camera._isMoveable),
	_renderState(NULL), _isAnimating(false), _start(), _stop(), 
	_aniZoom(1.0), _aniPanX(0.0), _aniPanY(0.0), _aniAngle(0.0), 
	_markZoom(1.0), _markPanX(0.0), _markPanY(0.0), _markAngle(0.0), 
	_aniQuaternion(), _markQuaternion() {

	//Copy the existing quaternion
	this->_quaternion = camera._quaternion;
}


WCCamera::WCCamera(xercesc::DOMElement* element, WCSerialDictionary *dictionary) : ::WCSerializeableObject(),
	_scene(NULL), _zoom(1.0), _panX(0.0), _panY(0.0), 
	_isOrtho(true), _quaternion(0.0, 0.0, 0.0, 1.0), _matrix(false), _inverseMatrix(false), 
	_name(), _isMoveable(false), _renderState(NULL), _isAnimating(false), 
	_start(), _stop(), 
	_aniZoom(1.0), _aniPanX(0.0), _aniPanY(0.0), _aniAngle(0.0), 
	_markZoom(1.0), _markPanX(0.0), _markPanY(0.0), _markAngle(0.0),
	_aniQuaternion(), _markQuaternion() {

	//Make sure element if not null
	if (element == NULL) return;
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

	//Get the scene
	this->_scene = (WCScene*)WCSerializeableObject::GetGUIDAttrib(element, "scene", dictionary);
	//Get the name
	this->_name = WCSerializeableObject::GetStringAttrib(element, "name");
	//Get the panX attribute
	this->_panX = WCSerializeableObject::GetFloatAttrib(element, "panx");
	//Get the panY attribute
	this->_panY = WCSerializeableObject::GetFloatAttrib(element, "pany");
	//Get the zoom attribute
	this->_zoom = WCSerializeableObject::GetFloatAttrib(element, "zoom");
	//Get the isMoveable attribute
	this->_isMoveable = WCSerializeableObject::GetBoolAttrib(element, "moveable");
	//Get the isOrtho attribute
	this->_isOrtho = WCSerializeableObject::GetBoolAttrib(element, "ortho");

	//Create the renderState
	this->_renderState = new WCRenderState( WCSerializeableObject::ElementFromName(element,"RenderState"), dictionary );
	//Retain the render state
	this->_renderState->Retain(*this);

	//Get the quaternion object
	this->_quaternion.FromElement( WCSerializeableObject::ElementFromName(element,"Quaternion") );
}


WCCamera::~WCCamera() {
	//Nothing for now
	if (this->_renderState != NULL) {
		this->_renderState->Release(*this);
		delete this->_renderState;
	}
}


void WCCamera::Quaternion(const WCQuaternion &quat) {
	//Make sure the camera is moveable
	if (!this->_isMoveable) return;
	//Update the rotation quaternion
	this->_quaternion = quat;
	//Generate matrices
	this->GenerateMatrices();
	//Mark this scene as dirty
	this->_scene->MarkDirty();
}

void WCCamera::Zoom(const WPFloat factor) {
	//Make sure the camera is moveable
	if (!this->_isMoveable) return;
	//Now update the zoom factor
	this->_zoom = factor;
	//Generate matrices
	this->GenerateMatrices();
	//Mark this scene as dirty
	this->_scene->MarkDirty();

}


void WCCamera::IncrementZoom(const WPFloat factor) {
	//Make sure the camera is moveable
	if (!this->_isMoveable) return;
	//Now update the zoom factor
	this->_zoom = this->_zoom * factor;	
	//Generate matrices
	this->GenerateMatrices();
	//Mark this scene as dirty
	this->_scene->MarkDirty();
}


void WCCamera::Pan(const WPFloat x, const WPFloat y) {
	//Make sure the camera is moveable
	if (!this->_isMoveable) return;
	//Set the offset values
	this->_panX = x;
	this->_panY = y;
	//Generate matrices
	this->GenerateMatrices();
	//Mark this scene as dirty
	this->_scene->MarkDirty();
}


void WCCamera::IncrementPan(const WPFloat x, const WPFloat y) {
	//Make sure the camera is moveable
	if (!this->_isMoveable) return;
	//Increment the offset values
	this->_panX += x;
	this->_panY += y;
	//Generate matrices
	this->GenerateMatrices();
	//Mark this scene as dirty
	this->_scene->MarkDirty();
}


void WCCamera::RotationXYZ(const WPFloat phi, const WPFloat theta, const WPFloat psi) {
	//Make sure the camera is moveable
	if (!this->_isMoveable) return;
	//Load the rotation quaternion
	this->_quaternion.FromEulerXYZ(phi, theta, psi);
	//Generate matrices
	this->GenerateMatrices();
	//Mark this scene as dirty
	this->_scene->MarkDirty();
}


void WCCamera::RotationZXZ(const WPFloat phi, const WPFloat theta, const WPFloat psi) {
	//Make sure the camera is moveable
	if (!this->_isMoveable) return;
	//Load the rotation quaternion
	this->_quaternion.FromEulerZXZ(phi, theta, psi);
	//Generate matrices
	this->GenerateMatrices();
	//Mark this scene as dirty
	this->_scene->MarkDirty();
}


void WCCamera::IncrementXRotation(const WPFloat rot) {
	//Make sure the camera is moveable
	if (!this->_isMoveable) return;
	//Load inc with the Euler Axis data
	WCQuaternion inc(1.0, 0.0, 0.0, rot, true);
	//Multiply the two together (right hand mult gives screen aligned rotation)
	this->_quaternion = inc * this->_quaternion;
	//Generate matrices
	this->GenerateMatrices();
	//Mark this scene as dirty
	this->_scene->MarkDirty();
}


void WCCamera::IncrementYRotation(const WPFloat rot) {
	//Make sure the camera is moveable
	if (!this->_isMoveable) return;
	//Load inc with the Euler Axis data
	WCQuaternion inc(0.0, 1.0, 0.0, rot, true);
	//Multiply the two together (right hand mult gives screen aligned rotation)	
	this->_quaternion = inc * this->_quaternion;
	//Generate matrices
	this->GenerateMatrices();
	//Mark this scene as dirty
	this->_scene->MarkDirty();
}


void WCCamera::IncrementZRotation(const WPFloat rot) {
	//Make sure the camera is moveable
	if (!this->_isMoveable) return;
	//Load inc with the Euler Axis data
	WCQuaternion inc(0.0, 0.0, 1.0, rot, true);
	//Multiply the two together (right hand mult gives screen aligned rotation)	
	this->_quaternion = inc * this->_quaternion;
	//Generate matrices
	this->GenerateMatrices();
	//Mark this scene as dirty
	this->_scene->MarkDirty();
}


//This function is equivalent to a prerotation by rot followed by the existing rotation
//  - should result in a rotation strictly about the x axis
void WCCamera::IncrementXAxisAlignedRotation(const WPFloat rot) {
	//Make sure the camera is moveable
	if (!this->_isMoveable) return;
	//Load inc with the Euler Axis data
	WCQuaternion inc(1.0, 0.0, 0.0, rot, true);
	//Post multiply
	this->_quaternion = this->_quaternion * inc;
	//Generate matrices
	this->GenerateMatrices();
	//Mark this scene as dirty
	this->_scene->MarkDirty();
}


//This function is equivalent to a prerotation by rot followed by the existing rotation
//  - should result in a rotation strictly about the y axis
void WCCamera::IncrementYAxisAlignedRotation(const WPFloat rot) {
	//Make sure the camera is moveable
	if (!this->_isMoveable) return;
	//Load inc with the Euler Axis data
	WCQuaternion inc(0.0, 1.0, 0.0, rot, true);
	//Post multiply
	this->_quaternion = this->_quaternion * inc;
	//Generate matrices
	this->GenerateMatrices();
	//Mark this scene as dirty
	this->_scene->MarkDirty();
}


//This function is equivalent to a prerotation by rot followed by the existing rotation
//  - should result in a rotation strictly about the z axis
void WCCamera::IncrementZAxisAlignedRotation(const WPFloat rot) {
	//Make sure the camera is moveable
	if (!this->_isMoveable) return;
	//Load inc with the Euler Axis data
	WCQuaternion inc(0.0, 0.0, 1.0, rot, true);
	//Post multiply
	this->_quaternion = this->_quaternion * inc;
	//Generate matrices
	this->GenerateMatrices();
	//Mark this scene as dirty
	this->_scene->MarkDirty();
}


void WCCamera::FitToWindow(void) {
	//Make sure the camera is moveable
	if (!this->_isMoveable) return;
	//Determine pan and zoom values
	WCAlignedBoundingBox box = this->_scene->BoundingBox();
	WCMatrix mat = box.ToMatrix();
	//Project box to using rot
	WCMatrix4 rot4 = this->_quaternion.ToDCM();
	WCMatrix rot = rot4.ToMatrix();
	mat = rot * mat;
	//Find projected min/max values
	WPFloat xMin = mat.RowMinimum(0);
	WPFloat xMax = mat.RowMaximum(0);
	WPFloat yMin = mat.RowMinimum(1);
	WPFloat yMax = mat.RowMaximum(1);
	//Calculate the pan and zoom factors
	WPFloat width =  this->_scene->Width() / ((xMax - xMin) + (2.0 * CAMERA_FITWINDOW_BOUNDRY));
	WPFloat height = this->_scene->Height() / ((yMax - yMin) + (2.0 * CAMERA_FITWINDOW_BOUNDRY));
	WPFloat zoom = STDMIN(width, height);
	WPFloat panX = -0.5 * (xMin + xMax) * zoom;
	WPFloat panY = -0.5 * (yMin + yMax) * zoom;
	//Call AnimateToViewpoint
	this->AnimateToViewpoint(this->_quaternion, panX, panY, zoom, CAMERA_ANIMATE_DURATION);
}

	
void WCCamera::AnimateToViewpoint(const WCQuaternion &rot, const WPFloat xPan, const WPFloat yPan, const WPFloat zoom, const WPFloat seconds) {
	//Make sure the camera is moveable
	if (!this->_isMoveable) return;
	//Setup the animation information
#ifdef __APPLE__
	this->_start = UpTime();
	Nanoseconds duration = UInt64ToUnsignedWide(1000000000L * seconds);
	this->_stop = AddNanosecondsToAbsolute(duration, this->_start);
#elif __WIN32__
	this->_start = CFileTime::GetCurrentTime();
	CFileTimeSpan duration(1000000L * (LONGLONG)seconds);
	this->_stop = this->_start + duration;
#endif
	//Set the stop points
	this->_aniQuaternion = new WCQuaternion(rot);
	this->_aniPanX = xPan;
	this->_aniPanY = yPan;
	this->_aniZoom = zoom;
	//Determine angle (bounds check it)
	this->_aniAngle = this->_quaternion.InnerProduct(rot);
	this->_aniAngle = STDMAX(STDMIN(this->_aniAngle, 1.0), -1.0);
	this->_aniAngle = acos(this->_aniAngle);
	//Do some checking on angle (just make sure not zero)
	if (this->_aniAngle == 0.0) this->_aniAngle = 0.01;
	//Set the mark values
	this->_markZoom = this->_zoom;
	this->_markPanX = this->_panX;
	this->_markPanY = this->_panY;
	this->_markQuaternion = this->_quaternion;
	//Mark the camera as animating
	this->_isAnimating = true;
}


void WCCamera::AnimateToViewpoint(const WCNamedView &view, WPFloat duration) {
	//Calculate pan and zoom if named view is auto
	if (view.IsAuto()) {
		//Determine pan and zoom values
		WCAlignedBoundingBox box = this->_scene->BoundingBox();
		WCMatrix mat = box.ToMatrix();
		//Project box to using rot
		WCMatrix4 rot4 = view.Quaternion().ToDCM();
		WCMatrix rot = rot4.ToMatrix();
		mat = rot * mat;
		//Find projected min/max values
		WPFloat xMin = mat.RowMinimum(0);
		WPFloat xMax = mat.RowMaximum(0);
		WPFloat yMin = mat.RowMinimum(1);
		WPFloat yMax = mat.RowMaximum(1);
		//Calculate the pan and zoom factors
		WPFloat width =  this->_scene->Width() / ((xMax - xMin) + (2.0 * CAMERA_FITWINDOW_BOUNDRY));
		WPFloat height = this->_scene->Height() / ((yMax - yMin) + (2.0 * CAMERA_FITWINDOW_BOUNDRY));
		WPFloat zoom = STDMIN(width, height);
		WPFloat panX = -0.5 * (xMin + xMax) * zoom;
		WPFloat panY = -0.5 * (yMin + yMax) * zoom;
		this->AnimateToViewpoint(view.Quaternion(), panX, panY, zoom, duration);
	}
	//Otherwise, use the named view pan and zoom values
	else {
		this->AnimateToViewpoint(view.Quaternion(), view.PanX(), view.PanY(), view.Zoom(), duration);
	}
}


WCNamedView WCCamera::NamedView(void) {
	//Get the current view
	WCNamedView view("capture", this->_quaternion, this->_isOrtho);
	view.Zoom(this->_zoom);
	view.PanX(this->_panX);
	view.PanY(this->_panY);
	return view;
}


void WCCamera::Reset(void) {
	//Reset the rotation quaternion
	this->_quaternion.FromEulerAxis(0.0, 0.0, 0.0, 0.0);
	//Reset the remaining view parameters
	this->_zoom = 1.0;
	this->_panX = 0.0;
	this->_panY = 0.0;
	//Mark as not animating
	this->_isAnimating = false;
	//Mark this scene as dirty
	this->_scene->MarkDirty();
	
}


void WCCamera::ReadyScene(void) {
	//Reset the modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//Animation path
	if (this->_isAnimating) {
		//Calculate interpolate
#ifdef __APPLE__
		AbsoluteTime now = UpTime();
		WPFloat elapsed = UnsignedWideToUInt64(AbsoluteDeltaToNanoseconds(now, this->_start)) / 10.0;
		WPFloat togo = UnsignedWideToUInt64(AbsoluteDeltaToNanoseconds(this->_stop, now)) / 10.0;
#elif __WIN32__
		CFileTime now = CFileTime::GetCurrentTime();
		CFileTimeSpan sofar = now - this->_start;
		WPFloat elapsed = sofar.GetTimeSpan() / 10.0;
		CFileTimeSpan remains = this->_stop - now;
		WPFloat togo = remains.GetTimeSpan() / 10.0;
#endif
		WPFloat percent = elapsed / (elapsed + togo);
		//Interpolate the quaternion
		WPFloat sinAngle = sin(this->_aniAngle);
		WCQuaternion quat = this->_markQuaternion * (sin( (1.0-percent) * this->_aniAngle ) / sinAngle);
		quat += this->_aniQuaternion * (sin( percent * this->_aniAngle ) / sinAngle);
		//Interpolate the pan
		WPFloat panX = (1.0 - percent) * this->_markPanX + (percent * this->_aniPanX);
		WPFloat panY = (1.0 - percent) * this->_markPanY + (percent * this->_aniPanY);
		//Interpolate the zoom
		WPFloat zoom = (1.0 - percent) * this->_markZoom + (percent * this->_aniZoom);
		//Update the camera info
		this->_zoom = zoom;
		this->_panX = panX;
		this->_panY = panY;
		this->_quaternion = quat;
		//Generate matrices
		this->GenerateMatrices();
		//Mark the scene as dirty
		this->_scene->MarkDirty();
		if(togo <= 0.01) {
			//Mark the animation as false
			this->_isAnimating = false;
			//Update the camera info
			this->_zoom = this->_aniZoom;
			this->_panX = this->_aniPanX;
			this->_panY = this->_aniPanY;
			this->_quaternion = this->_aniQuaternion;
		}
	}

	//Pan the scene
	glTranslated(this->_panX, this->_panY, 0.0);
	//And apply the zoom factor
	glScaled(this->_zoom, this->_zoom, this->_zoom);
	//Rotate the scene
	this->_quaternion.ToGLRotation();
	//Check for errors
	if (glGetError() != GL_NO_ERROR)
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCCamera::ReadyScene - Gl errors at Cleanup.");	
}


void WCCamera::EnableLighting(GLuint prog) {
	//Make sure we can set parameters
	if (prog == 0) return;	
	//Set program params (if accepted)
	GLint panZoomLocation = glGetUniformLocation(prog, "panZoomFactor");
	if (panZoomLocation != -1) 
		glUniform3f(panZoomLocation, (GLfloat)this->_panX, (GLfloat)this->_panY, (GLfloat)this->_zoom);
}


xercesc::DOMElement* WCCamera::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create primary element for this object
	XMLCh* xmlString = xercesc::XMLString::transcode("Camera");
	xercesc::DOMElement*  element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Add in parent object
//	xercesc::DOMElement* baseElement = this->WCSerializeableObject::Serialize(document, dictionary);
//	element->appendChild(baseElement);
	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);

	//Add Scene attribute
	WCSerializeableObject::AddGUIDAttrib(element, "scene", this->_scene, dictionary);
	//Add name attribute
	WCSerializeableObject::AddStringAttrib(element, "name", this->_name);
	//Add isOrtho attribute
	WCSerializeableObject::AddBoolAttrib(element, "ortho", this->_isOrtho);
	//Add isMoveable attribute
	WCSerializeableObject::AddBoolAttrib(element, "moveable", this->_isMoveable);
	//Add zoom attribute
	WCSerializeableObject::AddFloatAttrib(element, "zoom", this->_zoom);
	//Add panX attribute
	WCSerializeableObject::AddFloatAttrib(element, "panx", this->_panX);
	//Add panY attribute
	WCSerializeableObject::AddFloatAttrib(element, "pany", this->_panY);
	
	//Add quaternion
	xmlString = xercesc::XMLString::transcode("Quaternion");
	xercesc::DOMElement* quatElem = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	this->_quaternion.ToElement(quatElem);
	element->appendChild(quatElem);
	
	//Add render state
	xercesc::DOMElement* renderStateElement = this->_renderState->Serialize(document, dictionary);
	element->appendChild(renderStateElement);
	
	//Return the primary element
	return element;
}


/***********************************************~***************************************************/

