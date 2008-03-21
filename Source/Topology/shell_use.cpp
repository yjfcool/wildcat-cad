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
#include "shell_use.h"


/*****************************************************************************/


CShellUse::CShellUse(CRegionUse* parent) : 
	CTopologyUse<void*, CRegionUse*, CShellUse*>::CTopologyUse(NULL, parent, this), _faceUses(NULL), _edgeUse(NULL), _vertexUse(NULL) {
	CLOGGER_INFO(CTopologyManager::Logger(), "CShellUse::CShellUse - Primary constructor");
	//Insert this ShellUse into the parent RegionUse's ShellUse list
	this->_parent->AddShellUse(this);
}


CShellUse::~CShellUse() {
	CLOGGER_INFO(CTopologyManager::Logger(), "(" << this << ")CShellUse::~CShellUse - Default destructor");
	this->Remove();
}


void CShellUse::SetFaceUses(CFaceUse* fu) {
	//Check to see if there is already a FaceUse attached, if not, throw an error
	if (this->_faceUses == NULL) {
		CLOGGER_ERROR(CTopologyManager::Logger(), "CShellUse::SetFaceUses Error - Already have objects attached to this shell");
		//throw error
		return;
	}
	//Otherwise, set the faceUses pointer
	this->_faceUses = fu;
}


void CShellUse::AddFaceUse(CFaceUse* fu) {
	//Check to make sure that there aren't any edges or vertices already attached to this shell
	if ((this->_edgeUse != NULL) || (this->_vertexUse != NULL)) {
		CLOGGER_ERROR(CTopologyManager::Logger(), "CShellUse::AddFaceUse Error - Already have objects attached to this shell");
		//throw error
		return;
	}
	//Otherwise, attach this faceuse to the shell
	if (this->_faceUses == NULL) {
		this->_faceUses = fu;
	}
	else {
		this->_faceUses->Insert(fu);
	}
}


void CShellUse::SetEdgeUse(CEdgeUse* eu) {
	//Check to see if there is already an EdgeUse attached, if not, throw an error
	if (this->_edgeUse == NULL) {
		CLOGGER_ERROR(CTopologyManager::Logger(), "CShellUse::SetEdgeUse Error - Already have objects attached to this shell");
		//throw error
		return;
	}
	//Otherwise, set the edgeUse pointer
	this->_edgeUse = eu;
}


void CShellUse::AddEdgeUse(CEdgeUse* eu) {
	//Check to make sure that there aren't any objects already attached to this shell
	if ((this->_faceUses != NULL) || (this->_edgeUse != NULL) || (this->_vertexUse == NULL)) {
		CLOGGER_ERROR(CTopologyManager::Logger(), "CShellUse::AddEdgeUse Error - Already have objects attached to this shell");
		//throw error
		return;
	}
	//Otherwise, attach this edgeuse to the shell
	this->_edgeUse = eu;
}


void CShellUse::SetVertexUse(CVertexUse *vu) {
	//Check to see if there is already an VertexUse attached, if not, throw an error
	if (this->_vertexUse == NULL) {
		CLOGGER_ERROR(CTopologyManager::Logger(), "CShellUse::SetVertexUse Error - Already have objects attached to this shell");
		//throw error
		return;
	}
	//Otherwise, set the vertexUse pointer
	this->_vertexUse = vu;
}


void CShellUse::AddVertexUse(CVertexUse* vu) {
	//Check to make sure that there aren't any objects already attached to this shell
	if ((this->_faceUses != NULL) || (this->_edgeUse != NULL) || (this->_vertexUse != NULL)) {
		CLOGGER_ERROR(CTopologyManager::Logger(), "CShellUse::AddVertexUse Error - Already have objects attached to this shell");
		//throw error
		return;
	}
	//Otherwise, attach this vertexuse to the shell
	this->_vertexUse = vu;
}


void CShellUse::Remove(void) {
	//Check to see if the parent points to this ru as the head of the RegionUse linked-list
	if (this->_parent->GetShellUses() == this) {
		//If so, check to see if there is another ru in the list
		if (this->_next != this) {
			//If so, set the parent to point to that ru
			this->_parent->SetShellUses(this->_next);
			//Remove the item from the linked-list of ru's for the parent ModelUse
			this->_next->_previous = this->_previous;
			this->_previous->_next = this->_next;
			//Set the next and previous pointers to itself
			this->_next = this;
			this->_previous = this;
		}
		else
			//Otherwise, set the parent to point to NULL
			this->_parent->SetShellUses(NULL);
	}
}


void CShellUse::Print(int indent) {
	for (int i=0; i < indent; i++) cout << "\t";
	cout << "ShellUse(" << this << ", RegionUse-" << this->_parent << ", Next-" << this->_next << ", Prev-" << this->_previous << ")\n";
	//Print out the owner's information
//	this->_owner->Print(indent+1);
	//Check to see if there are face sub-elements
	if (this->_faceUses != NULL) {
		this->_faceUses->Print(indent+1);
		CFaceUse* fPtr = this->_faceUses->GetNext();
		while (fPtr != this->_faceUses) {
			fPtr->Print(indent+1);
			fPtr = fPtr->GetNext();
		}
	}
	//Otherwise, check to see if there are edge sub-elements
	else if (this->_edgeUse != NULL) {
		this->_edgeUse->Print(indent+1);
	}
	//Finally, check to see if there are vertex sub-elements
	else if (this->_vertexUse != NULL) {
		this->_vertexUse->Print(indent+1);
	}
}


/*****************************************************************************/
