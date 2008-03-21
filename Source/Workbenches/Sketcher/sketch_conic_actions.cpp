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
#include "Sketcher/sketch_conic_actions.h"
#include "Sketcher/sketch_conic_twopoint.h"
#include "Sketcher/sketch.h"


/***********************************************~***************************************************/


WCActionSketchConicTwoPointCreate::WCActionSketchConicTwoPointCreate(WCSketch *sketch, const std::string &conicName,
	const WCVector4 &start, const WCVector4 &startTangent, const WCVector4 &end, const WCVector4 &endTangent, const WCVector4 &fifthPoint) : 
	::WCAction("Create Sketch Two-Point Conic", sketch), _conic(NULL), _conicName(conicName), _sketch(sketch), 
	_start(start), _startTangent(startTangent), _end(end), _endTangent(endTangent), _fifthPoint(fifthPoint) {
	//Validate sketch
	if (sketch == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionSketchConicTwoPointCreate::WCActionSketchConicTwoPointCreate Error - NULL Sketch passed.");
		//throw error
		return;
	}
}


WCFeature* WCActionSketchConicTwoPointCreate::Execute(void) {
	WCSketchConicTwoPoint *conic = NULL;
	//Create the conic
	conic = new WCSketchConicTwoPoint(this->_sketch, this->_conicName, this->_start, this->_startTangent,
		this->_end, this->_endTangent, this->_fifthPoint);
	//Make sure conic is not null
	if (conic == NULL)
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionSketchConicTwoPointCreate::Execute Error - Sketch conic could not be created.");
	//Set the pointer
	this->_conic = conic;
	return this->_sketch;
}


bool WCActionSketchConicTwoPointCreate::Merge(WCAction *action) { 
	return false; 
}


bool WCActionSketchConicTwoPointCreate::Rollback(void) {
	return false;
}


xercesc::DOMElement* WCActionSketchConicTwoPointCreate::Serialize(xercesc::DOMDocument *document) {
	return NULL;
}


WCActionSketchConicTwoPointCreate* WCActionSketchConicTwoPointCreate::Deserialize(xercesc::DOMElement* obj) {
	return NULL;
}


bool WCActionSketchConicTwoPointCreate::Validate(xercesc::DOMElement* obj) {
	return false;
}


/***********************************************~***************************************************/

