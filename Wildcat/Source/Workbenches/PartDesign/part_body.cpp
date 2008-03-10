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
#include "PartDesign/part_body.h"
#include "PartDesign/part.h"
#include "Kernel/document.h"


/***********************************************~***************************************************/


WCBody::WCBody(WCPart *part, const std::string name) : ::WCPartFeature(part, name) {

	//Check feature name
	if (this->_name == "") this->_name = this->_part->GenerateFeatureName(this);
	//Create event handler
	this->_controller = new WCBodyController(this);
	//Create tree element
	WSTexture* bodyIcon = this->_document->Scene()->TextureManager()->TextureFromName("body32");
	this->_treeElement = new WCTreeElement(this->_document->TreeView(), this->_name, this->_controller, bodyIcon);
	//Add tree view element
	this->_creator->TreeElement()->AddLastChild(this->_treeElement);
	//Open the tree element
	this->_treeElement->IsOpen(true);
	//Add the body to the document
	this->_part->AddFeature(this, false);
	//Check to see if part has a current body
	if (this->_part->Body() == NULL) this->_part->Body(this);
}

WCBody::WCBody(xercesc::DOMElement *element, WCSerialDictionary *dictionary) : 
	::WCPartFeature( WCSerializeableObject::ElementFromName(element,"PartFeature"), dictionary ) {

	//Make sure element if not null
	if (element == NULL) return;
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

	//Create event handler
	this->_controller = new WCBodyController(this);
	//Create tree element
	WSTexture* bodyIcon = this->_document->Scene()->TextureManager()->TextureFromName("body32");
	this->_treeElement = new WCTreeElement(this->_document->TreeView(), this->_name, this->_controller, bodyIcon);
	//Add tree view element
	this->_creator->TreeElement()->AddLastChild(this->_treeElement);
	//Open the tree element
	this->_treeElement->IsOpen(true);
	//Add the body to the document
	this->_part->AddFeature(this, false);
	//Check to see if part has a current body
	if (this->_part->Body() == NULL) this->_part->Body(this);
}


WCBody::~WCBody() {
	//Remove the body from the document
	this->_part->RemoveFeature(this);
	//Delete objects
	if (this->_treeElement != NULL) delete this->_treeElement;
	if (this->_controller != NULL) delete this->_controller;
}


void WCBody::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
}


xercesc::DOMElement* WCBody::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create the base element for the object
	XMLCh* xmlString = xercesc::XMLString::transcode("PartBody");
	xercesc::DOMElement* element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Include the part feature element
	xercesc::DOMElement* featureElement = this->WCPartFeature::Serialize(document, dictionary);
	element->appendChild(featureElement);

	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);

	//Return the primary element
	return element;
}


/***********************************************~***************************************************/


WCActionPartBodyCreate* WCBody::ActionCreate(WCPart *part, const std::string &bodyName) {
	//Create a new body create action
	return new WCActionPartBodyCreate(part, bodyName);
}



/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCBody &body) {
	return out;
}


/***********************************************~***************************************************/
