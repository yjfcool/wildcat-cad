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
#include "Sketcher/sketch_point_actions.h"
#include "Sketcher/sketch_point.h"
#include "Sketcher/sketch.h"


/***********************************************~***************************************************/


WCActionSketchPointCreate::WCActionSketchPointCreate(WCSketch *sketch, const std::string &pointName, const WCVector4 &pos) :
	::WCAction("Create Sketch Point", sketch), _point(NULL), _pointName(pointName), _sketch(sketch), _position(pos) {
	//Nothing else to do for now
}


WCActionSketchPointCreate::WCActionSketchPointCreate(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCAction( WCSerializeableObject::ElementFromName(element,"Action"), dictionary ),
	_point(NULL), _pointName(""), _sketch(NULL), _position() {
	//Do something here
}


WCFeature* WCActionSketchPointCreate::Execute(void) {
	//Update pointers (based on rollback flag)
	if (this->_rollback) {
		this->_sketch = (WCSketch*)this->_dictionary->AddressFromGUID(this->_sketchGUID);
	}

	//Create the object
	WCSketchPoint *point = new WCSketchPoint(this->_sketch, this->_pointName, this->_position);
	//Make sure object is not null
	if (point == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionSketchPointCreate::Execute Error - Sketch Point could not be created.");
		return NULL;
	}
	
	//If a rollback execution, update guid and address
	if (this->_rollback) this->_dictionary->UpdateAddress(this->_guid, point);
	//Set the pointer
	this->_point = point;
	return this->_sketch;
}


bool WCActionSketchPointCreate::Rollback(void) {
	//If the object is preset
	if (this->_point != NULL) {
		//Set the rollback flag
		this->_rollback = true;
		//Set self guid
		this->_guid = this->_dictionary->InsertAddress(this->_point);
		//Delete the object
		delete this->_point;
		//Record GUIDs for sketch
		this->_sketchGUID = this->_dictionary->GUIDFromAddress(this->_sketch);
		//Return success
		return true;
	}
	//Not rolled back
	return false;
}


xercesc::DOMElement* WCActionSketchPointCreate::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	return NULL;
}


/***********************************************~***************************************************/


WCActionSketchPointMove::WCActionSketchPointMove(WCSketchPoint *point, const WCVector4 &pos) : 
	::WCAction("Move Sketch Point", point->Sketch()), _point(point), _position(pos), _oldPosition() {
	//Nothing else to do for now
}


WCActionSketchPointMove::WCActionSketchPointMove(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCAction( WCSerializeableObject::ElementFromName(element,"Action"), dictionary ),
	_point(NULL), _position(), _oldPosition() {
	//Do something here
}


WCFeature* WCActionSketchPointMove::Execute(void) {
	//Update line based on rollback flag
	if (this->_rollback) this->_point = (WCSketchPoint*)this->_dictionary->AddressFromGUID(this->_pointGUID);

	//Record the current position
	this->_oldPosition = this->_point->Position();
	//Move the point
	this->_point->Set(this->_position);
	//Return the sketch
	return this->_point->Sketch();
}


bool WCActionSketchPointMove::Rollback(void) {
	//If the point is preset
	if (this->_point != NULL) {
		//Set the rollback flag
		this->_rollback = true;
		//Set self guid
		this->_guid = this->_dictionary->InsertAddress(this);
		//Move the object back to original position
		this->_point->Set(this->_oldPosition);
		//Record GUID for object
		this->_pointGUID = this->_dictionary->GUIDFromAddress(this->_point);
		//Return success
		return true;
	}
	//Not rolled back
	return false;
}


xercesc::DOMElement* WCActionSketchPointMove::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	return NULL;
}


/***********************************************~***************************************************/

