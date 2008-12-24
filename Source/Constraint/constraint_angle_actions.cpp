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
#include <Constraint/constraint_angle_actions.h>
#include <Constraint/constraint_angle.h>
#include <Sketcher/sketch.h>
#include <Sketcher/sketch_line.h>


/***********************************************~***************************************************/


WCActionConstraintAngleCreate::WCActionConstraintAngleCreate(WCSketch *sketch, const std::string &constraintName,
	WCSketchLine *lineA, WCSketchLine *lineB) : ::WCAction("Create Angle Constraint", sketch), _lineA(lineA), _lineB(lineB), 
	_constraintName(constraintName), _sketch(sketch), _constraint(NULL) {
	//Validate sketch
	if (sketch == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionConstraintAngleCreate::WCActionConstraintAngleCreate - NULL Sketch passed.");
		//throw error
		return;
	}
}


WCActionConstraintAngleCreate::WCActionConstraintAngleCreate(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCAction( WCSerializeableObject::ElementFromName(element,"Action"), dictionary ), 
	_lineA(NULL), _lineB(NULL), _constraintName(), _sketch(NULL), _constraint(NULL) {
	//Make sure element if not null
	if (element == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionConstraintAngleCreate:: - NULL Element passed.");
		//throw error
		return;
	}
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

	//Setup line A
	this->_lineA = (WCSketchLine*)WCSerializeableObject::GetGUIDAttrib(element, "lineA", dictionary);
	this->_sketch = this->_lineA->Sketch();
	//Setup line B
	this->_lineB = (WCSketchLine*)WCSerializeableObject::GetGUIDAttrib(element, "lineB", dictionary);
	//Setup constraint name
	this->_constraintName = WCSerializeableObject::GetStringAttrib(element, "constraintName");
}


WCFeature* WCActionConstraintAngleCreate::Execute(void) {
	//Update pointers (based on rollback flag)
	if (this->_rollback) {
		this->_sketch = (WCSketch*)this->_dictionary->AddressFromGUID(this->_sketchGUID);
		this->_lineA = (WCSketchLine*)this->_dictionary->AddressFromGUID(this->_lineAGUID);
		this->_lineB = (WCSketchLine*)this->_dictionary->AddressFromGUID(this->_lineBGUID);
	}

	//Create the constraint
	WCConstraintAngle *constraint = new WCConstraintAngle(this->_sketch, this->_constraintName, this->_lineA, this->_lineB);
	//Make sure object is not null
	if (constraint == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionConstraintAngleCreate::Execute - Constraint could not be created.");
		return NULL;
	}

	//If a rollback execution, update guid and address
	if (this->_rollback) this->_dictionary->UpdateAddress(this->_guid, constraint);
	//Set the pointer
	this->_constraint = constraint;
	return this->_sketch;
}


bool WCActionConstraintAngleCreate::Rollback(void) {
	//If the constraint is preset
	if (this->_constraint != NULL) {
		//Set the rollback flag
		this->_rollback = true;
		//Set self guid
		this->_guid = this->_dictionary->InsertAddress(this->_constraint);
		//Delete the constraint
		delete this->_constraint;
		//Record GUIDs for lines and sketch
		this->_lineAGUID = this->_dictionary->GUIDFromAddress(this->_lineA);
		this->_lineBGUID = this->_dictionary->GUIDFromAddress(this->_lineB);
		this->_sketchGUID = this->_dictionary->GUIDFromAddress(this->_sketch);
		//Return success
		return true;
	}
	//Not rolled back
	return false;
}


xercesc::DOMElement* WCActionConstraintAngleCreate::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create primary element for this object
	XMLCh* xmlString = xercesc::XMLString::transcode("ActionConstraintAngleCreate");
	xercesc::DOMElement* element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Include the parent element
	xercesc::DOMElement* actionElement = this->WCAction::Serialize(document, dictionary);
	element->appendChild(actionElement);
	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);

	//Add line A attribute
	WCSerializeableObject::AddGUIDAttrib(element, "lineA", this->_lineA, dictionary);
	//Add line B attribute
	WCSerializeableObject::AddGUIDAttrib(element, "lineB", this->_lineB, dictionary);
	//Add constraint name attribute
	WCSerializeableObject::AddStringAttrib(element, "constraintName", this->_constraintName);

	//Return the new element
	return element;
}


