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
#include "PartDesign/part_shaft_actions.h"
#include "PartDesign/part_shaft.h"
#include "PartDesign/part_body.h"
#include "Kernel/document.h"


/***********************************************~***************************************************/


WCActionShaftCreate::WCActionShaftCreate(WCPartBody *body, const std::string &shaftName, const std::list< std::pair<WCSketchProfile*,bool> > &profiles,
	WCSketchAxis *axis,	const bool profilesOnRight, const WPFloat &cwAngle, const WPFloat &ccwAngle) : ::WCAction("Create Shaft", body), _body(body),
	_shaftName(shaftName), _profiles(profiles), _axis(axis), _profilesOnRight(profilesOnRight),
	_cwAngle(cwAngle), _ccwAngle(ccwAngle), _shaft(NULL) {
	//Nothing else for now
}


WCFeature* WCActionShaftCreate::Execute(void) {
	//Create the shaft
	WCShaft *shaft = new WCShaft(this->_body, this->_shaftName, this->_profiles, this->_axis, this->_profilesOnRight, 
		this->_cwAngle, this->_ccwAngle);
	//Make sure shaft is not null
	if (shaft == NULL) 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionShaftCreate::Execute - Shaft could not be created.");
	//Set the pointer and return
	this->_shaft = shaft;
	return this->_shaft;
}


xercesc::DOMElement* WCActionShaftCreate::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Create primary element for this object
	xercesc::DOMElement*  actionElement = document->createElement(xercesc::XMLString::transcode("ShaftCreate"));
	//Add the creator attribute
	actionElement->setAttribute(xercesc::XMLString::transcode("Creator"), xercesc::XMLString::transcode("12345"));
	//Add the object name attribute
	actionElement->setAttribute(xercesc::XMLString::transcode("shaftName"), xercesc::XMLString::transcode(this->_shaftName.c_str()));
	return actionElement;
}


/***********************************************~***************************************************/

