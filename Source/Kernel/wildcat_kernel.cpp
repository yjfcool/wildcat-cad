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
#include <Kernel/wildcat_kernel.h>
#include <Kernel/document_type_manager.h>
#include <PartDesign/part.h>
#include <PartDesign/part_feature.h>
#include <RTVisualization/visualization.h>
#include <boost/filesystem.hpp>


/*** Static Member Initialization ***/ 
int WCWildcatKernel::_refCount = 0;
bool	WCWildcatKernel::_headless = false;
WCGLContext* WCWildcatKernel::_context = NULL;


/***********************************************~***************************************************/


void WCWildcatKernel::CreateContext(void) {
	//Try to create a GL Context
	try {
		WCWildcatKernel::_context = new WCGLContext();
		//Make the context active
		WCWildcatKernel::_context->MakeActive();
	}
	catch (WCException ex) {
		//Log an error message
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCWildcatKernel::CreateContext - Not able to create context");
		exit(0);
	}
	//Initialize the adapter
	WCAdapter::Initialize();
	//Check to make sure minimum levels are met
	///...
}


void WCWildcatKernel::DestroyContext(void) {
	//Make sure there is a context
	if (!WCWildcatKernel::_context) return;
	//Try to destroy teh context
	try {
		delete WCWildcatKernel::_context;
	}
	catch (WCException &ex) {
		//Log an error message
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCWildcatKernel::DestroyContext - Not able to destroy context");
		exit(0);
	}
}




bool WCWildcatKernel::Initialize(const bool &logToFile, const WCLoggerLevel &loggerLevel, const std::string &loggerFile, const bool &headless) {
	//Increment the reference counter
	WCWildcatKernel::_refCount++;
	//If already initialized, exit
	if (WCWildcatKernel::_refCount > 1) return true;

	try {
		//Set the headless status
		WCWildcatKernel::_headless = headless;
		//Initialize logger manager
		WCLogManager::Initialize(loggerLevel);
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCWildcatKernel::Initializing.");
		//Initialize xml manager
		xercesc::XMLPlatformUtils::Initialize();
		//Try to grab lock file
		//...
		//Initialize document type manager
		WCDocumentTypeManager::Initialize();
		//Register base document types
		//								----Name----------------Description-------------------------Extension-------DTD Filename--------Factory---
		WCDocumentTypeManager::RegisterType("Part",				"Wildcat Part Document",			".wildPart",	"wildpart.dtd",		new WCPartDocumentFactory());
		WCDocumentTypeManager::RegisterType("Visualization",	"Wildcat Visualization Document",	".wildVis",		"wildvis.dtd",		new WCVisDocumentFactory());

		//Create primary OpenGL context
		WCWildcatKernel::CreateContext();
	}
	//Only catch Wildcat exceptions here
	catch(WCException ex) {
		std::cerr << "Exception caught in WCWildcatKernel::Initialize: " << ex.what() << std::endl;
	}
	//Done initializing
	return true;
}


bool WCWildcatKernel::Terminate(void) {
	//Check to make sure the kernel has been started
	if (WCWildcatKernel::_refCount == 0) {
		std::cerr << "WCWildcatKernel::Terminate Error - Kernel already Terminated completely\n";
		ASSERT(false);
	}
	//Check to see if this is just a reference count decrement or a true Terminate
	if (WCWildcatKernel::_refCount > 1) {
		//Decrement the reference count
		WCWildcatKernel::_refCount--;
		CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCWildcatKernel::Terminate - " << WCWildcatKernel::_refCount << " references remaining");
		return false;
	}

	CLOGGER_WARN(WCLogManager::RootLogger(), "WCWildcatKernel::Shutting Down.");
	//Terminate the managers
	try {
		//Destroy the context
		WCWildcatKernel::DestroyContext();
		//Terminate dialog manager
//		WCDialogManager::Terminate();
		//Terminate DocType manger
		WCDocumentTypeManager::Terminate();
		//Terminate XML utilities
		xercesc::XMLPlatformUtils::Terminate();
		//Terminate Log Manager
		WCLogManager::Terminate();
	}
	//Only catch Wildcat exceptions here
	catch(WCException ex) {
		//Output an error
		std::cerr << "Exception caught in WCWildcatKernel::Terminate: " << ex.what() << std::endl;
		//Quit the app
		exit(-1);
	}
	//Return true
	return true;
}


WCDocument* WCWildcatKernel::CreateDocument(const std::string &extension, const std::string &name, const std::string &directory) {
	//Make sure kernel is running
	if(!WCWildcatKernel::Started()) throw WCException();
	//Get the appropraite factory
	WCDocumentFactory *factory = WCDocumentTypeManager::FactoryFromType(extension);
	//Create the document type
	WCDocument *document = factory->Create(name, directory);
	//Return new document
	return document;
}


WCDocument* WCWildcatKernel::OpenDocument(const std::string &fullpath) {
	//Make sure kernel is running
	if(!WCWildcatKernel::Started()) throw WCException();
	//Determine if file exists
	if( !boost::filesystem::exists(fullpath) ) {
		//There is an error here
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCWildcatKernel::OpenDocument - " << fullpath << " does not exist.");
		throw WCException("File: " + fullpath + " does not exist.");
	}
	//Determine the file extension
	std::string extension = boost::filesystem::extension(fullpath);
	//Get the appropraite factory
	WCDocumentFactory *factory = WCDocumentTypeManager::FactoryFromType(extension);
	WCDocument* document;

	//Create xml parser
	xercesc::XercesDOMParser* parser = new xercesc::XercesDOMParser();
	//Set validation scheme (would be really nice to use DTD here)
	parser->setValidationScheme(xercesc::XercesDOMParser::Val_Always);    
	xercesc::ErrorHandler* errHandler = (xercesc::ErrorHandler*) new xercesc::HandlerBase();
	parser->setErrorHandler(errHandler);
	xercesc::DOMElement *rootElement;
	xercesc::DOMNodeList *elementList;
	XMLCh* xmlString;
	
	//Try to parse
	try {
		//Parse the manifest
		parser->parse( fullpath.c_str() );
		//Get the root document node (Wildcat)
		rootElement = parser->getDocument()->getDocumentElement();
		//Get list of Part nodes
		xmlString = xercesc::XMLString::transcode("Part");
		elementList = rootElement->getElementsByTagName(xmlString);
		xercesc::XMLString::release(&xmlString);
		
		//Create SerialDictionary
		WCSerialDictionary *dictionary = new WCSerialDictionary();
		//Try to create document using parsed XML
		document = factory->Open( (xercesc::DOMElement*)(elementList->item(0)), dictionary );
		//Delete the dictionary
		delete dictionary;
	}
	//Error checking
	catch (const xercesc::XMLException& toCatch) {
		char* message = xercesc::XMLString::transcode(toCatch.getMessage());
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCWildcatKernel::OpenDocument - Exception message is: \n" << message);
		xercesc::XMLString::release(&message);
	}
	catch (const xercesc::DOMException& toCatch) {
		char* message = xercesc::XMLString::transcode(toCatch.msg);
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCWildcatKernel::OpenDocument - Exception message is: \n" << message);
		xercesc::XMLString::release(&message);
	}
	catch (...) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCWildcatKernel::OpenDocument - Unexpected Exception");
	}
	//Clean up
	delete parser;
	delete errHandler;

	//Return opened document
	return document;
}


/***********************************************~***************************************************/

