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
#include "edge_use.h"


/*****************************************************************************/


CEdgeUse::CEdgeUse(CCurve* pair, CShellUse* parent, COrient orientation) : 
	CTopologyUse<CCurve*,CLoopUse*,CEdgeUse*>::CTopologyUse(pair, NULL, this), 
	_radial(NULL), _orientation(orientation.GetOrientation()), _shellUse(parent), _vertexUse(NULL) {
	CLOGGER_INFO(CTopologyManager::Logger(), "CEdgeUse::CEdgeUse - ShellUse constructor");
	//Insert this EdgeUse in the parent ShellUse's EdgeUse list
	if (this->_shellUse == NULL) {
		CLOGGER_ERROR(CTopologyManager::Logger(), "CEdgeUse::CEdgeUse Error - Invalid ShellUse passed");	
		//throw error
		return;
	}
	this->_shellUse->AddEdgeUse(this);
}


CEdgeUse::CEdgeUse(CCurve* pair, CLoopUse* parent, CEdgeUse* eu, COrient orientation) :
	CTopologyUse<CCurve*,CLoopUse*,CEdgeUse*>::CTopologyUse(pair, parent, this),
	_radial(NULL), _orientation(orientation.GetOrientation()), _shellUse(NULL), _vertexUse(NULL) {
	CLOGGER_INFO(CTopologyManager::Logger(), "CEdgeUse::CEdgeUse - LoopUse constructor");
	//Insert this EdgeUse in the parent LoopUse's EdgeUse list
	if (this->_parent == NULL) {
		CLOGGER_ERROR(CTopologyManager::Logger(), "CEdgeUse::CEdgeUse Error - Invalid LoopUse passed");	
		//throw error
		return;
	}
	this->_parent->AddEdgeUse(this, eu);
}


CEdgeUse::~CEdgeUse() {
	CLOGGER_INFO(CTopologyManager::Logger(), "(" << this << ")CEdgeUse::~CEdgeUse - Default destructor");
	this->Remove();
}


void CEdgeUse::AddVertexUse(CVertexUse *vu) {
	if (this->_vertexUse != NULL) {
		CLOGGER_ERROR(CTopologyManager::Logger(), "CEdgeUse::AddVertexUse Error - Already have a VertexUse for this EdgeUse");
		//throw error
		return;
	}
	this->_vertexUse = vu;
}


CEdgeUse* CEdgeUse::Reinsert(CLoopUse* newLoopUse, CEdgeUse* eu) {
	CEdgeUse* tmpEU = this->_next;	//We return the next EdgeUse in the linked-list
	
	//Do some basic error checking
	if (newLoopUse == NULL) {
		CLOGGER_ERROR(CTopologyManager::Logger(), "CEdgeUse::Reinsert Error - Must pass valid LoopUse");		
		//throw error
		return NULL;
	}
	//Add this eu to the newLoop
	this->_parent = newLoopUse;
	//If there is no passed eu, then set the eu to point to itself
	if ((eu == this) || (eu == NULL)) {
		this->_previous = this;
		this->_next = this;
	}
	//Otherwise, add it in to the linked-list eu points to
	else {
		//Now insert this eu after the passed eu
		this->_previous = eu;
		this->_next = eu->_next;
		eu->_next->_previous = this;
		eu->_next = this;
		//Return the next eu in the linked-list
	}
	return tmpEU;
}


void CEdgeUse::Remove(void) {
	//Check to see if the parent points to this eu as the head of the EdgeUse linked-list
	if (this->_parent->GetEdgeUses() == this) {
		//If so, check to see if there is another eu in the list
		if (this->_next != this) {
			//If so, set the parent to point to that eu
			this->_parent->SetEdgeUses(this->_next);
			//Remove the item from the linked-list of eu's for the parent LoopUse
			this->_next->_previous = this->_previous;
			this->_previous->_next = this->_next;
			//Set the next and previous pointers to itself
			this->_next = this;
			this->_previous = this;
		}
		else
			//Otherwise, set the parent to point to NULL
			this->_parent->SetEdgeUses(NULL);
	}
}


void CEdgeUse::Print(int indent) {
	for (int i=0; i < indent; i++) cout << "\t";
	cout << "EdgeUse(" << this;
	if (this->_shellUse != NULL) {
		cout << ", Parent(ShellUse):" << this->_shellUse;
	}
	else if (this->_parent != NULL) {
		cout << ", Parent(LoopUse):" << this->_parent;
	}
	cout << ", CCW:" << this->_previous << ", CW:" << this->_next << ", Orient:" << this->_orientation << ", Radial:" << this->_radial << ")\n";
	//Print out the owner's information
//	this->_owner->Print(indent+1);
	//Check to see if there is a VertexUse sub-element
	if (this->_vertexUse != NULL) {
		this->_vertexUse->Print(indent+1);
	}
}


/*****************************************************************************/

