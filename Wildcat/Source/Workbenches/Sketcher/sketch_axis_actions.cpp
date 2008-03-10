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
#include "Sketcher/sketch_axis_actions.h"
#include "Sketcher/sketch_axis.h"
#include "Sketcher/sketch.h"


/***********************************************~***************************************************/


WCActionSketchAxisCreate::WCActionSketchAxisCreate(WCSketch *sketch, const std::string &axisName, const WCVector4 &begin,
	const WCVector4 &end) : ::WCAction("Create Sketch Axis", sketch), _axis(NULL),
	_axisName(axisName), _sketch(sketch), _begin(begin), _end(end) {
	//Validate sketch
	if (sketch == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionSketchAxisCreate::WCActionSketchAxisCreate Error - NULL References passed.");
		//throw error
		return;
	}
}


WCFeature* WCActionSketchAxisCreate::Execute(void) {
	//Update pointers (based on rollback flag)
	if (this->_rollback) {
		this->_sketch = (WCSketch*)this->_dictionary->AddressFromGUID(this->_sketchGUID);
	}

	//Create the axis
	WCSketchAxis *axis = new WCSketchAxis(this->_sketch, this->_axisName, this->_begin, this->_end);
	//Make sure object is not null
	if (axis == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionSketchAxisCreate::Execute Error - Sketch Axis could not be created.");
		return NULL;
	}

	//If a rollback execution, update guid and address
	if (this->_rollback) this->_dictionary->UpdateAddress(this->_guid, axis);
	//Set the pointer
	this->_axis = axis;
	return this->_sketch;
}


bool WCActionSketchAxisCreate::Rollback(void) {
	//If the object is preset
	if (this->_axis != NULL) {
		//Set the rollback flag
		this->_rollback = true;
		//Set self guid
		this->_guid = this->_dictionary->InsertAddress(this->_axis);
		//Delete the object
		delete this->_axis;
		//Record GUIDs for sketch
		this->_sketchGUID = this->_dictionary->GUIDFromAddress(this->_sketch);
		//Return success
		return true;
	}
	//Not rolled back
	return false;
}


xercesc::DOMElement* WCActionSketchAxisCreate::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	return NULL;
}


/***********************************************~***************************************************/


WCActionSketchAxisMove::WCActionSketchAxisMove(WCSketchAxis *axis, const WCVector4 &begin, const WCVector4 &end) :
	::WCAction("Move Axis Line", axis->Sketch()), _axis(axis),
	_begin(begin),  _end(end), _oldBegin(), _oldEnd() {
	//Validate axis
	if (axis == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionSketchAxisMove::WCActionSketchAxisMove Error - NULL References passed.");
		//throw error
		return;
	}
}


WCActionSketchAxisMove::WCActionSketchAxisMove(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCAction( WCSerializeableObject::ElementFromName(element,"Action"), dictionary ),
	_axis(NULL), _begin(), _end(), _oldBegin(), _oldEnd() {
	//Do something here
}


WCFeature* WCActionSketchAxisMove::Execute(void) {
	//Update object based on rollback flag
	if (this->_rollback) this->_axis = (WCSketchAxis*)this->_dictionary->AddressFromGUID(this->_axisGUID);

	//Record the current position
	this->_oldBegin = this->_axis->Begin();
	this->_oldEnd = this->_axis->End();
	//Move the object
	this->_axis->Set(this->_begin, this->_end);
	//Return the sketch
	return this->_axis->Sketch();
}


bool WCActionSketchAxisMove::Rollback(void) {
	//If the object is preset
	if (this->_axis != NULL) {
		//Set the rollback flag
		this->_rollback = true;
		//Set self guid
		this->_guid = this->_dictionary->InsertAddress(this);
		//Move the object back to original position
		this->_axis->Set(this->_oldBegin, this->_oldEnd);
		//Record GUID for object
		this->_axisGUID = this->_dictionary->GUIDFromAddress(this->_axis);
		//Return success
		return true;
	}
	//Not rolled back
	return false;
}


xercesc::DOMElement* WCActionSketchAxisMove::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	return NULL;
}


/***********************************************~***************************************************/

