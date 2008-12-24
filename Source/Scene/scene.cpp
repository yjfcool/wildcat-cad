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
#include "Scene/scene.h"
#include "Scene/camera.h"
#include "Scene/light_source.h"
#include "Scene/camera_layer.h"
//Include Font Manager only if not on OSX
#ifndef __APPLE__
#include "Application/Win32/font_manager.h"
#endif


//Initialize shader manifest statics
std::string WCScene::ShaderManifest = "shader_manifest.xml";
//Initialize texture manifest statics
std::string WCScene::TextureManifest = "texture_manifest.xml";


/***********************************************~***************************************************/


WCScene::WCScene(WCGLContext *copyContext) : :: WCSerializeableObject(), 
	_glContext(NULL), _shaderManager(NULL), _textureManager(NULL), _fontManager(NULL), _geomContext(NULL), _isDirty(true),
	_useShadowPass(false), _useSelectionPass(false),
	_xMin(-1.0), _yMin(-1.0), _xMax(1.0), _yMax(1.0), _width(2.0), _height(2.0), _winWidth(1), _winHeight(1),
	_projectionInverse(true), _mouseX(0.0), _mouseY(0.0), _activeCamera(NULL), _defaultCamera(NULL),
	_firstResponder(NULL), _cameraLayer(NULL), _topLayer(NULL), _bottomLayer(NULL),
	_frameTickObject(NULL) {
	std::string resourcesDirectory = _ResourceDirectory();
	//Make sure there is a context to copy
	ASSERT(copyContext);
	//Create a new gl Context (copying from a root context)
	this->_glContext = new WCGLContext(*copyContext);

	//Check to see if GL1.5 is present
	if(WCAdapter::HasGL15()) {
		//Start new shader manager
		this->_shaderManager = new WCShaderManager(WCScene::ShaderManifest, resourcesDirectory, false);
	}
	//Start new texture manager
	this->_textureManager = new WCTextureManager(WCScene::TextureManifest, resourcesDirectory, false);
	
#ifdef __WIN32__
	this->_fontManager = new WCFontManager("", "", false);
#endif

	//Create new geometry context
	this->_geomContext = new WCGeometryContext(this->_glContext, this->_shaderManager);

	//Initialize the mouse button array
	for (int i=0; i < SCENE_MAX_MOUSEBUTTONS; i++) this->_mouseButtons[i] = false;
	//Create a default camera for the scene
	this->_defaultCamera = new WCCamera(this, "DefaultCamera");
	this->ActiveCamera(this->_defaultCamera);
	//Add in camera and selection layers
	this->_cameraLayer = new WCCameraLayer(this);
	this->RegisterLayer(this->_cameraLayer);
	//Check for errors
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCScene::WCScene Error - Unspecified Errors.");

	//Setup default GL rendering context
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glFrontFace(GL_CW);
	glEnable(GL_CULL_FACE);
}


WCScene::WCScene(xercesc::DOMElement* element, WCSerialDictionary *dictionary, WCGLContext *copyContext) : :: WCSerializeableObject(), 
	_glContext(NULL), _shaderManager(NULL), _textureManager(NULL), _geomContext(NULL), _isDirty(true),
	_useShadowPass(false), _useSelectionPass(false),
	_xMin(-1.0), _yMin(-1.0), _xMax(1.0), _yMax(1.0), _width(2.0), _height(2.0), _winWidth(1), _winHeight(1),
	_projectionInverse(true), _mouseX(0.0), _mouseY(0.0), _activeCamera(NULL), _defaultCamera(NULL),
	_firstResponder(NULL), _cameraLayer(NULL), _topLayer(NULL), _bottomLayer(NULL),
	_frameTickObject(NULL) {
	std::string resourcesDirectory = _ResourceDirectory();
	//Make sure element and dictionary are not null
	ASSERT(element);
	ASSERT(dictionary);
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

	//Make sure there is a context to copy
	ASSERT(copyContext);
	//Create a new gl Context (copying from a root context)
	this->_glContext = new WCGLContext(*copyContext);

	//Start new shader manager
	this->_shaderManager = new WCShaderManager(WCScene::ShaderManifest, resourcesDirectory, false);
	//Start new texture manager
	this->_textureManager = new WCTextureManager(WCScene::TextureManifest, resourcesDirectory, false);

#ifdef __WIN32__
	this->_fontManager = new WCFontManager("", "", false);
#endif

	//Create new geometry context
	this->_geomContext = new WCGeometryContext(this->_glContext, this->_shaderManager);
	//Register context guid
	WCGUID contextGuid = WCSerializeableObject::GetStringAttrib(element, "contextGUID");
	dictionary->InsertGUID(contextGuid, this->_geomContext);

	//Initialize the mouse button array
	for (int i=0; i < SCENE_MAX_MOUSEBUTTONS; i++) this->_mouseButtons[i] = false;

	//Create camera DOMNode
	this->_defaultCamera = new WCCamera( WCSerializeableObject::ElementFromName(element,"Camera"), dictionary);
	this->ActiveCamera(this->_defaultCamera);

	//Add in camera and selection layers
	this->_cameraLayer = new WCCameraLayer(this);
	this->RegisterLayer(this->_cameraLayer);
	//Check for errors
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCScene::WCScene Error - Unspecified Errors.");

	//Setup default GL rendering context
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glFrontFace(GL_CW);
	glEnable(GL_CULL_FACE);
}


