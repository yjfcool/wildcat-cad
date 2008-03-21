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


#ifndef __TOPOLOGY_USE_H__
#define __TOPOLOGY_USE_H__


/*** Included Wildcat Header Files ***/
#include "Topology/stpkl.h"


/*** Locally Defined Values ***/
//None


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


class WCTopologyUse : public WCSerializeableObject {
public:
//	CTopologyUse(G pair, P parent, T use) : _pair(pair), _parent(parent), _next(use), _previous(use) { }
	virtual ~WCTopologyUse()					{ }													//!< Default destructor
	
	//Required Virtual Methods
	virtual void* Geometry(void)=0;																	//!<
	virtual void* Parent(void)=0;
	virtual void* Pair(void)=0;

//	  GetPair(void)				{ return this->_pair; }
//	P GetParent(void)			{ return this->_parent; }
//	T GetNext(void)				{ return this->_next; }
//	T GetPrevious(void)			{ return this->_previous; }
//	void Insert(T use)			{ use->_previous = this->_previous;				//Insert this Use before the passed Use
//								  use->_next = (T)this;
//								  this->_previous->_next = use;
//								  this->_previous = use; }
//	void Remove(void)			{ if (this->_next != NULL)						//Remove the use from the list
//									this->_next->_previous = this->_previous;
//								  if (this->_previous != NULL)
//									this->_previous->_next = this->_next;
//								  this->_next = (T)this;
//								  this->_previous = (T)this; }
//	void Print(int indent=0)	{ }
};


/***********************************************~***************************************************/


#endif //__TOPOLOGY_USE_H__

