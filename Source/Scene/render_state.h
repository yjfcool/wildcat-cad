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


#ifndef __RENDER_STATE_H__
#define __RENDER_STATE_H__


/*** Included Header Files ***/
#include "Scene/wscnl.h"


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


class WCRenderState : virtual public WCSerializeableObject {
private:
	bool										_isDirty;											//!< Has the render state changed
	WCColor										_clearColor;										//!< Clear color
	GLfloat										_clearDepth;										//!< Clear depth
	GLenum										_frontFace;											//!< Front face winding
	GLint										_polygonMode;										//!< GL_LINE or GL_FILL
//	GLuint										_program;											//!< Active program
	GLenum										_shadeModel;										//!< Shading model
	bool										_enableDepthTest;									//!< Is depth testing on
	bool										_enableSmoothing;									//!< Is smoothing on
public:
	//Constructors and Destructors
	WCRenderState();																				//!< Default constructor
	WCRenderState(const WCRenderState &state);														//!< Copy constructor
	WCRenderState(xercesc::DOMNode* node, WCSerialDictionary *dictionary);							//!< Persistance constructor
	~WCRenderState()							{ }													//!< Default destructor
	
	//
	void Enforce(const WCRenderState *currentState);												//!< Make sure all of state is set
	
	inline bool	DepthTest(void) const			{ return this->_enableDepthTest; }					//!< Get depth test state
	inline void DepthTest(bool status)			{ this->_enableDepthTest;							//!< Set depth test state
												  if (status) glEnable(GL_DEPTH_TEST);
												  else glDisable(GL_DEPTH_TEST); }
	
	inline WCColor ClearColor(void) const		{ return this->_clearColor; }						//!< Get the clear color
	inline void ClearColor(const GLfloat r, const GLfloat g, const GLfloat b, const GLfloat a)		//!< Set the clear colorb
												{ this->_clearColor.Set(r, g, b, a);
												  glClearColor(r, g, b, a); }
	inline GLfloat ClearDepth(void) const		{ return this->_clearDepth; }						//!< Get the clear depth
	inline void ClearDepth(const GLfloat depth)	{ this->_clearDepth = depth;						//!< Set the clear depth
												  glClearDepth(depth); }
	inline GLenum FrontFace(void) const			{ return this->_frontFace; }						//!< Get the front face winding
	inline void FrontFace(const GLenum winding)	{ this->_frontFace = winding;						//!< Set the front face winding
												  glFrontFace(winding); }
	inline GLint PolygonMode(void) const		{ return this->_polygonMode; }						//!< Get the mode
	inline void PolygonMode(const GLint mode)	{ this->_polygonMode = mode;						//!< Set the mode
												  glPolygonMode(GL_FRONT_AND_BACK, mode); }
//	inline GLuint Program(void) const			{ return this->_program; }							//!< Get the active program
//	inline void Program(const GLuint prog)		{ this->_program = prog;							//!< Set the active program
//												  glUseProgram(prog); }
	inline GLenum ShadeModel(void) const		{ return this->_shadeModel; }						//!< Get the shading model
	inline void ShadeModel(const GLenum model)	{ this->_shadeModel = model;						//!< Set the shading model
												  glShadeModel(model); }
	inline bool	Smoothing(void) const			{ return this->_enableSmoothing; }					//!< Get smoothing state
	inline void Smoothing(bool status)			{ this->_enableSmoothing;							//!< Set smoothing state
												  if (status) glEnable(GL_SMOOTH);
												  else glDisable(GL_SMOOTH); }
	
	//Serialization and Object Methods
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__RENDER_STATE_H__