WCScene::~WCScene() {
	//Release active camera and delete the default camera
	if (this->_activeCamera != NULL) this->_activeCamera->Release(*this);
	delete this->_defaultCamera;
	//Release remaining layers
	for (WCLayer* ptr=this->_topLayer; ptr != NULL; ptr=ptr->Next()) {
		//Release layers
		ptr->Release(*this);
	}
	//Delete camera and selection layers
	if (this->_cameraLayer != NULL) delete this->_cameraLayer;
	//Delete the shader manager
	if (this->_shaderManager != NULL) delete this->_shaderManager;
	//Delete the texture manager
	if (this->_textureManager != NULL) delete this->_textureManager;
	//Delete the geometryc context
	if (this->_geomContext != NULL) delete this->_geomContext;
#ifdef __APPLE__
	//Delete the GL context
//	CGLDestroyContext(this->_glContext);
#elif __WIN32__
	//Delete the font manager
	if (this->_fontManager != NULL) delete this->_fontManager;
#endif
}


bool WCScene::OnIdle(void) {
	//Test all the remaining layers
	for (WCLayer* ptr=this->_topLayer; ptr != NULL; ptr=ptr->Next())
		//Check to see if layer absorbs call
		if (ptr->OnIdle()) return true;
	//Otherwise
	return false;
}


bool WCScene::OnMouseMove(const int &x, const int &y) {
	//Check to make sure the mouse is within the scene
	if ((x < 0) || (y < 0) || (x > (int)this->_winWidth) || (y > (int)this->_winHeight)) return true;
	//Convert to x,y scene coords and save values
	this->_mouseX = ((x * this->_width) / this->_winWidth) - (this->_width / 2.0);
	this->_mouseY = (this->_height / 2.0) - ((y * this->_height) / this->_winHeight);
	
	//Handle firstResponder case
	if (this->_firstResponder != NULL) {
		if (!this->_firstResponder->OnMouseMove(this->_mouseX, this->_mouseY)) this->_firstResponder = NULL;
		else return true;
	}
	//Test all the remaining layers
	if (this->_firstResponder == NULL) {
		for (WCLayer* ptr=this->_topLayer; ptr != NULL; ptr=ptr->Next()) {
			//Check to see if layer absorbs call
			if (ptr->OnMouseMove(this->_mouseX, this->_mouseY)) return true;
		}
	}
	//Mouse was in scene
	return false;
}


bool WCScene::OnMouseDown(const WCMouseButton &button) {
	//Set the state for the button
	this->_mouseButtons[button._button] = true;
	//Check for hit in each layer
	for (WCLayer* ptr=this->_topLayer; ptr != NULL; ptr=ptr->Next()) {
		//Check to see if layer absorbs call
		if (ptr->OnMouseDown(button)) return true;
	}
	//Otherwise,
	return false;
}


