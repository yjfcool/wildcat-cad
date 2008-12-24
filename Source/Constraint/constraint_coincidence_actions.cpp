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
#include <Constraint/constraint_coincidence_actions.h>
#include <Constraint/constraint_coincidence.h>
#include <Sketcher/sketch.h>
#include <Sketcher/sketch_point.h>


/***********************************************~***************************************************/


WCActionConstraintCoincidenceCreate::WCActionConstraintCoincidenceCreate(WCSketch *sketch, const std::string &constraintName,
	WCSketchFeature *featureA, const WCMeasurePosition &posA, WCSketchFeature *featureB, const WCMeasurePosition &posB) : 
	::WCAction("Create Distance Constraint", sketch), _sketch(sketch), _constraintName(constraintName),
	 _featureA(featureA), _featureB(featureB), _posA(posA), _posB(posB), _constraint(NULL) {
	//Validate sketch
	if (sketch == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionConstraintCoincidenceCreate::WCActionConstraintCoincidenceCreate Error - NULL Sketch passed.");
		//throw error
		return;
	}
}


WCActionConstraintCoincidenceCreate::WCActionConstraintCoincidenceCreate(xercesc::DOMElement *element, WCSerialDictionary *dictionary) : 
	::WCAction( WCSerializeableObject::ElementFromName(element,"Action"), dictionary ),
	_sketch(NULL), _constraintName(), _featureA(NULL), _featureB(NULL),
	_posA(WCMeasurePosition::Closest()), _posB(WCMeasurePosition::Closest()), _constraint(NULL) {
	//Make sure element if not null
	if (element == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionConstraintAngleCreate:: - NULL Element passed.");
		//throw error
		return;
	}
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

	//Setup feature A
	this->_featureA = (WCSketchFeature*)WCSerializeableObject::GetGUIDAttrib(element, "featureA", dictionary);
	this->_sketch = this->_featureA->Sketch();
	//Setup feature B
	this->_featureB = (WCSketchFeature*)WCSerializeableObject::GetGUIDAttrib(element, "featureB", dictionary);
	//Setup constraint name
	this->_constraintName = WCSerializeableObject::GetStringAttrib(element, "constraintName");
	//Setup pos A
	this->_posA.FromElement( WCSerializeableObject::ElementFromName(element,"PosA") );
	//Setup pos B
	this->_posB.FromElement( WCSerializeableObject::ElementFromName(element,"PosB") );	
}


WCFeature* WCActionConstraintCoincidenceCreate::Execute(void) {
	//Update pointers (based on rollback flag)
	if (this->_rollback) {
		this->_sketch = (WCSketch*)this->_dictionary->AddressFromGUID(this->_sketchGUID);
		this->_featureA = (WCSketchFeature*)this->_dictionary->AddressFromGUID(this->_featureAGUID);
		this->_featureB = (WCSketchFeature*)this->_dictionary->AddressFromGUID(this->_featureBGUID);
	}

	//Create the constraint
	WCConstraintCoincidence* constraint = new WCConstraintCoincidence(this->_sketch, this->_constraintName, this->_featureA, this->_posA,
																	  this->_featureB, this->_posB);
	//Make sure object is not null
	if (constraint == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionConstraintCoincidenceCreate::Execute Error - Constraint could not be created.");
		return NULL;
	}

	//If a rollback execution, update guid and address
	if (this->_rollback) this->_dictionary->UpdateAddress(this->_guid, constraint);
	//Set the pointer
	this->_constraint = constraint;
	return this->_sketch;
}


bool WCActionConstraintCoincidenceCreate::Rollback(void) {
	//If the constraint is preset
	if (this->_constraint != NULL) {
		//Set the rollback flag
		this->_rollback = true;
		//Set self guid
		this->_guid = this->_dictionary->InsertAddress(this->_constraint);
		//Delete the constraint
		delete this->_constraint;
		//Record GUIDs for objects
		this->_featureAGUID = this->_dictionary->GUIDFromAddress(this->_featureA);
		this->_featureBGUID = this->_dictionary->GUIDFromAddress(this->_featureB);
		this->_sketchGUID = this->_dictionary->GUIDFromAddress(this->_sketch);
		//Return success
		return true;
	}
	//Not rolled back
	return false;
}


xercesc::DOMElement* WCActionConstraintCoincidenceCreate::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
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

	//Add feature A attribute
	WCSerializeableObject::AddGUIDAttrib(element, "featureA", this->_featureA, dictionary);
	//Add feature B attribute
	WCSerializeableObject::AddGUIDAttrib(element, "featureB", this->_featureB, dictionary);
	//Add constraint name attribute
	WCSerializeableObject::AddStringAttrib(element, "constraintName", this->_constraintName);

	//Add pos A
	xmlString = xercesc::XMLString::transcode("PosA");
	xercesc::DOMElement* posElement = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	this->_posA.ToElement(posElement);
	element->appendChild(posElement);
	//Add pos B
	xmlString = xercesc::XMLString::transcode("PosB");
	posElement = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	this->_posB.ToElement(posElement);
	element->appendChild(posElement);

	//Return the new element
	return element;
}


/***********************************************~***************************************************/

