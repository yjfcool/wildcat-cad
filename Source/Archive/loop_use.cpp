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
#include "loop_use.h"


/*****************************************************************************/


CLoopUse::CLoopUse(CFaceUse* parent) :
	CTopologyUse<void*, CFaceUse*, CLoopUse*>::CTopologyUse(NULL, parent, this), _edgeUses(NULL), _vertexUse(NULL) {
	CLOGGER_INFO(CTopologyManager::Logger(), "CLoopUse::CLoopUse - Primary constructor");
	//Insert this lu in the parent FaceUses lu list
	this->_parent->AddLoopUse(this);
}


CLoopUse::~CLoopUse() {
	CLOGGER_INFO(CTopologyManager::Logger(), "(" << this << ")CLoopUse::~CLoopUse - Default destructor");
	this->Remove();
}


void CLoopUse::SetEdgeUses(CEdgeUse* eu) {
	//Check to see if there is already an EdgeUse attached, if not, throw an error
	if (this->_edgeUses == NULL) {
		CLOGGER_ERROR(CTopologyManager::Logger(), "CLoopUse::SetFaceUses Error - Already have objects attached to this shell");
		//throw error
		return;
	}
	//Otherwise, set the edgeUses pointer
	this->_edgeUses = eu;
}


int CLoopUse::GetEdgeUseCount(void) {
	CEdgeUse* tmp;
	int i=1;
	
	if (this->_edgeUses == NULL) return 0;
	tmp = this->_edgeUses->GetClockwise();
	while (tmp != this->_edgeUses) {
		tmp = tmp->GetClockwise();
		i++;
	}
	return i;
}

	
void CLoopUse::AddEdgeUse(CEdgeUse* eu, CEdgeUse* ref) {
	//Check to make sure that a VertexUse is not already attached
	if (this->_vertexUse != NULL) {
		CLOGGER_INFO(CTopologyManager::Logger(), "CLoopUse::AddEdgeUse - Removing reference to VertexUse:" << this->_vertexUse);			
		//Remove it from the linked list of VertexUses
		this->_vertexUse->Remove();
		//Remove it from the Vertex itself
//		if (this->_vertexUse->GetOwner()->GetUses() == this->_vertexUse)
//			this->_vertexUse->GetOwner()->ResetUses();
		//Remove the vertexUse from the list
		this->_vertexUse->Remove();
		//Delete the VertexUse
		delete this->_vertexUse;
		//Remove the reference to it
		this->_vertexUse = NULL;
	}
	//Add this EdgeUse into the linked list
	if (this->_edgeUses == NULL) {
		if (ref != NULL) {
			CLOGGER_WARN(CTopologyManager::Logger(), "CLoopUse::AddEdgeUse Warning - Unknown reference passed: " << ref);						
			//throw error
			return;
		}
		this->_edgeUses = eu;
	}
	else {
		if (ref == NULL)
			//If a reference was not given, just insert the new eu before the current head of the linked-list
			this->_edgeUses->Insert(eu);
		else
			//Otherwise, insert the new eu before the ref eu
			ref->Insert(eu);  
	}
}


void CLoopUse::SetVertexUse(CVertexUse* vu) {
	//Check to see if there is already a VertexUse attached, if not, throw an error
	if (this->_vertexUse == NULL) {
		CLOGGER_ERROR(CTopologyManager::Logger(), "CLoopUse::SetVertexUse Error - Already have objects attached to this LoopUse");
		//throw error
		return;
	}
	//Otherwise, set the vertexUse pointer
	this->_vertexUse = vu;
}


void CLoopUse::AddVertexUse(CVertexUse* vu) {
	//If nothing else is already attached, then add this as the one VertexUse
	if ((this->_edgeUses != NULL) || (this->_vertexUse != NULL)) {
		CLOGGER_ERROR(CTopologyManager::Logger(), "CLoopUse::AddVertexUse Error - Already have EdgeUses associated with this LoopUse");			
		//throw error
		return;
	}
	else this->_vertexUse = vu;
}


void CLoopUse::Remove(void) {
	//Check to see if the parent points to this lu as the head of the LoopUse linked-list
	if (this->_parent->GetLoopUses() == this) {
		//If so, check to see if there is another lu in the list
		if (this->_next != this) {
			//If so, set the parent to point to that lu
			this->_parent->SetLoopUses(this->_next);
			//Remove the item from the linked-list of lu's for the parent FaceUse
			this->_next->_previous = this->_previous;
			this->_previous->_next = this->_next;
			//Set the next and previous pointers to itself
			this->_next = this;
			this->_previous = this;
		}
		else
			//Otherwise, set the parent to point to NULL
			this->_parent->SetLoopUses(NULL);
	}
}


void CLoopUse::Print(int indent) {
	for (int i=0; i < indent; i++) cout << "\t";
	cout << "LoopUse(" << this << ", FaceUse:" << this->_parent << ", Next:" << this->_next;
	cout << ", Prev:" << this->_previous << ")\n";
	//Print out the information about the Loop
//	this->_owner->Print(indent+1);
	//Check to see if there are Edge sub-elements
	if (this->_edgeUses != NULL) {
		this->_edgeUses->Print(indent+1);
		CEdgeUse* ePtr = this->_edgeUses->GetClockwise();
		while (ePtr != this->_edgeUses) {
			ePtr->Print(indent+1);
			ePtr = ePtr->GetClockwise();
		}
	}
	//Otherwise see if there is a VertexUse to print
	else if (this->_vertexUse != NULL) {
		this->_vertexUse->Print(indent+1);
	}
}


/*****************************************************************************/

