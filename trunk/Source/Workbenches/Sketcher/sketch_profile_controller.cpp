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
#include <Sketcher/sketch_profile_controller.h>
#include <Sketcher/sketch_profile.h>
#include <Sketcher/sketch.h>
#include <Kernel/workbench.h>
#include <Kernel/document.h>


/***********************************************~***************************************************/


WCSketchProfileController::WCSketchProfileController(WCSketchProfile *profile) : ::WCEventController(NULL),
	_profile(profile) { 
	//Nothing else for now
}


WCSketchProfileController::~WCSketchProfileController() {
	//Make sure no longer selected
	this->_profile->Sketch()->Workbench()->SelectionManager()->ForceDeselection(this, false);
}


inline WCObject* WCSketchProfileController::Associate(void) {
	return this->_profile;
}


void WCSketchProfileController::OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) {
	std::cout << "Profile: " << this->_profile->GetName() << " was selected.\n";
	std::cout << *(this->_profile);
	//See if not in sketcher
	if (!this->_profile->Sketch()->Workbench()->IsActive()) {
		//Is this from the selection manager
		if (!fromManager) {
			//Do some things if multi-select is not on
			if (!this->_profile->Sketch()->Document()->ActiveWorkbench()->IsMultiSelect())
				this->_profile->Sketch()->Document()->ActiveWorkbench()->SelectionManager()->Clear(true);
			//Add this profile to the selection manager
			this->_profile->Sketch()->Document()->ActiveWorkbench()->SelectionManager()->ForceSelection(this, false);
			//Tell the profile it has been selected
			if (!this->_profile->IsSelected()) this->_profile->OnSelection(false, objects);
			//Mark the tree element as selected
			this->_profile->TreeElement()->IsSelected(true);
		}
	}
}


void WCSketchProfileController::OnDeselection(const bool fromManager) {
//	std::cout << "Profile: " << this->_profile->GetName() << " was deselected.\n";
	//See if not in sketcher
	if (!this->_profile->Sketch()->Workbench()->IsActive()) {
		//Remove the item from the selection list, if not from manager
		if (!fromManager) this->_profile->Sketch()->Document()->ActiveWorkbench()->SelectionManager()->ForceDeselection(this, false);
		//Tell the profile it has been deselected
		if (this->_profile->IsSelected()) this->_profile->OnDeselection(false);
		//Mark the tree element as not selected
		this->_profile->TreeElement()->IsSelected(false);
	}
}


void WCSketchProfileController::OnContextClick(void) {
}


/***********************************************~***************************************************/
