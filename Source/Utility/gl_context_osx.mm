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
#import <AppKit/NSOpenGL.h>


/***********************************************~***************************************************/


WCGLContext::WCGLContext() : _context(NULL) {
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
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGLContext::WCGLContext - CGLChoosePixelFormat Error.");
		//Throw exception
		throw WCException();
	}
	//Check to make sure pixel format is necessary level
	//...

	//Create the context
	this->_context = (void*)[[NSOpenGLContext alloc] initWithFormat:pixFmt shareContext:Nil];
	//Do some error checking
	if (this->_context == nil) {
		//Log an error message
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGLContext::WCGLContext - CGLCreateContext Error.");
		//Throw exception
		throw WCException();
	}
	//Destroy the pixel format
	[pixFmt release];
}


WCGLContext::WCGLContext(const WCGLContext &context) : _context(NULL) {
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
}


WCGLContext::~WCGLContext() {
	//Delete the bridge
	if (this->_context) [(NSOpenGLContext*)this->_context release];
}


bool WCGLContext::IsActive(void) {
	//Get what the current context is
	void* current = (void*)[NSOpenGLContext currentContext];
	//Compare it to current
	return (this->_context == current);
}


void WCGLContext::MakeActive(void) {
	//Just call to the underlying context
	if (this->_context) {
		//Cast to the proper type
		NSOpenGLContext *context = (NSOpenGLContext*)this->_context;
		//Make this context current
		[context makeCurrentContext];
	}
}


void WCGLContext::FlushBuffer(void) {
	//Make sure there is a base context
	ASSERT(this->_context);
	//Cast to the proper type
	NSOpenGLContext *context = (NSOpenGLContext*)this->_context;
	//Flush the buffer
	[context flushBuffer];
}


/***********************************************~***************************************************/

