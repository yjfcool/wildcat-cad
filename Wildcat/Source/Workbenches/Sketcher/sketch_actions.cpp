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
#include "Sketcher/sketch_actions.h"
#include "Sketcher/sketch.h"
#include "Sketcher/sketch_feature.h"
#include "Kernel/document.h"
#include "PartDesign/part.h"
#include "PartDesign/plane.h"


/***********************************************~***************************************************/


WCActionSketchCreate::WCActionSketchCreate(WCFeature *creator, const std::string &sketchName, WCPlane *refPlane) : 
	::WCAction("Create Sketch", creator), _sketchName(sketchName), _refPlane(refPlane) {
	//Make sure creator and refPlane are valid
	if ((creator == NULL) || (refPlane == NULL)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionSketchCreate::WCActionSketchCreate - NULL creator or refPlane passed."); 
		return;		
	}
}

WCFeature* WCActionSketchCreate::Execute(void) {
	//Create the sketch
	WCSketch *sketch = new WCSketch(this->_creator, this->_sketchName, this->_refPlane);
	//Make sure sketch is not null
	if (sketch == NULL) { 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionSketchCreate::Execute - Sketch could not be created."); return false; }
	//Enter sketch workbench
	sketch->Document()->EnterWorkbench(sketch->Workbench());
	//Record the created sketch
	this->_sketch = sketch;
	return this->_sketch;
}


xercesc::DOMElement* WCActionSketchCreate::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	return NULL;
}


/***********************************************~***************************************************/


WCActionSketchMarkConstruction::WCActionSketchMarkConstruction(WCSketch *sketch, std::list<WCSketchFeature*> features) :
	::WCAction("Mark as Construction Element", sketch), _sketch(sketch) {
	//Make sure sketch is not null
	if (sketch == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionSketchMarkConstruction::WCActionSketchMarkConstruction - NULL sketch passed."); 
		return;		
	}
	//Copy list of sketch features
	this->_features = features;
}


WCFeature* WCActionSketchMarkConstruction::Execute(void) {
	std::list<WCSketchFeature*>::iterator iter;
	//Go through each feature
	for (iter = this->_features.begin(); iter != this->_features.end(); iter++) {
		//Record current state
		this->_history.push_back( (*iter)->IsConstruction() );
		//Set as construction
		(*iter)->IsConstruction(true);
	}
	CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCActionSketchMarkConstruction::Execute - Processed " << this->_features.size() << " elements.");
	//Quick final consistency check
	if (this->_features.size() != this->_history.size()) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionSketchMarkConstruction::Execute - Inconsistent feature vs. history list sizes.");
		//throw error
		return NULL;
	}
	//All is good
	else return this->_sketch;
}


xercesc::DOMElement* WCActionSketchMarkConstruction::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	return NULL;
}


WCActionSketchMarkNonConstruction::WCActionSketchMarkNonConstruction(WCSketch *sketch, std::list<WCSketchFeature*> features) :
	::WCAction("Mark as Non-Construction Element", sketch), _sketch(sketch) {
	//Make sure sketch is not null
	if (sketch == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionSketchMarkNonConstruction::WCActionSketchMarkNonConstruction - NULL sketch passed."); 
		return;		
	}
	//Copy list of sketch features
	this->_features = features;
}


WCFeature* WCActionSketchMarkNonConstruction::Execute(void) {
	std::list<WCSketchFeature*>::iterator iter;
	//Go through each feature
	for (iter = this->_features.begin(); iter != this->_features.end(); iter++) {
		//Make sure feature is not the Origin
		if ((*iter)->GetName() != "Origin") {
			//Record current state
			this->_history.push_back( (*iter)->IsConstruction() );
			//Set as construction
			(*iter)->IsConstruction(false);
		}
	}
	CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCActionSketchMarkNonConstruction::Execute - Processed " << this->_features.size() << " elements.");
	//Quick final consistency check
	if (this->_features.size() != this->_history.size()) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionSketchMarkNonConstruction::Execute - Inconsistent feature vs. history list sizes.");
		//throw error
		return NULL;
	}
	//All is good
	else return this->_sketch;
}


xercesc::DOMElement* WCActionSketchMarkNonConstruction::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	return NULL;
}


/***********************************************~***************************************************/


WCActionSketchDelete::WCActionSketchDelete(WCSketch *sketch, std::list<WCSketchFeature*> features) :
	::WCAction("Delete Sketch Elements", sketch), _sketch(sketch) {
	//Make sure sketch is not null
	if (sketch == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionSketchDelete::WCActionSketchDelete - NULL sketch passed."); 
		return;		
	}
	//Copy list of sketch features
	this->_features = features;
}


WCFeature* WCActionSketchDelete::Execute(void) {
	bool retVal = this->_sketch->DeleteFeatures(this->_features);
	//Set up count string
	std::ostringstream os;
	os << this->_features.size();
	std::string countStr = os.str();
	//Update document status
	this->_sketch->Document()->Status("Deleted " + countStr + " elements");
	CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCActionSketchDelete::Execute - Deleted " << this->_features.size() << " elements.");
	//Quick final consistency check
	if (!retVal) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCActionSketchMarkNonConstruction::Execute - Inconsistent feature vs. history list sizes.");
		//throw error
		return NULL;
	}
	//All is good
	else return this->_sketch;
}


xercesc::DOMElement* WCActionSketchDelete::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	return NULL;
}


/***********************************************~***************************************************/


