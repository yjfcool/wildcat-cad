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
#include "Constraint/constraint_radius_actions.h"
#include "Constraint/constraint_radius.h"
#include "Sketcher/sketch.h"


/***********************************************~***************************************************/


WCActionConstraintRadiusCreate::WCActionConstraintRadiusCreate(WCSketch *sketch, const std::string &constraintName,
	WCSketchArc *arc) : ::WCAction("Create Radius Constraint", sketch), _arc(arc), _circle(NULL), 
	_constraintName(constraintName), _sketch(sketch), _constraint(NULL) {
	//Validate sketch
	if (sketch == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionConstraintRadiusCreate::WCActionConstraintRadiusCreate - NULL Sketch passed.");
		//throw error
		return;
	}
}


WCActionConstraintRadiusCreate::WCActionConstraintRadiusCreate(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) :
	::WCAction("Create Radius Constraint", NULL), _arc(NULL), _circle(NULL), _constraintName(), _sketch(NULL), _constraint(NULL) {
	//Nothing else for now
}


WCActionConstraintRadiusCreate::WCActionConstraintRadiusCreate(WCSketch *sketch, const std::string &constraintName,
	WCSketchCircle *circle) : ::WCAction("Create Radius Constraint", sketch), _arc(NULL), _circle(circle), 
	_constraintName(constraintName), _sketch(sketch), _constraint(NULL) {
	//Validate sketch
	if (sketch == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionConstraintRadiusCreate::WCActionConstraintRadiusCreate - NULL Sketch passed.");
		//throw error
		return;
	}
}


WCFeature* WCActionConstraintRadiusCreate::Execute(void) {
	//Update pointers (based on rollback flag)
	if (this->_rollback) {
		this->_sketch = (WCSketch*)this->_dictionary->AddressFromGUID(this->_sketchGUID);
		this->_arc = (WCSketchArc*)this->_dictionary->AddressFromGUID(this->_arcGUID);
		this->_circle = (WCSketchCircle*)this->_dictionary->AddressFromGUID(this->_circleGUID);
	}

	//Create depending on type
	WCConstraintRadius *constraint;
	if (this->_arc != NULL) {
		//Create the constraint
		constraint = new WCConstraintRadius(this->_sketch, this->_constraintName, this->_arc);
	}
	else {
		//Create the constraint
		constraint = new WCConstraintRadius(this->_sketch, this->_constraintName, this->_circle);
	}
	//Make sure object is not null
	if (constraint == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionConstraintRadiusCreate::Execute - Constraint could not be created.");
		return NULL;
	}
	
	//If a rollback execution, update guid and address
	if (this->_rollback) this->_dictionary->UpdateAddress(this->_guid, constraint);
	//Set the pointer
	this->_constraint = constraint;
	return this->_sketch;
}


bool WCActionConstraintRadiusCreate::Rollback(void) {
	//If the constraint is preset
	if (this->_constraint != NULL) {
		//Set the rollback flag
		this->_rollback = true;
		//Set self guid
		this->_guid = this->_dictionary->InsertAddress(this->_constraint);
		//Delete the constraint
		delete this->_constraint;
		//Record GUIDs for line and sketch
		this->_arcGUID = this->_dictionary->GUIDFromAddress(this->_arc);
		this->_circleGUID = this->_dictionary->GUIDFromAddress(this->_circle);
		this->_sketchGUID = this->_dictionary->GUIDFromAddress(this->_sketch);
		//Return success
		return true;
	}
	//Not rolled back
	return false;
}


xercesc::DOMElement* WCActionConstraintRadiusCreate::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	return NULL;
}


/***********************************************~***************************************************/


WCActionConstraintRadiusMove::WCActionConstraintRadiusMove(WCConstraintRadius *constraint, const WPFloat &offset, const WPFloat &labelOffset) :
	::WCAction("Move Radius Constraint", constraint), _constraint(constraint), _oldOffset(0.0), _oldLabelOffset(0.0),
	_offset(offset), _labelOffset(labelOffset) {
	//Validate object
	if (constraint == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionConstraintRadiusMove::WCActionConstraintRadiusMove - NULL Constraint passed.");
		//throw error
		return;
	}
}


WCActionConstraintRadiusMove::WCActionConstraintRadiusMove(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) :
	::WCAction("Move Radius Constraint", NULL), _oldOffset(0.0), _oldLabelOffset(0.0),
	_offset(0.0), _labelOffset(0.0) {
	//Nothing else for now
}


WCFeature* WCActionConstraintRadiusMove::Execute(void) {
	//Update constraint based on rollback flag
	if (this->_rollback) this->_constraint = (WCConstraintRadius*)this->_dictionary->AddressFromGUID(this->_constraintGUID);

	//Record the current offsets
	this->_oldOffset = this->_constraint->Measure()->Offset();
	this->_oldLabelOffset = this->_constraint->Measure()->LabelOffset();
	//Move the measure
	this->_constraint->Measure()->Offsets(this->_offset, this->_labelOffset);
	//Return the sketch
	return this->_constraint->Sketch();
}


bool WCActionConstraintRadiusMove::Rollback(void) {
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


xercesc::DOMElement* WCActionConstraintRadiusMove::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	return NULL;
}


/***********************************************~***************************************************/

