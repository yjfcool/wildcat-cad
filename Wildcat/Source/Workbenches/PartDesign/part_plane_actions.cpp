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
#include "PartDesign/part_plane_actions.h"
#include "PartDesign/part_plane.h"
#include "PartDesign/part.h"
#include "Kernel/document.h"


/***********************************************~***************************************************/


WCActionPartPlaneCreate::WCActionPartPlaneCreate(WCFeature *creator, const std::string &planeName, const WCVector4 &p0,
	const WCVector4 &p1, const WCVector4 &p2) : ::WCAction("Create Part Plane", creator),
	_plane(NULL), _planeName(planeName), _p0(p0), _p1(p1), _p2(p2) {
	//Nothing else for now
}


WCActionPartPlaneCreate::WCActionPartPlaneCreate(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCAction( WCSerializeableObject::ElementFromName(element,"Action"), dictionary ),
	_plane(NULL), _planeName(""), _p0(), _p1(), _p2() {
	//Do something here
}


WCFeature* WCActionPartPlaneCreate::Execute(void) {
	//Update pointers (based on rollback flag)
	if (this->_rollback) {
		this->_creator = (WCPart*)this->_dictionary->AddressFromGUID(this->_partGUID);
	}

	//Create the plane
	WCPartPlane *plane = new WCPartPlane(this->_creator, this->_planeName, this->_p0, this->_p1, this->_p2);
	//Make sure plane is not null
	if (plane == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionPartPlaneCreate::Execute - Plane could not be created.");
		return NULL;
	}


	//If a rollback execution, update guid and address
	if (this->_rollback) this->_dictionary->UpdateAddress(this->_guid, plane);
	//Set the plane pointer and return
	this->_plane = plane;
	return plane;
}


bool WCActionPartPlaneCreate::Rollback(void) {
	//If the object is preset
	if (this->_plane != NULL) {
		//Set the rollback flag
		this->_rollback = true;
		//Set self guid
		this->_guid = this->_dictionary->InsertAddress(this->_plane);
		//Delete the object
		delete this->_plane;
		//Record GUIDs for sketch
		this->_partGUID = this->_dictionary->GUIDFromAddress(this->_creator);
		//Return success
		return true;
	}
	//Not rolled back
	return false;
}


xercesc::DOMElement* WCActionPartPlaneCreate::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create primary element for this object
	XMLCh* xmlString = xercesc::XMLString::transcode("ActionPartPlaneCreate");
	xercesc::DOMElement* element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Include the parent element
	xercesc::DOMElement* featureElement = this->WCAction::Serialize(document, dictionary);
	element->appendChild(featureElement);
	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);

	//Add name attribute
	WCSerializeableObject::AddStringAttrib(element, "planeName", this->_planeName);
	//Add p0 vector
	xmlString = xercesc::XMLString::transcode("P0");
	xercesc::DOMElement *p0Element = document->createElement(xmlString);
	this->_p0.ToElement(p0Element);
	element->appendChild(p0Element);
	xercesc::XMLString::release(&xmlString);
	//Add p1 vector
	xmlString = xercesc::XMLString::transcode("P1");
	xercesc::DOMElement *p1Element = document->createElement(xmlString);
	this->_p1.ToElement(p1Element);
	element->appendChild(p1Element);
	xercesc::XMLString::release(&xmlString);
	//Add p2 vector
	xmlString = xercesc::XMLString::transcode("P2");
	xercesc::DOMElement *p2Element = document->createElement(xmlString);
	this->_p2.ToElement(p2Element);
	element->appendChild(p2Element);
	xercesc::XMLString::release(&xmlString);
	
	//Return the new element
	return element;
}


/***********************************************~***************************************************/


