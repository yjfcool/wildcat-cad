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
#include "Kernel/document.h"


/***********************************************~***************************************************/


WCActionPartPlaneCreate::WCActionPartPlaneCreate(WCFeature *creator, const std::string &planeName, const WCVector4 &p0,
	const WCVector4 &p1, const WCVector4 &p2) : ::WCAction("Create Part Plane", creator), _plane(NULL), _planeName(planeName), _p0(p0), _p1(p1), _p2(p2) {
	//Nothing else for now
}

WCFeature* WCActionPartPlaneCreate::Execute(void) {
	//Create the plane
	WCPartPlane *plane = new WCPartPlane(this->_creator, this->_planeName, this->_p0, this->_p1, this->_p2);
	//Make sure plane is not null
	if (plane == NULL)
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionPartPlaneCreate::Execute - Plane could not be created.");
	//Set the plane pointer and return
	this->_plane = plane;
	return plane;
}


xercesc::DOMElement* WCActionPartPlaneCreate::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Create primary element for this object
	xercesc::DOMElement*  actionElement = document->createElement(xercesc::XMLString::transcode("ActionPartPlaneCreate"));
	//Add the creator attribute
	actionElement->setAttribute(xercesc::XMLString::transcode("Creator"), xercesc::XMLString::transcode("12345"));
	//Add the plane name attribute
	actionElement->setAttribute(xercesc::XMLString::transcode("PlaneName"), xercesc::XMLString::transcode(this->_planeName.c_str()));
	//Add elements for each vector
	xercesc::DOMElement* p0Element = document->createElement(xercesc::XMLString::transcode("P0"));
	this->_p0.ToElement(p0Element);
	actionElement->appendChild(p0Element);
	xercesc::DOMElement* p1Element = document->createElement(xercesc::XMLString::transcode("P1"));
	this->_p1.ToElement(p1Element);
	actionElement->appendChild(p1Element);
	xercesc::DOMElement* p2Element = document->createElement(xercesc::XMLString::transcode("P2"));
	this->_p2.ToElement(p2Element);
	actionElement->appendChild(p2Element);
	return actionElement;
}


/***********************************************~***************************************************/


