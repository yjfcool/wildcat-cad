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


#ifndef __VISUAL_LAYER_H__
#define __VISUAL_LAYER_H__


/*** Included Header Files ***/
#include "Scene/wscnl.h"
#include "Scene/layer.h"


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


class WCVisualLayer : public WCLayer {
protected:
	std::list<WCVisualObject*>					_objectList;										//!< List of all visual objects
	std::list<WCVisualObject*>					_visibleList;										//!< List of visible visual objects
	GLuint										_renderProg;										//!< Render program for the entire layer
	
private:
	//Deny access
	WCVisualLayer();																				//!< Deny access to default constructor
	WCVisualLayer(const WCVisualLayer &layer);														//!< Deny access to copy constructor
	WCVisualLayer& operator=(const WCVisualLayer &layer);											//!< Deny access to equals operator
	bool operator==(const WCVisualLayer &layer);													//!< Deny access to equality operator
public:
	//Constructors and Destructors
	WCVisualLayer(WCScene *scene, std::string name);												//!< Primary constructor
	~WCVisualLayer();																				//!< Default destructor

	//Member Access Methods
	inline void RenderProgram(const GLuint prog){ this->_renderProg = prog; }						//!< Set the rendering prog
	inline GLuint RenderProgram(void) const		{ return this->_renderProg; }						//!< Get the rendering prog

	//Geometry Object Methods
	void AddObject(WCVisualObject *object);															//!< Register the object
	template <class T>void RemoveObject(const std::vector<T> &collection) {							//!< Remove a collection of objects
												 for (int i=0; i<collection.size(); ++i)
													this->RemoveObject(collection.at(i)); }	
	void RemoveObject(WCVisualObject *object);														//!< Unregister the object
	
	//Required Inherited Methods
	WCAlignedBoundingBox BoundingBox(void);															//!< Bounding box for the layer
	void Render(WCRenderState *state);																//!< Render the layer
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__VISUAL_LAYER_H__

