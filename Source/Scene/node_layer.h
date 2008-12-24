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


#ifndef __NODE_LAYER_H__
#define __NODE_LAYER_H__


/*** Included Header Files ***/
#include <Scene/wscnl.h>
#include <Scene/layer.h>


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


class WCNodeLayer : virtual public WCLayer {
protected:
	std::vector<WCLayer*>						_layers;											//!< List of sub-layers
	
private:
	//Deny Access
	WCNodeLayer();																					//! Deny access to default constructor
	
public:
	//Constructors and Destructors
	WCNodeLayer(WCScene *scene, std::string name);													//!< Primary constructor
	~WCNodeLayer();																					//!< Default destructor
	
	//Sub-Layer Methods
	void AddLayer(void);
	void RemoveLayer(void);
	
	//Inherited Methods
	WCAlignedBoundingBox BoundingBox(void);															//!< Bounding box for the layer
	
	//Event Handling Methods
	bool OnIdle(void);																				//!< Handle an idle event
	bool OnMouseMove(const WPFloat x, const WPFloat y);												//!< Track the mouse x and y position
	bool OnMouseDown(const WCMouseButton &button);													//!< Mouse button down event
	bool OnMouseUp(const WCMouseButton &button);													//!< Mouse button up event
	bool OnReshape(const WPFloat width, const WPFloat height);										//!< Window reshape info
	
	//Required Methods
	void Render(WCRenderState *state);
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__NODE_LAYER_H__

