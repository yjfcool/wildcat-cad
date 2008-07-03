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
#include "Kernel/workbench.h"
#include "Kernel/feature.h"
#include "Kernel/document.h"
#include "Kernel/workbench_layer.h"
#include "Kernel/drawing_mode.h"
#include "Kernel/selection_mode.h"
#include "Application/keymap.h"
#include <xercesc/framework/MemBufFormatTarget.hpp>


/***********************************************~***************************************************/


WCWorkbench::WCWorkbench(WCFeature *feature, const std::string &name) : ::WCObject(), _name(name), 
	_feature(feature), _drawMode(NULL), _layer(NULL), _selectionManager(NULL), 
	_defaultView(name + "-default", WCQuaternion(0.0, 0.0, 0.0, 1.0)), 
	_restoreView(name + "-restore", WCQuaternion(0.0, 0.0, 0.0, 1.0)),
	_isPan(false), _isZoom(false), _isRotate(false), _isMultiSelect(false) {
	//Make sure document is not null
	if (this->_feature == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCWorkbench::WCWorkbench - NULL feature passed.");
		//throw error
		return;
	}
	//Create the layer for the sketch
	this->_layer = new WCWorkbenchLayer(this->_feature->Document()->Scene(), this->_name + ".layer");
	//Create and retain the selectionManager
	this->_selectionManager = new WCSelectionManager(this->_feature->Document()->Scene());
	this->_selectionManager->Retain(*this);
	//Create the keymap
	this->_keyMap = new WCKeyMap();

	//Add standard items (slightly difference based on platform)
//Apple mappings
#ifdef __APPLE__ 
	this->_keyMap->AddMapping( WCKeyEvent('z', false, false, false, true, false), WCUserMessage("undo") );
	this->_keyMap->AddMapping( WCKeyEvent('Z', false, false, false, true, false), WCUserMessage("redo") );
#endif
//Windows mappings
#ifdef __WIN32__
	this->_keyMap->AddMapping( WCKeyEvent('z', true, false, false, false, false), WCUserMessage("undo") );
	this->_keyMap->AddMapping( WCKeyEvent('y', true, false, false, false, false), WCUserMessage("redo") );
#endif
}


WCWorkbench::~WCWorkbench() {
	//See about exiting the workbench
	if (this->_feature->Document()->ActiveWorkbench() == this) 
		this->_feature->Document()->ExitWorkbench(); 
	//Delete the layer
	if (this->_layer != NULL) delete this->_layer;
	//See about deleting the mode
	if (this->_drawMode != NULL) delete this->_drawMode;
	//Delete the selectionManager
	if (this->_selectionManager != NULL) {
		this->_selectionManager->Release(*this);
		delete this->_selectionManager;
	}
	//Delete the keymap
	if (this->_keyMap != NULL) delete this->_keyMap;
}


bool WCWorkbench::IsActive(void) const {
	//True if this workbench is the document's active workbench
	return (this == this->_feature->Document()->ActiveWorkbench());
}


bool WCWorkbench::DrawingMode(WCDrawingMode *mode) {
	if (mode == NULL) {
		//NULL-NULL case
		if (this->_drawMode == NULL) return false;
		//NULL-Non-NULL case
		this->_drawMode->OnExit();
		delete this->_drawMode;
	}
	//If both are non-null check for same mode
	else if (this->_drawMode != NULL) {
		if (*this->_drawMode == *mode) return false;
		//NULL-Non-NULL case
		this->_drawMode->OnExit();
		delete this->_drawMode;		
	}
	//Set the drawMode
	this->_drawMode = mode;
	//Set the layer to the same drawMode
	this->_layer->DrawingMode(this->_drawMode);
	//If drawMode is now NULL, make layer inactive
	if (this->_drawMode == NULL) this->_layer->IsActive(false);
	//Otherwise, activate layer and enter drawMode
	else {
		this->_layer->IsActive(true);
		//Enter the drawMode
		this->_drawMode->OnEntry();
		//Also render the drawMode
		this->_drawMode->Render();
	}
	//Good results
	return true;
}


/***********************************************~***************************************************/


