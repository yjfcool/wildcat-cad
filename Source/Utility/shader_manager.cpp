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
#include "Utility/shader_manager.h"
#include "Utility/adapter.h"
#include "Utility/serializeable_object.h"
#include "Utility/log_manager.h"


/***********************************************~***************************************************/


GLuint CompileGLSLShader(GLenum type, const char* shader) {
	//Create the shader object
	GLuint shader_id = glCreateShader(type);
	//Check to make sure you actually created it
    if (!shader_id) {
		//Check for GL errors
		GLenum glErr = glGetError();
		if (glErr != GL_NO_ERROR) 
			CLOGGER_ERROR(WCLogManager::RootLogger(), "ComplieGLSLShader - Unspecified GL Errors:" << glErr);
		CLOGGER_ERROR(WCLogManager::RootLogger(), "ComplieGLSLShader - No shader object created"); 
		return 0;
	}
	glShaderSource(shader_id, 1, &shader, NULL);
	glCompileShader(shader_id);
	GLint status = 0;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
	//Check to make sure it compiled correctly
    if (!status) {
		char temp[256] = "";
		//If not, print out compilation errors
		glGetShaderInfoLog(shader_id, 256, NULL, temp);
		CLOGGER_ERROR(WCLogManager::RootLogger(), "CompileGLSLShader - Shader Compilation failed:\n" << temp);
		//Make sure to destroy the shader object
		glDeleteShader(shader_id);
		return 0;
	}
	//Return the valid shader object
	return shader_id;
}


GLuint CompileGLSLShaderFromFile(GLenum type, const char* filename) {
	FILE *shaderFile;
	char *text;
	long size;

	//Check for no filename
	if (filename == NULL) return 0;
	//Must read files as binary to prevent problems from newline translation
	shaderFile = fopen(filename, "rb");
	if (shaderFile == NULL) { 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "CompileGLSLShaderFromFile - Unable to open " << filename); return 0; }
	fseek(shaderFile, 0, SEEK_END);
	size = ftell(shaderFile);
	fseek(shaderFile, 0, SEEK_SET);
	text = new char[size+1];
	fread(text, size, 1, shaderFile);
	fclose(shaderFile);
	text[size] = '\0';
	GLuint object = CompileGLSLShader(type, text);
	delete []text;
	return object;
}


GLenum ConvertGLTypeStringToEnum(const std::string &type) {
	if (type == "GL_POINTS") return GL_POINTS;
	if (type == "GL_LINES") return GL_LINES;
	if (type == "GL_LINE_LOOP") return GL_LINE_LOOP;
	if (type == "GL_LINE_STRIP") return GL_LINE_STRIP;
	if (type == "GL_TRIANGLES") return GL_TRIANGLES;
	if (type == "GL_TRIANGLE_STRIP") return GL_TRIANGLE_STRIP;
	if (type == "GL_TRIANGLE_FAN") return GL_TRIANGLE_FAN;
	if (type == "GL_QUADS") return GL_QUADS;
	if (type == "GL_QUAD_STRIP") return GL_QUAD_STRIP;
	if (type == "GL_POLYGON") return GL_POLYGON;
	return GL_POINTS;
}


/***********************************************~***************************************************/


