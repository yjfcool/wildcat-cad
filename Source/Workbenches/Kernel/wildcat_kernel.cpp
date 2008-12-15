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
#include "Kernel/wildcat_kernel.h"
#include "Kernel/document_type_manager.h"


/*** Static Member Initialization ***/ 
int WCWildcatKernel::_refCount = 0;
std::string	WCWildcatKernel::_manifest = "";
WPGLContext WCWildcatKernel::_context = NULL;
CGLPixelFormatObj WCWildcatKernel::_pixelFormat = NULL;


/***********************************************~***************************************************/


void WCWildcatKernel::CreateContext(void) {
#ifdef __APPLE__
	//Create structure with pixel format attributes
	CGLPixelFormatAttribute attribs[] = {
										kCGLPFAAccelerated,
										kCGLPFAColorSize, (CGLPixelFormatAttribute)24,
										kCGLPFADepthSize, (CGLPixelFormatAttribute)16,
										kCGLPFADoubleBuffer,
										(CGLPixelFormatAttribute)0
									};

	//Create the best possible pixel format
	GLint numVirtualScreens = 0;
	CGLError err = CGLChoosePixelFormat(attribs, &WCWildcatKernel::_pixelFormat, &numVirtualScreens);
	//Do some error checking
	if ((err != kCGLNoError) || (WCWildcatKernel::_pixelFormat == NULL)) {
		//Log an error message
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCWildcatKernel::CreateContext - CGLChoosePixelFormat Error: " << err);
		//Throw exception
		throw WCException();
	}
	//Check to make sure pixel format is necessary level
	//...

	//Create the master context
	err = CGLCreateContext (WCWildcatKernel::_pixelFormat, NULL, &WCWildcatKernel::_context);
	//Do some error checking
	if (err != kCGLNoError) {
		//Log an error message
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCWildcatKernel::CreateContext - CGLCreateContext Error: " << err);
		//Throw exception
		throw WCException();
	}
	//Destroy the pixel format
	err = CGLDestroyPixelFormat(WCWildcatKernel::_pixelFormat);
	//Do some error checking
	if (err != kCGLNoError) {
		//Log an error message
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCWildcatKernel::CreateContext - CGLDestroyPixelFormat Error: " << err);
		//Throw exception
		throw WCException();		
	}
#elif __WIN32__
	//Windows context creation code
	//...
#else
	//Linux context creation code
	//...
#endif
	//Initialize the adapter
	WCAdapter::Initialize();
	//Check to make sure minimum levels are met
}


void WCWildcatKernel::DestroyContext(void) {
#ifdef __APPLE__
	//Destroy the context
	if (WCWildcatKernel::_context) {
		CGLError err = CGLDestroyContext(WCWildcatKernel::_context);
		//Do some error checking
		if (err != kCGLNoError) {
			//Log an error message
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCWildcatKernel::DestroyContext - CGLDestroyContext Error: " << err);
			//Throw exception
			throw WCException();
		}
	}
#elif __WIN32__
	//Windows context destruction code
	//...
#else
	//Linux context destruction code
	//...
#endif
}




bool WCWildcatKernel::Initialize(const std::string &manifest) {
	//Increment the reference counter
	WCWildcatKernel::_refCount++;
	//If already initialized, exit
	if (WCWildcatKernel::_refCount > 1) return true;

	std::cout << "WCWildcatKernel::Initializing...\n";
	try {
		//Parse the manifest
		//...
		//Initialize logger manager
		WCLogManager::Initialize();
		//Initialize xml manager
		xercesc::XMLPlatformUtils::Initialize();
		//Try to grab lock file
		//...
		//Initialize document type manager
		WCDocumentTypeManager::Initialize();
		//Register base document types
		//								----Name----------------Description-------------------------Extension-------DTD Filename--------Factory---
//		WCDocumentTypeManager::RegisterType("Part",				"Wildcat Part Document",			"wildPart",		"wildpart.dtd",		NULL);
//		WCDocumentTypeManager::RegisterType("Visualization",	"Wildcat Visualization Document",	"wildVis",		"wildvis.dtd",		NULL);

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


/***********************************************~***************************************************/

