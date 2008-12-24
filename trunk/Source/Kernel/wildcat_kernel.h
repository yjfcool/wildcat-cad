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


#ifndef __WILDCAT_KERNEL_H__
#define __WILDCAT_KERNEL_H__


/*** Included Headers ***/
#include "Kernel/wftrl.h"


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {
	
	
/*** Class Predefines ***/
class WCDocument;
	
	
/***********************************************~***************************************************/
	
	
class WCWildcatKernel {
private:
	//Static Members
	static int									_refCount;											//!< Reference count for startup-Terminate
	static bool									_headless;											//!< Is there going to be a GUI
	static WCGLContext							*_context;											//!< Master OpenGL context
	//Static Methods
	static void CreateContext(void);																//!<
	static void DestroyContext(void);																//!<

	//Hidden Constructors
	WCWildcatKernel();																				//!< Deny access to default constructor
	~WCWildcatKernel();																				//!< Deny access to default destructor
	WCWildcatKernel(const WCWildcatKernel &kernel);													//!< Deny access to copy constructor
	WCWildcatKernel& operator=(const WCWildcatKernel &kernel);										//!< Deny access to equals operator
	bool operator==(const WCWildcatKernel &kernel);													//!< Deny access to equality operator
public:
	//Startup - Terminate Functions
	static bool Started(void)					{ return WCWildcatKernel::_refCount > 0; }			//!< Check to see if the kernel has been started
	static bool Initialize(const bool &logToFile, const WCLoggerLevel &loggerLevel,					//!< Initialize the manager with a few options
												const std::string &loggerFile, const bool &headless);
	static bool Terminate(void);																	//!< Terminate the logger

	//Member Access Methods
	inline static WCGLContext* Context(void)	{ return WCWildcatKernel::_context; }				//!< Get the base context
	inline static bool IsHeadless(void)			{ return WCWildcatKernel::_headless; }				//!< Get headless status

	//Document Management Methods
	static WCDocument* CreateDocument(const std::string &extension, const std::string &name, 		//!< Create a new document of type "extension"
												const std::string &directory);
	static WCDocument* OpenDocument(const std::string &fullpath);									//!< Open the document with full path "filename"

	//Module Management Methods
	static bool LoadModule(const std::string &fullpath);											//!< Load an optional module
	static bool UnloadModule(const std::string &name);												//!< Unload an optional module
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__WILDCAT_KERNEL_H__

