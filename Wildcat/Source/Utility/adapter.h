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


#ifndef __GRAPHICS_ADAPTER_H__
#define __GRAPHICS_ADAPTER_H__


/*** Included Header Files ***/
#include "Utility/wutil.h"


/*** Local Defines ***/
#define ADAPTER_OPENGL_VERSION_14				0
#define ADAPTER_OPENGL_VERSION_15				1
#define ADAPTER_OPENGL_VERSION_20				2
#define ADAPTER_OPENGL_VERSION_21				3
#define ADAPTER_OPENGL_VERSION_30				4
#define ADAPTER_EXTENSION_COUNT					348
#define CHKINIT { if(!WCAdapter::_isInitialized) WCAdapter::Initialize(); }


/***********************************************~***************************************************/


class WCAdapter {
private:
	static bool									_isInitialized;										//!< Initialization flag
	static bool									*_extensions;										//!< Array of extension flags
	
	//Card limits
	static GLint								_maxGeometryOutputVertices;							//!< Maximum number of verts a geometry shader can output
	static GLint								_max2DTextureSize;									//!< Maximum size of one dimension of a 2D texture	
private:
	WCAdapter();																					//!< Deny access to default constructor
public:
	//Initialize and Terminate
	static void Initialize(void);																	//!< Static initialization method
	static void Terminate(void);																	//!< Static Terminate method
	
	//Extension functions
	inline static bool HasGLARBFragmentShader(void)		{ CHKINIT; return WCAdapter::_extensions[33]; }
	inline static bool HasGLARBShadingLanguage100(void)	{ CHKINIT; return WCAdapter::_extensions[34]; }
	inline static bool HasGLARBTextureRectange(void)	{ CHKINIT; return WCAdapter::_extensions[39]; }
	inline static bool HasGLEXTTextureFloat(void)		{ CHKINIT; return WCAdapter::_extensions[44]; }	
	inline static bool HasGLARBPixelBufferObject(void)	{ CHKINIT; return WCAdapter::_extensions[45]; }
	inline static bool HasGLEXTFramebufferObject(void)	{ CHKINIT; return WCAdapter::_extensions[307]; }
	inline static bool HasGLEXTGeometryShader4(void)	{ CHKINIT; return WCAdapter::_extensions[321]; }
	inline static bool HasGLNVVertexProgram4(void)		{ CHKINIT; return WCAdapter::_extensions[322]; }
	inline static bool HasGLEXTGPUShader4(void)			{ CHKINIT; return WCAdapter::_extensions[323]; }
	inline static bool HasGLEXTBindableUniform(void)	{ CHKINIT; return WCAdapter::_extensions[343]; }
	inline static bool HasGLEXTTextureInteger(void)		{ CHKINIT; return WCAdapter::_extensions[344]; }
	inline static bool HasGLXEXTTextureFromPixmap(void)	{ CHKINIT; return WCAdapter::_extensions[345]; }	
	inline static bool HasGLEXTTransformFeedback(void)	{ CHKINIT; return WCAdapter::_extensions[346]; }
	
	inline static GLint GetMaxGeometryOutputVertices(void)	{ CHKINIT; return WCAdapter::_maxGeometryOutputVertices; }
	inline static GLint GetMax2DTextureSize(void)			{ CHKINIT; return WCAdapter::_max2DTextureSize; }	
};


/***********************************************~***************************************************/

/*** Any Undefines ***/
#undef CHKINIT


#endif //__GRAPHICS_ADAPTER_H__

