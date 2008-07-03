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


#ifndef __DOCUMENT_H__
#define __DOCUMENT_H__


/*** Included Header Files ***/
#include "Kernel/wftrl.h"
#include "Kernel/feature.h"
#include "Kernel/unit_types.h"


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCScene;
class WCUserInterfaceLayer;
class WCBackgroundLayer;
class WCTreeView;
class WCAction;
class WCWorkbench;
class WCToolbarManager;


/***********************************************~***************************************************/


class WCDocument : public WCFeature, public virtual WCVisualObject {
protected:
	//Static Members
	static std::string							ToolbarManifest;									//!< Name of the resource manifest
	//Instance Data Members
	std::string									_filename;											//!< Filename for this document
	WCScene										*_scene;											//!< Scene associated with document
	WCUserInterfaceLayer						*_uiLayer;											//!< UI Layer
	WCBackgroundLayer							*_backgroundLayer;									//!< Background layer
	WCTreeView									*_treeView;											//!< Treeview object
	std::string									_statusText;										//!< Document status text
	std::list<WCAction*>						_actions;											//!< List of executed actions
	std::list<WCAction*>						_redoActions;										//!< List of redo actions
	WCSerialDictionary*							_undoDictionary;									//!< Dictionary for undo-redo actions
	WCWorkbench*								_activeWorkbench;									//!< Current active workbench
	std::stack<WCWorkbench*>					_workbenchStack;									//!< Workbench stack
	std::map<std::string, WCNamedView>			_namedViews;										//!< Named views
	WCToolbarManager							*_toolbarManager;									//!< Toolbar manager
	WCUnitType									*_lengthUnit;										//!< Length unit type
	WCUnitType									*_angleUnit;										//!< Angle unit type
private:
	void Initialize(void);																			//!< Initialization method
	//Hidden Constructors
	WCDocument();																					//!< Deny access to default constructor
	WCDocument(const WCDocument& doc);																//!< Deny access to copy constructor
	WCDocument& operator=(const WCDocument& doc);													//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCDocument(WCFeature *creator, const std::string &name, const std::string &filename);			//!< Primary constructor
	WCDocument(xercesc::DOMElement *element, WCSerialDictionary *dictionary);						//!< Persistance constructor
	virtual ~WCDocument();																			//!< Default destructor

	//General Access Methods
	inline void Status(const std::string &text)	{ this->_statusText = text;							//!< Set the document status text
												this->IsVisualDirty(true); }
	inline std::string Status(void) const		{ return this->_statusText; }						//!< Get the document status text
	inline std::string Filename(void)			{ return this->_filename; }							//!< Get the document filename

	//Scene Access Methods
	inline WCScene* Scene(void)					{ return this->_scene; }							//!< Get the document's scene
	virtual WCLayerNode* ExportLayers(void);														//!< Export layers to another document
	inline WCUserInterfaceLayer* UserInterfaceLayer(void){ return this->_uiLayer; }					//!< Get the UI layer
	inline WCBackgroundLayer *BackgroundLayer(void)	{ return this->_backgroundLayer; }				//!< Get the background layer

	//Member Access Methods
	WCTreeView* TreeView(void)					{ return this->_treeView; }							//!< Get the treeView object
	inline WCWorkbench* ActiveWorkbench(void)	{ return this->_activeWorkbench; }					//!< Get the workbench
	inline WCSerialDictionary* UndoDictionary(void) { return this->_undoDictionary; }				//!< Get the undo dictionary
	bool EnterWorkbench(WCWorkbench *workbench);													//!< Enter a new workbench
	bool ExitWorkbench(void);																		//!< Exit the current workbench
	inline WCToolbarManager* ToolbarManager(void) const { return this->_toolbarManager; }			//!< Get the toolbar manager
	inline void LengthUnit(WCUnitType *type)	{ delete this->_lengthUnit; this->_lengthUnit = type; }	//!< Set the length unit type
	inline WCUnitType* LengthUnit(void)			{ return this->_lengthUnit; }						//!< Get the length unit type
	inline void AngleUnit(WCUnitType *type)		{ delete this->_angleUnit; this->_angleUnit = type; }	//!< Set the angle unit type
	inline WCUnitType* AngleUnit(void)			{ return this->_angleUnit; }						//!< Get the angle unit type

	//Named View Methods
	bool AddView(const WCNamedView &view);															//!< Add a named view
	bool RemoveView(const std::string &name);														//!< Remove a named view
	bool NamedView(const std::string &name);														//!< Go to a named view
	
	//Command Methods
	virtual WCFeature* ExecuteAction(WCAction *action);												//!< Execute an action
	virtual bool Undo(const WPUInt &count=1);														//!< Undo the last <count> actions
	virtual bool Redo(const WPUInt &count=1);														//!< Redo the last <count> undone actions

	//Inherited Methods
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object

	//Feature Tree Methods
	friend std::ostream& operator<<(std::ostream& out, const WCDocument &document);					//!< Overloaded output operator
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__DOCUMENT_H__

