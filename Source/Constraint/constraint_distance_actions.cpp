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
#include <Constraint/constraint_distance_actions.h>
#include <Constraint/constraint_distance.h>
#include <Sketcher/sketch.h>
#include <Sketcher/sketch_point.h>
#include <Sketcher/sketch_line.h>


/***********************************************~***************************************************/


WCActionConstraintDistanceCreate::WCActionConstraintDistanceCreate(WCSketch *sketch, const std::string &constraintName,
	const WCMeasureType &type, WCSketchFeature *first, WCSketchFeature *second, const WCMeasurePosition &posA,
	const WCMeasurePosition &posB) : ::WCAction("Create Distance Constraint", sketch), _sketch(sketch),
	_constraintName(constraintName), _type(type), _first(first), _second(second), _posA(posA), _posB(posB), _constraint(NULL) {
	//Validate sketch
	if (sketch == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionConstraintDistanceCreate::WCActionConstraintDistanceCreate Error - NULL Sketch passed.");
		//throw error
		return;
	}
}


WCActionConstraintDistanceCreate::WCActionConstraintDistanceCreate(xercesc::DOMElement *element, WCSerialDictionary *dictionary) : 
	::WCAction( WCSerializeableObject::ElementFromName(element,"Action"), dictionary ),
	_sketch(NULL), _constraintName(), _type(WCMeasureType::Horizontal()), _first(NULL), _second(NULL),
	_posA(WCMeasurePosition::Closest()), _posB(WCMeasurePosition::Closest()), _constraint(NULL) {
	//Make sure element if not null
	if (element == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionConstraintDistanceCreate:: - NULL Element passed.");
		//throw error
		return;
	}
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

	//Setup first
	this->_first = (WCSketchFeature*)WCSerializeableObject::GetGUIDAttrib(element, "first", dictionary);
	this->_sketch = this->_first->Sketch();
	//Setup second
	this->_second = (WCSketchFeature*)WCSerializeableObject::GetGUIDAttrib(element, "second", dictionary);
	//Setup constraint name
	this->_constraintName = WCSerializeableObject::GetStringAttrib(element, "constraintName");

	//Setup type
	this->_type.FromElement( WCSerializeableObject::ElementFromName(element,"PosA") );
	//Setup pos A
	this->_posA.FromElement( WCSerializeableObject::ElementFromName(element,"PosA") );
	//Setup pos B
	this->_posB.FromElement( WCSerializeableObject::ElementFromName(element,"PosB") );	
}


WCFeature* WCActionConstraintDistanceCreate::Execute(void) {
	WCConstraintDistance *constraint;
	WCMeasurePosition posA(WCMeasurePosition::Closest());
	WCSketchFeature *feature;
	
	//Update pointers (based on rollback flag)
	if (this->_rollback) {
		this->_sketch = (WCSketch*)this->_dictionary->AddressFromGUID(this->_sketchGUID);
		this->_first = (WCSketchFeature*)this->_dictionary->AddressFromGUID(this->_firstGUID);
		this->_second = (WCSketchFeature*)this->_dictionary->AddressFromGUID(this->_secondGUID);
	}

	//Figure out what type of constraint
	WCSketchPoint *p0 = dynamic_cast<WCSketchPoint*>(this->_first);
	WCSketchPoint *p1 = dynamic_cast<WCSketchPoint*>(this->_second);

	//Check point - point case
	if ((p0 != NULL) && (p1 != NULL)) {
		//Create the constraint
		constraint = new WCConstraintDistance(this->_sketch, this->_constraintName, this->_type, p0, p1);
	}

	//Check point-curve case
	else if (((p0 != NULL) || (p1 != NULL)) && ((p0 == NULL) || (p1 == NULL))) {
		//Make sure p0 is the point
		if (p1 != NULL) {
			p0 = p1;
			posA = this->_posA;
			feature = this->_first;
		}
		else {
			posA = this->_posB;
			feature = this->_second;
		}
		//Create the constraint (use point-curve constructor)
		constraint = new WCConstraintDistance(this->_sketch, this->_constraintName, this->_type, p0, feature, posA);
	}

	//Default to curve-curve case
	else {
		//Create the constraint
		constraint = new WCConstraintDistance(this->_sketch, this->_constraintName, this->_type, this->_first, this->_posA, 
											  this->_second, this->_posB);
	}

	//If a rollback execution, update guid and address
	if (this->_rollback) this->_dictionary->UpdateAddress(this->_guid, constraint);
	//Make sure object is not null
	if (constraint == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionConstraintDistanceCreate::Execute Error - Constraint could not be created.");
		return NULL;
	}
	

	//Set the pointer
	this->_constraint = constraint;
	return this->_sketch;
}


