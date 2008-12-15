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


#ifndef __SCENE_H__
#define __SCENE_H__


/*** Included Header Files ***/
#include "Scene/wscnl.h"


/*** Locally Defined Values ***/
//Default depth values for the Z axis
#define SCENE_ZMIN_DEFAULT						-1000.0
#define SCENE_ZMAX_DEFAULT						1000.0
//Object Messages
#define SCENE_LAYERPRIORITY_CHANGE				1
//Other parameters
#define SCENE_MAX_LIGHTSOURCES					8
#define SCENE_MAX_MOUSEBUTTONS					16


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCLayer;
class WCLayerNode;
class WCCameraLayer;
class WCCamera;
class WCLightSource;
class WCFontManager;


/***********************************************~***************************************************/


class WCScene : virtual public WCSerializeableObject {
public:
	//Public static members
	static std::string							ShaderManifest, TextureManifest;					//!< Name of the resource manifests
	
protected:
	//General Scene Properties
	WCGLContext									*_glContext;										//!< OpenGL context object
	WCShaderManager								*_shaderManager;									//!< Shader manager for the scene
	WCTextureManager							*_textureManager;									//!< Texture manager for the scene
	WCFontManager								*_fontManager;										//!< Font manager for the scene
	WCGeometryContext							*_geomContext;										//!< Geometry context pointer
	bool										_isDirty;											//!< Does the scene need rerendering
	bool										_useShadowPass;										//!< Shadow pass flag
	bool										_useSelectionPass;									//!< Selection pass flag
	WPFloat										_xMin, _yMin, _xMax, _yMax;							//!< 4 corners parameters	
	WPFloat										_width, _height;									//!< Width and height	
	WPUInt										_winWidth, _winHeight;								//!< Window size
	WCMatrix4									_projectionInverse;									//!< Projection inverse matrix
	WPFloat										_mouseX, _mouseY;									//!< Mouse Y and Y coordinates (in scene coords)
	bool										_mouseButtons[SCENE_MAX_MOUSEBUTTONS];				//!< Array of mouse button status (false = no down)
	WCCamera									*_activeCamera;										//!< Pointer to the currently active camera
	WCCamera									*_defaultCamera;									//!< Default camera
	//Layer Objects
	WCLayer										*_firstResponder;									//!< First responder layer	
	WCCameraLayer								*_cameraLayer;										//!< Camera layer
	WCLayer										*_topLayer, *_bottomLayer;							//!< Head and tail layer pointers
	//Callback Objects
	WCObject									*_frameTickObject;									//!< Frame render (or idle) object
	//Lighting and Shading Objects
	std::list<WCLightSource*>					_lightSources;										//! List of registered light sources

private:
	//Access Denied
	WCScene();																						//!< Deny access to default constructor
	WCScene(const WCScene& scene);																	//!< Deny access to copy operator
	WCScene(const WCScene* scene);																	//!< Deny access to ref copy operator
	WCScene& operator=(const WCScene& scene);														//!< Deny access to equals operator
	bool operator==(const WCScene& scene);															//!< Deny access to equals operator

public:
	//Constructors and Destructors
	WCScene(WCGLContext *copyContext=NULL);															//!< Primary context
	WCScene(xercesc::DOMElement* element, WCSerialDictionary *dictionary);							//!< Persistance constructor
	~WCScene();																						//!< Default destructor

