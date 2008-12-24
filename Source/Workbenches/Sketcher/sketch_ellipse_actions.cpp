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
#include <Sketcher/sketch_ellipse_actions.h>
#include <Sketcher/sketch_ellipse.h>
#include <Sketcher/sketch.h>


/***********************************************~***************************************************/


WCActionSketchEllipseCreate::WCActionSketchEllipseCreate(WCSketch *sketch, const std::string &ellipseName, const WCVector4 &center,
	const WCVector4 &majorAxis, const WPFloat &semiMajor, const WPFloat &semiMinor) : ::WCAction("Create Sketch Ellipse", sketch),
	_ellipse(NULL),	_ellipseName(ellipseName), _sketch(sketch), _center(center), _majorAxis(majorAxis),
	_semiMajor(semiMajor), _semiMinor(semiMinor) {
	//Validate sketch
	if (sketch == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionSketchEllipseCreate::WCActionSketchEllipseCreate Error - NULL References passed.");
		//throw error
		return;
	}
}


WCActionSketchEllipseCreate::WCActionSketchEllipseCreate(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCAction( WCSerializeableObject::ElementFromName(element,"Action"), dictionary ),
	_ellipse(NULL), _ellipseName(), _sketch(NULL), _center(), _semiMajor(), _semiMinor(0.0) {
	//Do something here
}


WCFeature* WCActionSketchEllipseCreate::Execute(void) {
	//Update pointers (based on rollback flag)
	if (this->_rollback) {
		this->_sketch = (WCSketch*)this->_dictionary->AddressFromGUID(this->_sketchGUID);
	}

	//Create the ellipse
	WCSketchEllipse *ellipse = new WCSketchEllipse(this->_sketch, this->_ellipseName, this->_center, this->_majorAxis, 
												   this->_semiMajor, this->_semiMinor);
	//Make sure object is not null
	if (ellipse == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionSketchEllipseCreate::Execute Error - Sketch ellipse could not be created.");
		return NULL;
	}
	
	//If a rollback execution, update guid and address
	if (this->_rollback) this->_dictionary->UpdateAddress(this->_guid, ellipse);
	//Set the pointer
	this->_ellipse = ellipse;
	return this->_sketch;
}


bool WCActionSketchEllipseCreate::Rollback(void) {
	//If the object is preset
	if (this->_ellipse != NULL) {
		//Set the rollback flag
		this->_rollback = true;
		//Set self guid
		this->_guid = this->_dictionary->InsertAddress(this->_ellipse);
		//Delete the object
		delete this->_ellipse;
		//Record GUIDs for sketch
		this->_sketchGUID = this->_dictionary->GUIDFromAddress(this->_sketch);
		//Return success
		return true;
	}
	//Not rolled back
	return false;
}


xercesc::DOMElement* WCActionSketchEllipseCreate::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	return NULL;
}


/***********************************************~***************************************************/


WCActionSketchEllipseMove::WCActionSketchEllipseMove(WCSketchEllipse *ellipse, const WCVector4 &center) :
	::WCAction("Move Sketch Ellipse", ellipse->Sketch()), _ellipse(ellipse), _center(center), _oldCenter() {
	//Validate feature
	if (ellipse == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionSketchEllipseMove::WCActionSketchEllipseMove Error - NULL References passed.");
		//throw error
		return;
	}
}


WCActionSketchEllipseMove::WCActionSketchEllipseMove(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCAction( WCSerializeableObject::ElementFromName(element,"Action"), dictionary ),
	_ellipse(NULL), _center(), _oldCenter() {
	//Do something here
}


WCFeature* WCActionSketchEllipseMove::Execute(void) {
	//Update object based on rollback flag
	if (this->_rollback) this->_ellipse = (WCSketchEllipse*)this->_dictionary->AddressFromGUID(this->_ellipseGUID);

	//Record the current position
	this->_oldCenter = this->_ellipse->Center();
	//Move the object
	this->_ellipse->Center(this->_center);
	//Return the sketch
	return this->_ellipse->Sketch();
}


bool WCActionSketchEllipseMove::Rollback(void) {
	//If the object is preset
	if (this->_ellipse != NULL) {
		//Set the rollback flag
		this->_rollback = true;
		//Set self guid
		this->_guid = this->_dictionary->InsertAddress(this);
		//Move the object back to original position
		this->_ellipse->Center(this->_oldCenter);
		//Record GUID for object
		this->_ellipseGUID = this->_dictionary->GUIDFromAddress(this->_ellipse);
		//Return success
		return true;
	}
	//Not rolled back
	return false;
}


xercesc::DOMElement* WCActionSketchEllipseMove::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	return NULL;
}


