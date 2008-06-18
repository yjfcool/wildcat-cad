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
#include "face_use.h"


/*****************************************************************************/


CFaceUse::CFaceUse(CSurface* owner, CShellUse* parent, COrient orientation) :
	CTopologyUse<CSurface*, CShellUse*, CFaceUse*>::CTopologyUse(owner, parent, this),
	_orientation(orientation.GetOrientation()), _loopUses(NULL) {
	CLOGGER_INFO(CTopologyManager::Logger(), "CFaceUse::CFaceUse - ShellUse constructor");
	//Insert this FaceUse in the parent ShellUse's Face list
	this->_parent->AddFaceUse(this);
}


CFaceUse::~CFaceUse() {
	CLOGGER_INFO(CTopologyManager::Logger(), "(" << this << ")CFaceUse::~CFaceUse - Default destructor");
}
	

void CFaceUse::AddLoopUse(CLoopUse* lu) {
	//Otherwise, attach this LoopUse to the FaceUse
	if (this->_loopUses == NULL)
		this->_loopUses = lu;
	else
		this->_loopUses->Insert(lu);
}


void CFaceUse::Remove(void) {
	//Check to see if the parent points to this fu as the head of the ShellUse linked-list
	if (this->_parent->GetFaceUses() == this) {
		//If so, check to see if there is another fu in the list
		if (this->_next != this) {
			//If so, set the parent to point to that fu
			this->_parent->SetFaceUses(this->_next);
			//Remove the item from the linked-list of fu's for the parent ShellUse
			this->_next->_previous = this->_previous;
			this->_previous->_next = this->_next;
			//Set the next and previous pointers to itself
			this->_next = this;
			this->_previous = this;
		}
		else
			//Otherwise, set the parent to point to NULL
			this->_parent->SetFaceUses(NULL);
	}
}


void CFaceUse::Print(int indent) {
	for (int i=0; i < indent; i++) cout << "\t";
	cout << "FaceUse(" << this << ", ShellUse:" << this->_parent << ", Next:" << this->_next;
	cout << ", Prev:" << this->_previous << ", Orient:" << this->_orientation << ")\n";
	//Print out the information about the Face
//	this->_owner->Print(indent+1);
	//Check to see if there are loop sub-elements
	if (this->_loopUses != NULL) {
		this->_loopUses->Print(indent+1);
		CLoopUse* lPtr = this->_loopUses->GetNext();
		while (lPtr != this->_loopUses) {
			lPtr->Print(indent+1);
			lPtr = lPtr->GetNext();
		}
	}
}



/*****************************************************************************/

