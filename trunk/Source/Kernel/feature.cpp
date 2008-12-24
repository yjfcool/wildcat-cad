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
#include <Kernel/feature.h>
#include <Kernel/document.h>


/***********************************************~***************************************************/


WCFeature::WCFeature(WCFeature *creator, const std::string &name) : ::WCSerializeableObject(), //::WCVisualObject(), ::WCSelectionObject(),
	_name(name), _document(NULL), _creator(creator), _controller(NULL), _treeElement(NULL),
	_isFeatureDirty(false), _isFixed(false), _isConstruction(false), _isActive(true) {
	//Check if creator is not NULL
	if (this->_creator != NULL) this->_document = this->_creator->_document;
}


WCFeature::WCFeature(xercesc::DOMElement *element, WCSerialDictionary *dictionary) : ::WCSerializeableObject(), 
	_name(), _document(NULL), _creator(NULL), _controller(NULL), _treeElement(NULL),
	_isFeatureDirty(false), _isFixed(false), _isConstruction(false) {
	//Make sure element if not null
	if (element == NULL) return;
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

	//Get feature attribute
	this->_name = WCSerializeableObject::GetStringAttrib(element, "name");
	//Get creator attribute
	this->_creator = (WCFeature*)WCSerializeableObject::GetGUIDAttrib(element, "creator", dictionary);
	//Try to set document
	if (this->_creator != NULL) this->_document = this->_creator->_document;
	//Get fixed attribute
	this->_isFixed = WCSerializeableObject::GetBoolAttrib(element, "fixed");
	//Get construction attribute
	this->_isConstruction = WCSerializeableObject::GetBoolAttrib(element, "construction");
	//Get active attribute
	this->_isActive = WCSerializeableObject::GetBoolAttrib(element, "active");
}


WCFeature::~WCFeature() {
	//If there is a tree element, destroy it
	if (this->_treeElement != NULL) {
		//Delete the tree element (which will remove it from the tree)
		delete this->_treeElement;
	}
	//If there is a controller, destroy it
	if (this->_controller != NULL) {
		//Delete the controller
		delete this->_controller;
	}
}


void WCFeature::MarkDirty(void) {
	//Mark this as feature dirty
	this->_isFeatureDirty = true;
	//Mark document as dirty
	this->_document->MarkDirty();
}


xercesc::DOMElement* WCFeature::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create primary element for this object
	XMLCh* xmlString = xercesc::XMLString::transcode("Feature");
	xercesc::DOMElement*  element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);

	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);
	//Set feature name attribute
	WCSerializeableObject::AddStringAttrib(element, "name", this->_name);
//	//Set feature document attribute
//	WCSerializeableObject::AddGUIDAttrib(element, "document", this->_document, dictionary);
	//Set feature creator attribute
	WCSerializeableObject::AddGUIDAttrib(element, "creator", this->_creator, dictionary);
	//Set isFixed boolean attribute
	WCSerializeableObject::AddBoolAttrib(element, "fixed", this->_isFixed);
	//Set isConstruction boolean attribute
	WCSerializeableObject::AddBoolAttrib(element, "construction", this->_isConstruction);
	//Set isActive boolean attribute
	WCSerializeableObject::AddBoolAttrib(element, "active", this->_isActive);

	//Return the primary element
	return element;
}



/***********************************************~***************************************************/


std::ostream& __WILDCAT_NAMESPACE__::operator<<(std::ostream& out, const WCFeature &feature) {
	//Print out some info
	out << feature._name << " (" << &feature << ")\n";
	return out;
}


/***********************************************~***************************************************/


