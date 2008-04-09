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
#include "RTVisualization/visualization.h"
#include "RTVisualization/vis_feature.h"
#include "RTVisualization/vis_listener.h"
#include "Kernel/workbench_layer.h"


/***********************************************~***************************************************/


void WCVisualization::Initialize(void) {
	//Check feature name
	if (this->_name == "") this->_name = this->RootName() + ".1";
	//Create event handler
	this->_controller = new WCVisController(this);

	//Create tree elements
	WSTexture* icon = this->_scene->TextureManager()->TextureFromName("folder32");
	WSTexture* folderIcon = this->_scene->TextureManager()->TextureFromName("folder32");
	this->_treeElement = new WCTreeElement(this->_treeView, this->_name, this->_controller, icon);
	this->_featureTreeElement = new WCTreeElement(this->_treeView, "Features", this->_controller, folderIcon);
	this->_listenerTreeElement = new WCTreeElement(this->_treeView, "Listeners", this->_controller, folderIcon);
	this->_recorderTreeElement = new WCTreeElement(this->_treeView, "Recorders", this->_controller, folderIcon);
	//Add child elements to sketch tree element
	this->_treeElement->AddLastChild(this->_listenerTreeElement);
	this->_treeElement->AddLastChild(this->_recorderTreeElement);
	this->_treeElement->AddLastChild(this->_featureTreeElement);
	this->_listenerTreeElement->IsOpen(false);
	this->_recorderTreeElement->IsOpen(false);
	this->_featureTreeElement->IsOpen(true);

	//Create rendering layers
	this->_visualLayer = new WCVisualLayer(this->_scene, "Visualization");
	//Register layers
	this->_scene->RegisterLayer(this->_visualLayer);
	//Need to order layers
	this->_scene->LayerBelow(this->_visualLayer, this->_uiLayer);

	//Create workbench and enter it
	this->_workbench = new WCVisWorkbench(this);
	this->_workbench->Retain(*this);
}


/***********************************************~***************************************************/


WCVisualization::WCVisualization(const std::string &configFile, const std::string &outputFile) : ::WCDocument(NULL, "", configFile), 
	_featureMap(), _featureList(), _workbench(NULL), _visualLayer(NULL), 
	_featureTreeElement(NULL), _listenerTreeElement(NULL), _recorderTreeElement(NULL), _state(WCVisualizationState::Stopped()) {
	//Just initialize the visualization
	this->Initialize();
	//Create initial object and enter the workbench
	this->_workbench->CreateInitialObjects();
	if (this->_document == this)
		this->EnterWorkbench(this->_workbench);
}


WCVisualization::WCVisualization(xercesc::DOMElement *element, WCSerialDictionary *dictionary) : 
	::WCDocument( WCSerializeableObject::ElementFromName(element,"Document"), dictionary),
	_featureMap(), _featureList(), _workbench(NULL), _visualLayer(NULL),
	_featureTreeElement(NULL), _listenerTreeElement(NULL), _recorderTreeElement(NULL), _state(WCVisualizationState::Stopped()) {
	//Make sure element if not null
	if (element == NULL) return;
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

	//Initalize
	this->Initialize();

	//Read in all signals
	//...

	//Create visualization features
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
			retVal = WCVisFeature::Deserialize(featureElement, dictionary);
		}
	}

	//Enter the workbench if primary document
	if (this->_document == this)
		this->EnterWorkbench(this->_workbench);
}


WCVisualization::~WCVisualization() {
	//Stop visualizing (if needed)
	if (this->_state == WCVisualizationState::Running()) this->StopVisualization();

	//Delete all features
	while (!this->_featureList.empty()) {
		//Delete the back listener
		delete this->_featureList.back();
	}

	//Delete the workbench
	this->_workbench->Release(*this);
	if (this->_workbench != NULL) delete this->_workbench;
	//Delete the layer
	delete this->_visualLayer;
}


bool WCVisualization::Name(const std::string &name) {
	//Need to figure out how to set the name of the visualization
	//...
	//For now return false;
	return false;
}