bool WCScene::OnMouseUp(const WCMouseButton &button) {
	//Set the state for the button
	this->_mouseButtons[button._button] = false;
	//Handle firstResponder case
	if (this->_firstResponder != NULL) {
		if (this->_firstResponder->OnMouseUp(button)) return true;
	}
	//Otherwise,
	return false;
}


bool WCScene::OnScroll(const WPFloat &x, const WPFloat &y) {
	//Test all the remaining layers
	for (WCLayer* ptr=this->_topLayer; ptr != NULL; ptr=ptr->Next())
		//Check to see if layer absorbs call
		if (ptr->OnScroll(x, y)) return true;
	//Otherwise
	return false;
}


bool WCScene::OnArrowKeyPress(const WCArrowKey &key) {
	//Test all the remaining layers
	for (WCLayer* ptr=this->_topLayer; ptr != NULL; ptr=ptr->Next())
		//Check to see if layer absorbs call
		if (ptr->OnArrowKeyPress(key)) return true;
	//Otherwise
	return false;
}


bool WCScene::OnReshape(const int &width, const int &height) {
	//Update the screen size(in pixels)
	this->_winWidth = width;
	this->_winHeight = height;
	//Determine screen parameters
	this->_width = width / SCREEN_RESOLUTION_DPI;		//Roughly converts to inches
	this->_height = height / SCREEN_RESOLUTION_DPI;		//Roughly converts to inches
	//Calculate the x/y min/max parameters
	this->_xMax = this->_width * 0.5;
	this->_yMax = this->_height * 0.5;
	this->_xMin = -this->_xMax;
	this->_yMin = -this->_yMax;
	//Setup a new glViewport
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	//Reset the projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (this->_activeCamera->IsOrthographic())
		glOrtho(this->_xMin, this->_xMax, this->_yMin, this->_yMax, SCENE_ZMIN_DEFAULT, SCENE_ZMAX_DEFAULT);
	else {
/*** DEBUG ***/
//Need platform indep implementation
//		gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, SCENE_ZMIN_DEFAULT, SCENE_ZMAX_DEFAULT);
/*** DEBUG ***/
	}

	//Recalculate the inverse projection matrix
	GLfloat data[16];
	glGetFloatv(GL_TRANSPOSE_PROJECTION_MATRIX, data);
	WCMatrix4 proj(data[0], data[1], data[2], data[3],
				   data[4], data[5], data[6], data[7],
				   data[8], data[9], data[10], data[11],
				   data[12], data[13], data[14], data[15]);
	this->_projectionInverse = proj.Inverse();
	this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);
	
	//Update all of the layers
	for (WCLayer* ptr=this->_topLayer; ptr != NULL; ptr=ptr->Next()) {
		ptr->OnReshape(width, height);
	}	
	//Make sure to mark the scene as dirty
	this->_isDirty = true;
	return false;
}


void WCScene::RegisterLayer(WCLayer *layer, WCLayer *mark) {
	//Make sure layer is not NULL
	if (layer == NULL) { 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCScene::RegisterLayer Error - NULL Layer passed."); 
		return; }
	//Make sure layer name is not ""
	if (layer->Name() == "") {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCScene::RegisterLayer Error - Layer name must not be empty."); 
		return; }
	//If layer list is empty insert as top and bottom
	if ((this->_topLayer == NULL) || (this->_bottomLayer == NULL)) {
		this->_topLayer = layer;
		this->_bottomLayer = layer;
		layer->Next(NULL);
		layer->Previous(NULL);
	}
	//Else if mark is NULL or mark is front of list
	else if ((mark == NULL)||(mark == this->_topLayer)) {
		this->_topLayer->Previous(layer);
		layer->Next(this->_topLayer);
		layer->Previous(NULL);
		this->_topLayer = layer;
	}
	//Otherwise, add in front of mark
	else {
		//Insert layer before mark
		layer->Previous(mark->Previous());
		mark->Previous()->Next(layer);
		mark->Previous(layer);
		layer->Next(mark);
	}
	//Retain the layer
	layer->Retain(*this);
}