	//General Access Methods
	inline WPFloat XMin(void) const				{ return this->_xMin; }								//!< Return the xMin value
	inline WPFloat XMax(void) const				{ return this->_xMax; }								//!< Return the xMax value
	inline WPFloat YMin(void) const				{ return this->_yMin; }								//!< Return the yMin value
	inline WPFloat YMax(void) const				{ return this->_yMax; }								//!< Return the yMax value
	inline WPFloat Width(void) const			{ return this->_width; }							//!< Return the width value
	inline WPFloat Height(void) const			{ return this->_height; }							//!< Return the height value
	inline WPFloat MouseX(void)	const			{ return this->_mouseX; }							//!< Return the x position of the mouse (in scene coords)
	inline WPFloat MouseY(void)	const			{ return this->_mouseY; }							//!< Return the y position of the mouse (in scene coords)	
	bool MouseState(const WCMouseButton &button){ return this->_mouseButtons[button._button]; }		//!< Return the state of the mouse button
	inline WPInt WindowWidth(void)	const		{ return this->_winWidth; }							//!< Return the integer window width
	inline WPInt WindowHeight(void) const		{ return this->_winHeight; }						//!< Return the integer window height
	inline WCMatrix4 ProjectionInverse(void) const { return this->_projectionInverse; }				//!< Return the projection inverse
	inline WCLayer* FirstResponder(void) const	{ return this->_firstResponder; }					//!< Return the first responder layer
	inline void FirstResponder(WCLayer* layer)	{ this->_firstResponder = layer; }					//!< Set the first responder layer
	inline bool IsDirty(void) const				{ return this->_isDirty; }							//!< Get the dirty flag for the scene
	inline void MarkDirty(void)					{ this->_isDirty = true; }							//!< Mark the views as dirty
	
	//Callback Objects
	void FrameTick(WCObject *frameTickObj)		{ this->_frameTickObject = frameTickObj; }			//!< Set the frame tick callback object

	//Event Handling Methods
	bool OnIdle(void);																				//!< Handle an idle event
	bool OnMouseMove(const int &x, const int &y);													//!< Track the mouse x and y position
	bool OnMouseDown(const WCMouseButton &button);													//!< Mouse button down event
	bool OnMouseUp(const WCMouseButton &button);													//!< Mouse button up event
	bool OnScroll(const WPFloat &x, const WPFloat &y);												//!< Respond to scroll-wheel event
	bool OnArrowKeyPress(const WCArrowKey &key);													//!< React to arrow key event
	bool OnReshape(const int &width, const int &height);											//!< Window reshape info
		
	//Layer Related Methods
	void RegisterLayer(WCLayer *layer, WCLayer *layerAbove=NULL);									//!< Add a new layer into the scene
	void UnregisterLayer(WCLayer *layer);															//!< Remove a layer from the scene
	WCLayer* TopLayer(void)						{ return this->_topLayer; }							//!< Highest priority layer
	WCLayer* BottomLayer(void)					{ return this->_bottomLayer; }						//!< Lowest priority layer
	void LayerAbove(WCLayer *layer, WCLayer *mark=NULL);											//!< Order "layer" just above mark
	void LayerBelow(WCLayer *layer, WCLayer *mark=NULL);											//!< Order "layer" just below mark
	inline WCCameraLayer* CameraLayer(void)		{ return this->_cameraLayer; }						//!< Camera layer
	WCLayer* LayerFromName(std::string name);														//!< Return a pointer to layer with given name

	//Camera Methods
	inline WCCamera* ActiveCamera(void) const	{ return this->_activeCamera; }						//!< Return pointer to the current camera
	void ActiveCamera(WCCamera* camera);															//!< Set the active camera
	
	//Manager Methods
	inline WCGLContext* GLContext(void) const			{ return this->_glContext; }				//!< Get the OpenGL context for the scene
	inline WCShaderManager* ShaderManager(void) const	{ return this->_shaderManager; }			//!< Get the scene's shader manager
	inline WCTextureManager* TextureManager(void) const	{ return this->_textureManager; }			//!< Get the scene's texture manager
	inline WCFontManager* FontManager(void) const		{ return this->_fontManager; }				//!< Get the scene's font manager
	inline WCGeometryContext* GeometryContext(void) const{ return this->_geomContext; }				//!< Get the scene's geometry context

	//LightSource Methods
	void RegisterLightSource(WCLightSource* light);													//!< Add a light source to the scene
	void UnregisterLightSource(WCLightSource* light);												//!< Remove a light source from the scene

	//Environment Methods
	WCAlignedBoundingBox BoundingBox(void);															//!< Return an aligned bounding box for the scene

	//Primary Class Functions
	void Render(void);																				//!< Primary rendering entry point
	void ReceiveNotice(WCObjectMsg msg, WCObject *sender);											//!< Receive notice
	
	//Serialization and Object Methods
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object

	/*** Non-Member Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCScene &scene);						//!< Overloaded output operator
	
	//Shader Access Methods
	GLuint ProgramFromName(const std::string &name);												//!< Get a program from a name
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__SCENE_H__

