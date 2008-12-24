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


#ifndef __CAMERA_H__
#define __CAMERA_H__


/*** Included Header Files ***/
#include <Scene/wscnl.h>
#include <Scene/layer.h>
#include <Scene/named_view.h>


/*** Locally Defined Values ***/
#define CAMERA_ANIMATE_DURATION					0.5
#define CAMERA_THREEQUATER_VIEW					-0.209315, -0.508694, -0.767096, 0.330123
#define CAMERA_FITWINDOW_BOUNDRY				0.25


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCScene;


/***********************************************~***************************************************/


class WCCamera : virtual public WCSerializeableObject {
private:
	WCScene										*_scene;											//!< Scene that owns the camera
	WPFloat										_zoom, _panX, _panY;								//!< Zoom and pan factors
	WCQuaternion								_quaternion;										//!< Rotation quaternion
	WCMatrix4									_matrix, _inverseMatrix;							//!< Camera matrices
	bool										_isOrtho;											//!< Is the camera in orthographic view
	std::string									_name;												//!< Camera name
	bool										_isMoveable;										//!< Can the camera be changed
	WCRenderState								*_renderState;										//!< Camera specific render state
	bool										_isAnimating;										//!< Animation flag
	WPTime										_start, _stop;										//!< Animation start and stop time
	WPFloat										_aniZoom, _aniPanX, _aniPanY, _aniAngle;			//!< Animation end points
	WPFloat										_markZoom, _markPanX, _markPanY, _markAngle;		//!< Animation start points
	WCQuaternion								_aniQuaternion, _markQuaternion;					//!< Animation quaternions
	//Private Methods
	void GenerateMatrices(void);																	//< Generate matrices
	//Deny Access
	WCCamera();																						//!< Deny access to default constructor
	WCCamera& operator=(const WCCamera &camera);													//!< Deny access to equals operator
	bool operator==(const WCCamera &camera);														//!< For now deny access to equality operator
public:
	//Constructors and Destructors
	WCCamera(WCScene *scene, const std::string &name, const bool &moveable=true);					//!< Primary constructor
	WCCamera(const WCCamera &camera);																//!< Copy constructor
	WCCamera(xercesc::DOMElement* element, WCSerialDictionary *dictionary);							//!< Persistance constructor
	~WCCamera();																					//!< Default destructor
	
	//Member Access Methods
	inline bool IsOrthographic(void) const		{ return this->_isOrtho; }							//!< Is the camera in orthographic mode
	inline bool IsAnimating(void) const			{ return this->_isAnimating; }						//!< Is the camera in animation mode
	inline bool IsMoveable(void) const			{ return this->_isMoveable; }						//!< Is the camera moveable
	WCMatrix4 ViewMatrix(void) const			{ return this->_matrix; }							//!< Matrix of current position
	WCMatrix4 ViewInverseMatrix(void) const		{ return this->_inverseMatrix; }					//!< Inverse matrix of current pos
	WCQuaternion& Quaternion(void)				{ return this->_quaternion; }						//!< Get the rotation quaternion
	void Quaternion(const WCQuaternion &quat);														//!< Set the rotation quaternion
	WPFloat Zoom(void) const					{ return this->_zoom; }								//!< Get the current zoom factor
	void Zoom(const WPFloat &factor);																//!< Set the zoom factor
	void IncrementZoom(const WPFloat &factor);														//!< Increment the zoom factor
	WPFloat PanX(void) const					{ return this->_panX; }								//!< Get the x pan amount
	WPFloat PanY(void) const					{ return this->_panY; }								//!< Get the y pan amount
	void Pan(const WPFloat &x, const WPFloat &y);													//!< Set the pan value
	void IncrementPan(const WPFloat &x, const WPFloat &y);											//!< Increment the pan value
	WCRenderState* RenderState(void)			{ return this->_renderState; }						//!< Get the camera render state
	
	//Rotation Manipulation Methods
	void RotationXYZ(const WPFloat &phi, const WPFloat &theta, const WPFloat &psi);					//!< Rotate values
	void RotationZXZ(const WPFloat &phi, const WPFloat &theta, const WPFloat &psi);					//!< Rotate values
	void IncrementXRotation(const WPFloat &rot);													//!< Increment the rotate values
	void IncrementYRotation(const WPFloat &rot);													//!< Increment the rotate values
	void IncrementZRotation(const WPFloat &rot);													//!< Increment the rotate values
	void IncrementXAxisAlignedRotation(const WPFloat &rot);											//!< X-Axis aligned rotation
	void IncrementYAxisAlignedRotation(const WPFloat &rot);											//!< Y-Axis aligned rotation
	void IncrementZAxisAlignedRotation(const WPFloat &rot);											//!< Z-Axis aligned rotation
	
	//Projection Methods
	void Projection(void);																			//!< Set project values
	void Orthographic(void);																		//!< Set to orthographic projection
	
	//Animation Methods
	void FitToWindow(void);																			//!< Zoom the window to just fit the model
	void AnimateToViewpoint(const WCQuaternion &rot, const WPFloat &xPan, const WPFloat &yPan,		//!< Animate to a given viewpoint
												 const WPFloat &zoom, const WPFloat &seconds=CAMERA_ANIMATE_DURATION);
	void AnimateToViewpoint(const WCNamedView &view, const WPFloat &duration=CAMERA_ANIMATE_DURATION); //!< Animate to a set view
	
	//Other Methods
	WCNamedView NamedView(void);																	//!< Record current position
	void Reset(void);																				//!< Reset the camera back to zero point	
	void ReadyScene(void);																			//!< Update the camera for rendering
	void EnableLighting(const GLuint &prog);														//!< Force lighting on for a given program
	
	//Serialization and Object Methods
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__CAMERA_H__

