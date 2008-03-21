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


#ifndef __WTPKL_VERTEX_USE_H__
#define __WTPKL_VERTEX_USE_H__


/*** Included Header Files ***/
#include "wtpkl.h"
#include "shell_use.h"
#include "loop_use.h"
#include "edge_use.h"


/*** Locally Defined Values ***/
//None


/*** Class Order ***/
//---CVertexUse


/*****************************************************************************/


class CVertexUse : public CTopologyUse<CPoint*, CEdgeUse*, CVertexUse*> {
private:
	//Use inherited _parent as holder for _edgeUse parent pointer
	CShellUse						*_shellUse;								//!< Upward pointer to a ShellUse
	CLoopUse						*_loopUse;								//!< Upward pointer to a LoopUse
private:
	CVertexUse();															//!< Deny access to the default constructor
	CVertexUse(const CVertexUse&);											//!< Deny access to the copy constructor
	CVertexUse& operator=(const CVertexUse&);								//!< Deny access to the equals operator
	CEdgeUse* GetParent(void);												//!< Deny access to the inherited GetParent function (must use type)
public:
	CVertexUse(CPoint* pair, CShellUse* parent);							//!< ShellUse parent constructor
	CVertexUse(CPoint* pair, CLoopUse* parent);								//!< LoopUse parent constructor
	CVertexUse(CPoint* pair, CEdgeUse* parent);								//!< EdgeUse parent constructor 
	~CVertexUse();															//!< Default destructor
	
	CShellUse* GetShellUse(void)	{ return this->_shellUse; }				//!< Get the parent ShellUse
	CLoopUse* GetLoopUse(void)		{ return this->_loopUse; }				//!< Get the parent LoopUse
	CEdgeUse* GetEdgeUse(void)		{ return this->_parent; }				//!< Get the parent EdgeUse
	
	void Print(int indent=0);												//!< Print the contents of the VertexUse
};


/*****************************************************************************/


#endif //__WTPKL_VERTEX_USE_H__