void WCVisualization::MarkDirty(void) {
	//Mark as dirty
	this->_isFeatureDirty = true;
	//Do we need to go up in the heirarchy?
	//...
}


void WCVisualization::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
	CLOGGER_WARN(WCLogManager::RootLogger(), "WCVisualization::ReceiveNotice - Not yet implemented.");
}


bool WCVisualization::Regenerate(void) {
	//Really need to figure out how we are going to implement this
	//...
	//For now always return true
	return true;
}


bool WCVisualization::AddFeature(WCVisFeature *feature, const bool &selectable) {
	//Make sure feature is non-null
	if (feature == NULL) { 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCVisualization::AddFeature - Passed feature is null."); 
		return false;
	}
	//Make sure the feature's name is unique and not empty - if not, rename it
	std::map<std::string, WCVisFeature*>::iterator iter = this->_featureMap.find(feature->GetName());
	if (iter != this->_featureMap.end()) {
		feature->Name( this->GenerateFeatureName(feature) );
	}
	//Add the feature into the maps
	this->_featureMap.insert( std::make_pair(feature->GetName(), feature) );
	//Add the feature into the list
	this->_featureList.push_back(feature);
	//If selectable...
	if (selectable) {
		//For now this will hide the motion trackers vs. all other features
		this->_idMap.insert( std::make_pair(feature->ID(), feature));
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


bool WCVisualization::RemoveFeature(WCVisFeature *feature, const bool &selectable) {
	//Make sure feature is non-null
	if (feature == NULL) { 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCVisualization::RemoveFeature - Passed feature is null."); 
		return false;
	}
	//Make sure the feature in the document
	std::map<std::string, WCVisFeature*>::iterator featureIter = this->_featureMap.find(feature->GetName());
	std::map<unsigned int, WCVisFeature*>::iterator idIter = this->_idMap.find(feature->ID());
	if (featureIter == this->_featureMap.end()) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCVisualization::RemoveFeature - Feature not in Visualization."); 
		return false;
	}
	//Remove the feature from the maps
	this->_featureMap.erase(featureIter);
	//Remove the feature from the list
	this->_featureList.remove((*featureIter).second);
	//Release the feature
	feature->Release(*this);
	//If selectable...
	if (selectable) {
		//Remove from the id map
		this->_idMap.erase(idIter);
		//Remove from selection manager
		WCVisualObject *so = dynamic_cast<WCVisualObject*>(feature);
		this->_workbench->SelectionManager()->RemoveObject(so);
		//Remove from workbench layer
		this->_workbench->Layer()->RemoveObject(so);
	}
	//Force a regeneration of entire part
	return this->Regenerate();
}


bool WCVisualization::DeleteFeatures(std::list<WCVisFeature*> features) {
	std::list<WCVisFeature*>::iterator iter;
	bool found;
	//Loop through all requested objects
	for (iter = features.begin(); iter != features.end(); iter++) {
		if ((*iter) == NULL) {
			CLOGGER_WARN(WCLogManager::RootLogger(), "WCVisualization::DeleteFeatures - NULL Feature found."); 
		} else {
			//Iterate through the list of features
			std::map<std::string,WCVisFeature*>::iterator featureIter;
			found = false;
			//Check to see if feature is still in part
			for (featureIter = this->_featureMap.begin(); featureIter != this->_featureMap.end(); featureIter++) {
				if ((*iter) == (*featureIter).second) found = true;
			}
			//If the feature was found, it was still in the part
			if (found) {
				//Make sure to remove from selectable
				this->_workbench->SelectionManager()->ForceDeselection((*iter)->Controller(), true);
				//Delete the object
				std::cout << "Deleting: " << (*iter)->GetName() << std::endl;
				delete *iter;
			}
		}
	}
	//Regenerate
	//...
	//All is good for now
	return true;
}


WCVisFeature* WCVisualization::FeatureFromName(const std::string &name) {
	//Find the feature
	std::map<std::string, WCVisFeature*>::iterator iter = this->_featureMap.find(name);
	//If at end of map, return NULL
	if (iter == this->_featureMap.end()) return NULL;
	//Otherwise, return pointer to feature
	return iter->second;
}


WCVisFeature* WCVisualization::FeatureFromID(const unsigned int &id) {
	//Find the feature in the ID map
	std::map<unsigned int, WCVisFeature*>::iterator iter = this->_idMap.find(id);
	//If at end of map, return NULL
	if (iter == this->_idMap.end()) return NULL;
	//Otherwise, return pointer to feature
	return iter->second;
}


std::string WCVisualization::GenerateFeatureName(WCFeature *feature) {
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
		newString = root + '.' + id;
		index++;
//		std::cout << "Index = " << index << " Name: " << newString << std::endl;
	} while (!this->CheckName(newString));
	//Found a good name
	return newString;
}


