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


#ifndef __WTPKL_EDGE_USE_H__
#define __WTPKL_EDGE_USE_H__


/*** Included Header Files ***/
#include "wtpkl.h"
#include "shell_use.h"
#include "loop_use.h"
#include "vertex_use.h"


/*** Locally Defined Values ***/
//None


/*** Class Order ***/
//---CEdgeUse


/*****************************************************************************/


class CEdgeUse : public CTopologyUse<CCurve*, CLoopUse*, CEdgeUse*> {
private:
	//Use the inherited _next and _previous as the loopCW and loopCCW pointers
	CEdgeUse							*_radial;							//!< Pointer to the next radial EdgeUse
	int									_orientation;						//!< Orientation of the EdgeUse compared to the geometric definition
	//Use the inherited _parent as the parent for LoopUse
	CShellUse							*_shellUse;							//!< Upward pointer to a ShellUse
	CVertexUse							*_vertexUse;						//!< Downward pointer to a VertexUse
private:
	CEdgeUse();																//!< Deny access to default constructor
	CEdgeUse(const CEdgeUse&);												//!< Deny access to the copy constructor
	CEdgeUse& operator=(const CEdgeUse&);									//!< Deny access to the equals operator
	CEdgeUse* GetParent(void);												//!< Deny access to the inherited GetParent (must use proper type)
public:
	CEdgeUse(CCurve* pair, CShellUse* parent, COrient orientation);			//!< ShellUse constructor
	CEdgeUse(CCurve* pair, CLoopUse* parent, CEdgeUse *nextEdgeUse, COrient orientation);	//!< LoopUse constructor
	~CEdgeUse();															//!< Default destructor
	
	CEdgeUse* GetClockwise(void)		{ return this->_next; }				//!< Get the next clockwise EdgeUse in the LoopUse's linked list
	CEdgeUse* GetCounterClockwise(void)	{ return this->_previous; }			//!< Get the next counter-clockwise EdgeUse in the LoopUse
	CShellUse* GetShellUse(void)		{ return this->_shellUse; }			//!< Return a pointer to the parent Shell
	CLoopUse* GetLoopUse(void)			{ return this->_parent; }			//!< Return a pointer to the parent LoopUse
	CVertexUse* GetVertexUse(void)		{ return this->_vertexUse; }		//!< Get the VertexUse associated with this EdgeUse
	void AddVertexUse(CVertexUse *vertexUse);								//!< Add a VertexUse to this EdgeUse
	int GetOrientation(void)			{ return this->_orientation; }		//!< Get the orientation of the EdgeUse
	CEdgeUse* GetRadial(void)			{ return this->_radial; }			//!< Get the next radial EdgeUse
	void SetRadial(CEdgeUse* use)		{ this->_radial = use; }			//!< Set the next radial EdgeUse
	CEdgeUse* Reinsert(CLoopUse* newLoopUse, CEdgeUse* previousEdge);		//!< Re-associate this EdgeUse with another LoopUse
	void Splice(CEdgeUse* use) {this->_previous = use; use->_next = this; }	//!< Splice the two eu's together in the linked-list
	void Remove(void);														//!< Remove this EdgeUse from the parent loop
	void Print(int indent=0);												//!< Print the contents of this EdgeUse
};



/*****************************************************************************/


#endif //__WTPKL_EDGE_USE_H__

