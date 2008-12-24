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
#include <Sketcher/sketch_circle_actions.h>
#include <Sketcher/sketch_circle.h>
#include <Sketcher/sketch.h>


/***********************************************~***************************************************/


WCActionSketchCircleCreate::WCActionSketchCircleCreate(WCSketch *sketch, const std::string &circleName,
	const WCVector4 &center, const WPFloat &radius) : ::WCAction("Create Sketch Circle", sketch), _circle(NULL),
	_circleName(circleName), _sketch(sketch), _center(center), _radius(radius) {
	//Validate sketch, p0, and p1
	if (sketch == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionSketchCircleCreate::WCActionSketchCircleCreate Error - NULL References passed.");
		//throw error
		return;
	}
}


WCActionSketchCircleCreate::WCActionSketchCircleCreate(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCAction( WCSerializeableObject::ElementFromName(element,"Action"), dictionary ),
	_circle(NULL), _circleName(""), _sketch(NULL), _center(), _radius(0.0) {
	//Do something here
}


WCFeature* WCActionSketchCircleCreate::Execute(void) {
	//Update pointers (based on rollback flag)
	if (this->_rollback) {
		this->_sketch = (WCSketch*)this->_dictionary->AddressFromGUID(this->_sketchGUID);
	}

	//Create the circle
	WCSketchCircle *circle = new WCSketchCircle(this->_sketch, this->_circleName, this->_center, this->_radius);
	//Make sure circle is not null
	if (circle == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionSketchCircleCreate::Execute Error - Sketch Circle could not be created.");
		return NULL;
	}
	
	//If a rollback execution, update guid and address
	if (this->_rollback) this->_dictionary->UpdateAddress(this->_guid, circle);
	//Set the pointer
	this->_circle = circle;
	return this->_sketch;
}


bool WCActionSketchCircleCreate::Rollback(void) {
	//If the object is preset
	if (this->_circle != NULL) {
		//Set the rollback flag
		this->_rollback = true;
		//Set self guid
		this->_guid = this->_dictionary->InsertAddress(this->_circle);
		//Delete the object
		delete this->_circle;
		//Record GUIDs for sketch
		this->_sketchGUID = this->_dictionary->GUIDFromAddress(this->_sketch);
		//Return success
		return true;
	}
	//Not rolled back
	return false;
}


xercesc::DOMElement* WCActionSketchCircleCreate::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	return NULL;
}


/***********************************************~***************************************************/


WCActionSketchCircleMove::WCActionSketchCircleMove(WCSketchCircle *circle, const WCVector4 &center) :
	::WCAction("Move Sketch Circle", circle->Sketch()), _circle(circle),
	_center(center), _oldCenter() {
	//Validate feature
	if (circle == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionSketchCircleMove::WCActionSketchCircleMove Error - NULL References passed.");
		//throw error
		return;
	}
}


WCActionSketchCircleMove::WCActionSketchCircleMove(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCAction( WCSerializeableObject::ElementFromName(element,"Action"), dictionary ),
	_circle(NULL), _center(), _oldCenter() {
	//Do something here
}


WCFeature* WCActionSketchCircleMove::Execute(void) {
	//Update object based on rollback flag
	if (this->_rollback) this->_circle = (WCSketchCircle*)this->_dictionary->AddressFromGUID(this->_circleGUID);

	//Record the current position
	this->_oldCenter = this->_circle->Center();
	//Move the object
	this->_circle->Center(this->_center);
	//Return the sketch
	return this->_circle->Sketch();
}


bool WCActionSketchCircleMove::Rollback(void) {
	//If the object is preset
	if (this->_circle != NULL) {
		//Set the rollback flag
		this->_rollback = true;
		//Set self guid
		this->_guid = this->_dictionary->InsertAddress(this);
		//Move the object back to original position
		this->_circle->Center(this->_oldCenter);
		//Record GUID for object
		this->_circleGUID = this->_dictionary->GUIDFromAddress(this->_circle);
		//Return success
		return true;
	}
	//Not rolled back
	return false;
}


xercesc::DOMElement* WCActionSketchCircleMove::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	return NULL;
}


/***********************************************~***************************************************/


WCActionSketchCircleRadius::WCActionSketchCircleRadius(WCSketchCircle *circle, const WPFloat &radius) :
	::WCAction("Set Sketch Circle Radius", circle->Sketch()), _circle(circle),
	_radius(radius), _oldRadius(0.0) {
	//Validate feature
	if (circle == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionSketchCircleRadius::WCActionSketchCircleRadius Error - NULL References passed.");
		//throw error
		return;
	}
}


WCActionSketchCircleRadius::WCActionSketchCircleRadius(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCAction( WCSerializeableObject::ElementFromName(element,"Action"), dictionary ),
	_circle(NULL), _radius(0.0), _oldRadius(0.0) {
	//Do something here
}


WCFeature* WCActionSketchCircleRadius::Execute(void) {
	//Update object based on rollback flag
	if (this->_rollback) this->_circle = (WCSketchCircle*)this->_dictionary->AddressFromGUID(this->_circleGUID);

	//Record the current radius
	this->_oldRadius = this->_circle->Radius();
	//Set the radius
	this->_circle->Radius(this->_radius);
	//Return the sketch
	return this->_circle->Sketch();
}


bool WCActionSketchCircleRadius::Rollback(void) {
	//If the object is preset
	if (this->_circle != NULL) {
		//Set the rollback flag
		this->_rollback = true;
		//Set self guid
		this->_guid = this->_dictionary->InsertAddress(this);
		//Move the object back to original radius
		this->_circle->Radius(this->_oldRadius);
		//Record GUID for object
		this->_circleGUID = this->_dictionary->GUIDFromAddress(this->_circle);
		//Return success
		return true;
	}
	//Not rolled back
	return false;
}


xercesc::DOMElement* WCActionSketchCircleRadius::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	return NULL;
}


/***********************************************~***************************************************/

