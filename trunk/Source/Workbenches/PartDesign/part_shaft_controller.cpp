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
#include <PartDesign/part_shaft_controller.h>
#include <PartDesign/part_shaft.h>
#include <Kernel/document.h>
#include <Kernel/workbench.h>


/***********************************************~***************************************************/


WCPartShaftController::WCPartShaftController(WCPartShaft *shaft) : ::WCEventController(shaft->TreeElement()), _shaft(shaft) {
	//Make sure reference is not null
	if (this->_shaft == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCPartShaftController::WCPartShaftController - NULL shaft passed.\n");
		//throw error
		return;
	}
}


inline WCObject* WCPartShaftController::Associate(void) {
	return this->_shaft;
}


void WCPartShaftController::OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) {
	this->_shaft->Document()->Status("Shaft " + this->_shaft->GetName() + " was selected");
//	std::cout << "Shaft: " << this->_shaft->GetName() << "(" << this << ") was selected.\n";
	//Is this from the selection manager
	if (!fromManager) {
		//Clear selection buffer if appropriate
		if (!this->_shaft->Document()->ActiveWorkbench()->IsMultiSelect())
			this->_shaft->Document()->ActiveWorkbench()->SelectionManager()->Clear(true);
		//Add this object to the selection manager
		this->_shaft->Document()->ActiveWorkbench()->SelectionManager()->ForceSelection(this, false);
	}
	//Tell the object it has been selected
	if (!this->_shaft->IsSelected()) this->_shaft->OnSelection(false, std::list<WCVisualObject*>());
	//Mark the tree element as selected
	this->_shaft->TreeElement()->IsSelected(true);
}


void WCPartShaftController::OnDeselection(const bool fromManager) {
//	std::cout << "Shaft: " << this->_shaft->GetName() << " was deselected.\n";
	if (!fromManager) {
		//Remove the item from the selection list
		this->_shaft->Document()->ActiveWorkbench()->SelectionManager()->ForceDeselection(this, false);
	}
	//Tell the object it has been deselected
	if (this->_shaft->IsSelected()) this->_shaft->OnDeselection(false);
	//Mark the tree element as not selected
	this->_shaft->TreeElement()->IsSelected(false);
}


void WCPartShaftController::OnContextClick(void) {
}


/***********************************************~***************************************************/

