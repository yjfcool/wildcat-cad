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
#include <Scene/light_source.h>


/*** Locally Defined Values ***/
//None


/***********************************************~***************************************************/


WCLightSource::WCLightSource() : ::WCSerializeableObject(),
	_active(true), _ambient(LIGHTSOURCE_DEFAULT_AMBIENT), _diffuse(LIGHTSOURCE_DEFAULT_DIFFUSE), 
	_specular(LIGHTSOURCE_DEFAULT_SPECULAR), _position(LIGHTSOURCE_DEFAULT_POSITION), 
	_direction(LIGHTSOURCE_DEFAULT_DIRECTION), _locations(NULL), _lastProg(0) {
	//Allocate space for locations
	this->_locations = new GLuint[5];
}


WCLightSource::WCLightSource(const WCColor& ambient, const WCColor& diffuse, const WCColor& specular, 
	const WCVector4& position, const WCVector4& direction) : ::WCSerializeableObject(),
	_active(true), _ambient(ambient), _diffuse(diffuse), _specular(specular),
	_position(position), _direction(direction), _locations(NULL), _lastProg(0) {
	//Allocate space for locations
	this->_locations = new GLuint[5];
}


WCLightSource::WCLightSource(const WCLightSource& light) : ::WCSerializeableObject(),
	_active(light._active), _ambient(light._ambient), _diffuse(light._diffuse), 
	_specular(light._specular), _position(light._position), 
	_direction(light._direction), _locations(NULL), _lastProg(0) {
	//Allocate space for locations
	this->_locations = new GLuint[5];
}


WCLightSource::~WCLightSource() {
	//Make sure to delete locations
	if (this->_locations != NULL) {
		delete this->_locations;
		this->_locations = NULL;
	}
}

	
WCLightSource& WCLightSource::operator=(const WCLightSource& light) {
	//Copy all the parameters
	this->_active  = light._active;
	this->_ambient = light._ambient;
	this->_diffuse = light._diffuse;
	this->_specular = light._specular;
	this->_position = light._position;
	this->_direction = light._direction;
	//Set last prog
	this->_lastProg = 0;
	return *this;
}

	
void WCLightSource::Render(GLuint currentProg) {
	//if currentProg is lastProg, do nothing
	if ((currentProg == 0) || (this->_lastProg == currentProg) || (!this->_active)) return;
	//Otherwise get and set all of the parameters
	GLint loc = glGetUniformLocation(currentProg, "lightAmbientColor");
	if (loc != -1) glUniform3f(currentProg, this->_ambient.R(), this->_ambient.G(), this->_ambient.B());
	loc = glGetUniformLocation(currentProg, "lightDiffuseColor");
	if (loc != -1) glUniform3f(currentProg, this->_diffuse.R(), this->_diffuse.G(), this->_diffuse.B());
	loc = glGetUniformLocation(currentProg, "lightSpecularColor");
	if (loc != -1) glUniform3f(currentProg, this->_specular.R(), this->_specular.G(), this->_specular.B());
	loc = glGetUniformLocation(currentProg, "lightPosition");
	if (loc != -1) glUniform3f(currentProg, (GLfloat)this->_position.I(), (GLfloat)this->_position.J(), (GLfloat)this->_position.K());
	loc = glGetUniformLocation(currentProg, "lightDirection");
	if (loc != -1) glUniform3f(currentProg, (GLfloat)this->_direction.I(), (GLfloat)this->_direction.J(), (GLfloat)this->_direction.K());
	//Make sure to update lastProg
	this->_lastProg = currentProg;
}


xercesc::DOMElement* WCLightSource::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Create the base element for the object
	xercesc::DOMElement*  lightElem = document->createElement(xercesc::XMLString::transcode("LightSource"));
	return lightElem;
}


/***********************************************~***************************************************


WCLightSource::WCLightSource() : ::WCSerializeableObject(),
	_active(true), _color(LIGHTSOURCE_DEFAULT_COLOR), _position(0.0, 0.0, 1.0), _direction(0.0, 0.0, -1.0),
	_barnShaping(false), _seWidth(1.0), _seHeight(1.0), _seWidthEdge(1.0), _seHeightEdge(1.0), 
	_seRoundness(1.0), _dsNear(1.0), _dsFar(1.0), _dsNearEdge(1.0), _dsFarEdge(1.0) {
}


WCLightSource::~WCLightSource() {
}


/***********************************************~***************************************************/

