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
#include "Kernel/document.h"
#include "Kernel/action.h"
#include "Kernel/workbench.h"
#include "Application/toolbar_manager.h"


/*** Local Defines ***/
#define DOCUMENT_NAMEDVIEW_FRONT				0.0,  0.0, 0.0, 1.0
#define DOCUMENT_NAMEDVIEW_BACK					0.0, -1.0, 0.0, 0.0
#define DOCUMENT_NAMEDVIEW_LEFT					0.0,  0.707106, 0.0, 0.707106
#define DOCUMENT_NAMEDVIEW_RIGHT				0.0,  -0.707106, 0.0, 0.707106
#define DOCUMENT_NAMEDVIEW_TOP					0.707106, 0.0, 0.0, 0.707106
#define DOCUMENT_NAMEDVIEW_BOTTOM				-0.707106, 0.0, 0.0, 0.707106
#define DOCUMENT_NAMEDVIEW_ISOMETRIC			-0.209315, -0.508694, -0.767096, 0.330123


/*** Static Member Initialization ***/
std::string WCDocument::ToolbarManifest = "toolbar_manifest.xml";


/***********************************************~***************************************************/


void WCDocument::Initialize(void) {
	//If this is root document
	if (this->_document == NULL) {
		//Retain self (avoid garbage collection)
		this->Retain(*this);
		//Set document root to self
		this->_document = this;

		//Create all of the layers for the document
		this->_uiLayer = new WCUserInterfaceLayer(this->_scene);
		this->_backgroundLayer = new WCBackgroundLayer(this->_scene);

		//Register the layers
		this->_scene->RegisterLayer(this->_backgroundLayer);
		this->_scene->RegisterLayer(this->_uiLayer);

		//Create Feature TreeView
		this->_treeView = new WCTreeView(this->_uiLayer);

		//Setup undo-redo dictionary
		this->_undoDictionary = new WCSerialDictionary();

		//Create toolbar manager
		std::string resourcesDirectory = _ResourceDirectory();
		_toolbarManager = new WCToolbarManager(this, WCDocument::ToolbarManifest, resourcesDirectory, false);
		//Set the default units
		this->_lengthUnit = WCUnitType::TypeFromName("Millimeter");
		this->_angleUnit = WCUnitType::TypeFromName("Degrees");
	}
	//Otherwise, get info from root document
	else {
		//Set the scene
		this->_scene = this->_document->Scene();
		//Create all of the layers for the document
		this->_uiLayer = this->_document->UserInterfaceLayer();
		this->_backgroundLayer = this->_document->BackgroundLayer();
		//Reference the treeview
		this->_treeView = this->_document->TreeView();
		//Reference the toolbar manager
		this->_toolbarManager = this->_document->ToolbarManager();
	}

}


/***********************************************~***************************************************/


WCDocument::WCDocument(WCFeature *creator, const std::string &name, const std::string &filename) : ::WCFeature(creator, name), 
	_filename(filename), _scene(NULL), _uiLayer(NULL), _backgroundLayer(NULL), _treeView(NULL), _statusText("Ready"),
	_actions(), _redoActions(), _undoDictionary(NULL), _activeWorkbench(NULL), _workbenchStack(), _namedViews(), _toolbarManager(NULL), 
	_lengthUnit(NULL), _angleUnit(NULL) {

	//If this is root document
	if (this->_document == NULL) {
		//Create scene for the document
		this->_scene = new WCScene();
		//Create named views
		this->AddView( WCNamedView("Front", WCQuaternion(DOCUMENT_NAMEDVIEW_FRONT)) );
		this->AddView( WCNamedView("Back", WCQuaternion(DOCUMENT_NAMEDVIEW_BACK)) );
		this->AddView( WCNamedView("Left", WCQuaternion(DOCUMENT_NAMEDVIEW_LEFT)) );
		this->AddView( WCNamedView("Right", WCQuaternion(DOCUMENT_NAMEDVIEW_RIGHT)) );
		this->AddView( WCNamedView("Top", WCQuaternion(DOCUMENT_NAMEDVIEW_TOP)) );
		this->AddView( WCNamedView("Bottom", WCQuaternion(DOCUMENT_NAMEDVIEW_BOTTOM)) );
		this->AddView( WCNamedView("Isometric", WCQuaternion(DOCUMENT_NAMEDVIEW_ISOMETRIC)) );

	}
	//Initialize
	this->Initialize();
}


