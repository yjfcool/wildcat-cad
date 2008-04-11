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


#ifndef __TEXTURE_MANAGER_H__
#define __TEXTURE_MANAGER_H__


/*** Included Headers ***/
#include "Utility/wutil.h"
#include "Utility/object.h"


/*** Locally Defined Values ***/
#define TEXTUREMANAGER_STRING_SIZE				256


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


struct WSTexture {
	std::string									_name;												//!< Filename of the texture
	GLuint										_id;												//!< GL id of the loaded texture
	GLenum										_target;											//!< Target of texture
	GLint										_mipLevel;											//!< Mipmapping levels
	GLfloat										_width, _height;									//!< Width and height of the texture
	GLenum										_internalFormat, _format;							//!< GL_RGBA, GL_RGBA_FLOAT32_APPLE, etc.
	GLenum										_type;												//!< GL_UNSIGNED_BYTE etc.
	GLenum										_minFilter, _magFilter;								//!< GL_NEAREST, GL_LINEAR, etc.
	GLenum										_wrapS, _wrapT;										//!< GL_CLAMP, etc.
};


/***********************************************~***************************************************/


class WCTextureManager {
private:
	std::map<std::string, WSTexture*>			_textureMap;										//!< Map of names to texture objects
	
	//Private Methods
	void LoadTexture(WSTexture *texObj);															//!< Load a texture from disk
	void ParseManifest(const std::string &manifest, const std::string &dir, const bool &verbose);	//!< Parse the given manifest
	
	//Deny Access
	WCTextureManager();																				//!< Deny access to default constructor
	WCTextureManager(const WCTextureManager& mgr);													//!< Deny access to copy constructor
	WCTextureManager& operator=(const WCTextureManager& mgr);										//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCTextureManager(const std::string &manifest, const std::string &dir, const bool &verbose=false);//!< Initialize the manager with a manifest
	~WCTextureManager();																			//!< Default destructor
	
	//Shader access info
	WSTexture* TextureFromName(const std::string &name);											//!< Get a shader from a name
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__TEXTURE_MANAGER_H__

