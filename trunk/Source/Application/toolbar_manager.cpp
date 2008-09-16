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


//Included Header Files
#include "Application/toolbar_manager.h"
#include "Application/toolbar.h"


/***********************************************~***************************************************/


void WCToolbarManager::ParseManifest(const std::string &manifest, const std::string &directory, const bool &verbose) {
	//Create xml parser
	xercesc::XercesDOMParser* parser = new xercesc::XercesDOMParser();
	//Set validation scheme
	parser->setValidationScheme(xercesc::XercesDOMParser::Val_Always);    
	xercesc::ErrorHandler* errHandler = (xercesc::ErrorHandler*) new xercesc::HandlerBase();
	parser->setErrorHandler(errHandler);

	//Get the full path to the manifest
	std::string fullPath = directory + "/" + manifest;
	const char* xmlFile = fullPath.c_str();
	xercesc::DOMElement *rootElement, *buttonsRoot, *toolbarsRoot, *toolbar;
	xercesc::DOMNode *tmpNode;
	xercesc::DOMNodeList *list, *buttonsList;
	xercesc::DOMNamedNodeMap *nodeMap;
	XMLCh* xmlString;
	WCToolbarButton *newButton = NULL;
	WCToolbar *newToolbar = NULL;

	//Try to parse
	try {
		//Parse the manifest
		parser->parse(xmlFile);
		//Get the root document node
		rootElement = parser->getDocument()->getDocumentElement();

		//Find all button child nodes
		xmlString = xercesc::XMLString::transcode("buttons");
		buttonsList = rootElement->getElementsByTagName(xmlString);
		xercesc::XMLString::release(&xmlString);
		buttonsRoot = (xercesc::DOMElement*)buttonsList->item(0);
		xmlString = xercesc::XMLString::transcode("button");
		buttonsList = buttonsRoot->getElementsByTagName(xmlString );
		xercesc::XMLString::release(&xmlString);
		int buttonsCount = buttonsList->getLength();

		XMLCh* xmlMessage = xercesc::XMLString::transcode("message");
		XMLCh* xmlTooltip = xercesc::XMLString::transcode("tooltip");
		XMLCh* xmlType = xercesc::XMLString::transcode("type");
		XMLCh* xmlStdIcon = xercesc::XMLString::transcode("standardIcon");
		XMLCh* xmlActiveIcon = xercesc::XMLString::transcode("activeIcon");
		XMLCh* xmlActive = xercesc::XMLString::transcode("active");
		XMLCh* xmlEnabled = xercesc::XMLString::transcode("enabled");
		char* chars;

		//Loop through all buttons and create buttons
		for (int index=0; index < buttonsCount; index++) {
			//Get the node at index
			nodeMap = buttonsList->item(index)->getAttributes();

			//Get the message
			tmpNode = nodeMap->getNamedItem(xmlMessage);
			chars = xercesc::XMLString::transcode(tmpNode->getNodeValue());
			WCUserMessage message(chars);
			xercesc::XMLString::release(&chars);

			//Get the tooltip
			tmpNode = nodeMap->getNamedItem(xmlTooltip);
			chars = xercesc::XMLString::transcode(tmpNode->getNodeValue());
			std::string tooltip(chars);
			xercesc::XMLString::release(&chars);

			//Get the type
			int type;
			tmpNode = nodeMap->getNamedItem(xmlType);
			chars = xercesc::XMLString::transcode(tmpNode->getNodeValue());
			std::string typeString(chars);
			xercesc::XMLString::release(&chars);
			if (typeString == "MODE") type = TOOLBARELEMENT_TYPE_MODE;
			else if (typeString == "MOMENTARY") type = TOOLBARELEMENT_TYPE_MOMENTARY;
			else type = TOOLBARELEMENT_TYPE_TOGGLE;

			//Get the standard icon
			tmpNode = nodeMap->getNamedItem(xmlStdIcon);
			chars = xercesc::XMLString::transcode(tmpNode->getNodeValue());
			std::string standardIcon(chars);
			xercesc::XMLString::release(&chars);

			//Get the active icon
			tmpNode = nodeMap->getNamedItem(xmlActiveIcon);
			chars = xercesc::XMLString::transcode(tmpNode->getNodeValue());
			std::string activeIcon(chars);
			xercesc::XMLString::release(&chars);

			//Get the active state
			bool active = false;
			tmpNode = nodeMap->getNamedItem(xmlActive);
			chars = xercesc::XMLString::transcode(tmpNode->getNodeValue());
			std::string activeString(chars);
			xercesc::XMLString::release(&chars);
			if (activeString == "true") active = true;

			//Get the enabled state
			bool enabled = false;
			tmpNode = nodeMap->getNamedItem(xmlEnabled);
			chars = xercesc::XMLString::transcode(tmpNode->getNodeValue());
			std::string enabledString(chars);
			xercesc::XMLString::release(&chars);
			if (enabledString == "true") enabled = true;
			//Create the new toolbar button
			newButton = new WCToolbarButton(message, tooltip, type, standardIcon, activeIcon, active, enabled);
			//Be verbose if appropriate
			if ((newButton != NULL) && verbose) 
				CLOGGER_DEBUG(WCLogManager::RootLogger(), "Button " << message << " successfully loaded");
			//Add object into map
			this->_buttonMap.insert( std::make_pair(message, newButton) );
		}
		//Clean up a bit
		xercesc::XMLString::release(&xmlMessage);
		xercesc::XMLString::release(&xmlTooltip);
		xercesc::XMLString::release(&xmlType);
		xercesc::XMLString::release(&xmlStdIcon);
		xercesc::XMLString::release(&xmlActiveIcon);
		xercesc::XMLString::release(&xmlActive);
		xercesc::XMLString::release(&xmlEnabled);

		//Find all toolbar child nodes
		xmlString = xercesc::XMLString::transcode("toolbars");
		list = rootElement->getElementsByTagName(xmlString);
		xercesc::XMLString::release(&xmlString);
		toolbarsRoot = (xercesc::DOMElement*)list->item(0);
		xmlString = xercesc::XMLString::transcode("toolbar");
		list = toolbarsRoot->getElementsByTagName(xmlString);
		xercesc::XMLString::release(&xmlString);

		XMLCh* xmlName = xercesc::XMLString::transcode("name");
		XMLCh* xmlX = xercesc::XMLString::transcode("x");
		XMLCh* xmlY = xercesc::XMLString::transcode("y");
		XMLCh* xmlWidth = xercesc::XMLString::transcode("width");
		XMLCh* xmlHeight = xercesc::XMLString::transcode("height");
		XMLCh* xmlButton = xercesc::XMLString::transcode("button");

		//Loop through all toolbars and create toolbars and add buttons
		int toolbarsCount = list->getLength();
		for (int index=0; index < toolbarsCount; index++) {
			//Get the node at index
			toolbar = (xercesc::DOMElement*)list->item(index);
			nodeMap = toolbar->getAttributes();

			//Get the name
			tmpNode = nodeMap->getNamedItem(xmlName);
			chars = xercesc::XMLString::transcode(tmpNode->getNodeValue());
			std::string name(chars);
			xercesc::XMLString::release(&chars);

			//Get the x position
			tmpNode = nodeMap->getNamedItem(xmlX);
			int xPos = xercesc::XMLString::parseInt(tmpNode->getNodeValue());

			//Get the y position
			tmpNode = nodeMap->getNamedItem(xmlY);
			int yPos = xercesc::XMLString::parseInt(tmpNode->getNodeValue());

			//Get the width
			tmpNode = nodeMap->getNamedItem(xmlWidth);
			int width = xercesc::XMLString::parseInt(tmpNode->getNodeValue());

			//Get the height
			tmpNode = nodeMap->getNamedItem(xmlHeight);
			int height = xercesc::XMLString::parseInt(tmpNode->getNodeValue());

			//Create the new toolbar
			newToolbar = new WCToolbar(this->_document, name, WCVector4(xPos, yPos, width, height));
			//Be verbose if appropriate
			if ((newToolbar != NULL) && verbose) 
				CLOGGER_DEBUG(WCLogManager::RootLogger(), "Toolbar " << name << " successfully loaded");
			//Add toolbar into the map
			this->_toolbarMap.insert( std::make_pair(name, newToolbar) );

			//Add buttons to toolbars
			buttonsList = toolbar->getElementsByTagName(xmlButton);
			//Loop through all buttons
			for (unsigned int buttonIndex=0; buttonIndex < buttonsList->getLength(); buttonIndex++) {
				//Get the value node
				tmpNode = buttonsList->item(buttonIndex)->getFirstChild();
				//Find the button
				chars = xercesc::XMLString::transcode(tmpNode->getNodeValue());
				newButton = this->ButtonFromName(chars);
				xercesc::XMLString::release(&chars);
				//Make sure is not null
				if (newButton != NULL) {
					//If not null, add to toolbar
					newToolbar->AddButton(newButton);
				}
				else {
					CLOGGER_ERROR(WCLogManager::RootLogger(), "WCToolbarManager::ParseManifest - Not able to load button");
				}
			}
		}
		//Clean up a bit
		xercesc::XMLString::release(&xmlName);
		xercesc::XMLString::release(&xmlX);
		xercesc::XMLString::release(&xmlY);
		xercesc::XMLString::release(&xmlWidth);
		xercesc::XMLString::release(&xmlHeight);
		xercesc::XMLString::release(&xmlButton);
	}

	//Error checking
	catch (const xercesc::XMLException& toCatch) {
		char* message = xercesc::XMLString::transcode(toCatch.getMessage());
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCToolbarManager::ParseManifest - Exception message is: \n" << message);
		xercesc::XMLString::release(&message);
	}
	catch (const xercesc::DOMException& toCatch) {
		char* message = xercesc::XMLString::transcode(toCatch.msg);
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCToolbarManager::ParseManifest - Exception message is: \n" << message);
		xercesc::XMLString::release(&message);
	}
	catch (...) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCToolbarManager::ParseManifest - Unexpected Exception");
	}
	//Clean up
	delete parser;
	delete errHandler;
}