/***********************************************~***************************************************/


WCActionSketchEllipseMajor::WCActionSketchEllipseMajor(WCSketchEllipse *ellipse, const WCVector4 &majorAxis, const WPFloat &major) :
	::WCAction("Move Sketch Ellipse Major", ellipse->Sketch()), _ellipse(ellipse),
	_majorAxis(majorAxis), _oldMajorAxis(), _major(major), _oldMajor() {
	//Validate feature
	if (ellipse == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionSketchEllipseMajor::WCActionSketchEllipseMajor Error - NULL References passed.");
		//throw error
		return;
	}
}


WCActionSketchEllipseMajor::WCActionSketchEllipseMajor(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCAction( WCSerializeableObject::ElementFromName(element,"Action"), dictionary ),
	_ellipse(NULL), _major(), _oldMajor() {
	//Do something here
}


WCFeature* WCActionSketchEllipseMajor::Execute(void) {
	//Update object based on rollback flag
	if (this->_rollback) this->_ellipse = (WCSketchEllipse*)this->_dictionary->AddressFromGUID(this->_ellipseGUID);

	//Record the current position
	this->_oldMajorAxis = this->_ellipse->MajorAxis();
	this->_oldMajor = this->_ellipse->MajorRadius();
	//Move the object
	this->_ellipse->SemiMajor(this->_majorAxis, this->_major);
	//Return the sketch
	return this->_ellipse->Sketch();
}


bool WCActionSketchEllipseMajor::Rollback(void) {
	//If the object is preset
	if (this->_ellipse != NULL) {
		//Set the rollback flag
		this->_rollback = true;
		//Set self guid
		this->_guid = this->_dictionary->InsertAddress(this);
		//Move the object back to original position
		this->_ellipse->SemiMajor(this->_oldMajorAxis, this->_oldMajor);
		//Record GUID for object
		this->_ellipseGUID = this->_dictionary->GUIDFromAddress(this->_ellipse);
		//Return success
		return true;
	}
	//Not rolled back
	return false;
}


xercesc::DOMElement* WCActionSketchEllipseMajor::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	return NULL;
}


/***********************************************~***************************************************/


WCActionSketchEllipseMinor::WCActionSketchEllipseMinor(WCSketchEllipse *ellipse, const WPFloat &minor) :
	::WCAction("Set Sketch Ellipse Minor", ellipse->Sketch()), _ellipse(ellipse), _minor(minor), _oldMinor() {
	//Validate feature
	if (ellipse == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionSketchEllipseMinor::WCActionSketchEllipseMinor Error - NULL References passed.");
		//throw error
		return;
	}
}


WCActionSketchEllipseMinor::WCActionSketchEllipseMinor(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCAction( WCSerializeableObject::ElementFromName(element,"Action"), dictionary ),
	_ellipse(NULL), _minor(), _oldMinor() {
	//Do something here
}


WCFeature* WCActionSketchEllipseMinor::Execute(void) {
	//Update object based on rollback flag
	if (this->_rollback) this->_ellipse = (WCSketchEllipse*)this->_dictionary->AddressFromGUID(this->_ellipseGUID);

	//Record the current position
	this->_oldMinor = this->_ellipse->SemiMinor();
	//Move the object
	this->_ellipse->SemiMinor(this->_minor);
	//Return the sketch
	return this->_ellipse->Sketch();
}


bool WCActionSketchEllipseMinor::Rollback(void) {
	//If the object is preset
	if (this->_ellipse != NULL) {
		//Set the rollback flag
		this->_rollback = true;
		//Set self guid
		this->_guid = this->_dictionary->InsertAddress(this);
		//Move the object back to original value
		this->_ellipse->SemiMinor(this->_oldMinor);
		//Record GUID for object
		this->_ellipseGUID = this->_dictionary->GUIDFromAddress(this->_ellipse);
		//Return success
		return true;
	}
	//Not rolled back
	return false;
}


xercesc::DOMElement* WCActionSketchEllipseMinor::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	return NULL;
}


/***********************************************~***************************************************/