WCDocument::WCDocument(xercesc::DOMElement *element, WCSerialDictionary *dictionary) : 
	::WCFeature( WCSerializeableObject::ElementFromName(element,"Feature"), dictionary),
	_filename(), _scene(NULL), _uiLayer(NULL), _backgroundLayer(NULL), _treeView(NULL), _statusText("Ready"),
	_actions(), _redoActions(), _undoDictionary(NULL), _activeWorkbench(NULL), _workbenchStack(), _namedViews(), _toolbarManager(NULL),
	_lengthUnit(NULL), _angleUnit(NULL) {

	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

	//If this is root document
	if (this->_document == NULL) {
		//Create scene for the document
		this->_scene = new WCScene( WCSerializeableObject::ElementFromName(element,"Scene"), dictionary);
		//Get filename attribute
		this->_filename = WCSerializeableObject::GetStringAttrib(element, "filename");

		//Create named views
		XMLCh* xmlString = xercesc::XMLString::transcode("NamedView");
		xercesc::DOMNodeList *list = element->getElementsByTagName(xmlString);
		xercesc::XMLString::release(&xmlString);
		xercesc::DOMElement *nvElement;
		WCNamedView *namedView;
		for (WPUInt nvIndex=0; nvIndex < list->getLength(); nvIndex++) {
			//Get the indexed node and cast to element
			nvElement = (xercesc::DOMElement*)list->item(nvIndex);
			//Create the named view
			namedView = new WCNamedView(nvElement, dictionary);
			//Add into named view map
			this->_namedViews.insert( std::make_pair(namedView->Name(), *namedView) );
			//Clean up
			delete namedView;
		}

		//Create actions
//		xmlString = xercesc::XMLString::transcode("Actions");
//		list = element->getElementsByTagName(xmlString);
//		xercesc::XMLString::release(&xmlString);
//		xercesc::DOMElement *actionElement;
//		bool retVal;
//		//Loop through all actions
//		for (int actionIndex=0; actionIndex < list->getLength(); actionIndex++) {
//			//Get the indexed node and cast to element
//			actionElement = (xercesc::DOMElement*)list->item(actionIndex);
//			//Pass to master action switch
//			retVal = WCAction::Deserialize(actionElement, dictionary);
//		}
	}
	//Initialize
	this->Initialize();
}


WCDocument::~WCDocument() {
	//If this is the root document
	if (this->_document == this) {
		//Release self
		this->Release(*this);
		//Clean up the scene
//		if (this->_scene != NULL) delete this->_scene;
		//Delete the tree element for this feature
		if (this->_treeElement != NULL) {
			delete this->_treeElement;
			this->_treeElement = NULL;
		}
		//Delete the treeview
		if (this->_treeView != NULL) delete this->_treeView;
		//Delete the layers
		if (this->_uiLayer != NULL) delete this->_uiLayer;
		if (this->_backgroundLayer != NULL) delete this->_backgroundLayer;
		//Delete the toolbar manager
		if (this->_toolbarManager != NULL) delete this->_toolbarManager;
		//Delete undo dictionary
		if (this->_undoDictionary != NULL) delete this->_undoDictionary;
	}
}


WCLayerNode* WCDocument::ExportLayers(void) {
	return NULL;
}


