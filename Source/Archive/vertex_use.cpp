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
#include "vertex_use.h"


/*****************************************************************************/


CVertexUse::CVertexUse(CPoint* pair, CShellUse* parent) :
	CTopologyUse<CPoint*,CEdgeUse*,CVertexUse*>::CTopologyUse(pair, NULL, this), _shellUse(parent), _loopUse(NULL) {
	CLOGGER_INFO(CTopologyManager::Logger(), "CVertexUse::CVertexUse - Shell constructor");
	//Insert this vu in the parent ShellUse's vu list
	this->_shellUse->AddVertexUse(this);
}


CVertexUse::CVertexUse(CPoint* pair, CLoopUse* parent) :
	CTopologyUse<CPoint*,CEdgeUse*,CVertexUse*>::CTopologyUse(pair, NULL, this), _shellUse(NULL), _loopUse(parent) {
	CLOGGER_INFO(CTopologyManager::Logger(), "CVertexUse::CVertexUse - LoopUse constructor");
	//Insert this vu in the parent LoopUse's vu list
	this->_loopUse->AddVertexUse(this);
}


CVertexUse::CVertexUse(CPoint* pair, CEdgeUse* parent) :
	CTopologyUse<CPoint*,CEdgeUse*,CVertexUse*>::CTopologyUse(pair, parent, this), _shellUse(NULL), _loopUse(NULL) {
	CLOGGER_INFO(CTopologyManager::Logger(), "CVertexUse::CVertexUse - EdgeUse constructor");
	//Insert this vu in the parent EdgeUse's vu list
	this->_parent->AddVertexUse(this);
}


CVertexUse::~CVertexUse() {
	CLOGGER_INFO(CTopologyManager::Logger(), "(" << this << ")CVertexUse::CVertexUse - Default destructor");
}


void CVertexUse::Print(int indent) {
	for (int i=0; i < indent; i++) cout << "\t";
	cout << "VertexUse(" << this;
	//Now print out the parent information
	if (this->_shellUse != NULL) {
		cout << ", Parent(ShellUse):" << this->_shellUse;
	}
	else if (this->_loopUse != NULL) {
		cout << ", Parent(LoopUse):" << this->_loopUse;
	}
	else if (this->_parent != NULL) {
		cout << ", Parent(EdgeUse):" << this->_parent;
	}
	cout << ", Next:" << this->_next << ", Previous:" << this->_previous << ")\n";
	//Finally, print out the owner's information
//	this->_pair->Print(indent+1);	
}


/*****************************************************************************/