bool WCWorkbench::OnEnter(void) {
	//Register the sketch drawing layer - and put behind UI
	this->_feature->Document()->Scene()->RegisterLayer(this->_layer);
	this->_feature->Document()->Scene()->LayerBelow(this->_layer, this->_feature->Document()->UserInterfaceLayer());
	//Record the current view
	this->_restoreView = this->_feature->Document()->Scene()->ActiveCamera()->NamedView();
	//Orient the view to the drawing plane
	this->_feature->Document()->Scene()->ActiveCamera()->AnimateToViewpoint(this->_defaultView, WORKBENCH_ANIMATE_DURATION);
	//Set workbench mode to selection
//	this->DrawingMode( WCDrawingMode::Selection(this) );
	this->DrawingMode( new WCSelectionMode(this) );
	return true;
}


bool WCWorkbench::OnExit(void) {
	//Unregister the sketch drawing layer
	this->_feature->Document()->Scene()->UnregisterLayer(this->_layer);
	//Set workbench mode to default
	this->DrawingMode(WCDrawingMode::Default());
	//Capture the current view - if not animating
	if (!this->_feature->Document()->Scene()->ActiveCamera()->IsAnimating())
		this->_defaultView = this->_feature->Document()->Scene()->ActiveCamera()->NamedView();
	//Return to the saved entry view
	this->_feature->Document()->Scene()->ActiveCamera()->AnimateToViewpoint(this->_restoreView, WORKBENCH_ANIMATE_DURATION);
	return true;
}


void WCWorkbench::OnPanPress(void) {
	//Mark pan as true
	this->_isPan = true;
	this->_feature->Document()->Scene()->LayerAbove( this->_feature->Document()->Scene()->CameraLayer());
	this->_feature->Document()->Scene()->CameraLayer()->Mode(WCCameraMode::Pan());
}


void WCWorkbench::OnPanRelease(void) {
	//Mark pan as false
	this->_isPan = false;
	this->_feature->Document()->Scene()->LayerBelow( this->_feature->Document()->Scene()->CameraLayer());
}


void WCWorkbench::OnZoomPress(void) {
	//Mark zoom as true
	this->_isZoom = true;
	this->_feature->Document()->Scene()->LayerAbove( this->_feature->Document()->Scene()->CameraLayer());
	this->_feature->Document()->Scene()->CameraLayer()->Mode(WCCameraMode::Zoom());
}


void WCWorkbench::OnZoomRelease(void) {
	//Mark zoom as false
	this->_isZoom = false;
	this->_feature->Document()->Scene()->LayerBelow( this->_feature->Document()->Scene()->CameraLayer());
}


void WCWorkbench::OnRotatePress(void) {
	//Mark rotate as true
	this->_isRotate = true;
	this->_feature->Document()->Scene()->LayerAbove( this->_feature->Document()->Scene()->CameraLayer());
	this->_feature->Document()->Scene()->CameraLayer()->Mode(WCCameraMode::Rotate());
}


void WCWorkbench::OnRotateRelease(void) {
	//Mark rotate as false
	this->_isRotate = false;
	this->_feature->Document()->Scene()->LayerBelow( this->_feature->Document()->Scene()->CameraLayer());
}


void WCWorkbench::OnMultiSelectPress(void) {
	//Mark select as true
	this->_isMultiSelect = true;
}


void WCWorkbench::OnMultiSelectRelease(void) {
	//Mark select as false
	this->_isMultiSelect = false;
}


void WCWorkbench::OnIdle(void) {
	//Pass event to scene
	this->_feature->Document()->Scene()->OnIdle();
}


void WCWorkbench::OnMouseMove(const int x, const int y) {
	//Pass movement on to the scene
	this->_feature->Document()->Scene()->OnMouseMove(x, y);
}


void WCWorkbench::OnMouseDown(const WCMouseButton &button) {
	//Pass mouse event to scene
	this->_feature->Document()->Scene()->OnMouseDown(button);
}


void WCWorkbench::OnMouseUp(const WCMouseButton &button) {
	//Pass mouse event to scene
	this->_feature->Document()->Scene()->OnMouseUp(button);	
}


void WCWorkbench::OnScroll(const float &x, const float &y) {
	//Pass scroll wheel event to scene
	this->_feature->Document()->Scene()->OnScroll(x, y);
}

void WCWorkbench::OnArrowKeyPress(const WCArrowKey &key) {
	//Pass arrow key event to scene
	this->_feature->Document()->Scene()->OnArrowKeyPress(key);	
}