void WCScene::UnregisterLayer(WCLayer *layer) {
	//Make sure layer is not NULL
	if (layer == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCScene::UnregisterLayer Error - NULL Layer passed."); 
		return; }
	//Remove the layer from the list
	if (layer == this->_topLayer) this->_topLayer = layer->Next();
	if (layer == this->_bottomLayer) this->_bottomLayer = layer->Previous();
	if (layer->Next() != NULL) layer->Next()->Previous( layer->Previous() );
	if (layer->Previous() != NULL) layer->Previous()->Next( layer->Next() );
	//Mark layer next and previous as null
	layer->Next(NULL);
	layer->Previous(NULL);
	//Release the layer
	layer->Release(*this);
}


void WCScene::LayerAbove(WCLayer *layer, WCLayer *mark) {
	//Make sure layer is not NULL
	if (layer == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCScene::LayerAbove Error - NULL Layer passed."); 
		return; }
	//Remove the layer from its current place
	if (layer == this->_topLayer) this->_topLayer = layer->Next();
	if (layer == this->_bottomLayer) this->_bottomLayer = layer->Previous();
	if (layer->Next() != NULL) layer->Next()->Previous( layer->Previous() );
	if (layer->Previous() != NULL) layer->Previous()->Next( layer->Next() );	
	//If mark is NULL or mark = topLayer
	if ((mark == NULL)||(mark == this->_topLayer)) {
		this->_topLayer->Previous(layer);
		layer->Next(this->_topLayer);
		layer->Previous(NULL);
		this->_topLayer = layer;
	}
	//Otherwise, add in front of mark
	else {
		//Insert layer before mark
		layer->Previous(mark->Previous());
		mark->Previous()->Next(layer);		
		mark->Previous(layer);
		layer->Next(mark);
	}
}


void WCScene::LayerBelow(WCLayer *layer, WCLayer *mark) {
	//Make sure layer is not NULL
	if (layer == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCScene::LayerBelow Error - NULL Layer passed.");
		return; }
	//Remove the layer from its current place
	if (layer == this->_topLayer) this->_topLayer = layer->Next();
	if (layer == this->_bottomLayer) this->_bottomLayer = layer->Previous();
	if (layer->Next() != NULL) layer->Next()->Previous( layer->Previous() );
	if (layer->Previous() != NULL) layer->Previous()->Next( layer->Next() );	
	//If mark is NULL or mark = bottomLayer
	if ((mark == NULL)||(mark == this->_bottomLayer)) {
		this->_bottomLayer->Next(layer);
		layer->Previous(this->_bottomLayer);
		layer->Next(NULL);
		this->_bottomLayer = layer;
	}
	//Otherwise, add in back of mark
	else {
		//Insert layer before mark
		layer->Next(mark->Next());
		mark->Next()->Previous(layer);
		mark->Next(layer);
		layer->Previous(mark);
	}
}


WCLayer* WCScene::LayerFromName(std::string name) {
	//Dont search for empty name
	if (name == "") return NULL;
	//Look through all layers for one with name
	for (WCLayer* ptr=this->_topLayer; ptr != NULL; ptr=ptr->Next())
		if (ptr->Name() == name) return ptr;
	//No such layer found
	return NULL;
}



void WCScene::ActiveCamera(WCCamera* camera) {
	//Make sure camera is not null
	if (camera == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSceneActiveCamera Error - NULL Camera passed."); 
		return; }
	//Release the current camera
	if (this->_activeCamera != NULL) this->_activeCamera->Release(*this);
	//Set the active camera
	this->_activeCamera = camera;
	//Retain the camera
	this->_activeCamera->Retain(*this);	
}

	
void WCScene::RegisterLightSource(WCLightSource* light) {
	//Make sure light is not NULL
	if (light == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCScene::RegisterLightSource Error - NULL Light passed."); 
		return; }
	//Make sure not to exceed light limit
	if (this->_lightSources.size() == SCENE_MAX_LIGHTSOURCES) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCScene::RegisterLightSource Error - Lightsource limit reached."); 
		return; }
	//Add the light into the list
	this->_lightSources.push_back(light);
	//Retain the light
	light->Retain(*this);
}


void WCScene::UnregisterLightSource(WCLightSource* light) {
	//Make sure light is not NULL
	if (light == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCScene::UnregisterLightSource Error - NULL Light passed.");
		return; }
	//Remove the light from the list
	this->_lightSources.remove(light);
	//Release the light
	light->Release(*this);
}


