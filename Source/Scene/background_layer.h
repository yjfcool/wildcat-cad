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


#ifndef __BACKGROUND_LAYER_H__
#define __BACKGROUND_LAYER_H__


/*** Included Header Files ***/
#include "Scene/wscnl.h"
#include "Scene/layer.h"


/*** Locally Defined Values ***/
#define BACKGROUNDLAYER_TOP_COLOR				0.3, 0.3, 0.8, 1.0
#define BACKGROUNDLAYER_BOTTOM_COLOR			0.1, 0.1, 0.1, 1.0


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCColor;


/***********************************************~***************************************************/


class WCBackgroundLayer : public WCLayer {
protected:
	GLuint										_vertexBuffer, _colorBuffer;						//!< Display buffers
	WCColor										_llColor, _lrColor, _ulColor, _urColor;
private:
	void GenerateBuffers(void);																		//!< Generate the buffers

	//Deny Access
	WCBackgroundLayer();																			//!< Deny access to default constructor
public:
	//Constructors and Destructors
	WCBackgroundLayer(WCScene *scene);																//!< Primary constructor
	~WCBackgroundLayer();																			//!< Default destructor

	//Inherited Methods
	bool OnReshape(const WPFloat width, const WPFloat height);										//!< Resize the scene
	void Render(WCRenderState *state);																//!< Render the layer
};



/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__USER_INTERFACE_LAYER_H__