void WCWorkbench::OnReshape(const int width, const int height) {
	//Pass event to scene
	this->_feature->Document()->Scene()->OnReshape(width, height);
}


bool  WCWorkbench::OnUserMessage(const WCUserMessage &message) {
	//Is this a view mode message
	if (message == "isoview")			{ this->NamedView("Isometric"); return true; }
	else if (message == "frontview")	{ this->NamedView("Front"); return true; }
	else if (message == "backview")		{ this->NamedView("Back"); return true; }
	else if (message == "leftview")		{ this->NamedView("Left"); return true; }
	else if (message == "rightview")	{ this->NamedView("Right"); return true; }
	else if (message == "topview")		{ this->NamedView("Top"); return true; }
	else if (message == "bottomview")	{ this->NamedView("Bottom"); return true; }

	//Handle undo, redo
	else if (message == "undo")			{ return this->_feature->Document()->Undo(1); }
	else if (message == "redo")			{ return this->_feature->Document()->Redo(1); }

	//Otherwise, did not capture
	return false;
}


void WCWorkbench::Render(void) {
	//Draw the scene
	this->_feature->Document()->Scene()->Render();
}


/***********************************************~***************************************************/


bool WCWorkbench::Open(const std::string &filename) {
	return false;
}


bool WCWorkbench::Close(void) {
	return false;
}


bool WCWorkbench::New(void) {
	return false;
}


bool WCWorkbench::NewFrom(void) {
	return false;
}


bool WCWorkbench::Save(void) {
	return false;
}


bool WCWorkbench::SaveAs(const std::string &filename) {
	//Create DOMImplementation
	XMLCh *xmlString = xercesc::XMLString::transcode("Core");
	xercesc::DOMImplementation* impl =  xercesc::DOMImplementationRegistry::getDOMImplementation(xmlString);
	xercesc::XMLString::release(&xmlString);

	//Create root of document
	xmlString = xercesc::XMLString::transcode("WildcatRoot");
	xercesc::DOMDocument* doc = impl->createDocument(0, xmlString, 0);
	xercesc::XMLString::release(&xmlString);

	//Create serialization dictionary
	WCSerialDictionary* dictionary = new WCSerialDictionary();

	//Build full document of noes
	xercesc::DOMElement* rootElem = doc->getDocumentElement();
	xercesc::DOMElement* docElem = this->_feature->Serialize(doc, dictionary);
	rootElem->appendChild(docElem);
/*
	xercesc::DOMElement*  prodElem = doc->createElement(XMLString::transcode("product"));
	rootElem->appendChild(prodElem);
	DOMText*    prodDataVal = doc->createTextNode(XMLString::transcode("Xerces-C"));
	prodElem->appendChild(prodDataVal);
	xercesc::DOMElement*  catElem = doc->createElement(XMLString::transcode("category"));
	rootElem->appendChild(catElem);
	catElem->setAttribute(XMLString::transcode("idea"), XMLString::transcode("great"));
	DOMText*    catDataVal = doc->createTextNode(XMLString::transcode("XML Parsing Tools"));
	catElem->appendChild(catDataVal);
	xercesc::DOMElement*  devByElem = doc->createElement(XMLString::transcode("developedBy"));
	rootElem->appendChild(devByElem);
	DOMText*    devByDataVal = doc->createTextNode(XMLString::transcode("Apache Software Foundation"));
	devByElem->appendChild(devByDataVal);
*/
	//Create the writer
	xercesc::DOMWriter* theSerializer = ((xercesc::DOMImplementationLS*)impl)->createDOMWriter();

	// optionally you can set some features on this serializer
	if (theSerializer->canSetFeature(xercesc::XMLUni::fgDOMWRTDiscardDefaultContent, true))
		theSerializer->setFeature(xercesc::XMLUni::fgDOMWRTDiscardDefaultContent, true);

	if (theSerializer->canSetFeature(xercesc::XMLUni::fgDOMWRTFormatPrettyPrint, true))
		theSerializer->setFeature(xercesc::XMLUni::fgDOMWRTFormatPrettyPrint, true);

	// StdOutFormatTarget prints the resultant XML stream
	// to stdout once it receives any thing from the serializer.
	xercesc::MemBufFormatTarget *myFormTarget = new xercesc::MemBufFormatTarget();

	try {
		// do the serialization through DOMWriter::writeNode();
		theSerializer->writeNode(myFormTarget, *rootElem);
		std::cout << "Buffer: " << myFormTarget->getRawBuffer() << std::endl;
		std::ofstream outputStream;
		outputStream.open(filename.c_str());
		if( !outputStream ) {
			std::cerr << "Error opening output stream" << std::endl;
			return false;
		}
		//Write the xml output
		unsigned int size = myFormTarget->getLen();
		char* data = (char*)myFormTarget->getRawBuffer();
		outputStream.write(data, size);
		outputStream.close();
	}
	catch (const xercesc::XMLException& toCatch) {
		char* message = xercesc::XMLString::transcode(toCatch.getMessage());
		std::cout << "Exception message is: \n" << message << "\n";
		xercesc::XMLString::release(&message);
		return false;
	}
	catch (const xercesc::DOMException& toCatch) {
		char* message = xercesc::XMLString::transcode(toCatch.msg);
		std::cout << "Exception message is: \n" << message << "\n";
		xercesc::XMLString::release(&message);
		return false;
	}
	catch (...) {
		std::cout << "Unexpected Exception \n" ;
		return false;
	}
	theSerializer->release();
//	delete myErrorHandler;
//	delete myFilter;
	delete myFormTarget;

	//Now count the number of elements in the above DOM tree.
	unsigned int elementCount = doc->getElementsByTagName(xercesc::XMLString::transcode("*"))->getLength();
	XERCES_STD_QUALIFIER cout << "The tree just created contains: " << elementCount << " elements." << XERCES_STD_QUALIFIER endl;
	doc->release();
	return true;
}