WCAlignedBoundingBox WCScene::BoundingBox(void) {
	//Create a holding box
	WCAlignedBoundingBox box;
	for (WCLayer* ptr=this->_bottomLayer; ptr != NULL; ptr=ptr->Previous()) 
		box += ptr->BoundingBox();	
	return box;
}


void WCScene::Render(void) {
	//Make sure the context is active
	this->_glContext->MakeActive();
	//Mark the scene as clean
	this->_isDirty = false;
	GLenum err;
	
	//Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		
	//Render from light sources
	if (this->_useShadowPass) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCScene::Render Error - Shadow pass not yet implemented.");
	}
	//Now set up the camera (all objects from here are camera dependent)
	this->_activeCamera->ReadyScene();
	//Render primary layers (in reverse order - bottom to top)
	for (WCLayer* ptr=this->_bottomLayer; ptr != NULL; ptr=ptr->Previous()) {
		//Render the layer
		ptr->Render(this->_activeCamera->RenderState());
		//Check for errors
		err = glGetError();
		if (err != GL_NO_ERROR)
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCScene::Render Layers Error: " << std::hex << err);
	}
	//Call frametick if present
	if (this->_frameTickObject != NULL) (*this->_frameTickObject)();
	//Flush the context
	this->_glContext->FlushBuffer();

	//Check for GL errors
	err = glGetError();
	if (err != GL_NO_ERROR)
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCScene::Render Error: " << std::hex << err);
}


void WCScene::ReceiveNotice(WCObjectMsg count, WCObject *sender) {
	//Nothing to do for now
}


xercesc::DOMElement* WCScene::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create primary element for this object
	XMLCh* xmlString = xercesc::XMLString::transcode("Scene");
	xercesc::DOMElement*  element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);

	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);
	//Add context GUID attribute
	WCGUID contextGuid = dictionary->InsertAddress(this->_geomContext);
	WCSerializeableObject::AddStringAttrib(element, "contextGUID", contextGuid);
	//Set window width attribute
	WCSerializeableObject::AddFloatAttrib(element, "windowwidth", this->_winWidth);
	//Set window height attribute
	WCSerializeableObject::AddFloatAttrib(element, "windowheight", this->_winHeight);
	//Need to include camera
	xercesc::DOMElement* cameraElem = this->_activeCamera->Serialize(document, dictionary);
	element->appendChild(cameraElem);

	//Need to include lights
	xmlString = xercesc::XMLString::transcode("LightSources");
	xercesc::DOMElement* lightsElem = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	xercesc::DOMElement* lightElem;
	std::list<WCLightSource*>::iterator iter;
	//Loop through all lights
	for (iter = this->_lightSources.begin(); iter != this->_lightSources.end(); iter++) {
		//Serialize each light
		lightElem = (*iter)->Serialize(document, dictionary);
		//If there is an actual serializatin element, add it to the list
		if (lightElem != NULL) lightsElem->appendChild(lightElem);
	}
	//Append list of lights
	element->appendChild(lightsElem);
	//Return the scene element
	return element;
}


/***********************************************~***************************************************/


std::ostream& __WILDCAT_NAMESPACE__::operator<<(std::ostream& out, const WCScene &scene) {
	//Print out basic surface info
	out << "Scene(*:" << (void*)&scene << ", T:" << scene._topLayer << ", B:" << scene._bottomLayer << ")\n";

	//Only print if layers are present
	if (scene._topLayer != NULL) {
		out << "\t\tLayers: " << std::endl;
		//Print out control point information
		for (WCLayer* ptr=scene._topLayer; ptr != NULL; ptr=ptr->Next())
			std::cout << "\t\t\t" << ptr->Name() << "(*:" << ptr << ", P:" << ptr->Previous() << ", N:" << ptr->Next() << ")\n";
	}
	//Return the output
	return out;
}


/***********************************************~***************************************************/

GLuint WCScene::ProgramFromName(const std::string &name) {
	// Get a program from a name
	if(this->_shaderManager == NULL)return 0;
	return this->_shaderManager->ProgramFromName(name);
}
