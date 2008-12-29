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
#include <Utility/gl_context.h>
#include <Utility/log_manager.h>
#include <Utility/assert_exception.h>


/***********************************************~***************************************************/


WCGLContext::WCGLContext() : _context(NULL) {
	//Rendering context
	HGLRC hRC = NULL;
	//GDI Device context
	HDC hDC = NULL;
	HWND hWnd = NULL;
	//Describe pixel format (adapted from NeHe tutorials)
	static	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),			// Size Of This Pixel Format Descriptor
		1,										// Version Number
		PFD_DRAW_TO_WINDOW |					// Format Must Support Window
		PFD_SUPPORT_OPENGL |					// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,						// Must Support Double Buffering
		PFD_TYPE_RGBA,							// Request An RGBA Format
		24,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,						// Color Bits Ignored
		8,										// 8-bit Alpha Buffer
		0,										// Shift Bit Ignored
		0,										// No Accumulation Buffer
		0, 0, 0, 0,								// Accumulation Bits Ignored
		24,										// 24-Bit Z-Buffer (Depth Buffer)
		0,										// No Stencil Buffer
		1,										// No Auxiliary Buffer
		PFD_MAIN_PLANE,							// Main Drawing Layer
		0,										// Reserved
		0, 0, 0									// Layer Masks Ignored
	};

	//Try to get the Device Context
	if ( !(hDC = GetDC(hWnd)) ) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGLContext::WCGLContext - Not able to get device context.");
		throw WCException("Not ablt to get device context.");
	}
	//Try to choose the pixel format
	GLuint pixelFormat;
	if ( !(pixelFormat = ChoosePixelFormat(hDC, &pfd)) )	{
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGLContext::WCGLContext - Not able to choose pixel format.");
		throw WCException("Not ablt to choose pixel format.");
	}
	//Try to set pixel format
	if( !SetPixelFormat(hDC, pixelFormat, &pfd) ) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGLContext::WCGLContext - Not able to set pixel format.");
		throw WCException("Not ablt to set pixel format.");
	}
	//Now create the context
	if ( !(hRC=wglCreateContext(hDC)) ) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGLContext::WCGLContext - Not able to create GL context.");
		throw WCException("Not ablt to create GL context.");
	}
	//Cast hRC to void* and set _context
	this->_context = hRC;
}


WCGLContext::WCGLContext(const WCGLContext &context) : _context(NULL) {
/*
	//Create structure with pixel format attributes
	NSOpenGLPixelFormatAttribute attrs[] =
	{
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFAColorSize, 24,
		NSOpenGLPFAAlphaSize, 8,
		NSOpenGLPFADepthSize, 24,
		NSOpenGLPFAAccelerated,
		NSOpenGLPFASampleBuffers, 4,
		NSOpenGLPFASamples, 8,
		NSOpenGLPFASampleAlpha,		
		0
	};
	//Create the best possible pixel format
	NSOpenGLPixelFormat* pixFmt = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
	//Do some error checking
	if (pixFmt == nil) {
		//Log an error message
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGLContext::WCGLContext Shared - CGLChoosePixelFormat Error.");
		//Throw exception
		throw WCException();
	}
	//Check to make sure pixel format is necessary level
	//...
	
	//Create the context - making sure to share it!
	this->_context = (void*)[[NSOpenGLContext alloc] initWithFormat:pixFmt shareContext:(NSOpenGLContext*)context._context];
	//Do some error checking
	if (this->_context == nil) {
		//Log an error message
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGLContext::WCGLContext Shared - CGLCreateContext Error.");
		//Throw exception
		throw WCException();
	}
	//Destroy the pixel format
	[pixFmt release];
*/
}


WCGLContext::~WCGLContext() {
	//Delete the bridge
	if (this->_context) {
	}
}


bool WCGLContext::IsActive(void) {
	//Get what the current context is
	HGLRC current = wglGetCurrentContext();
	//Compare it to current
	return (this->_context == current);
}


void WCGLContext::MakeActive(void) {
	//Make sure there are base contexts
	ASSERT(this->_context);
	ASSERT(this->_deviceContext);
	//Just call to the underlying context
	if (this->_context) {
		//Make this context current
		if ( !wglMakeCurrent(this->_deviceContext, this->_context) ) {
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGLContext::MakeActive - Not able to make context current.");
			throw WCException("Not able to make context current");
		}
	}
}


void WCGLContext::FlushBuffer(void) {
	//Make sure there is a base context
	ASSERT(this->_deviceContext);
	//Flush the buffer
	SwapBuffers(this->_deviceContext) ;
}


/***********************************************~***************************************************/

