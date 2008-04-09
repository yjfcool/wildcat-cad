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
#include "RTVisualization/vis_listener_actions.h"
#include "RTVisualization/vis_udp_listener.h"
#include "RTVisualization/visualization.h"
#include "Kernel/document.h"


/***********************************************~***************************************************/


WCActionVisUDPListenerCreate::WCActionVisUDPListenerCreate(WCFeature *creator, const std::string &listenerName,
	const unsigned int &port) : ::WCAction("Create Vis UDP Listener", creator),
	_listener(NULL), _listenerName(listenerName), _port(port) {
	//Nothing else for now
}


WCActionVisUDPListenerCreate::WCActionVisUDPListenerCreate(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCAction( WCSerializeableObject::ElementFromName(element,"Action"), dictionary ),
	_listener(NULL), _listenerName(""), _port(0) {
	//Do something here
}


WCFeature* WCActionVisUDPListenerCreate::Execute(void) {
	//Update pointers (based on rollback flag)
	if (this->_rollback) {
		this->_creator = (WCVisualization*)this->_dictionary->AddressFromGUID(this->_visGUID);
	}

	//Create the UDP listener
	WCVisualization *vis = dynamic_cast<WCVisualization*>(this->_creator);
	WCVisUDPListener *listener = new WCVisUDPListener(vis, this->_listenerName, this->_port);
	//Make sure listener is not null
	if (listener == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionVisUDPListenerCreate::Execute - Listener could not be created.");
		return NULL;
	}

	//If a rollback execution, update guid and address
	if (this->_rollback) this->_dictionary->UpdateAddress(this->_guid, listener);
	//Set the listener pointer and return
	this->_listener = listener;
	return listener;
}


bool WCActionVisUDPListenerCreate::Rollback(void) {
	//If the object is preset
	if (this->_listener != NULL) {
		//Set the rollback flag
		this->_rollback = true;
		//Set self guid
		this->_guid = this->_dictionary->InsertAddress(this->_listener);
		//Delete the object
		delete this->_listener;
		//Record GUIDs for visualization
		this->_visGUID = this->_dictionary->GUIDFromAddress(this->_creator);
		//Return success
		return true;
	}
	//Not rolled back
	return false;
}


xercesc::DOMElement* WCActionVisUDPListenerCreate::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create primary element for this object
	XMLCh* xmlString = xercesc::XMLString::transcode("ActionVisUDPListenerCreate");
	xercesc::DOMElement* element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Include the parent element
	xercesc::DOMElement* featureElement = this->WCAction::Serialize(document, dictionary);
	element->appendChild(featureElement);
	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);

	//Add name attribute
	WCSerializeableObject::AddStringAttrib(element, "listenerName", this->_listenerName);
	//Add port attribute
	WCSerializeableObject::AddFloatAttrib(element, "port", this->_port);
	
	//Return the new element
	return element;
}


/***********************************************~***************************************************/


