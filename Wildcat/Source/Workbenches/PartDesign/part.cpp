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
#include "PartDesign/part.h"
#include "PartDesign/part_feature.h"
#include "Kernel/workbench_layer.h"


/***********************************************~***************************************************/


WCPart::WCPart(std::string name, std::string filename) : ::WCDocument(NULL, name, filename), 
	_featureMap(), _featureList(), _workbench(NULL), _currentBody(NULL),
	_pointLayer(NULL), _lineLayer(NULL), _curveLayer(NULL), _surfaceLayer(NULL),
	_pointMap(), _lineMap(), _curveMap(), _surfaceMap() {

	//Check feature name
	if (this->_name == "") this->_name = this->RootName() + ".1";
	//Create event handler
	this->_controller = new WCPartController(this);
	//Create tree element
	WSTexture* partIcon = this->_scene->TextureManager()->TextureFromName("part32");
	this->_treeElement = new WCTreeElement(this->_treeView, this->_name, this->_controller, partIcon);
	
	//Create rendering layers
	this->_pointLayer = new WCPointLayer(this->_scene, "Points");
	this->_lineLayer = new WCLineLayer(this->_scene, "Lines");
	this->_curveLayer = new WCVisualLayer(this->_scene, "Curves");
	this->_surfaceLayer = new WCVisualLayer(this->_scene, "Surfaces");
	this->_surfaceLayer->RenderProgram(this->_scene->ShaderManager()->ProgramFromName("scn_basiclight"));

	//Register layers
	this->_scene->RegisterLayer(this->_pointLayer);
	this->_scene->RegisterLayer(this->_lineLayer);
	this->_scene->RegisterLayer(this->_curveLayer);
	this->_scene->RegisterLayer(this->_surfaceLayer);
	//Need to order layers
	this->_scene->LayerBelow(this->_pointLayer, this->_uiLayer);
	this->_scene->LayerBelow(this->_curveLayer, this->_pointLayer);
	this->_scene->LayerBelow(this->_lineLayer, this->_curveLayer);
	this->_scene->LayerBelow(this->_surfaceLayer, this->_lineLayer);

	//Create workbench and enter it
	this->_workbench = new WCPartWorkbench(this);
	this->_workbench->Retain(*this);
	this->_workbench->CreateInitialObjects();
	if (this->_document == this)
		this->EnterWorkbench(this->_workbench);
}


WCPart::WCPart(xercesc::DOMElement *element, WCSerialDictionary *dictionary) : 
	::WCDocument( WCSerializeableObject::ElementFromName(element,"Document"), dictionary),
	_featureMap(), _featureList(), _workbench(NULL), _currentBody(NULL),
	_pointLayer(NULL), _lineLayer(NULL), _curveLayer(NULL), _surfaceLayer(NULL),
	_pointMap(), _lineMap(), _curveMap(), _surfaceMap() {
	
	//Make sure element if not null
	if (element == NULL) return;
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

	//Create event handler
	this->_controller = new WCPartController(this);
	//Create tree element
	WSTexture* partIcon = this->_scene->TextureManager()->TextureFromName("part32");
	this->_treeElement = new WCTreeElement(this->_treeView, this->_name, this->_controller, partIcon);
	
	//Create rendering layers
	this->_pointLayer = new WCPointLayer(this->_scene, "Points");
	this->_lineLayer = new WCLineLayer(this->_scene, "Lines");
	this->_curveLayer = new WCVisualLayer(this->_scene, "Curves");
	this->_surfaceLayer = new WCVisualLayer(this->_scene, "Surfaces");
	this->_surfaceLayer->RenderProgram(this->_scene->ShaderManager()->ProgramFromName("scn_basiclight"));

	//Register layers
	this->_scene->RegisterLayer(this->_pointLayer);
	this->_scene->RegisterLayer(this->_lineLayer);
	this->_scene->RegisterLayer(this->_curveLayer);
	this->_scene->RegisterLayer(this->_surfaceLayer);
	//Need to order layers
	this->_scene->LayerBelow(this->_pointLayer, this->_uiLayer);
	this->_scene->LayerBelow(this->_curveLayer, this->_pointLayer);
	this->_scene->LayerBelow(this->_lineLayer, this->_curveLayer);
	this->_scene->LayerBelow(this->_surfaceLayer, this->_lineLayer);

	//Create workbench and enter it
	this->_workbench = new WCPartWorkbench(this);
	this->_workbench->Retain(*this);
	if (this->_document == this)
		this->EnterWorkbench(this->_workbench);

	//Create part features
	XMLCh* xmlString = xercesc::XMLString::transcode("Features");
	xercesc::DOMNodeList *list = element->getElementsByTagName(xmlString)->item(0)->getChildNodes();
	xercesc::XMLString::release(&xmlString);
	xercesc::DOMNode *tmpNode;
	xercesc::DOMElement *featureElement;
	bool retVal;
	//Loop through all features
	for (WPUInt featureIndex=0; featureIndex < list->getLength(); featureIndex++) {
		//Get the indexed node
		tmpNode = list->item(featureIndex);
		//Make sure node is element
		if (tmpNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
			//Cast node to element
			featureElement = (xercesc::DOMElement*)tmpNode;
			//Pass to master part feature switch
			retVal = WCPartFeature::Deserialize(featureElement, dictionary);
		}
	}
	//Get the current body
	this->_currentBody = (WCPartBody*)WCSerializeableObject::GetGUIDAttrib(element, "currentbody", dictionary);

	//Need to restore published geometry
	//...
}


