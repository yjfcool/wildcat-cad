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
#include "Utility/texture_manager.h"
#include "Utility/log_manager.h"
#include "Utility/adapter.h"


/***********************************************~***************************************************/


void WCTextureManager::ParseManifest(const std::string &manifest, const std::string &directory, const bool &verbose) {
	//Create xml parser
	xercesc::XercesDOMParser* parser = new xercesc::XercesDOMParser();
	//Set validation scheme
	parser->setValidationScheme(xercesc::XercesDOMParser::Val_Always);    
	xercesc::ErrorHandler* errHandler = (xercesc::ErrorHandler*) new xercesc::HandlerBase();
	parser->setErrorHandler(errHandler);

	//Get the full path to the manifest
#ifdef __WIN32__
	std::string fullPath = directory + "\\" + manifest;
#else
	//Forward slash for OSX and linux
	std::string fullPath = directory + "/" + manifest;
#endif
	const char* xmlFile = fullPath.c_str();
	WSTexture *newTexture;
	xercesc::DOMNode *textureNode, *nameNode;
	xercesc::DOMNamedNodeMap *nodeMap;
	XMLCh *xmlTextureString = xercesc::XMLString::transcode("texture");
	XMLCh *xmlNameString = xercesc::XMLString::transcode("name");
	char *tmpChars;

	//Try to parse
	try {
		//Parse the manifest
		parser->parse(xmlFile);
		//Get the root document
		xercesc::DOMDocument* rootDocument = parser->getDocument();
		//Find all nodes called texture
		xercesc::DOMNodeList *nodeList = rootDocument->getElementsByTagName(xmlTextureString);
		int textureCount = nodeList->getLength();
		//Loop through all nodes and extract texture information
		for (int index=0; index < textureCount; index++) {
			//Get the node at index
			textureNode = nodeList->item(index);
			nodeMap = textureNode->getAttributes();
			nameNode = nodeMap->getNamedItem(xmlNameString);
			tmpChars = xercesc::XMLString::transcode(nameNode->getNodeValue());
			std::string name(tmpChars);
			xercesc::XMLString::release(&tmpChars);
			//Create new texture
			newTexture = new WSTexture;
			newTexture->_name = name;
			newTexture->_id = 0;
			newTexture->_target = WCAdapter::HasGLARBTextureRectangle() ? GL_TEXTURE_RECTANGLE_ARB : GL_TEXTURE_2D;
			newTexture->_mipLevel = 0;
			newTexture->_width = 1;
			newTexture->_height = 1;
			newTexture->_internalFormat = GL_RGBA;
			newTexture->_format = GL_RGBA;
			newTexture->_type = GL_UNSIGNED_BYTE;
			newTexture->_minFilter = GL_NEAREST;
			newTexture->_magFilter = GL_NEAREST;
			newTexture->_wrapS = GL_CLAMP;
			newTexture->_wrapT = GL_CLAMP;
			//Load texture object from file
			this->LoadTexture(newTexture);
			//Be verbose if appropriate
			if (verbose) 
				CLOGGER_DEBUG(WCLogManager::RootLogger(), "Texture " << newTexture->_name << " successfully loaded: " << newTexture->_id);
			//Add object into map
			this->_textureMap.insert( std::make_pair(newTexture->_name, newTexture) );
		}
	}
	//Error checking
	catch (const xercesc::XMLException& toCatch) {
		char* message = xercesc::XMLString::transcode(toCatch.getMessage());
		std::cout << "Exception message is: \n" << message << "\n";
		xercesc::XMLString::release(&message);
	}
	catch (const xercesc::DOMException& toCatch) {
		char* message = xercesc::XMLString::transcode(toCatch.msg);
		std::cout << "Exception message is: \n" << message << "\n";
		xercesc::XMLString::release(&message);
	}
	catch (...) {
		std::cout << "Unexpected Exception \n";
	}
	//Clean up
	delete parser;
	delete errHandler;
	xercesc::XMLString::release(&xmlTextureString);
	xercesc::XMLString::release(&xmlNameString);
}


/***********************************************~***************************************************/


WCTextureManager::WCTextureManager(const std::string &manifest, const std::string &directory, const bool &verbose) :
	_textureMap() {
	//Parse the passed manifest
	this->ParseManifest(manifest, directory, verbose);
	//Check for errors
	if (glGetError() != GL_NO_ERROR) 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTextureManager::WCTextureManager - Unspecified Errors.");
}


WCTextureManager::~WCTextureManager(void) {
	//Delete all of the textures
	std::map<std::string, WSTexture*>::iterator texIter;
	while (!this->_textureMap.empty()) {
		//Get the texture object
		texIter = this->_textureMap.begin();
		//Delete it from the context
		glDeleteTextures(1, &(*texIter).second->_id);
		//Delete the object
		delete (*texIter).second;
		//Remove it from the map
		this->_textureMap.erase(texIter);
	}
}


WSTexture* WCTextureManager::TextureFromName(const std::string &name) {
	//Check to see if the name is in the texture map
	std::map<std::string, WSTexture*>::iterator iter = this->_textureMap.find(name);
	//See if you found anything
	if( iter == this->_textureMap.end() ) {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCTextureManager::TextureFromName - Not able to find texture: " << name);
		return NULL;
	}
	//Otherwise, get the object
	return (*iter).second;
}


/***********************************************~***************************************************/

