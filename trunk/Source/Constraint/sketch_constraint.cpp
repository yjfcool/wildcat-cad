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
#include <Constraint/sketch_constraint.h>
#include <Constraint/sketch_constraint_modes.h>


/***********************************************~***************************************************/


WCSketchConstraint::WCSketchConstraint(WCSketch *sketch, const std::string &name) : ::WCSketchFeature(sketch), ::WCVisualObject(),
	_isEvaluated(true), _isExplicit(false) {
	//Nothing else to do here for now
}


WCSketchConstraint::WCSketchConstraint(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	WCSketchFeature( WCSerializeableObject::ElementFromName(element,"SketchFeature"), dictionary ),
	::WCVisualObject(), _isEvaluated(false), _isExplicit(false) {
	//Make sure element if not null
	if (element == NULL) return;
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

	//Set the evaluated attribute
	this->_isEvaluated = WCSerializeableObject::GetBoolAttrib(element, "evaluated");
	//Set the explicit attribute
	this->_isExplicit = WCSerializeableObject::GetBoolAttrib(element, "explicit");
	//Restore visibile object attribs
	this->_isVisible = WCSerializeableObject::GetBoolAttrib(element, "visible");
}


xercesc::DOMElement* WCSketchConstraint::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create the base element for the object
	XMLCh* xmlString = xercesc::XMLString::transcode("SketchConstraint");
	xercesc::DOMElement* element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Include the part feature element
	xercesc::DOMElement* featureElement = this->WCSketchFeature::Serialize(document, dictionary);
	element->appendChild(featureElement);

	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);
	//Add evaluated attribute
	WCSerializeableObject::AddBoolAttrib(element, "evaluated", this->_isEvaluated);
	//Add evaluated attribute
	WCSerializeableObject::AddBoolAttrib(element, "explicit", this->_isExplicit);
	//Add visible attribute
	WCSerializeableObject::AddBoolAttrib(element, "visible", this->_isVisible);
	
	//Return the new element
	return element;
}


/***********************************************~***************************************************/


WCDrawingMode* WCSketchConstraint::ModeCreateGeometric(WCSketchWorkbench *wb) {
	//Create a new geometric constraint drawing mode
	return new WCModeSketchConstraintGeometricCreate(wb);
}


WCDrawingMode* WCSketchConstraint::ModeCreateDimensional(WCSketchWorkbench *wb) {
	//Create a new dimensional constraint drawing mode
	return new WCModeSketchConstraintDimensionalCreate(wb);
}


/***********************************************~***************************************************/

