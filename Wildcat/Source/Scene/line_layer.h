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


#ifndef __LINE_LAYER_H__
#define __LINE_LAYER_H__


/*** Included Headers ***/
#include "Scene/wscnl.h"
#include "Scene/layer.h"


/*** Local Defines ***/
#define LINELAYER_THICKNESS						1.0


/*** Class Predefines ***/
class WCGeometricLine;


/***********************************************~***************************************************/


class WCLineLayer : public WCLayer {
protected:
	std::list<WCGeometricLine*>					_lineList;											//!< List of all geometric lines
	GLuint										_renderProg;										//!< Render program for the entire layer
	WPUInt										_numVisible;										//!< Number of visible lines
	GLuint										_vertexBuffer, _colorBuffer;						//!< Vertex and color buffers
	
private:
	//Private Methods
	void GenerateBuffers(void);																		//!< Generate display buffers
	
	//Deny access
	WCLineLayer();																					//!< Deny access to default constructor
	WCLineLayer(const WCLineLayer &layer);															//!< Deny access to copy constructor
	WCLineLayer& operator=(const WCLineLayer &layer);												//!< Deny access to equals operator
	bool operator==(const WCLineLayer &layer);														//!< Deny access to equality operator
public:
	//Constructors and Destructors
	WCLineLayer(WCScene *scene, std::string name);													//!< Primary constructor
	~WCLineLayer();																					//!< Default destructor

	//Member Access Methods
	inline void RenderProgram(const GLuint prog) { this->_renderProg = prog; }						//!< Set the rendering prog
	inline GLuint RenderProgram(void) const	{ return this->_renderProg; }							//!< Get the rendering prog

	//Geometric Line Methods
	void AddLine(WCGeometricLine *line);															//!< Register the line
	void RemoveLine(WCGeometricLine *line);															//!< Unregister the line
	
	//Required Inherited Methods
	void ReceiveNotice(WCObjectMsg msg, WCObject *sender);											//!< Receive message
	WCAlignedBoundingBox BoundingBox(void);															//!< Bounding box for the layer
	void Render(WCRenderState *state);																//!< Render the layer
};


/***********************************************~***************************************************/


#endif //__LINE_LAYER_H__

