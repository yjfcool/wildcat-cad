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
#include "Constraint/constraint_fix_actions.h"
#include "Constraint/constraint_fix.h"
#include "Sketcher/sketch.h"


/***********************************************~***************************************************/


WCActionConstraintFixCreate::WCActionConstraintFixCreate(WCSketch *sketch, const std::string &constraintName,
	WCSketchFeature *feature) : ::WCAction("Create Fix Constraint", sketch), _feature(feature),
	_constraintName(constraintName), _sketch(sketch), _constraint(NULL) {
	//Validate object(s)
	if ((sketch == NULL) || (feature == NULL)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionConstraintFixCreate::WCActionConstraintFixCreate Error - NULL references passed.");
		//throw error
		return;
	}
}


WCActionConstraintFixCreate::WCActionConstraintFixCreate(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) :
	::WCAction("Create Fix Constraint", NULL), _feature(NULL), _constraintName(), _sketch(NULL), _constraint(NULL) {
	//Nothing else for now
}


WCFeature* WCActionConstraintFixCreate::Execute(void) {
	//Update pointers (based on rollback flag)
	if (this->_rollback) {
		this->_sketch = (WCSketch*)this->_dictionary->AddressFromGUID(this->_sketchGUID);
		this->_feature = (WCSketchFeature*)this->_dictionary->AddressFromGUID(this->_featureGUID);
	}

	//Make sure feature is not a constraint
	WCSketchConstraint *tmp = dynamic_cast<WCSketchConstraint*>(this->_feature);
	if (tmp != NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionConstraintFixCreate::Execute Error - Can not fix a constraint.");
		return NULL;
	}
	//Make sure feature is not already fixed
	if (this->_feature->IsFixed()) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionConstraintFixCreate::Execute Error - Can not fix a fixed feature.");
		return NULL;
	}
	//Create the constraint
	WCConstraintFix *constraint = new WCConstraintFix(this->_sketch, this->_constraintName, this->_feature);
	//Make sure object is not null
	if (constraint == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionConstraintFixCreate::Execute Error - Fix Constraint could not be created.");
		return NULL;
	}

	//If a rollback execution, update guid and address
	if (this->_rollback) this->_dictionary->UpdateAddress(this->_guid, constraint);
	//Set the pointer
	this->_constraint = constraint;
	return this->_sketch;
}


bool WCActionConstraintFixCreate::Rollback(void) {
	//If the constraint is preset
	if (this->_constraint != NULL) {
		//Set the rollback flag
		this->_rollback = true;
		//Set self guid
		this->_guid = this->_dictionary->InsertAddress(this->_constraint);
		//Delete the constraint
		delete this->_constraint;
		//Record GUIDs for line and sketch
		this->_featureGUID = this->_dictionary->GUIDFromAddress(this->_feature);
		this->_sketchGUID = this->_dictionary->GUIDFromAddress(this->_sketch);
		//Return success
		return true;
	}
	//Not rolled back
	return false;
}


xercesc::DOMElement* WCActionConstraintFixCreate::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	return NULL;
}


/***********************************************~***************************************************/

