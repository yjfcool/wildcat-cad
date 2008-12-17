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


#ifndef __GL_CONTEXT_H__
#define __GL_CONTEXT_H__


/*** Included Header Files ***/
#include "Utility/wutil.h"


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {
	
	
/*** Class Predefines ***/
//None
	
	
/***********************************************~***************************************************/
	
	
class WCGLContext {
private:
	void*										_context;											//!< GL Context base object
	//Hidden Constructors
	WCGLContext& operator=(const WCGLContext &context);												//!< Deny access to context equation
public:
	//Constructors and Destructors
	WCGLContext();																					//!< Default constructor
	WCGLContext(const WCGLContext &context);														//!< Share the context
	~WCGLContext();																					//!< Default destructor

	//Methods
	bool IsActive(void);																			//!< See if context is active
	void MakeActive(void);																			//!< Make the context active
	void FlushBuffer(void);																			//!< Flush the buffers
	inline void* Context(void)					{ return this->_context; }							//!< Get the base context
};


/***********************************************~***************************************************/

	
}	   // End Wildcat Namespace
#endif //__GL_CONTEXT_H__