bool WCActionConstraintDistanceCreate::Rollback(void) {
	//If the constraint is preset
	if (this->_constraint != NULL) {
		//Set the rollback flag
		this->_rollback = true;
		//Set self guid
		this->_guid = this->_dictionary->InsertAddress(this->_constraint);
		//Delete the constraint
		delete this->_constraint;
		//Record GUIDs for objects
		this->_firstGUID = this->_dictionary->GUIDFromAddress(this->_first);
		this->_secondGUID = this->_dictionary->GUIDFromAddress(this->_second);
		this->_sketchGUID = this->_dictionary->GUIDFromAddress(this->_sketch);
		//Return success
		return true;
	}
	//Not rolled back
	return false;
}


xercesc::DOMElement* WCActionConstraintDistanceCreate::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	return NULL;
}


/***********************************************~***************************************************/


WCActionConstraintDistanceMove::WCActionConstraintDistanceMove(WCConstraintDistance *constraint, const WPFloat &offset, const WPFloat &labelOffset) :
	::WCAction("Move Distance Constraint", constraint), _constraint(constraint), _oldOffset(0.0), _oldLabelOffset(0.0),
	_offset(offset), _labelOffset(labelOffset) {
	//Validate object
	if (constraint == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionConstraintDistanceMove::WCActionConstraintDistanceMove - NULL Constraint passed.");
		//throw error
		return;
	}
}


WCActionConstraintDistanceMove::WCActionConstraintDistanceMove(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCAction( WCSerializeableObject::ElementFromName(element,"Action"), dictionary ),
	_oldOffset(0.0), _oldLabelOffset(0.0), _offset(0.0), _labelOffset(0.0) {
	//Make sure element if not null
	if (element == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionConstraintDistanceMove:: - NULL Element passed.");
		//throw error
		return;
	}
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

	//Setup constraint attribute
	this->_constraint = (WCConstraintDistance*)WCSerializeableObject::GetGUIDAttrib(element, "constraint", dictionary);
	//Setup oldOffset
	this->_oldOffset = WCSerializeableObject::GetFloatAttrib(element, "oldOffset");
	//Setup oldLabelOffset
	this->_oldLabelOffset = WCSerializeableObject::GetFloatAttrib(element, "oldLabelOffset");
	//Setup offset
	this->_offset = WCSerializeableObject::GetFloatAttrib(element, "offset");
	//Setup labelOffset
	this->_labelOffset = WCSerializeableObject::GetFloatAttrib(element, "labelOffset");
}


WCFeature* WCActionConstraintDistanceMove::Execute(void) {
	//Update constraint based on rollback flag
	if (this->_rollback) this->_constraint = (WCConstraintDistance*)this->_dictionary->AddressFromGUID(this->_constraintGUID);

	//Record the current offsets
	this->_oldOffset = this->_constraint->Measure()->Offset();
	this->_oldLabelOffset = this->_constraint->Measure()->LabelOffset();
	//Move the measure
	this->_constraint->Measure()->Offsets(this->_offset, this->_labelOffset);
	//Return the sketch
	return this->_constraint->Sketch();
}


bool WCActionConstraintDistanceMove::Rollback(void) {
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


xercesc::DOMElement* WCActionConstraintDistanceMove::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create primary element for this object
	XMLCh* xmlString = xercesc::XMLString::transcode("ActionConstraintDistanceMove");
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

