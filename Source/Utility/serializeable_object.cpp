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
#include "Utility/serializeable_object.h"
#include "Utility/log_manager.h"


/***********************************************~***************************************************/


void WCSerializeableObject::AddFloatAttrib(xercesc::DOMElement *element, const std::string &name, const WPFloat &value) {
	//Convert value to xmlstring
	std::ostringstream os;
	os.precision(8);
	os << value;
	std::string valueStr = os.str();
	//Set attrib name and value
	XMLCh* attribName = xercesc::XMLString::transcode( name.c_str() );
	XMLCh* attribValue = xercesc::XMLString::transcode( valueStr.c_str() );
	//Add attributes to element
	element->setAttribute(attribName, attribValue);
	//Release attrib name and value
	xercesc::XMLString::release(&attribName);
	xercesc::XMLString::release(&attribValue);
}


void WCSerializeableObject::AddStringAttrib(xercesc::DOMElement *element, const std::string &name, const std::string &value) {
	//Set attrib name and value
	XMLCh* attribName = xercesc::XMLString::transcode( name.c_str() );
	XMLCh* attribValue = xercesc::XMLString::transcode( value.c_str() );
	//Add attributes to element
	element->setAttribute(attribName, attribValue);
	//Release attrib name and value
	xercesc::XMLString::release(&attribName);
	xercesc::XMLString::release(&attribValue);
}


void WCSerializeableObject::AddBoolAttrib(xercesc::DOMElement *element, const std::string &name, const bool &value) {
	//Set attrib name and value
	XMLCh* attribName = xercesc::XMLString::transcode( name.c_str() );
	XMLCh* attribValue;
	//Switch on value
	if (value) attribValue = xercesc::XMLString::transcode("true");
	else attribValue = xercesc::XMLString::transcode("false");
	//Add attributes to element
	element->setAttribute(attribName, attribValue);
	//Release attrib name and value
	xercesc::XMLString::release(&attribName);
	xercesc::XMLString::release(&attribValue);
}


void WCSerializeableObject::AddGUIDAttrib(xercesc::DOMElement *element, const std::string &name, const void* address, WCSerialDictionary *dictionary) {
	//Set attrib name and value
	XMLCh* attribName = xercesc::XMLString::transcode( name.c_str() );
	WCGUID guid = dictionary->GUIDFromAddress(address);
	XMLCh* attribValue = xercesc::XMLString::transcode( guid.c_str() );
	//Add attributes to element
	element->setAttribute(attribName, attribValue);
	//Release attrib name and value
	xercesc::XMLString::release(&attribName);
	xercesc::XMLString::release(&attribValue);
}


float WCSerializeableObject::GetFloatAttrib(xercesc::DOMElement *element, const std::string &name) {
	//Get attribute and node for the name
	XMLCh* attribName = xercesc::XMLString::transcode( name.c_str() );
	//Get the value of the attribute
	const XMLCh* attribValue = element->getAttribute(attribName);
	xercesc::XMLString::release(&attribName);
	//Convert to std::string
	char *str = xercesc::XMLString::transcode(attribValue);
	std::string strValue(str);
	delete str;
	//Convert to float
	float value = (float)atof(strValue.c_str());
	//Return the value	
	return value;
}


std::string WCSerializeableObject::GetStringAttrib(xercesc::DOMElement *element, const std::string &name) {
	//Get attribute and node for the name
	XMLCh* attribName = xercesc::XMLString::transcode( name.c_str() );
	//Get the value of the attribute
	const XMLCh* attribValue = element->getAttribute(attribName);
	xercesc::XMLString::release(&attribName);
	//Convert to std::string
	char *str = xercesc::XMLString::transcode(attribValue);
	std::string value(str);
	delete str;
	//Return the value	
	return value;
}


bool WCSerializeableObject::GetBoolAttrib(xercesc::DOMElement *element, const std::string &name) {
	//Get attribute and node for the name
	XMLCh* attribName = xercesc::XMLString::transcode( name.c_str() );
	//Get the value of the attribute
	const XMLCh* attribValue = element->getAttribute(attribName);
	xercesc::XMLString::release(&attribName);
	//Convert to std::string
	char *str = xercesc::XMLString::transcode(attribValue);
	std::string strValue(str);
	delete str;
	//Return the value
	if (strValue == "false") return false;
	else return true;
}


void* WCSerializeableObject::GetGUIDAttrib(xercesc::DOMElement *element, const std::string &name, WCSerialDictionary *dictionary) {
	//Get attribute and node for the name
	XMLCh* attribName = xercesc::XMLString::transcode( name.c_str() );
	//Get the value of the attribute
	const XMLCh* attribValue = element->getAttribute(attribName);
	xercesc::XMLString::release(&attribName);
	//Convert to WCGUID
	char *str = xercesc::XMLString::transcode(attribValue);
	WCGUID guid(str);
	delete str;
	void* value = dictionary->AddressFromGUID(guid);
	//Return the value	
	return value;
}


