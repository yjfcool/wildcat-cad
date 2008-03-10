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
#include "PartDesign/part_pad_actions.h"
#include "PartDesign/part_pad.h"
#include "PartDesign/part_body.h"
#include "Kernel/document.h"


/***********************************************~***************************************************/


WCActionPadCreate::WCActionPadCreate(WCBody *body, const std::string &padName, const std::list< std::pair<WCSketchProfile*,bool> > &profiles,
	const WCVector4 &direction,	const WPFloat &posDepth, const WPFloat &negDepth) : ::WCAction("Create Pad", body), _body(body),
	_padName(padName), _profiles(profiles), _direction(direction), _posDepth(posDepth), _negDepth(negDepth), _pad(NULL) {
	//Nothing else for now
}


WCFeature* WCActionPadCreate::Execute(void) {
	//Create the pad
	WCPad *pad = new WCPad(this->_body, this->_padName, this->_profiles, this->_direction, this->_posDepth, this->_negDepth);
	//Make sure pad is not null
	if (pad == NULL) 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionPadCreate::Execute - Pad could not be created.");
	//Set the pointer and return
	this->_pad = pad;
	return this->_pad;
}


xercesc::DOMElement* WCActionPadCreate::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Create primary element for this object
	xercesc::DOMElement*  actionElement = document->createElement(xercesc::XMLString::transcode("PadCreate"));
	//Add the creator attribute
	actionElement->setAttribute(xercesc::XMLString::transcode("Creator"), xercesc::XMLString::transcode("12345"));
	//Add the plane name attribute
	actionElement->setAttribute(xercesc::XMLString::transcode("padName"), xercesc::XMLString::transcode(this->_padName.c_str()));
	return actionElement;
}


/***********************************************~***************************************************/

