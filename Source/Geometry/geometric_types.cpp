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
#include "Geometry/geometric_types.h"


/*** Static Member Initialization ***/
WPFloat WCGeometricCurve::DefaultThickness = GEOMETRICCURVE_THICKNESS;


/***********************************************~***************************************************/


WCGeometricObject::WCGeometricObject(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCSerializeableObject(), ::WCVisualObject(), _isConstruction(false) {
	//Make sure element if not null
	if (element == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometricObject::WCGeometricObject - NULL Element passed.");
		//throw error
		return;
	}
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

	//Get the construction flag
	this->_isConstruction = WCSerializeableObject::GetBoolAttrib(element, "construction");
	//Get the visible flag
	this->_isVisible = WCSerializeableObject::GetBoolAttrib(element, "visible");
	//Get the renderProg
	this->_renderProg = (GLuint)WCSerializeableObject::GetFloatAttrib(element, "renderProg");
	//Get the color value
	this->_color.FromElement(  WCSerializeableObject::ElementFromName(element,"Color") );
}


xercesc::DOMElement* WCGeometricObject::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create primary element for this object
	XMLCh* xmlString = xercesc::XMLString::transcode("GeometricObject");
	xercesc::DOMElement* element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);

	//Set construction attribute
	WCSerializeableObject::AddBoolAttrib(element, "construction", this->_isConstruction);
	//Set visible attribute
	WCSerializeableObject::AddBoolAttrib(element, "visible", this->_isVisible);
	//Set the renderProg attribute
	WCSerializeableObject::AddFloatAttrib(element, "renderProg", this->_renderProg);
	//Add the color value
	this->_color.ToElement(element, "Color");

	//Return the new element
	return element;
}


/***********************************************~***************************************************/


WCGeometricCurve::WCGeometricCurve(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCGeometricObject( WCSerializeableObject::ElementFromName(element,"GeometricObject"), dictionary),
	_isClosed(), _isSelfIntersecting(), _thickness() {
	//Make sure element if not null
	if (element == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometricCurve::WCGeometricCurve - NULL Element passed.");
		//throw error
		return;
	}
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

	//Get the closed flag
	this->_isClosed = WCSerializeableObject::GetBoolAttrib(element, "closed");
	//Get the self-intersection flag
	this->_isSelfIntersecting = WCSerializeableObject::GetBoolAttrib(element, "selfintersecting");
	//Get the thickness value
	this->_thickness = WCSerializeableObject::GetFloatAttrib(element, "thickness");
}


xercesc::DOMElement* WCGeometricCurve::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create primary element for this object
	XMLCh* xmlString = xercesc::XMLString::transcode("GeometricCurve");
	xercesc::DOMElement* element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);
	//Include the parent element
	xercesc::DOMElement* geometricElement = this->WCGeometricObject::Serialize(document, dictionary);
	element->appendChild(geometricElement);

	//Set closed attribute
	WCSerializeableObject::AddBoolAttrib(element, "closed", this->_isClosed);
	//Set self-intersection attribute
	WCSerializeableObject::AddBoolAttrib(element, "selfintersecting", this->_isSelfIntersecting);
	//Set thickness attribute
	WCSerializeableObject::AddFloatAttrib(element, "thickness", this->_thickness);

	//Return the new element
	return element;
}


/***********************************************~***************************************************/


WCGeometricSurface::WCGeometricSurface(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCGeometricObject( WCSerializeableObject::ElementFromName(element,"GeometricObject"), dictionary),
	_isClosedU(), _isClosedV(), _isSelfIntersecting() {
	//Make sure element if not null
	if (element == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometricSurface::WCGeometricSurface - NULL Element passed.");
		//throw error
		return;
	}
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

	//Get the closed-U flag
	this->_isClosedU = WCSerializeableObject::GetBoolAttrib(element, "closedu");
	//Get the closed-V flag
	this->_isClosedV = WCSerializeableObject::GetBoolAttrib(element, "closedv");
	//Get the self-intersection flag
	this->_isSelfIntersecting = WCSerializeableObject::GetBoolAttrib(element, "selfintersecting");
}


xercesc::DOMElement* WCGeometricSurface::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create primary element for this object
	XMLCh* xmlString = xercesc::XMLString::transcode("GeometricSurface");
	xercesc::DOMElement* element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);
	//Include the parent element
	xercesc::DOMElement* geometricElement = this->WCGeometricObject::Serialize(document, dictionary);
	element->appendChild(geometricElement);

	//Set closed-u attribute
	WCSerializeableObject::AddBoolAttrib(element, "closedu", this->_isClosedU);
	//Set closed-v attribute
	WCSerializeableObject::AddBoolAttrib(element, "closedv", this->_isClosedV);
	//Set self-intersection attribute
	WCSerializeableObject::AddBoolAttrib(element, "selfintersecting", this->_isSelfIntersecting);

	//Return the new element
	return element;
}


/***********************************************~***************************************************/

