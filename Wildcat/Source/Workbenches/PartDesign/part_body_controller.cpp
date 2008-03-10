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
#include "PartDesign/part_body_controller.h"
#include "PartDesign/part_body.h"
#include "PartDesign/part.h"
#include "Kernel/document.h"
#include "Kernel/workbench.h"


/***********************************************~***************************************************/


WCPartBodyController::WCPartBodyController(WCPartBody *body) : ::WCEventController(body->TreeElement()),
	_body(body) { 
	//Make sure body is not null
	if (body == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCPartBodyController::WCPartBodyController - NULL body passed.");
		//throw error
		return;
	}
}


inline WCObject* WCPartBodyController::Associate(void) {
	return this->_body;
}


void WCPartBodyController::OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) {
	this->_body->Document()->Status("Body " + this->_body->GetName() + " was selected");
	//Is this from the selection manager
	if (!fromManager) {
		//See if we need to clear the selection list
		if (!this->_body->Document()->ActiveWorkbench()->IsMultiSelect()) {
			this->_body->Document()->ActiveWorkbench()->SelectionManager()->Clear();
		}
		//Add this body to the selection manager
		this->_body->Document()->ActiveWorkbench()->SelectionManager()->ForceSelection(this, false);
	}
	//Tell the body it has been selected
//	if (!this->_body->IsSelected()) this->_body->OnSelection(false, std::list<WCVisualObject*>());
	//Mark the tree element as selected
	this->_body->TreeElement()->IsSelected(true);
	//Make this body the part default body (if document type is part)
	WCPart *part = dynamic_cast<WCPart*>(this->_body->Document());
	if (part != NULL) part->Body(this->_body);
}


void WCPartBodyController::OnDeselection(const bool fromManager) {
//	std::cout << "Body: " << this->_body->Name() << "(" << this->_body << ") was deselected.\n";
	if (!fromManager) {
		//Remove the item from the selection list
		this->_body->Document()->ActiveWorkbench()->SelectionManager()->ForceDeselection(this, false);
	}
	//Tell the body it has been deselected
//	if (this->_body->IsSelected()) this->_body->OnDeselection(false);
	//Mark the tree element as not selected
	this->_body->TreeElement()->IsSelected(false);
}


void WCPartBodyController::OnContextClick(void) {
}


/***********************************************~***************************************************/

