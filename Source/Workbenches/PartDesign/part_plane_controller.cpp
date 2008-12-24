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
#include <PartDesign/part_plane_controller.h>
#include <PartDesign/part_plane.h>
#include <Kernel/document.h>
#include <Kernel/workbench.h>


/***********************************************~***************************************************/


WCPartPlaneController::WCPartPlaneController(WCPartPlane *plane) : ::WCEventController(plane->TreeElement()), _plane(plane) {
	//Make sure plane is not null
	if (this->_plane == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCPartPlaneController::WCPartPlaneController - NULL plane passed.\n");
		//throw error
		return;
	}
}


inline WCObject* WCPartPlaneController::Associate(void) {
	return this->_plane;
}


void WCPartPlaneController::OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) {
	this->_plane->Document()->Status("Plane " + this->_plane->GetName() + " was selected");
	//Is this from the selection manager
	if (!fromManager) {
		//Clear selection buffer if appropriate
		if (!this->_plane->Document()->ActiveWorkbench()->IsMultiSelect())
			this->_plane->Document()->ActiveWorkbench()->SelectionManager()->Clear(true);
		//Add this plane to the selection manager
		this->_plane->Document()->ActiveWorkbench()->SelectionManager()->ForceSelection(this, false);
	}
	//Tell the plane it has been selected
	if (!this->_plane->IsSelected()) this->_plane->OnSelection(false, std::list<WCVisualObject*>());
	//Mark the tree element as selected
	this->_plane->TreeElement()->IsSelected(true);
}


void WCPartPlaneController::OnDeselection(const bool fromManager) {
//	std::cout << "Plane: " << this->_plane->GetName() << " was deselected.\n";
	if (!fromManager) {
		//Remove the item from the selection list
		this->_plane->Document()->ActiveWorkbench()->SelectionManager()->ForceDeselection(this, false);
	}
	//Tell the plane it has been deselected
	if (this->_plane->IsSelected()) this->_plane->OnDeselection(false);
	//Mark the tree element as not selected
	this->_plane->TreeElement()->IsSelected(false);
}


void WCPartPlaneController::OnContextClick(void) {
}


/***********************************************~***************************************************/