/***********************************************~***************************************************/


WCActionConstraintAngleMove::WCActionConstraintAngleMove(WCConstraintAngle *constraint, const WPFloat &offset, const WPFloat &labelOffset) :
	::WCAction("Move Angle Constraint", constraint), _constraint(constraint), _oldOffset(0.0), _oldLabelOffset(0.0),
	_offset(offset), _labelOffset(labelOffset) {
	//Validate object
	if (constraint == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionConstraintAngleMove::WCActionConstraintAngleMove - NULL Constraint passed.");
		//throw error
		return;
	}
}


WCActionConstraintAngleMove::WCActionConstraintAngleMove(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCAction( WCSerializeableObject::ElementFromName(element,"Action"), dictionary ), 
	_oldOffset(0.0), _oldLabelOffset(0.0), _offset(0.0), _labelOffset(0.0) {
	//Make sure element if not null
	if (element == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionConstraintAngleMove:: - NULL Element passed.");
		//throw error
		return;
	}
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

	//Setup constraint attribute
	this->_constraint = (WCConstraintAngle*)WCSerializeableObject::GetGUIDAttrib(element, "constraint", dictionary);
	//Setup oldOffset
	this->_oldOffset = WCSerializeableObject::GetFloatAttrib(element, "oldOffset");
	//Setup oldLabelOffset
	this->_oldLabelOffset = WCSerializeableObject::GetFloatAttrib(element, "oldLabelOffset");
	//Setup offset
	this->_offset = WCSerializeableObject::GetFloatAttrib(element, "offset");
	//Setup labelOffset
	this->_labelOffset = WCSerializeableObject::GetFloatAttrib(element, "labelOffset");
}


WCFeature* WCActionConstraintAngleMove::Execute(void) {
	//Update constraint based on rollback flag
	if (this->_rollback) this->_constraint = (WCConstraintAngle*)this->_dictionary->AddressFromGUID(this->_constraintGUID);

	//Record the current offsets
	this->_oldOffset = this->_constraint->Measure()->Offset();
	this->_oldLabelOffset = this->_constraint->Measure()->LabelOffset();
	//Move the measure
	this->_constraint->Measure()->Offsets(this->_offset, this->_labelOffset);
	//Return the sketch
	return this->_constraint->Sketch();
}


bool WCActionConstraintAngleMove::Rollback(void) {
	//If the constraint is preset
	if (this->_constraint != NULL) {
		//Set the rollback flag
		this->_rollback = true;
		//Set self guid
		this->_guid = this->_dictionary->InsertAddress(this);
		//Move the constraint back to original position
		this->_constraint->Measure()->Offsets(this->_oldOffset, this->_oldLabelOffset);
		//Record GUID for constraint
		this->_constraintGUID = this->_dictionary->GUIDFromAddress(this->_constraint);
		//Return success
		return true;
	}
	//Not rolled back
	return false;
}


xercesc::DOMElement* WCActionConstraintAngleMove::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create primary element for this object
	XMLCh* xmlString = xercesc::XMLString::transcode("ActionConstraintAngleMove");
	xercesc::DOMElement* element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Include the parent element
	xercesc::DOMElement* actionElement = this->WCAction::Serialize(document, dictionary);
	element->appendChild(actionElement);
	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);

	//Add offset attribute
	WCSerializeableObject::AddFloatAttrib(element, "offset", this->_offset);
	//Add labelOffset attribute
	WCSerializeableObject::AddFloatAttrib(element, "labelOffset", this->_labelOffset);
	//Add oldOffset attribute
	WCSerializeableObject::AddFloatAttrib(element, "oldOffset", this->_oldOffset);
	//Add oldLabelOffset attribute
	WCSerializeableObject::AddFloatAttrib(element, "oldLabelOffset", this->_oldLabelOffset);

	//Return the new element
	return element;
}


/***********************************************~***************************************************/