bool WCDocument::EnterWorkbench(WCWorkbench *workbench) {
	//Check to make sure workbench is not null
	if (workbench == NULL) {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCDocument::EnterWorkbench - NULL workbench passed."); return false; }
	//Only root document can set active workbench
	if (this->_document != this) {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCDocument::ActiveWorkbench - Only root document can set active workbench."); return false; }
	
	//Check current stack size
	if (this->_workbenchStack.size() != 0) {
		//Clear the selection manager
		this->_workbenchStack.top()->SelectionManager()->Clear();		
		//Check to see if already is active workbench
		if (workbench == this->_workbenchStack.top()) return true;
/*** DEBUG ***/		
		//Otherwise exit the workbench
		else this->_workbenchStack.top()->OnExit();
/*** DEBUG ***/
	}
	//Enter the new workbech
	this->_workbenchStack.push(workbench);
	this->_activeWorkbench = workbench;
	this->_workbenchStack.top()->OnEnter();
	return true;
}


bool WCDocument::ExitWorkbench(void) {
	//Make sure there is one more workbench in the stack
	if (this->_workbenchStack.size() == 1) return false;
	//Exit the current workbench
	this->_workbenchStack.top()->OnExit();
	//Remove it and enter the next down
	this->_workbenchStack.pop();
	this->_activeWorkbench = this->_workbenchStack.top();
/*** DEBUG ***/		
	//Enter the workbench
	this->_workbenchStack.top()->OnEnter();
/*** DEBUG ***/

	return true;
}


bool WCDocument::AddView(const WCNamedView &view) {
	//Check to see if name already exists
	if (this->_namedViews.find(view.Name()) != this->_namedViews.end()) {
		return false; }
	//Add new pair - retain the view
	this->_namedViews.insert( make_pair(view.Name(), view) );
	return true;
}


bool WCDocument::RemoveView(const std::string &name) {
	//Make sure not to remove reserved names
	if ((name == "Front") || (name == "Back") || (name == "Left") ||
		(name == "Right") || (name == "Top") || (name == "Bottom") || (name == "Isometric")) return false;
	//Check to see if name already exists
	if (this->_namedViews.find(name) != this->_namedViews.end()) {
		return false; }
	//Add new pair
	this->_namedViews.erase(name);
	return true;
}


bool WCDocument::NamedView(const std::string &name) {
	//Try to find named view
	std::map<std::string,WCNamedView>::iterator iter = this->_namedViews.find(name);
	if (iter == this->_namedViews.end()) return false;
	//Otherwise, animated to the view
	this->_scene->ActiveCamera()->AnimateToViewpoint((*iter).second, CAMERA_ANIMATE_DURATION);
	return true;
}


WCFeature* WCDocument::ExecuteAction(WCAction *action) {
	//Make sure action is not null
	if (action == NULL) return false;
	CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCDocument::ExecuteAction - Executing: " << action->Name() << ".");
	//Execute the action
	WCFeature *result = action->Execute();
	if(result == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCDocument::ExecuteAction - Action execution failed.");
		return false;
	}
	//Record the action
	this->_actions.push_back(action);
	//Retain the action
	action->Retain(*this);

	//Delete all of redo actions list
	while (!this->_redoActions.empty()) {
		//Delete the action
		delete this->_redoActions.back();
		//Remove it from the redo list
		this->_redoActions.pop_back();
	}
	//Clear the undoDictionary
	this->_undoDictionary->Clear();

	//See if undo menu item needs to be enabled
	//...
	//See if redo menu item needs to be disabled
	//...

	//Return the result
	return result;
}


bool WCDocument::Undo(const WPUInt &count) {
	bool status;
	//Make sure there are actions
	if (this->_actions.empty()) return true;
	//Make sure count < actions.size
	if (count > this->_actions.size()) {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCDocument::Undo - Too many undo actions requested.");
		return true;
	}
	//Repeat count times
	for (WPUInt i=0; i<count; i++) {
		//Rollback the last count actions
		CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCDocument::Undo - Rolling Back: " << this->_actions.back()->Name() << ".");
		status = this->_actions.back()->Rollback();
		//If successfully rolledback
		if (status) {
			//Move action to redoActions list
			this->_redoActions.push_back(this->_actions.back());
			//Remove from actions list
			this->_actions.pop_back();
			//See if redo menu item needs to be enabled
			//...
			//See if undo menu item needs to be disabled
			//...
		}
		//Failure to rollback
		else {
			CLOGGER_WARN(WCLogManager::RootLogger(), "WCDocument::Undo - Unable to rollback action: " << this->_actions.back()->Name());
			return true;
		}
	}
	//All rollbacks successful
	return true;
}


bool WCDocument::Redo(const WPUInt &count) {
	WCFeature *status;
	//Make sure there are redo actions
	if (this->_redoActions.empty()) return true;
	//Make sure count < redoActions.size
	if (count > this->_redoActions.size()) {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCDocument::Redo - Too many redo actions requested.");
		return true;
	}
	//Repeat count times
	for (WPUInt i=0; i<count; i++) {
		//Redo the last count redo actions
		CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCDocument::Redo - Re-Executing: " << this->_redoActions.back()->Name() << ".");
		status = this->_redoActions.back()->Execute();
		//If successfully rolledback
		if (status != NULL) {
			//Move action to redoActions list
			this->_actions.push_back(this->_redoActions.back());
			//Remove from actions list
			this->_redoActions.pop_back();
			//See if redo menu item needs to be disabled
			//...
			//See if undo menu item needs to be enabled
			//...
		}
		//Failure to rollback
		else {
			CLOGGER_WARN(WCLogManager::RootLogger(), "WCDocument::Redo - Unable to redo action: " << this->_redoActions.back()->Name());
			return true;
		}
	}
	//All rollbacks successful
	return true;
}


xercesc::DOMElement* WCDocument::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create primary element for this object
	XMLCh* xmlString = xercesc::XMLString::transcode("Document");
	xercesc::DOMElement* element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Include the parent element
	xercesc::DOMElement* featureElement = this->WCFeature::Serialize(document, dictionary);
	element->appendChild(featureElement);

	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);
	//Set filename attribute
	WCSerializeableObject::AddStringAttrib(element, "filename", this->_filename);

	//Include the scene element
	xercesc::DOMElement* sceneElement = this->_scene->Serialize(document, dictionary);
	element->appendChild(sceneElement);

	//Include named views
	xmlString = xercesc::XMLString::transcode("NamedViews");
	xercesc::DOMElement* viewsElement = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	xercesc::DOMElement* viewElem;
	std::map<std::string, WCNamedView>::iterator viewIter;
	for (viewIter = this->_namedViews.begin(); viewIter != this->_namedViews.end(); viewIter++) {
		//Get the serialization element
		viewElem = (*viewIter).second.Serialize(document, dictionary);
		//Add it to the list
		if (viewElem != NULL) viewsElement->appendChild(viewElem);
	}
	element->appendChild(viewsElement);

	//Return the primary element
	return element;
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCDocument &document) {
	//Print out some info
	out << "Document: " << document._name << " (" << &document << ")\n";

	//Print out list of actions
	out << "\tActions (" << document._actions.size() << ")\n";
	std::list<WCAction*>::const_iterator actionIter;
	for (actionIter = document._actions.begin(); actionIter != document._actions.end(); actionIter++) {
		out << "\t\t" << *(*actionIter);
	}
	return out;
}


/***********************************************~***************************************************/

