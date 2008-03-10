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
#include "Sketcher/sketch_arc_actions.h"
#include "Sketcher/sketch_arc.h"
#include "Sketcher/sketch.h"


/***********************************************~***************************************************/


WCActionSketchArcCreate::WCActionSketchArcCreate(WCSketch *sketch, const std::string &arcName, const WCVector4 &center, 
	const WPFloat &radius, const WPFloat &startAngle, const WPFloat &endAngle) : ::WCAction("Create Sketch Arc", sketch), 
	_type(WCSketchArcType::Standard()), _arc(NULL), _arcName(arcName), _sketch(sketch), _center(center), _radius(radius), 
	_startAngle(startAngle), _endAngle(endAngle), _threePoint() {
	//Validate sketch
	if (sketch == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionSketchArcCreate::WCActionSketchArcCreate Error - NULL References passed.");
		//throw error
		return;
	}
}


WCActionSketchArcCreate::WCActionSketchArcCreate(WCSketch *sketch, const std::string &arcName, const WCVector4 &center,
	const WPFloat &radius, const WPFloat &startAngle, const WPFloat &endAngle, const WCVector4 &threePoint) :
	::WCAction("Create Three-Point Sketch Arc", sketch), _type(WCSketchArcType::ThreePoint()), _arc(NULL), _arcName(arcName), 
	_sketch(sketch), _center(center), _radius(radius), _startAngle(startAngle), _endAngle(endAngle), _threePoint(threePoint) {
	//Validate sketch
	if (sketch == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchArcAction::WCSketchArcAction Error - NULL References passed.");
		//throw error
		return;
	}
}


WCActionSketchArcCreate::WCActionSketchArcCreate(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCAction( WCSerializeableObject::ElementFromName(element,"Action"), dictionary ),
	_type(WCSketchArcType::Standard()), _arc(NULL), _arcName(""), _sketch(NULL), _center(), _threePoint(),
	_radius(0.0), _startAngle(0.0), _endAngle(0.0) {
	//Do something here
}


WCFeature* WCActionSketchArcCreate::Execute(void) {
	//Update pointers (based on rollback flag)
	if (this->_rollback) {
		this->_sketch = (WCSketch*)this->_dictionary->AddressFromGUID(this->_sketchGUID);
	}

	WCSketchArc *arc = NULL;
	//Create the arc
	if (this->_type == WCSketchArcType::Standard())
		arc = new WCSketchArc(this->_sketch, this->_arcName, this->_center, this->_radius, this->_startAngle, this->_endAngle);
	else if (this->_type == WCSketchArcType::ThreePoint())
		arc = new WCSketchArc(this->_sketch, this->_arcName, this->_center, this->_radius, this->_startAngle, this->_endAngle, this->_threePoint);
	//Make sure arc is not null
	if (arc == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchArcAction::Execute Error - Sketch Arc could not be created.");
		return NULL;
	}

	//If a rollback execution, update guid and address
	if (this->_rollback) this->_dictionary->UpdateAddress(this->_guid, arc);
	//Set the pointer
	this->_arc = arc;
	return this->_sketch;
}


bool WCActionSketchArcCreate::Rollback(void) {
	//If the object is preset
	if (this->_arc != NULL) {
		//Set the rollback flag
		this->_rollback = true;
		//Set self guid
		this->_guid = this->_dictionary->InsertAddress(this->_arc);
		//Delete the object
		delete this->_arc;
		//Record GUIDs for sketch
		this->_sketchGUID = this->_dictionary->GUIDFromAddress(this->_sketch);
		//Return success
		return true;
	}
	//Not rolled back
	return false;
}


xercesc::DOMElement* WCActionSketchArcCreate::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	return NULL;
}


/***********************************************~***************************************************/


WCActionSketchArcMove::WCActionSketchArcMove(WCSketchArc *arc, const WCVector4 &center) :
	::WCAction("Move Sketch Arc", arc->Sketch()), _arc(arc),
	_center(center), _oldCenter() {
	//Validate feature
	if (arc == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionSketchArcMove::WCActionSketchArcMove Error - NULL References passed.");
		//throw error
		return;
	}
}


WCActionSketchArcMove::WCActionSketchArcMove(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCAction( WCSerializeableObject::ElementFromName(element,"Action"), dictionary ),
	_arc(NULL), _center(), _oldCenter() {
	//Do something here
}


WCFeature* WCActionSketchArcMove::Execute(void) {
	//Update object based on rollback flag
	if (this->_rollback) this->_arc = (WCSketchArc*)this->_dictionary->AddressFromGUID(this->_arcGUID);

	//Record the current position
	this->_oldCenter = this->_arc->Center();
	//Move the object
	this->_arc->Center(this->_center);
	//Return the sketch
	return this->_arc->Sketch();
}


bool WCActionSketchArcMove::Rollback(void) {
	//If the object is preset
	if (this->_arc != NULL) {
		//Set the rollback flag
		this->_rollback = true;
		//Set self guid
		this->_guid = this->_dictionary->InsertAddress(this);
		//Move the arc back to original position
		this->_arc->Center(this->_oldCenter);
		//Record GUID for object
		this->_arcGUID = this->_dictionary->GUIDFromAddress(this->_arc);
		//Return success
		return true;
	}
	//Not rolled back
	return false;
}


xercesc::DOMElement* WCActionSketchArcMove::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	return NULL;
}


/***********************************************~***************************************************/


WCActionSketchArcRadius::WCActionSketchArcRadius(WCSketchArc *arc, const WPFloat &radius) :
	::WCAction("Set Sketch Arc Radius", arc->Sketch()), _arc(arc),
	_radius(radius), _oldRadius(0.0) {
	//Validate feature
	if (arc == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionSketchArcRadius::WCActionSketchArcRadius Error - NULL References passed.");
		//throw error
		return;
	}
}


WCActionSketchArcRadius::WCActionSketchArcRadius(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCAction( WCSerializeableObject::ElementFromName(element,"Action"), dictionary ),
	_arc(NULL), _radius(0.0), _oldRadius(0.0) {
	//Do something here
}


WCFeature* WCActionSketchArcRadius::Execute(void) {
	//Update object based on rollback flag
	if (this->_rollback) this->_arc = (WCSketchArc*)this->_dictionary->AddressFromGUID(this->_arcGUID);

	//Record the current radius
	this->_oldRadius = this->_arc->Radius();
	//Set the radius
	this->_arc->Radius(this->_radius);
	//Return the sketch
	return this->_arc->Sketch();
}


bool WCActionSketchArcRadius::Rollback(void) {
	//If the object is preset
	if (this->_arc != NULL) {
		//Set the rollback flag
		this->_rollback = true;
		//Set self guid
		this->_guid = this->_dictionary->InsertAddress(this);
		//Move the arc back to original radius
		this->_arc->Radius(this->_oldRadius);
		//Record GUID for object
		this->_arcGUID = this->_dictionary->GUIDFromAddress(this->_arc);
		//Return success
		return true;
	}
	//Not rolled back
	return false;
}


xercesc::DOMElement* WCActionSketchArcRadius::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	return NULL;
}


/***********************************************~***************************************************/

