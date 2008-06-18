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
#include "region_use.h"


/*****************************************************************************/


CRegionUse::CRegionUse(CModelUse* parent) : 
	CTopologyUse<void*, CModelUse*, CRegionUse*>::CTopologyUse(NULL, parent, this), _shellUses(NULL) {
	CLOGGER_INFO(CTopologyManager::Logger(), "CRegionUse::CRegionUse - Primary constructor");
	//Insert this RegionUse into the parent ModelUse's RegionUse list
	this->_parent->AddRegionUse(this);
}


CRegionUse::~CRegionUse() {
	CLOGGER_INFO(CTopologyManager::Logger(), "(" << this << ")CRegionUse::~CRegionUse - Default destructor");
	this->Remove();
}


void CRegionUse::AddShellUse(CShellUse *shellUse) {
	//Check to see if there are any existing ShellUses
	if (this->_shellUses == NULL) 
		this->_shellUses = shellUse;
	//Otherwise insert this ShellUses into the list of ShellUses
	else {
		this->_shellUses->Insert(shellUse);
	}
}


void CRegionUse::Remove(void) {
	//Check to see if the parent points to this ru as the head of the RegionUse linked-list
	if (this->_parent->GetRegionUses() == this) {
		//If so, check to see if there is another ru in the list
		if (this->_next != this) {
			//If so, set the parent to point to that ru
			this->_parent->SetRegionUses(this->_next);
			//Remove the item from the linked-list of ru's for the parent ModelUse
			this->_next->_previous = this->_previous;
			this->_previous->_next = this->_next;
			//Set the next and previous pointers to itself
			this->_next = this;
			this->_previous = this;
		}
		else
			//Otherwise, set the parent to point to NULL
			this->_parent->SetRegionUses(NULL);
	}
}


void CRegionUse::Print(int indent) {
	for (int i=0; i < indent; i++) cout << "\t";
	cout << "RegionUse(" << this << ", ModelUse-" << this->_parent << ", Next-" << this->_next << ", Prev-" << this->_previous << ")\n";
	//Make sure there are valid shells before trying to print them
	if (this->_shellUses != NULL) {
		//Print the first shell
		this->_shellUses->Print(indent+1);
		CShellUse* sPtr = this->_shellUses->GetNext();
		//Loop through the shells until you return to the start of the list
		while (sPtr != this->_shellUses) {
			//Print the shells info
			sPtr->Print(indent+1);
			//Get the next shell
			sPtr = sPtr->GetNext();
		}
	}
}


/*****************************************************************************/