WCPart::~WCPart() {
	//Delete the workbench
	this->_workbench->Release(*this);
	if (this->_workbench != NULL) delete this->_workbench;
	//Delete the layers
	delete this->_pointLayer;
	delete this->_lineLayer;
	delete this->_curveLayer;
	delete this->_surfaceLayer;
}


void WCPart::Body(WCPartBody* body) {
	//Make sure body is not null
	if (body == NULL) {CLOGGER_ERROR(WCLogManager::RootLogger(), "WCPart::Body - NULL value passed.");}
	else this->_currentBody = body;
}


bool WCPart::AddPoint(WCGeometricPoint *point, WCEventController* controller) {
	//Make sure point is non-null
	if (point == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCPart::AddPoint - NULL point passed.");
		return false;
	}
	//Check to see if point already in list
	std::map<WCGeometricPoint*,WCEventController*>::iterator iter = this->_pointMap.find(point);
	if (iter != this->_pointMap.end()) {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCPart::AddPoint - Point is already in the sketch.");
		return false;
	}
	//Add into the sketch list
	this->_pointMap.insert( std::make_pair(point, controller) );
	//Add point into selector
	this->_workbench->SelectionManager()->AddObject(point, controller);
	//Add point into drawing layer
	this->_pointLayer->AddPoint(point);
	//Mark sketch as dirty
	this->_isFeatureDirty = true;
	//All is good
	return true;
}


bool WCPart::RemovePoint(WCGeometricPoint *point) {
	//Make sure point is non-null
	if (point == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCPart::RemovePoint - NULL point passed.");
		return false;
	}
	//Try to erase it
	if (this->_pointMap.erase(point) > 0) {
		//Remove point from selector
		this->_workbench->SelectionManager()->RemoveObject(point);
		//Remove point from layer
		this->_pointLayer->RemovePoint(point);
		//Mark the part as dirty
		this->_isFeatureDirty = true;
		//Return true
		return true;
	}
	//Otherwise, not found
	return false;
}


bool WCPart::AddLine(WCGeometricLine *line, WCEventController* controller) {
	//Make sure line is non-null
	if (line == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCPart::AddLine - NULL line passed.");
		return false;
	}
	//Check to see if line already in list
	std::map<WCGeometricLine*,WCEventController*>::iterator iter = this->_lineMap.find(line);
	if (iter != this->_lineMap.end()) {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCPart::AddLine - Line is already in the part.");
		return false;
	}
	//Add into the part list
	this->_lineMap.insert( std::make_pair(line, controller) );
	//Add line into selector
	this->_workbench->SelectionManager()->AddObject(line, controller);
	//Add line into drawing layer
	this->_lineLayer->AddLine(line);
	//Mark part as dirty
	this->_isFeatureDirty = true;
	//All is good
	return true;
}


bool WCPart::RemoveLine(WCGeometricLine *line) {
	//Make sure line is non-null
	if (line == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCPart::RemoveLine - NULL line passed.");
		return false;
	}
	//Try to erase it
	if (this->_lineMap.erase(line) > 0) {
		//Remove line from selector
		this->_workbench->SelectionManager()->RemoveObject(line);
		//Remove line from layer
		this->_lineLayer->RemoveLine(line);
		//Mark the sketch as dirty
		this->_isFeatureDirty = true;
		//Return true
		return true;
	}
	//Otherwise, not found
	return false;
}


