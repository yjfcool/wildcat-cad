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


#ifndef __DOCUMENT_TYPE_MANAGER_H__
#define __DOCUMENT_TYPE_MANAGER_H__


/*** Included Header Files ***/
#include <Kernel/wftrl.h>
#include <Kernel/feature.h>


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {
	
	
/*** Class Predefines ***/
class WCDocument;
class WCDocumentFactory;


/***********************************************~***************************************************/


struct WSDocType {
	std::string									Name;
	std::string									Description;
	std::string									Extension;
	std::string									DTDFileName;
	WCDocumentFactory*							Factory;
};


/***********************************************~***************************************************/


class WCDocumentTypeManager {
protected:
	//Static Members
	static int									_refCount;											//!< Reference count for startup-Terminate
	static std::string							_manifest;											//!< Name of the document type manifest
	static std::map<std::string,WSDocType>		_docTypeMap;										//!< Map of document type definitions
private:
	//Hidden Constructors
	WCDocumentTypeManager();																		//!< Deny access to default constructor
	~WCDocumentTypeManager();																		//!< Default destructor
	WCDocumentTypeManager(const std::string &manifest);												//!< Primary constructor
	WCDocumentTypeManager(const WCDocumentTypeManager& dtm);										//!< Deny access to copy constructor
	WCDocumentTypeManager& operator=(const WCDocumentTypeManager& dtm);								//!< Deny access to equals operator
public:
	//Startup - Terminate Functions
	static bool Started(void)					{ return WCDocumentTypeManager::_refCount > 0; }	//!< Check to see if the LogManager has been started
	static bool Initialize(const std::string &manifest="");											//!< Initialize the manager with a manifest
	static bool Terminate(void);																	//!< Terminate the logger

	//Document Type Methods
	static bool RegisterType(const std::string &name, const std::string &description,				//!< Document type registration method
												const std::string &extension, const std::string &dtdFile, WCDocumentFactory *factory);
	static WCDocumentFactory* FactoryFromType(const std::string &extension);						//!< Get document factory from extension name
	
	//Feature Tree Methods
	friend std::ostream& operator<<(std::ostream& out, const WCDocumentTypeManager &dtm);			//!< Overloaded output operator
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__DOCUMENT_TYPE_MANAGER_H__