void WCToolbarManager::ParseUserConfig(const std::string &prefFile, const std::string &directory, const bool &verbose) {
}


/***********************************************~***************************************************/


WCToolbarManager::WCToolbarManager(WCDocument *doc, const std::string &manifest, const std::string &directory, const bool &verbose) : 
	_document(doc), _buttonMap(), _toolbarMap() {
	if (verbose) CLOGGER_WARN(WCLogManager::RootLogger(), "WCToolbarManager::WCToolbarManager - Initializing toolbars.");
	//Parse the toolbar manifest
	this->ParseManifest(manifest, directory, verbose);
	//Create user config
	this->ParseUserConfig("", "", true);
}


WCToolbarManager::~WCToolbarManager() {
	//Loop through all of the toolbars
	std::map<std::string, WCToolbar*>::iterator toolbarIter;
	for (toolbarIter = this->_toolbarMap.begin(); toolbarIter != this->_toolbarMap.end(); toolbarIter++) {
		//Delete the element
		delete (*toolbarIter).second;
	}

	//Loop through all buttons and delete
	std::map<std::string, WCToolbarButton*>::iterator buttonIter;
	for (buttonIter = this->_buttonMap.begin(); buttonIter != this->_buttonMap.end(); buttonIter++) {
		//Delete the element
		delete (*buttonIter).second;
	}
}
	

void WCToolbarManager::HideAll(void) {
}


void WCToolbarManager::PushState(void) {
}


void WCToolbarManager::PopState(void) {
}


WCToolbar* WCToolbarManager::ToolbarFromName(const std::string &name) {
	//Check to see if the name is in the toolbar map
	std::map<std::string, WCToolbar*>::iterator iter = this->_toolbarMap.find(name);
	//See if you found anything
	if( iter == this->_toolbarMap.end() ) {
		//Not able to find
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCToolbarManager::ToolbarFromName - Not able to find toolbar: " << name);
		return NULL;
	}
	//Otherwise, get the object
	return (*iter).second;
}


WCToolbarButton* WCToolbarManager::ButtonFromName(const std::string &name) {
	//Check to see if the name is in the button map
	std::map<std::string, WCToolbarButton*>::iterator iter = this->_buttonMap.find(name);
	//See if you found anything
	if( iter == this->_buttonMap.end() ) {
	//Not able to find
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCToolbarManager::ButtonFromName - Not able to find button: " << name);
		return NULL;
	}
	//Otherwise, get the object
	return (*iter).second;
}


/***********************************************~***************************************************/

