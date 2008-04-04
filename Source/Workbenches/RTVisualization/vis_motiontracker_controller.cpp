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
#include "RTVisualization/vis_motiontracker.h"
#include "RTVisualization/vis_motiontracker_controller.h"
#include "RTVisualization/visualization.h"
#include "Kernel/workbench.h"
#include "Kernel/document.h"


/***********************************************~***************************************************/


WCVisMotionTrackerController::WCVisMotionTrackerController(WCVisMotionTracker *tracker) : ::WCEventController(NULL),
	_tracker(tracker) { 
	//Nothing else for now
}


WCVisMotionTrackerController::~WCVisMotionTrackerController() {
	//Make sure no longer selected
	this->_tracker->Visualization()->Workbench()->SelectionManager()->ForceDeselection(this, false);
}


inline WCObject* WCVisMotionTrackerController::Associate(void) {
	return this->_tracker;
}


void WCVisMotionTrackerController::OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) {
	this->_tracker->Visualization()->Document()->Status("Motion Tracker " + this->_tracker->GetName() + " was selected.");
	//See if in visualization or in other workbench
	if (this->_tracker->Visualization()->Workbench()->IsActive()) {
		//Is this from the selection manager
		if (!fromManager) {
			//Do some things if multi-select is not on
			if (!this->_tracker->Visualization()->Workbench()->IsMultiSelect())
				this->_tracker->Visualization()->Workbench()->SelectionManager()->Clear(true);
			//Add this tracker to the selection manager
			this->_tracker->Visualization()->Workbench()->SelectionManager()->ForceSelection(this, false);
		}
		//Tell the tracker it has been selected
		if (!this->_tracker->IsSelected()) this->_tracker->OnSelection(false, std::list<WCVisualObject*>());
	}
	//Otherwise, let the visualization handle it
	else {
		//Tell the visualization
		this->_tracker->Visualization()->Controller()->OnSelection(fromManager, objects);
	}
	//Mark the tree element as selected
	this->_tracker->TreeElement()->IsSelected(true);
}


void WCVisMotionTrackerController::OnDeselection(const bool fromManager) {
	//See if in visualization or in other workbench
	if (this->_tracker->Visualization()->Workbench()->IsActive()) {
		//Remove the item from the selection list, if not from manager
		if (!fromManager) this->_tracker->Visualization()->Workbench()->SelectionManager()->ForceDeselection(this, false);
		//Tell the object it has been deselected
		if (this->_tracker->IsSelected()) this->_tracker->OnDeselection(false);
	}
	//Otherwise, let the parent handle it
	else {
		//Tell the parent
		this->_tracker->Visualization()->Controller()->OnDeselection(fromManager);
	}
	//Mark the tree element as not selected
	this->_tracker->TreeElement()->IsSelected(false);
}


void WCVisMotionTrackerController::OnContextClick(void) {
}


/***********************************************~***************************************************/

