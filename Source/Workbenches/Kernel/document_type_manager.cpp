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
#include "Kernel/document_type_manager.h"
#include "Kernel/document.h"


/*** Static Member Initialization ***/ 
int WCDocumentTypeManager::_refCount = 0;
std::string	WCDocumentTypeManager::_manifest = "";
std::map<std::string,WSDocType> WCDocumentTypeManager::_docTypeMap = std::map<std::string,WSDocType>();


/***********************************************~***************************************************/


bool WCDocumentTypeManager::Initialize(const std::string &manifest) {
	//Increment the reference counter
	WCDocumentTypeManager::_refCount++;
	CLOGGER_WARN(WCLogManager::RootLogger(), "WCDocumentTypeManager::Initializing...");
	//Parse the manifest
	//...
	return true;
}


bool WCDocumentTypeManager::Terminate(void) {
	//Check to make sure the docType manager has been started
	if (WCDocumentTypeManager::_refCount == 0) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCDocumentTypeManager::Terminate Error - DocumentTypeManager already Terminate completely");
		return false;
	}
	//Check to see if this is just a reference count decrement or a true Terminate
	if (WCDocumentTypeManager::_refCount > 1) {
		//Decrement the reference count
		WCDocumentTypeManager::_refCount--;
		CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCDocumentTypeManager::Terminate - " << WCDocumentTypeManager::_refCount << " references remaining");
		return false;
	}
	CLOGGER_WARN(WCLogManager::RootLogger(), "WCDocumentTypeManager::Shutting Down.");

	//Loop through all doc types
	std::map<std::string,WSDocType>::iterator iter;
	for (iter = WCDocumentTypeManager::_docTypeMap.begin(); iter != WCDocumentTypeManager::_docTypeMap.end(); iter++) {
		//Need to delete all of the factories
		if((*iter).second.Factory) delete (*iter).second.Factory;
	}
	//Clear the type map
	WCDocumentTypeManager::_docTypeMap.clear();
	//Also reset the reference counter
	WCDocumentTypeManager::_refCount = 0;
	//Return true
	return true;
}


bool WCDocumentTypeManager::RegisterType(const std::string &name, const std::string &description, const std::string &extension, 
	const std::string &dtdFile, WCDocumentFactory *factory) {
	//Make sure manager is initialized
	if (!WCDocumentTypeManager::Started()) WCDocumentTypeManager::Initialize();
	//Make sure factory is present
	if (!factory) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCDocumentTypeManager::RegisterType - NULL factory passed.");
		throw WCException(); 
	}

	//Create document type struct
	WSDocType type;
	type.Name = name;
	type.Description = description;
	type.Extension = extension;
	type.DTDFileName = dtdFile;
	type.Factory = factory;
	//Insert type into map
	std::pair<std::map<std::string,WSDocType>::iterator,bool> retVal = WCDocumentTypeManager::_docTypeMap.insert( std::make_pair(extension, type) );
	//Make sure it inserted well
	if (!retVal.second) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCDocumentTypeManager::RegisterType - Failed to insert " << extension << " into map.");
		throw WCException();
	}
	//All good
	return true;
}


WCDocumentFactory*  WCDocumentTypeManager::FactoryFromType(const std::string &extension) {
	//See if the extension can be found
	std::map<std::string,WSDocType>::iterator iter = WCDocumentTypeManager::_docTypeMap.find(extension);
	//Was it not found
	if (iter == WCDocumentTypeManager::_docTypeMap.end()) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCDocumentTypeManager::FactoryFromType - Failed to find extension: " << extension << ".");
		throw WCException();
	}
	//Must have found it, return the factory pointer
	return (*iter).second.Factory;
}


/***********************************************~***************************************************/