WSShader* WCShaderManager::ParseShader(xercesc::DOMElement *element, const std::string &directory, const bool &verbose) {
	//Get the name
	std::string name = WCSerializeableObject::GetStringAttrib(element, "name");
	//Get the type
	std::string type = WCSerializeableObject::GetStringAttrib(element, "type");
	//Get the filename
	std::string filename = WCSerializeableObject::GetStringAttrib(element, "filename");
	//Create shader object
	WSShader *obj = new WSShader();
	obj->_name = name;

	//Create full path name
#ifdef __WIN32__
	//Backslash for windows
	obj->_filename = directory + "\\" + filename;
#else
	//Forward slash for OSX and linux
	obj->_filename = directory + "/" + filename;
#endif
	if (type == "GL_VERTEX_SHADER") obj->_type = GL_VERTEX_SHADER;
	else if (type == "GL_FRAGMENT_SHADER") obj->_type = GL_FRAGMENT_SHADER;
	else if (type == "GL_GEOMETRY_SHADER_EXT") {
		//Set the type
		obj->_type = GL_GEOMETRY_SHADER_EXT;
		//Make sure card can handle geometry shader
		if (!WCAdapter::HasGLEXTGeometryShader4()) {
			//If not...
			CLOGGER_INFO(WCLogManager::RootLogger(), "WCShaderManager::ParseShader - " << name << " requires geometry shader, which is not supported."); 
			//Set id to zero and return
			obj->_id = 0;
			return obj;
		}
	}
	//Dont know this shader type
	else {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCShaderManager::ParseShader - Unknown shader type " << type);
		delete obj;
		return NULL;
	}
	//Compile the program
	obj->_id = CompileGLSLShaderFromFile(obj->_type, obj->_filename.c_str());
	//Check for errors thus far
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCShaderManager::ParseShader - Error with " << name << ": " << err);
	//Validate obj
	if (obj->_id == 0) { 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCShaderManager::ParseShader - " << name << "(" << type << ": " << filename << ") did not compile."); 
		delete obj;
		return NULL;
	}
	//Return the shader
	return obj;
}


