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
#include "RTVisualization/vis_actions.h"
#include "RTVisualization/visualization.h"
#include "Kernel/document.h"


/***********************************************~***************************************************


WCActionPartCreate::WCActionPartCreate(const std::string &partName, const std::string &filename) : 
	::WCAction("Create Part", NULL), _part(NULL), _partName(partName), _filename(filename) {
	//Nothing else for now
}

WCFeature* WCActionPartCreate::Execute(void) {
	//Create the part
	WCPart *part = new WCPart(this->_partName, this->_filename);
	//Make sure part is not null
	if (part == NULL)
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCPartAction::Execute - Part could not be created.");
	//Enter the workbench
	part->EnterWorkbench(part->Workbench());
	//Record the part
	this->_part = part;
	return this->_part;
}


xercesc::DOMElement* WCActionPartCreate::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Create primary element for this object
	xercesc::DOMElement*  actionElement = document->createElement(xercesc::XMLString::transcode("PartCreate"));
	//Add the plane name attribute
	actionElement->setAttribute(xercesc::XMLString::transcode("PartName"), xercesc::XMLString::transcode(this->_partName.c_str()));
	return actionElement;
}


/***********************************************~***************************************************


WCActionPartDelete::WCActionPartDelete(WCPart *part, std::list<WCPartFeature*> features) :
	::WCAction("Delete Part Elements", part), _part(part) {
	//Make sure part is not null
	if (part == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionPartDelete::WCActionPartDelete - NULL sketch passed."); 
		return;		
	}
	//Copy list of part features
	this->_features = features;
}


WCFeature* WCActionPartDelete::Execute(void) {
	bool retVal = this->_part->DeleteFeatures(this->_features);
	//Set up count string
	std::ostringstream os;
	os << this->_features.size();
	std::string countStr = os.str();
	//Quick final consistency check
	if (retVal) {
		//Update document status
		this->_part->Document()->Status("Deleted " + countStr + " elements");
		CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCActionPartDelete::Execute - Deleted " << this->_features.size() << " elements.");
	}
	//Failure case - not able to delete something
	else {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionPartDelete::Execute - Were not able to delete all elements.");
		//throw error
		return NULL;
	}
	//All is good
	return this->_part;
}


xercesc::DOMElement* WCActionPartDelete::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	return NULL;
}


/***********************************************~***************************************************/