xercesc::DOMElement* WCSerializeableObject::ElementFromName(xercesc::DOMElement* element, const std::string &name) {
	//Get the xmlstring
	XMLCh* xmlString = xercesc::XMLString::transcode(name.c_str());
	//Get the node list
	xercesc::DOMNodeList* list = element->getElementsByTagName(xmlString);
	//Make sure there is at least one
	if (list->getLength() != 1) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSerializeableObject::ElementFromName - " << list->getLength() << " copies of " << name << " found.");
		return NULL;
	}
	//Make sure is of type element
	if (list->item(0)->getNodeType() != xercesc::DOMNode::ELEMENT_NODE) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSerializeableObject::ElementFromName - Returned node not of type element");
		return NULL;	
	}
	xercesc::XMLString::release(&xmlString);
	//Get the first element
	return (xercesc::DOMElement*)list->item(0);
}

 
/***********************************************~***************************************************/


xercesc::DOMElement* WCSerializeableObject::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);

	//Create reference set list
	XMLCh* xmlString = xercesc::XMLString::transcode("SerializeableObject");
	xercesc::DOMElement* element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);

	//Set attrib name and value
	XMLCh* attribName = xercesc::XMLString::transcode("guid");
	XMLCh* attribValue = xercesc::XMLString::transcode( guid.c_str() );
	//Add attributes to element
	element->setAttribute(attribName, attribValue);
	//Release attrib name and value
	xercesc::XMLString::release(&attribName);
	xercesc::XMLString::release(&attribValue);

	//Return the full list of references
	return element;
}


/***********************************************~***************************************************/


WCSerialDictionary::WCSerialDictionary(xercesc::DOMElement* element) {
	//...
}


WCGUID WCSerialDictionary::InsertAddress(const void *address) {
	//Lookup address in address map
	std::map<void*,WCGUID>::iterator mapIter = this->_addressMap.find((void*)address);
	//If found
	if (mapIter != this->_addressMap.end()) {
		//Already in map, so just return the GUID
		return (*mapIter).second;
	}
	//If not found
	else {
		WCGUID newGUID;
		//Check for null address
		if (address == NULL) newGUID = "NULL";
		else {
			//Generate GUID
			std::ostringstream os;
			os << address;
			newGUID = os.str();
		}
		//Insert address and GUID into both maps
		this->_addressMap.insert( std::make_pair((void*)address, newGUID) );
		this->_guidMap.insert( std::make_pair(newGUID, (void*)address) );
		//Return the new GUID
		return newGUID;
	}
}


WCGUID WCSerialDictionary::GUIDFromAddress(const void *address) {
	//For now this executes exactly the same as InsertAddress
	return this->InsertAddress(address);
}


void WCSerialDictionary::InsertGUID(const WCGUID &guid, const void* address) {
	//See if already in guidMap
	std::map<WCGUID,void*>::iterator mapIter = this->_guidMap.find(guid);
	//If found
	if (mapIter != this->_guidMap.end()) {
		//Make sure address is updated
		if ((*mapIter).second != address) this->UpdateAddress(guid, address);
	}
	//If not found
	else {
		//Insert guid and address into maps
		this->_guidMap.insert( std::make_pair(guid,(void*)address) );
		this->_addressMap.insert( std::make_pair((void*)address, guid) );
	}
}


void* WCSerialDictionary::AddressFromGUID(const WCGUID &guid) {
	//Lookup guid in guid map
	std::map<WCGUID,void*>::iterator mapIter = this->_guidMap.find(guid);
	//If found
	if (mapIter != this->_guidMap.end()) {
		//Just return the address
		return (*mapIter).second;
	}
	//If not found
	else {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSerialDictionary::AddressFromGUID - GUID not found");
		//Return NULL
		return NULL;
	}
}


void WCSerialDictionary::UpdateAddress(const WCGUID &guid, const void* address) {
	//Lookup guid in guid map
	std::map<WCGUID,void*>::iterator guidIter = this->_guidMap.find(guid);
	//If found
	if (guidIter != this->_guidMap.end()) {
//		std::cout << "Switching address: " << (*guidIter).second << " to " << address << std::endl;
		//Lookup address in address map
		std::map<void*,WCGUID>::iterator addressIter = this->_addressMap.find( (*guidIter).second );
		//Update the address in guidMap
		(*guidIter).second = (void*)address;
		//Remove addressIter from addressMap
		this->_addressMap.erase(addressIter);
		//Add new pair into addressMap
		this->_addressMap.insert( std::make_pair( (void*)address, guid) );
	}
	//If not found
	else {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSerialDictionary::UpdateAddress - GUID not found");
	}
}


void WCSerialDictionary::Clear(void) {
	//Clear the guidMap
	this->_guidMap.clear();
	//Clear the addressMap
	this->_addressMap.clear();
}


xercesc::DOMElement* WCSerialDictionary::Serialize(xercesc::DOMDocument *document, WCSerialDictionary* dictionary) {
	return NULL;
}


/***********************************************~***************************************************/