bool WCWorkbench::SaveAll(void) {
	return false;
}


bool WCWorkbench::Print(void) {
	return false;
}


bool WCWorkbench::DocumentProperties(void) {
	return false;
}


/***********************************************~***************************************************/


bool WCWorkbench::Cut(void) {
	return false;
}


bool WCWorkbench::Copy(void) {
	return false;
}


bool WCWorkbench::Paste(void) {
	return false;
}


bool WCWorkbench::PasteSpecial(void) {
	return false;
}


bool WCWorkbench::Undo(void) {
	return false;
}


bool WCWorkbench::Redo(void) {
	return false;
}


bool WCWorkbench::Delete(void) {
	return false;
}


bool WCWorkbench::Properties(void) {
	return false;
}


bool WCWorkbench::DocumentSearch(void) {
	return false;
}


bool WCWorkbench::SelectionSet(void) {
	return false;
}


/***********************************************~***************************************************/


bool WCWorkbench::Toolbars(void) {
	return false;
}


bool WCWorkbench::CommandsList(void) {
	return false;
}


bool WCWorkbench::Geometry(void) {
	return false;
}


bool WCWorkbench::Specifications(void) {
	return false;
}


bool WCWorkbench::Compass(void) {
	return false;
}


bool WCWorkbench::Tree(void) {
	return false;
}
	

bool WCWorkbench::ZoomToFit(void) {
	this->_feature->Document()->Scene()->ActiveCamera()->FitToWindow();
	return true;
}


bool WCWorkbench::ZoomIn(const WPFloat &factor) {
	return false;
}


bool WCWorkbench::ZoomOut(const WPFloat &factor) {
	return false;
}


bool WCWorkbench::Pan(const WPFloat &x, const WPFloat &y) {
	return false;
}


bool WCWorkbench::Rotate(void) {
	return false;
}


bool WCWorkbench::LookAt(void) {
	return false;
}


bool WCWorkbench::NamedView(const std::string &name) {
	return this->_feature->Document()->NamedView(name);
}


bool WCWorkbench::RenderStyle(void)	{
	return false;
}


bool WCWorkbench::NavigationMode(void) {
	return false; 
}


bool WCWorkbench::Lighting(void) {
	return false;
}


bool WCWorkbench::DepthEffect(void) {
	return false;
}


bool WCWorkbench::Ground(void) {
	return false;
}


bool WCWorkbench::Magnifier(void) {
	return false;
}


bool WCWorkbench::HideShow(void) {
	return false;
}



/***********************************************~***************************************************/

