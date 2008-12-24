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


#ifndef __LAYER_H__
#define __LAYER_H__


/*** Included Headers ***/
#include <Scene/wscnl.h>
#include <Scene/scene.h>
#include <Scene/render_state.h>


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


class WCLayer : virtual public WCObject {
protected:
	WCScene										*_scene;											//!< Pointer to parent scene object
	std::string									_name;												//!< Layer name
	WCLayer										*_prev, *_next;										//!< DDList pointers
	bool										_isVisible;											//!< Visibility flag
	bool										_isShadowPass;										//!< Shadowing pass flag
	bool										_isSelectionPass;									//!< Selection pass flag
	bool										_isDirty;											//!< Dirty flag
	bool										_texBacked;											//!< Texture backing flag
	GLint										_tex;												//!< Backing texture

private:
	//Deny access
	WCLayer();																						//!< Deny access to default constructor
	WCLayer(const WCLayer &layer);																	//!< Deny access to copy constructor
	WCLayer& operator=(const WCLayer &layer);														//!< Deny access to equals operator
	bool operator==(const WCLayer &layer);															//!< Deny access to equality operator
public:
	//Constructors and Destructors
	WCLayer(WCScene *scene, std::string name) : ::WCObject(),										//!< Primary constructor
												_scene(scene), _name(name), _isVisible(true),
												_isShadowPass(true), _isSelectionPass(true),
												_texBacked(false), _tex(0) { }	
	virtual ~WCLayer()							{ this->_scene->UnregisterLayer(this); }			//!< Default destructor
	
	//Member Access Methods
	virtual inline WCScene* Scene(void) const	{ return this->_scene; }							//!< Get the layer's parent scene
	virtual inline std::string Name(void) const	{ return this->_name; }								//!< Get the layer name
	inline WCLayer* Previous(void)				{ return this->_prev; }								//!< Get the previous layer
	inline void Previous(WCLayer *prev)			{ this->_prev = prev; }								//!< Set the previous layer
	inline WCLayer* Next(void)					{ return this->_next; }								//!< Get the next layer
	inline void Next(WCLayer *next)				{ this->_next = next; }								//!< Set the next layer
	virtual inline bool IsShadowPass(void) const { return this->_isShadowPass; }					//!< Get the shadow pass flag
	virtual inline void IsShadowPass(const bool status) { this->_isShadowPass; }					//!< Set the shadow pass flag
	virtual inline bool IsSelectionPass(void) const { return this->_isSelectionPass; }				//!< Get the selection pass flag
	virtual inline void IsSelectionPass(const bool status) { this->_isSelectionPass; }				//!< Set the selection pass flag
	virtual inline void MarkDirty(void)			{ this->_isDirty = true;							//!< Set the dirty flag
												  this->_scene->MarkDirty(); }
	virtual inline bool IsDirty(void) const		{ return this->_isDirty; }							//!< Get the dirty flag
	virtual inline void IsVisible(const bool status){ this->_isVisible = status;					//!< Set the visible flag
												  this->_isDirty = true; 
												  this->_scene->MarkDirty(); }
	virtual inline bool IsVisible(void) const	{ return this->_isVisible; }						//!< Get the visible flag
	
	//Other Methods
	virtual WCAlignedBoundingBox BoundingBox(void) { return WCAlignedBoundingBox();	}				//!< Bounding box for the layer
	
	//Event Handling Methods
	virtual bool OnIdle(void)					{ return false; }									//!< Handle an idle event
	virtual bool OnMouseMove(const WPFloat x, const WPFloat y)	{ return false; }					//!< Track the mouse x and y position
	virtual bool OnMouseDown(const WCMouseButton &button)		{ return false; }					//!< Mouse button down event
	virtual bool OnMouseUp(const WCMouseButton &button)			{ return false; }					//!< Mouse button up event
	virtual bool OnScroll(const WPFloat &x, const WPFloat &y)	{ return false; }					//!< Scroll-wheel event
	virtual bool OnArrowKeyPress(const WCArrowKey &key)			{ return false; }					//!< Arrow key event
	virtual bool OnReshape(const WPFloat width, const WPFloat height)	{ return false; }			//!< Window reshape info
	
	//Required Methods
	virtual void Render(WCRenderState *state)=0;													//!< Mandatory render command
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__LAYER_H__

