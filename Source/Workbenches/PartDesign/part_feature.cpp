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
#include "PartDesign/part_feature.h"
#include "PartDesign/part.h"
#include "PartDesign/part_body.h"
#include "PartDesign/part_pad.h"
#include "PartDesign/part_plane.h"
#include "PartDesign/part_shaft.h"
#include "Sketcher/sketch.h"


//Hidden Color Definitions
#define PARTFEATURE_DEFAULT_POINT_COLOR			0.75, 0.75, 1.0, 1.0
#define PARTFEATURE_DEFAULT_CURVE_COLOR			1.0, 1.0, 1.0, 0.5
#define PARTFEATURE_DEFAULT_SURFACE_COLOR		0.804, 0.7451, 0.4392, 0.9
#define PARTFEATURE_CONSTRAINT_COLOR			1.0, 1.0, 0.0, 0.8
//#define PARTFEATURE_SELECTED_COLOR				0.9333, 0.706, 0.706, 0.75
#define PARTFEATURE_SELECTED_COLOR				1.0, 1.0, 0.1, 0.75
#define PARTFEATURE_CONSTRUCTION_COLOR			0.2, 0.8, 1.0, 0.5
#define PARTFEATURE_INPROCESS_COLOR				0.75, 0.75, 0.25, 0.75


//Static Color Initializations
WCColor	WCPartFeature::DefaultPointColor(PARTFEATURE_DEFAULT_POINT_COLOR);
WCColor	WCPartFeature::DefaultCurveColor(PARTFEATURE_DEFAULT_CURVE_COLOR);
WCColor	WCPartFeature::DefaultSurfaceColor(PARTFEATURE_DEFAULT_SURFACE_COLOR);
WCColor	WCPartFeature::ConstraintColor(PARTFEATURE_CONSTRAINT_COLOR);
WCColor	WCPartFeature::SelectedColor(PARTFEATURE_SELECTED_COLOR);
WCColor	WCPartFeature::ConstructionColor(PARTFEATURE_CONSTRUCTION_COLOR);
WCColor	WCPartFeature::InprocessColor(PARTFEATURE_INPROCESS_COLOR);
//Static Shader Initializations
GLuint WCPartFeature::DefaultPointRenderer(0);
GLuint WCPartFeature::DefaultCurveRenderer(0);
GLuint WCPartFeature::DefaultSurfaceRenderer(0);


/***********************************************~***************************************************/


WCPartFeature::WCPartFeature(WCFeature *creator, const std::string &name) : ::WCFeature(creator, name) {

	//See what type of feature creator is
	WCPart* part = dynamic_cast<WCPart*>(this->_creator);
	if (part != NULL) this->_part = part;
	else {
		WCPartFeature* pf = dynamic_cast<WCPartFeature*>(creator);
		if (pf != NULL) this->_part = pf->Part();
		else {
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCPartFeature::WCPartFeature - Neither Part nor PartFeature was passed as creator.");
		}
	}
}


WCPartFeature::WCPartFeature(xercesc::DOMElement *element, WCSerialDictionary *dictionary) : 
	::WCFeature( WCSerializeableObject::ElementFromName(element,"Feature"), dictionary) {
	//Make sure element if not null
	if (element == NULL) return;
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);
	//Set the part attribute
	this->_part = (WCPart*)WCSerializeableObject::GetGUIDAttrib(element, "part", dictionary);
}


WCPartFeature::~WCPartFeature() {
	//Nothing to do here
}


bool WCPartFeature::Name(const std::string &name) {
	return false;
}


void WCPartFeature::MarkDirty(void) {
}


xercesc::DOMElement* WCPartFeature::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create the base element for the object
	XMLCh* xmlString = xercesc::XMLString::transcode("PartFeature");
	xercesc::DOMElement* element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Include the part feature element
	xercesc::DOMElement* featureElement = this->WCFeature::Serialize(document, dictionary);
	element->appendChild(featureElement);

	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);
	//Add part attribute
	WCSerializeableObject::AddGUIDAttrib(element, "part", this->_part, dictionary);
	//Return the new element
	return element;
}


/***********************************************~***************************************************/


bool WCPartFeature::Deserialize(xercesc::DOMElement* featureElement, WCSerialDictionary *dictionary) {
	//Make sure feature element is not null
	if ((featureElement == NULL) || (dictionary == NULL)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCPartFeature::Deserialize - Feature element or dictionary was NULL.");
		//throw error
		return false;
	}
	//Get the feature name
	const XMLCh* xmlString = featureElement->getTagName();
	std::string name( xercesc::XMLString::transcode(xmlString) );

	//Find the feature to create
	if (name == "PartBody") {
		//Create the body feature
		new WCPartBody(featureElement, dictionary);
		//All is good here
		return true;
	}
	else if (name == "PartPad") {
		//Create the pad feature
		new WCPartPad(featureElement, dictionary);
		//All is good here
		return true;
	}
	else if (name == "PartPlane") {
		//Create the plane feature
		new WCPartPlane(featureElement, dictionary);
		//All is good here
		return true;
	}
	else if (name == "PartShaft") {
		//Create the shaft feature
		new WCPartShaft(featureElement, dictionary);
		//All is good here
		return true;
	}
	else if (name == "Sketch") {
		//Create the sketch feature
		WCSketch *sketch = new WCSketch(featureElement, dictionary);
		//All is good here
		return true;
	}

	//Unknown part feature case
	CLOGGER_WARN(WCLogManager::RootLogger(), "WCPartFeature::Deserialize - Unknown part feature: " << name);
	return false;
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCPartFeature &feature) {
	return out;
}


/***********************************************~***************************************************/