void WCVisualization::ResetVisualization(void) {
	CLOGGER_INFO(WCLogManager::RootLogger(), "WCVisualization::ResetVisualization - Resetting...");
	//Set state to reset
	this->_state = WCVisualizationState::Reset();

	//Go through all features and call reset
	std::list<WCVisFeature*>::iterator featureIter;
	for (featureIter = this->_featureList.begin(); featureIter != this->_featureList.end(); featureIter++) {
		//Call reset
		(*featureIter)->ResetVisualization();
	}
}


void WCVisualization::StartVisualization(void) {
	CLOGGER_INFO(WCLogManager::RootLogger(), "WCVisualization::StartVisualization - Starting...");
	//Set state to running
	this->_state = WCVisualizationState::Running();

	//Go through all features and call reset
	std::list<WCVisFeature*>::iterator featureIter;
	for (featureIter = this->_featureList.begin(); featureIter != this->_featureList.end(); featureIter++) {
		//Call reset
		(*featureIter)->StartVisualization();
	}
}


void WCVisualization::PauseVisualization(void) {
	CLOGGER_INFO(WCLogManager::RootLogger(), "WCVisualization::PauseVisualization - Pausing...");
	//Set state to pause
	this->_state = WCVisualizationState::Paused();

	//Go through all features and call reset
	std::list<WCVisFeature*>::iterator featureIter;
	for (featureIter = this->_featureList.begin(); featureIter != this->_featureList.end(); featureIter++) {
		//Call reset
		(*featureIter)->PauseVisualization();
	}
}


void WCVisualization::StopVisualization(void) {
	CLOGGER_INFO(WCLogManager::RootLogger(), "WCVisualization::StopVisualization - Stopping...");
	//Set state to stop
	this->_state = WCVisualizationState::Stopped();

	//Go through all features and call reset
	std::list<WCVisFeature*>::iterator featureIter;
	for (featureIter = this->_featureList.begin(); featureIter != this->_featureList.end(); featureIter++) {
		//Call reset
		(*featureIter)->StopVisualization();
	}
}


xercesc::DOMElement* WCVisualization::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create the base element for the part
	XMLCh* xmlString = xercesc::XMLString::transcode("Visualization");
	xercesc::DOMElement*  element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Include the parent element
	xercesc::DOMElement *inheritedElem = this->WCDocument::Serialize(document, dictionary);
	if (inheritedElem != NULL) element->appendChild(inheritedElem);
	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);
/*
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
*/
	//Return the element
	return element;
}


/***********************************************~***************************************************


WCActionPartDelete* WCVisualization::ActionDelete(WCPart *part, std::list<WCPartFeature*> features) {
	//Create new action to delete selected objects from the part
	return new WCActionPartDelete(part, features);
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCVisualization &vis) {
	//Print out some info
	out << "Visualization(" << &vis << ")\n";
	//Print the scene
	out << "\t" << *(vis._scene);
	//Print the tree
	out << "\t" << *(vis._treeView);
	//Print all of the features in order
	out << "\t Features(" << vis._featureMap.size() << ")\n";
	std::list<WCVisFeature*>::iterator featureIter;
//	for (featureIter = part._featureList.begin(); featureIter != part._featureList.end(); featureIter++) {
//		out << "\t" << *featureIter << std::endl;
//	}
	return out;
}


/***********************************************~***************************************************/

