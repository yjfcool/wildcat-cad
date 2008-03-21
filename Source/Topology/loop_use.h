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


#ifndef __WTPKL_LOOP_USE_H__
#define __WTPKL_LOOP_USE_H__


/*** Included Header Files ***/
#include "wtpkl.h"
#include "face_use.h"
#include "edge_use.h"
#include "vertex_use.h"


/*** Locally Defined Values ***/
//None


/*** Class Order ***/
//---CLoopUse


/*****************************************************************************/


class CLoopUse : public CTopologyUse<void*, CFaceUse*, CLoopUse*> {
private:
	CEdgeUse						*_edgeUses;								//!< Pointers to the downward EdgeUses
	CVertexUse						*_vertexUse;							//!< Pointers to the downward VertexUse
private:
	CLoopUse();																//!< Deny access to default constructor
	CLoopUse(const CLoopUse&);												//!< Deny access to copy constructor
	CLoopUse& operator=(const CLoopUse&);									//!< Deny access to equals operator
public:
	CLoopUse(CFaceUse* parent);												//!< Primary constructor
	~CLoopUse();															//!< Default destructor

	CEdgeUse* GetEdgeUses(void)		{ return this->_edgeUses; }				//!< Return a pointer to the linked list of related EdgeUses
	void SetEdgeUses(CEdgeUse* use);										//!< Set the value of the pointer to the EdgeUses in the loop
	void AddEdgeUse(CEdgeUse* edgeUse, CEdgeUse* ref=NULL);					//!< Add a EdgeUse into the linked list, before REF if given
	int GetEdgeUseCount(void);												//!< Return the number of EdgeUses in the loop
	CVertexUse* GetVertexUse(void)	{ return this->_vertexUse; }			//!< Return a pointer to the single related VertexUse
	void SetVertexUse(CVertexUse* vertexUse);								//!< Set the value of the pointer to the VertexUse in the loop
	void AddVertexUse(CVertexUse* vertexUse);								//!< Add a reference to the single VertexUse
	void Remove(void);														//!< Remove the LoopUse from the parent's linked-list
	void Print(int indent=0);												//!< Print the contents of this loop
};



/*****************************************************************************/


#endif //__WTPKL_LOOP_USE_H__

