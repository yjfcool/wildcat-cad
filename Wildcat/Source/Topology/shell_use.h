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


#ifndef __WTPKL_SHELL_USE_H__
#define __WTPKL_SHELL_USE_H__


/*** Included Header Files ***/
#include "wtpkl.h"
#include "region_use.h"
#include "face_use.h"
#include "edge_use.h"
#include "vertex_use.h"


/*** Locally Defined Values ***/
//None


/*** Class Order ***/
//---CShellUse


/*****************************************************************************/


class CShellUse : public CTopologyUse<void*, CRegionUse*, CShellUse*> {
private:
	CFaceUse						*_faceUses;								//!<
	CEdgeUse						*_edgeUse;								//!<
	CVertexUse						*_vertexUse;							//!<
private:
	CShellUse();															//!< Deny access to the default constructor
	CShellUse(const CShellUse&);											//!<
	CShellUse& operator=(const CShellUse&);									//!<
public:
	CShellUse(CRegionUse* parent);											//!<
	~CShellUse();															//!<
	
	CFaceUse* GetFaceUses(void)		{ return this->_faceUses; }				//!<
	void SetFaceUses(CFaceUse* faceUse);
	void AddFaceUse(CFaceUse* faceUse);										//!<
	CEdgeUse *GetEdgeUse(void)		{ return this->_edgeUse; }				//!<
	void SetEdgeUse(CEdgeUse* edgeUse);
	void AddEdgeUse(CEdgeUse* edgeUse);										//!<
	CVertexUse* GetVertexUse(void)	{ return this->_vertexUse; }			//!<
	void SetVertexUse(CVertexUse* vertexUse);
	void AddVertexUse(CVertexUse* vertexUse);								//!<
	void Remove(void);														//!<
	void Print(int indent=0);												//!<
};


/*****************************************************************************/


#endif //__WTPKL_SHELL_USE_H__

