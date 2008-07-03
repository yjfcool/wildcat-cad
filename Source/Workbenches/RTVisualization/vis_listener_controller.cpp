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
#include "RTVisualization/vis_listener_controller.h"
#include "RTVisualization/vis_listener.h"
#include "RTVisualization/visualization.h"
#include "Kernel/workbench.h"
#include "Kernel/document.h"


/***********************************************~***************************************************/


WCVisListenerController::WCVisListenerController(WCVisListener *listener) : ::WCEventController(NULL),
	_listener(listener) { 
	//Nothing else for now
}


WCVisListenerController::~WCVisListenerController() {
	//Make sure no longer selected
	this->_listener->Visualization()->Workbench()->SelectionManager()->ForceDeselection(this, false);
}


inline WCObject* WCVisListenerController::Associate(void) {
	return this->_listener;
}


void WCVisListenerController::OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) {
	this->_listener->Visualization()->Document()->Status("Listener " + this->_listener->GetName() + " was selected.");
	//See if in visualization or in other workbench
	if (this->_listener->Visualization()->Workbench()->IsActive()) {
		//Is this from the selection manager
		if (!fromManager) {
			//Do some things if multi-select is not on
			if (!this->_listener->Visualization()->Workbench()->IsMultiSelect())
				this->_listener->Visualization()->Workbench()->SelectionManager()->Clear(true);
			//Add this object to the selection manager
			this->_listener->Visualization()->Workbench()->SelectionManager()->ForceSelection(this, false);
		}
		//Tell the object it has been selected
		if (!this->_listener->IsSelected()) { this->_listener->OnSelection(false, std::list<WCVisualObject*>());
			std::cout << *this->_listener << std::endl;
		}
	}
	//Otherwise, let the visualization handle it
	else {
		//Tell the visualization
		this->_listener->Visualization()->Controller()->OnSelection(fromManager, objects);
	}
	//Mark the tree element as selected
	this->_listener->TreeElement()->IsSelected(true);
}


void WCVisListenerController::OnDeselection(const bool fromManager) {
	//See if in visualization or in other workbench
	if (this->_listener->Visualization()->Workbench()->IsActive()) {
		//Remove the item from the selection list, if not from manager
		if (!fromManager) this->_listener->Visualization()->Workbench()->SelectionManager()->ForceDeselection(this, false);
		//Tell the object it has been deselected
		if (this->_listener->IsSelected()) this->_listener->OnDeselection(false);
	}
	//Otherwise, let the parent handle it
	else {
		//Tell the parent
		this->_listener->Visualization()->Controller()->OnDeselection(fromManager);
	}
	//Mark the tree element as not selected
	this->_listener->TreeElement()->IsSelected(false);
}


void WCVisListenerController::OnContextClick(void) {
}


/***********************************************~***************************************************/