bool WCPart::AddCurve(WCGeometricCurve *curve, WCEventController* controller) {
	//Make sure curve is non-null
	if (curve == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCPart::AddCurve - NULL line passed.");
		return false;
	}
	//Check to see if line already in list
	std::map<WCGeometricCurve*,WCEventController*>::iterator iter = this->_curveMap.find(curve);
	if (iter != this->_curveMap.end()) {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCPart::AddCurve - Line is already in the part.");
		return false;
	}
	//Add into the part list
	this->_curveMap.insert( std::make_pair(curve, controller) );
	//Add object into selector
	this->_workbench->SelectionManager()->AddObject(curve, controller);
	//Add object into drawing layer
	this->_curveLayer->AddObject(curve);
	//Mark part as dirty
	this->_isFeatureDirty = true;
	//All is good
	return true;
}


bool WCPart::RemoveCurve(WCGeometricCurve *curve) {
	//Make sure curve is non-null
	if (curve == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCPart::RemoveCurve - NULL line passed.");
		return false;
	}
	//Try to erase it
	if (this->_curveMap.erase(curve) > 0) {
		//Remove curve from selector
		this->_workbench->SelectionManager()->RemoveObject(curve);
		//Remove line from layer
		this->_curveLayer->RemoveObject(curve);
		//Mark the sketch as dirty
		this->_isFeatureDirty = true;
		//Return true
		return true;
	}
	//Otherwise, not found
	return false;
}


bool WCPart::AddSurface(WCGeometricSurface *surface, WCEventController* controller) {
	//Make sure object is non-null
	if (surface == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCPart::AddSurface - NULL surface passed.");
		return false;
	}
	//Check to see if line already in list
	std::map<WCGeometricSurface*,WCEventController*>::iterator iter = this->_surfaceMap.find(surface);
	if (iter != this->_surfaceMap.end()) {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCPart::AddSurface - Surface is already in the part.");
		return false;
	}
	//Add into the part list
	this->_surfaceMap.insert( std::make_pair(surface, controller) );
	//Add object into selector
	this->_workbench->SelectionManager()->AddObject(surface, controller);
	//Add object into drawing layer
	this->_surfaceLayer->AddObject(surface);
	//Mark part as dirty
	this->_isFeatureDirty = true;
	//All is good
	return true;
}


bool WCPart::RemoveSurface(WCGeometricSurface *surface) {
	return false;
}


bool WCPart::Name(const std::string &name) {
	return false;
}


void WCPart::MarkDirty(void) {
	this->_isFeatureDirty = true;
}


void WCPart::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
	CLOGGER_WARN(WCLogManager::RootLogger(), "WCPart::ReceiveNotice - Not yet implemented.");
}


bool WCPart::Regenerate(void) {
	return false;
}


bool WCPart::AddFeature(WCPartFeature *feature, const bool &selectable) {
	//Make sure feature is non-null
	if (feature == NULL) { 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCPart::AddFeature - Passed feature is null."); 
		return false;
	}
	//Make sure the feature's name is unique and not empty - if not, rename it
	std::map<std::string, WCPartFeature*>::iterator iter = this->_featureMap.find(feature->GetName());
	if (iter != this->_featureMap.end()) {
		feature->Name( this->GenerateFeatureName(feature) );
	}
	//Add the feature into the map
	this->_featureMap.insert( make_pair(feature->GetName(), feature) );
	//Add the feature into the list
	this->_featureList.push_back(feature);
	//If selectable...
	if (selectable) {
		//Add to the selection manager
		WCVisualObject *so = dynamic_cast<WCVisualObject*>(feature);
		this->_workbench->SelectionManager()->AddObject(so, feature->Controller());
		//Add to the part workbench layer
		this->_workbench->Layer()->AddObject(so);
	}
	//Retain the feature
	feature->Retain(*this);
	return true;
}


bool WCPart::RemoveFeature(WCPartFeature *feature) {
	//Make sure feature is non-null
	if (feature == NULL) { 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCPart::RemoveFeature - Passed feature is null."); 
		return false;
	}
	//Make sure the feature in the part
	std::map<std::string, WCPartFeature*>::iterator iter = this->_featureMap.find(feature->GetName());
	if (iter == this->_featureMap.end()) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCPart::RemoveFeature - Feature not in Part."); 
		return false;
	}
	//Remove the feature from the map
	this->_featureMap.erase(iter);
	//Remove the feature from the list
	this->_featureList.remove((*iter).second);
	//Retain the feature
	feature->Retain(*this);
	//Force a regeneration of entire part
	return this->Regenerate();
}


