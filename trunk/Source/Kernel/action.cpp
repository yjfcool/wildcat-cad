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
#include <Kernel/action.h>
#include <Kernel/feature.h>
#include <Kernel/document.h>


/***********************************************~***************************************************/


WCAction::WCAction(std::string actionName, WCFeature *creator) : ::WCSerializeableObject(),
	_name(actionName), _creator(creator), _guid(), _dictionary(NULL), _rollback(false) {
	//If creator is valid
	if (this->_creator != NULL) this->_dictionary = this->_creator->Document()->UndoDictionary();
}


WCAction::WCAction(xercesc::DOMElement *element, WCSerialDictionary *dictionary) : ::WCSerializeableObject(),
	_name(), _creator(NULL), _guid(), _dictionary(NULL), _rollback(false) {
	//Make sure element if not null
	if (element == NULL) return;
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

	//Get feature attribute
	this->_name = WCSerializeableObject::GetStringAttrib(element, "name");
	//Get creator attribute
	this->_creator = (WCFeature*)WCSerializeableObject::GetGUIDAttrib(element, "creator", dictionary);
}


xercesc::DOMElement* WCAction::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create primary element for this object
	XMLCh* xmlString = xercesc::XMLString::transcode("Action");
	xercesc::DOMElement*  element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);

	//Set feature name attribute
	WCSerializeableObject::AddStringAttrib(element, "name", this->_name);
	//Set creator attribute
	WCSerializeableObject::AddGUIDAttrib(element, "creator", this->_creator, dictionary);

	//Return the primary element
	return element;}


/***********************************************~***************************************************/


void WCMacroAction::AddAction(WCAction *action) {
}
	

WCFeature* WCMacroAction::Execute(void) {
	return NULL;
}


bool WCMacroAction::Merge(WCAction *action) {
	return false;
}


bool WCMacroAction::Rollback(void) {
	return false;
}


/***********************************************~***************************************************/


bool WCAction::Deserialize(xercesc::DOMElement* actionElement, WCSerialDictionary *dictionary) {
	//Make sure feature element is not null
	if ((actionElement == NULL) || (dictionary == NULL)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCAction::Deserialize - Action element or dictionary was NULL.");
		//throw error
		return false;
	}
	//Get the action name
	const XMLCh* xmlString = actionElement->getTagName();
	std::string name( xercesc::XMLString::transcode(xmlString) );
	std::cout << "Action: " << name << std::endl;
	return false;
}


std::ostream& __WILDCAT_NAMESPACE__::operator<<(std::ostream& out, const WCAction &action) {
	//Print out some info
	out << "Action: " << action._name << " (" << &action << ")\n";
	return out;
}


/***********************************************~***************************************************/

