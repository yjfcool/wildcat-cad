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


#ifndef __SHADER_MANAGER_H__
#define __SHADER_MANAGER_H__


/*** Included Headers ***/
#include "Utility/wutil.h"


/*** Local Defines ***/
#define SHADERMANAGER_STRING_SIZE				256


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


struct WSShader {
	std::string									_name;
	GLuint										_id;
	GLenum										_type;
	std::string									_filename;
};


struct WSProgram {
	std::string									_name;
	GLuint										_id;
	std::vector<WSShader*>						_shaders;
	bool										_tf;
	std::vector<char*>							_tfVaryings;
	bool										_tfInterleaved;
	GLenum										_gsInputType, _gsOutputType;
	GLuint										_gsOutputVerts;
};


/***********************************************~***************************************************/


class WCShaderManager {
private:
	std::map<std::string, WSProgram*>			_programMap;										//!< Map of names to program objects
	std::map<std::string, WSShader*>			_shaderMap;											//!< Map of names to shader objects
	
	//Private Methods
	WSShader* ParseShader(xercesc::DOMElement *element, const std::string &directory,				//!< Parse a shader
												const bool &verbose);
	WSProgram* ParseProgram(xercesc::DOMElement *element, const bool &verbose);						//!< Parse a program
	void ParseManifest(const std::string &manifest, const std::string &dir, const bool &verbose);	//!< Parse the given manifest
	
	//Deny Access
	WCShaderManager();																				//!< Deny access to default constructor
	WCShaderManager(const WCShaderManager& mgr);													//!< Deny access to copy constructor
	WCShaderManager& operator=(const WCShaderManager& mgr);											//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCShaderManager(const std::string &manifest, const std::string &dir, const bool &verbose=false);//!< Initialize the manager with a manifest
	~WCShaderManager();																				//!< Default destructor
	
	//Shader Access Methods
	GLuint ShaderFromName(const std::string &name);													//!< Get a shader from a name
	GLuint ProgramFromName(const std::string &name);												//!< Get a program from a name
};


/***********************************************~***************************************************/


#endif //__SHADER_MANAGER_H__