bool WCPart::DeleteFeatures(std::list<WCPartFeature*> features) {
	std::list<WCPartFeature*>::iterator iter;
	bool found;
	//Loop through all requested objects
	for (iter = features.begin(); iter != features.end(); iter++) {
		if ((*iter) == NULL) {
			CLOGGER_WARN(WCLogManager::RootLogger(), "WCSketch::DeleteFeatures - NULL Feature found."); 
		} else {
			//Iterate through the list of features
			std::map<std::string,WCPartFeature*>::iterator featureIter;
			found = false;
			//Check to see if feature is still in sketch
			for (featureIter = this->_featureMap.begin(); featureIter != this->_featureMap.end(); featureIter++) {
				if ((*iter) == (*featureIter).second) found = true;
			}
			//If the feature was found, it was still in the part
			if (found) {
				std::cout << "Deleting: " << (*iter)->GetName() << std::endl;
				delete *iter;
			}
		}
	}
	//Regenerate part
	//...
	//All is good for now
	return true;
}


bool WCPart::ReorderFeature(WCPartFeature *feature) {
	//...
	//Force regeneration of entire part
	return this->Regenerate();
}


WCPartFeature* WCPart::FeatureFromName(const std::string &name) {
	std::map<std::string, WCPartFeature*>::iterator iter = this->_featureMap.find(name);
	if (iter == this->_featureMap.end()) return NULL;
	return iter->second;
}


std::string WCPart::GenerateFeatureName(WCFeature *feature) {
	std::string root = feature->GetName(); 
	//Catch empty case
	if (root == "") {
		root = feature->RootName();
	}
	WPUInt index = 1;
	std::string id, newString;
	//Loop until a good name is found
	do {
		//Get index into a string
		std::stringstream ss;
		ss << index;
		ss >> id;
		newString = root + "." + id;
		index++;
//		std::cout << "Index = " << index << " Name: " << newString << std::endl;
	} while (!this->CheckName(newString));
	//Found a good name
	return newString;
}


xercesc::DOMElement* WCPart::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create the base element for the part
	XMLCh* xmlString = xercesc::XMLString::transcode("Part");
	xercesc::DOMElement*  element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Include the parent element
	xercesc::DOMElement *inheritedElem = this->WCDocument::Serialize(document, dictionary);
	if (inheritedElem != NULL) element->appendChild(inheritedElem);

	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);
	//Add CurrentBody attribute
	WCSerializeableObject::AddGUIDAttrib(element, "currentbody", this->_currentBody, dictionary);

	//Add a features child
	xmlString = xercesc::XMLString::transcode("Features");
	xercesc::DOMElement *features = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	element->appendChild(features);

	//Loop through all features and add them in
	xercesc::DOMElement *featureElem;
	std::list<WCPartFeature*>::iterator featureIter;
	for (featureIter = this->_featureList.begin(); featureIter != this->_featureList.end(); featureIter++) {
		//Get the feature element
		featureElem = (*featureIter)->Serialize(document, dictionary);
		//Add the feature element as a child to this one
		if (featureElem != NULL) features->appendChild(featureElem);
	}

	//Add an actions child
	xmlString = xercesc::XMLString::transcode("Actions");
	xercesc::DOMElement *actions = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	element->appendChild(actions);

	//Loop through all actions and add them in
	xercesc::DOMElement *actionElem;
	std::list<WCAction*>::iterator actionIter;
	for (actionIter = this->_actions.begin(); actionIter != this->_actions.end(); actionIter++) {
		//Get the feature element
		actionElem = (*actionIter)->Serialize(document, dictionary);
		//Add the feature element as a child to this one
		if (actionElem != NULL) actions->appendChild(actionElem);
	}

	//Return the element
	return element;
}

xercesc::DOMDocument* WCPart::Serialize(void) {
	return NULL;
}


/***********************************************~***************************************************/


WCActionPartDelete* WCPart::ActionDelete(WCPart *part, std::list<WCPartFeature*> features) {
	//Create new action to delete selected objects from the part
	return new WCActionPartDelete(part, features);
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCPart &part) {
	//Print out some info
	out << "Part(" << &part << ")\n";
	//Print the scene
	out << "\t" << *(part._scene);
	//Print the tree
	out << "\t" << *(part._treeView);
	//Print all of the features in order
	out << "\t Features(" << part._featureMap.size() << ")\n";
	std::list<WCPartFeature*>::iterator featureIter;
//	for (featureIter = part._featureList.begin(); featureIter != part._featureList.end(); featureIter++) {
//		out << "\t" << *featureIter << std::endl;
//	}
	return out;
}


/***********************************************~***************************************************/

