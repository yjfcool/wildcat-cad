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
#include "Sketcher/sketch_line_actions.h"
#include "Sketcher/sketch_line.h"
#include "Sketcher/sketch.h"


/***********************************************~***************************************************/


WCActionSketchLineCreate::WCActionSketchLineCreate(WCSketch *sketch, const std::string &lineName, const WCVector4 &begin,
	const WCVector4 &end) : ::WCAction("Create Sketch Line", sketch), _line(NULL),
	_lineName(lineName), _sketch(sketch), _begin(begin), _end(end) {
	//Validate sketch
	if (sketch == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionSketchLineCreate::WCActionSketchLineCreate Error - NULL References passed.");
		//throw error
		return;
	}
}


WCActionSketchLineCreate::WCActionSketchLineCreate(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCAction( WCSerializeableObject::ElementFromName(element,"Action"), dictionary ),
	_line(NULL), _lineName(""), _sketch(NULL), _begin(), _end() {
	//Do something here
}


WCFeature* WCActionSketchLineCreate::Execute(void) {
	//Update pointers (based on rollback flag)
	if (this->_rollback) {
		this->_sketch = (WCSketch*)this->_dictionary->AddressFromGUID(this->_sketchGUID);
	}

	//Create the feature
	WCSketchLine *line = new WCSketchLine(this->_sketch, this->_lineName, this->_begin, this->_end);
	//Make sure object is not null
	if (line == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionSketchLineCreate::Execute Error - Feature could not be created.");
		return NULL;
	}
	
	//If a rollback execution, update guid and address
	if (this->_rollback) this->_dictionary->UpdateAddress(this->_guid, line);
	//Set the pointer
	this->_line = line;
	return this->_sketch;
}


bool WCActionSketchLineCreate::Rollback(void) {
	//If the object is preset
	if (this->_line != NULL) {
		//Set the rollback flag
		this->_rollback = true;
		//Set self guid
		this->_guid = this->_dictionary->InsertAddress(this->_line);
		//Delete the object
		delete this->_line;
		//Record GUIDs for sketch
		this->_sketchGUID = this->_dictionary->GUIDFromAddress(this->_sketch);
		//Return success
		return true;
	}
	//Not rolled back
	return false;
}


xercesc::DOMElement* WCActionSketchLineCreate::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	return NULL;
}


/***********************************************~***************************************************/


WCActionSketchLineMove::WCActionSketchLineMove(WCSketchLine *line, const WCVector4 &begin, const WCVector4 &end) :
	::WCAction("Move Sketch Line", line->Sketch()), _line(line),
	_begin(begin),  _end(end), _oldBegin(), _oldEnd() {
	//Validate line
	if (line == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionSketchLineMove::WCActionSketchLineMove Error - NULL References passed.");
		//throw error
		return;
	}
}


WCActionSketchLineMove::WCActionSketchLineMove(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCAction( WCSerializeableObject::ElementFromName(element,"Action"), dictionary ),
	_line(NULL), _begin(), _end(), _oldBegin(), _oldEnd() {
	//Do something here
}


WCFeature* WCActionSketchLineMove::Execute(void) {
	//Update object based on rollback flag
	if (this->_rollback) this->_line = (WCSketchLine*)this->_dictionary->AddressFromGUID(this->_lineGUID);

	//Record the current position
	this->_oldBegin = this->_line->Begin();
	this->_oldEnd = this->_line->End();
	//Move the object
	this->_line->Set(this->_begin, this->_end);
	//Return the sketch
	return this->_line->Sketch();
}


bool WCActionSketchLineMove::Rollback(void) {
	//If the object is preset
	if (this->_line != NULL) {
		//Set the rollback flag
		this->_rollback = true;
		//Set self guid
		this->_guid = this->_dictionary->InsertAddress(this);
		//Move the object back to original position
		this->_line->Set(this->_oldBegin, this->_oldEnd);
		//Record GUID for object
		this->_lineGUID = this->_dictionary->GUIDFromAddress(this->_line);
		//Return success
		return true;
	}
	//Not rolled back
	return false;
}


xercesc::DOMElement* WCActionSketchLineMove::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	return NULL;
}


/***********************************************~***************************************************/

