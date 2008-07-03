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
#include "PartDesign/part_body_actions.h"
#include "PartDesign/part_body.h"
#include "PartDesign/part.h"
#include "Kernel/document.h"


/***********************************************~***************************************************/


WCActionPartBodyCreate::WCActionPartBodyCreate(WCPart *part, const std::string &bodyName) : ::WCAction("Create PartBody", part),
	_part(part), _bodyName(bodyName), _partBody(NULL) {
	//Nothing else for now
}


WCActionPartBodyCreate::WCActionPartBodyCreate(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCAction( WCSerializeableObject::ElementFromName(element,"Action"), dictionary ),
	_part(NULL), _bodyName(""), _partBody(NULL) {
	//Do something here
}


WCFeature* WCActionPartBodyCreate::Execute(void) {
	//Update pointers (based on rollback flag)
	if (this->_rollback) {
		this->_part = (WCPart*)this->_dictionary->AddressFromGUID(this->_partGUID);
	}

	//Create the body
	WCPartBody *body = new WCPartBody(this->_part, this->_bodyName);
	//Make sure body is not null
	if (body == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionPartBodyCreate::Execute - Body could not be created.");
		return NULL;
	}
	
	//If a rollback execution, update guid and address
	if (this->_rollback) this->_dictionary->UpdateAddress(this->_guid, body);
	//Set the pointer and return
	this->_partBody = body;
	return this->_partBody;
}


bool WCActionPartBodyCreate::Rollback(void) {
	//If the object is preset
	if (this->_partBody != NULL) {
		//Set the rollback flag
		this->_rollback = true;
		//Set self guid
		this->_guid = this->_dictionary->InsertAddress(this->_partBody);
		//Delete the object
		delete this->_partBody;
		//Record GUIDs for sketch
		this->_partGUID = this->_dictionary->GUIDFromAddress(this->_part);
		//Return success
		return true;
	}
	//Not rolled back
	return false;
}


xercesc::DOMElement* WCActionPartBodyCreate::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create primary element for this object
	XMLCh* xmlString = xercesc::XMLString::transcode("ActionPartBodyCreate");
	xercesc::DOMElement* element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Include the parent element
	xercesc::DOMElement* featureElement = this->WCAction::Serialize(document, dictionary);
	element->appendChild(featureElement);
	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);

	//Add part attribute
	WCSerializeableObject::AddGUIDAttrib(element, "part", this->_part, dictionary);
	//Add body name attribute
	WCSerializeableObject::AddStringAttrib(element, "bodyName", this->_bodyName);
	
	//Return the new element
	return element;
}


/***********************************************~***************************************************/