WSProgram* WCShaderManager::ParseProgram(xercesc::DOMElement *element, const bool &verbose) {
	char *tmpChars;
	std::string tmpStr;

	//Create program object
	WSProgram *program = new WSProgram();
	program->_id = glCreateProgram();
	//Get the program name
	program->_name = WCSerializeableObject::GetStringAttrib(element, "name");

	//Process all shaders
	xercesc::DOMNode *tmpNode;
	std::map<std::string,WSShader*>::iterator iter;
	XMLCh *xmlString = xercesc::XMLString::transcode("shader");
	xercesc::DOMNodeList *nodeList = element->getElementsByTagName(xmlString);
	xercesc::XMLString::release(&xmlString);
	int numShaders = nodeList->getLength();
	xercesc::DOMElement *tmpElement;
	//Loop through all shaders and process
	for (int index=0; index < numShaders; index++) {
		tmpElement = (xercesc::DOMElement*)nodeList->item(index);
		//Get the value node
		tmpNode = tmpElement->getFirstChild();
		//Find the shader name
		tmpChars = xercesc::XMLString::transcode(tmpNode->getNodeValue());
		tmpStr = std::string(tmpChars);
		xercesc::XMLString::release(&tmpChars);
		//Look up in map
		iter = this->_shaderMap.find(tmpStr);
		//Is a shader found at all
		if (iter == this->_shaderMap.end()) { 
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCShaderManager::ParseProgram - " << tmpStr << " not found."); 
			delete program;
			return NULL;
		}
		//Is the shader valid
		if ((*iter).second->_id == 0) {
			CLOGGER_INFO(WCLogManager::RootLogger(), "WCShaderManager::ParseProgram - " << tmpStr << " requires excluding this program.");
			delete program;
			return NULL;
		}
		//Attach shader
		glAttachShader(program->_id, (*iter).second->_id);
		//Add shader to shader list
		program->_shaders.push_back( (*iter).second );
	}

	//Process transform feedback
	xmlString = xercesc::XMLString::transcode("transform_feedback");
	nodeList = element->getElementsByTagName(xmlString);
	xercesc::XMLString::release(&xmlString);
	if (nodeList->getLength() == 1) {
		//Found a transform feedback
		tmpElement = (xercesc::DOMElement*)nodeList->item(0);
		//Get type attribute
		std::string typeStr = WCSerializeableObject::GetStringAttrib(tmpElement, "type");
		GLenum type;
		//Convert string to enum
		if (typeStr == "GL_INTERLEAVED_ATTRIBS_EXT") type = GL_INTERLEAVED_ATTRIBS_EXT;
		else if (typeStr == "GL_SEPARATE_ATTRIBS_EXT") type = GL_SEPARATE_ATTRIBS_EXT;
		else { 
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCShaderManager::ParseProgram - Unknown transform feedback type(" << typeStr << "."); 
			delete program;
			return NULL;
		}

		//Get list of varyings
		xmlString = xercesc::XMLString::transcode("varying");
		nodeList = tmpElement->getElementsByTagName(xmlString);
		xercesc::XMLString::release(&xmlString);
		GLint varyingCount = nodeList->getLength();
		const GLchar** varyings = new const GLchar*[varyingCount];
		//Process varyings
		for (int index=0; index<varyingCount; index++) {
			//Get the varying element
			tmpElement = (xercesc::DOMElement*)nodeList->item(index);
			//Get the value node
			tmpNode = tmpElement->getFirstChild();
			//Get the name of the varying and put into array
			varyings[index] = xercesc::XMLString::transcode(tmpNode->getNodeValue());
		}
		//Set up the transform feedback for the program
		glTransformFeedbackVaryingsEXT(program->_id, varyingCount, varyings, type);
		//Delete all of the varyings
		for (int j=0; j<varyingCount; j++) delete varyings[j];
		delete varyings;
	}

	//Process geometry shader params (if present)
	xmlString = xercesc::XMLString::transcode("geometry_shader");
	nodeList = element->getElementsByTagName(xmlString);
	xercesc::XMLString::release(&xmlString);
	if (nodeList->getLength() == 1) {
		//Found a geometry shader
		tmpElement = (xercesc::DOMElement*)nodeList->item(0);		
		//Get input type
		std::string inputTypeStr = WCSerializeableObject::GetStringAttrib(tmpElement, "inputType");
		std::string outputTypeStr = WCSerializeableObject::GetStringAttrib(tmpElement, "outputType");
		GLint vertexOut = (GLint)WCSerializeableObject::GetFloatAttrib(tmpElement, "vertsOut");
		//Set the attributes
		GLenum inputType = ConvertGLTypeStringToEnum(inputTypeStr);
		GLenum outputType = ConvertGLTypeStringToEnum(outputTypeStr);
		//Set the program parameters
		glProgramParameteriEXT(program->_id, GL_GEOMETRY_INPUT_TYPE_EXT, inputType);
		glProgramParameteriEXT(program->_id, GL_GEOMETRY_VERTICES_OUT_EXT, vertexOut);
		glProgramParameteriEXT(program->_id, GL_GEOMETRY_OUTPUT_TYPE_EXT, outputType);
	}

	//Link the program
	glLinkProgram(program->_id);
	GLint linked;
	glGetProgramiv(program->_id, GL_OBJECT_LINK_STATUS_ARB, &linked);
	//Check the link status
	if (!linked) {
		char temp[256] = "";
		//If not, print out compilation errors
		glGetProgramInfoLog(program->_id, 256, NULL, temp);
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCShaderManager::ParseProgram - Program Link failed:\n" << temp);
		delete program; 
		program = NULL; 
	}
	else {
		if (verbose) CLOGGER_DEBUG(WCLogManager::RootLogger(), "Program " << program->_id << " successfully linked: " << program->_name);
	}
	//Return the program object
	return program;
}


void WCShaderManager::ParseManifest(const std::string &manifest, const std::string &directory, const bool &verbose) {
	//Create xml parser
	xercesc::XercesDOMParser* parser = new xercesc::XercesDOMParser();
	//Set validation scheme
	parser->setValidationScheme(xercesc::XercesDOMParser::Val_Always);    
	xercesc::ErrorHandler* errHandler = (xercesc::ErrorHandler*) new xercesc::HandlerBase();
	parser->setErrorHandler(errHandler);

	//Get the full path to the manifest
	std::string fullPath = directory + "/" + manifest;
	const char* xmlFile = fullPath.c_str();
	//Try to parse
	try {
		//Parse the manifest
		parser->parse(xmlFile);
		//Get the root document
		xercesc::DOMDocument* rootDocument = parser->getDocument();

		//Find all nodes called shaders
		XMLCh *xmlString = xercesc::XMLString::transcode("shaders");
		xercesc::DOMNodeList *nodeList = rootDocument->getElementsByTagName(xmlString);
		xercesc::XMLString::release(&xmlString);
		//Make sure there is a list of shaders
		if (nodeList->getLength() != 1) {
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCShaderManager::ParseManifest - Invalid shaders count: " << nodeList->getLength());
			return;
		}
		//Get the list
		xercesc::DOMNode *shadersNode = nodeList->item(0);
		xercesc::DOMElement *shaders = (xercesc::DOMElement*)shadersNode;
		xmlString = xercesc::XMLString::transcode("shader");
		nodeList = shaders->getElementsByTagName(xmlString);
		xercesc::XMLString::release(&xmlString);
//		std::cout << "Shaders: " << nodeList->getLength() << std::endl;
		int shaderCount = nodeList->getLength();
		WSShader *shader;
		xercesc::DOMElement *shaderElement;
		//Loop through all shaders and process
		for (int index=0; index < shaderCount; index++) {
			shaderElement = (xercesc::DOMElement*)nodeList->item(index);
			shader = this->ParseShader(shaderElement, directory, verbose);
			//Add shader to manager's vector
			if (shader != NULL) this->_shaderMap.insert(make_pair(shader->_name, shader));
		}

		//Find all node called programs
		xmlString = xercesc::XMLString::transcode("programs");
		nodeList = rootDocument->getElementsByTagName(xmlString);
		xercesc::XMLString::release(&xmlString);
		//Make sure there is a list of programs
		if (nodeList->getLength() != 1) {
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCShaderManager::ParseManifest - Invalid programs count: " << nodeList->getLength());
			return;
		}
		//Get the list
		xercesc::DOMNode *programsNode = nodeList->item(0);
		xercesc::DOMElement *programs = (xercesc::DOMElement*)programsNode;
		xmlString = xercesc::XMLString::transcode("program");
		nodeList = programs->getElementsByTagName(xmlString);
		xercesc::XMLString::release(&xmlString);
//		std::cout << "Programs: " << nodeList->getLength() << std::endl;
		int progCount = nodeList->getLength();
		WSProgram *program;
		xercesc::DOMElement *programElement;
		//Loop through all shaders and process
		for (int index=0; index < progCount; index++) {
			programElement = (xercesc::DOMElement*)nodeList->item(index);
			program = this->ParseProgram(programElement, verbose);
			//Add program to manager's vector
			if (program != NULL) this->_programMap.insert(make_pair(program->_name, program));
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
}


/***********************************************~***************************************************/


WCShaderManager::WCShaderManager(const std::string &manifest, const std::string &directory, const bool &verbose) : 
	_programMap(), _shaderMap() {
	//Make sure the adapter has been initialized
	WCAdapter::Initialize();
	//Parse the passed manifest
	this->ParseManifest(manifest, directory, verbose);
	//Check for errors
	if (glGetError() != GL_NO_ERROR) 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCShaderManager::WCShaderManager - Unspecified Errors: in constructor");
}


WCShaderManager::~WCShaderManager(void) {
	//Delete all of the shaders
	std::map<std::string, WSShader*>::iterator sIter;
	while (!this->_shaderMap.empty()) {
		sIter = this->_shaderMap.begin();
		glDeleteShader((*sIter).second->_id);
		delete (*sIter).second;
		this->_shaderMap.erase(sIter);
	}
	//Delete all of the programs
	std::map<std::string, WSProgram*>::iterator pIter;
	while (!this->_programMap.empty()) {
		pIter = this->_programMap.begin();
		glDeleteProgram((*pIter).second->_id);
		delete (*pIter).second;
		this->_programMap.erase(pIter);
	}
}


GLuint WCShaderManager::ShaderFromName(const std::string &name) {
	//Check to see if the name is in the shader map
	std::map<std::string, WSShader*>::iterator iter = this->_shaderMap.find(name);
	//See if you found anything
	if( iter == this->_shaderMap.end() ) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCShaderManager::ShaderFromName - Unable to find shader: " << name);
		return 0;
	}
	//Otherwise, get the object ID
	return (*iter).second->_id;
}


GLuint WCShaderManager::ProgramFromName(const std::string &name) {
	//Check to see if the name is in the program map
	std::map<std::string, WSProgram*>::iterator iter = this->_programMap.find(name);
	//See if you found anything
	if( iter == this->_programMap.end() ) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCShaderManager::ProgramFromName - Unable to find program: " << name);
		return 0;
	}
	//Otherwise, get the object ID
	return (*iter).second->_id;
}

	
/***********************************************~***************************************************/

