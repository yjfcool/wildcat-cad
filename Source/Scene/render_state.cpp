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
#include <Scene/render_state.h>


/***********************************************~***************************************************/


WCRenderState::WCRenderState() : ::WCSerializeableObject(),
	_clearColor(0.0, 0.0, 0.0, 1.0), 
	_frontFace(GL_CCW),
	_polygonMode(GL_FILL),
//	_program(0),
	_shadeModel(GL_FLAT),
	_enableDepthTest(false),
	_enableSmoothing(false)
	{

	//Nothing else to do for now
}


WCRenderState::WCRenderState(const WCRenderState &state) : ::WCSerializeableObject(),
	_clearColor(state._clearColor),
	_frontFace(state._frontFace),
	_polygonMode(state._polygonMode),
	_shadeModel(state._shadeModel),
	_enableDepthTest(state._enableDepthTest),
	_enableSmoothing(state._enableSmoothing) 
	{
	
	//Nothing more for now
}


WCRenderState::WCRenderState(xercesc::DOMNode* node, WCSerialDictionary *dictionary) : ::WCSerializeableObject(),
	_clearColor(0.0, 0.0, 0.0, 1.0), 
	_frontFace(GL_CCW),
	_polygonMode(GL_FILL),
//	_program(0),
	_shadeModel(GL_FLAT),
	_enableDepthTest(false),
	_enableSmoothing(false)
	{

	//Nothing more for now
}


void WCRenderState::Enforce(const WCRenderState *currentState) {
	//Go through all parameters and enable
	this->DepthTest(true);
	this->FrontFace(GL_CW);
	this->Smoothing(true);
}


xercesc::DOMElement* WCRenderState::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	dictionary->InsertAddress(this);	
	//Create the base element for the object
	XMLCh* xmlString = xercesc::XMLString::transcode("RenderState");
	xercesc::DOMElement*  stateElem = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);

	return stateElem;
}


/***********************************************~***************************************************/


