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
#include "Sketcher/sketch_feature.h"
#include "Sketcher/sketch.h"

/*** Feature Headers ***/
#include "Sketcher/sketch_point.h"
#include "Sketcher/sketch_line.h"
#include "Sketcher/sketch_circle.h"

/*** Constraint Headers ***/
#include "Constraint/constraint_angle.h"
#include "Constraint/constraint_coincidence.h"
#include "Constraint/constraint_distance.h"
#include "Constraint/constraint_fix.h"
#include "Constraint/constraint_horizontal.h"
#include "Constraint/constraint_length.h"
#include "Constraint/constraint_radius.h"
#include "Constraint/constraint_vertical.h"


/*** Local Defines ***/
#define SKETCHFEATURE_DEFAULT_COLOR				1.0, 1.0, 1.0, 0.8
#define SKETCHFEATURE_DEFAULT_TEXT_COLOR		1.0, 1.0, 1.0, 1.0
#define SKETCHFEATURE_CONSTRAINT_COLOR			1.0, 1.0, 0.0, 0.8
#define SKETCHFEATURE_SELECTED_COLOR			1.0, 0.0, 0.0, 1.0
#define SKETCHFEATURE_CONSTRUCTION_COLOR		0.2, 0.8, 1.0, 1.0
#define SKETCHFEATURE_INPROCESS_COLOR			0.0, 1.0, 0.0, 1.0
#define SKETCHFEATURE_POINT_SIZE				4.0
#define SKETCHFEATURE_LINE_THICKNESS			2.0
#define SKETCHFEATURE_LINESTIPPLE_FACTOR		2
#define SKETCHFEATURE_LINESTIPPLE_PATTERN		0x5555
#define SKETCHFEATURE_DEFAULT_CONSTRUCTION		false


//Static Member Initialization
WCColor	WCSketchFeature::DefaultColor(SKETCHFEATURE_DEFAULT_COLOR);
WCColor WCSketchFeature::DefaultTextColor(SKETCHFEATURE_DEFAULT_TEXT_COLOR);
WCColor	WCSketchFeature::ConstraintColor(SKETCHFEATURE_CONSTRAINT_COLOR);
WCColor	WCSketchFeature::SelectedColor(SKETCHFEATURE_SELECTED_COLOR);
WCColor	WCSketchFeature::ConstructionColor(SKETCHFEATURE_CONSTRUCTION_COLOR);
WCColor	WCSketchFeature::InprocessColor(SKETCHFEATURE_INPROCESS_COLOR);
GLfloat WCSketchFeature::PointSize(SKETCHFEATURE_POINT_SIZE);
GLfloat WCSketchFeature::LineThickness(SKETCHFEATURE_LINE_THICKNESS);
GLint WCSketchFeature::LineStippleFactor(SKETCHFEATURE_LINESTIPPLE_FACTOR);
GLushort WCSketchFeature::LineStipplePattern(SKETCHFEATURE_LINESTIPPLE_PATTERN);


/***********************************************~***************************************************/



WCSketchFeature::WCSketchFeature(WCSketch *sketch, const std::string &name) : ::WCFeature(sketch, name),
	_sketch(sketch), _constraintNodes() {
	//Make sure sketch is not null
	if (this->_sketch == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchFeature::WCSketchFeature - NULL Sketch passed.\n");
		//throw error
		return;
	}
}	


WCSketchFeature::WCSketchFeature(xercesc::DOMElement *element, WCSerialDictionary *dictionary) : 
	::WCFeature( WCSerializeableObject::ElementFromName(element,"Feature"), dictionary),
	_sketch(NULL), _constraintNodes() {

	//Make sure element if not null
	if (element == NULL) return;
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);
	//Set the sketch attribute
	this->_sketch = (WCSketch*)WCSerializeableObject::GetGUIDAttrib(element, "sketch", dictionary);
}


WCSketchFeature::~WCSketchFeature() {
}


bool WCSketchFeature::Name(const std::string &name) {
	std::cout << "WCSketchFeature::Name - Not yet implemented.\n";
	return false;
}


void WCSketchFeature::MarkDirty(void) {
	std::cout << "WCSketchFeature::MarkDirty - Not yet implemented.\n";
}


xercesc::DOMElement* WCSketchFeature::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create the base element for the object
	XMLCh* xmlString = xercesc::XMLString::transcode("SketchFeature");
	xercesc::DOMElement* element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Include the part feature element
	xercesc::DOMElement* featureElement = this->WCFeature::Serialize(document, dictionary);
	element->appendChild(featureElement);

	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);
	//Add part attribute
	WCSerializeableObject::AddGUIDAttrib(element, "sketch", this->_sketch, dictionary);
	//Return the new element
	return element;
}


/***********************************************~***************************************************/


bool WCSketchFeature::Deserialize(xercesc::DOMElement* featureElement, WCSerialDictionary *dictionary) {
	//Make sure feature element is not null
	if ((featureElement == NULL) || (dictionary == NULL)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchFeature::Deserialize - Feature element or dictionary was NULL.");
		//throw error
		return false;
	}
	//Get the feature name
	const XMLCh* xmlString = featureElement->getTagName();
	std::string name( xercesc::XMLString::transcode(xmlString) );

	/*** Features ***/

	//Find the feature to create
	if (name == "Point") {
		//Create the feature
		new WCSketchPoint(featureElement, dictionary);
		//All is good here
		return true;
	}
	else if (name == "SketchLine") {
		//Create the feature
		new WCSketchLine(featureElement, dictionary);
		//All is good here
		return true;
	}
	else if (name == "SketchCircle") {
		//Create the feature
		new WCSketchCircle(featureElement, dictionary);
		//All is good here
		return true;
	}

	/*** Constraints ***/

	else if (name == "ConstraintAngle") {
		//Create the feature
		new WCConstraintAngle(featureElement, dictionary);
		//All is good here
		return true;
	}
	else if (name == "ConstraintCoincidence") {
		//Create the feature
		new WCConstraintCoincidence(featureElement, dictionary);
		//All is good here
		return true;
	}
	else if (name == "ConstraintDistance") {
		//Create the feature
		new WCConstraintDistance(featureElement, dictionary);
		//All is good here
		return true;
	}
	else if (name == "ConstraintFix") {
		//Create the feature
		new WCConstraintFix(featureElement, dictionary);
		//All is good here
		return true;
	}
	else if (name == "ConstraintHorizontal") {
		//Create the feature
		new WCConstraintHorizontal(featureElement, dictionary);
		//All is good here
		return true;
	}
	else if (name == "ConstraintLength") {
		//Create the feature
		new WCConstraintLength(featureElement, dictionary);
		//All is good here
		return true;
	}
	else if (name == "ConstraintRadius") {
		//Create the feature
		new WCConstraintRadius(featureElement, dictionary);
		//All is good here
		return true;
	}
	else if (name == "ConstraintVertical") {
		//Create the feature
		new WCConstraintVertical(featureElement, dictionary);
		//All is good here
		return true;
	}

	/*** Cleanup ***/

	//Unknown part feature case
	CLOGGER_WARN(WCLogManager::RootLogger(), "WCSketchFeature::Deserialize - Unknown part feature: " << name);
	return false;
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCSketchFeature &feature) {
	out << "SketchFeature(" << &feature << ")\n";
	return out;
}


/***********************************************~***************************************************/

