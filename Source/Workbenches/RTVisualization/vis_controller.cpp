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
#include "RTVisualization/vis_controller.h"
#include "RTVisualization/visualization.h"


/***********************************************~***************************************************/


WCVisController::WCVisController(WCVisualization *vis) : ::WCEventController(vis->TreeElement()), _visualization(vis) { 
	//Nothing else for now
}


WCObject* WCVisController::Associate(void) {
	return this->_visualization;
}


void WCVisController::OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) {
	//Is this from the selection manager
	if (!fromManager) {
		//Clear selection buffer if appropriate
		if (!this->_visualization->Document()->ActiveWorkbench()->IsMultiSelect())
			this->_visualization->Document()->ActiveWorkbench()->SelectionManager()->Clear(true);
		//Add this object to the selection manager
		this->_visualization->Document()->ActiveWorkbench()->SelectionManager()->ForceSelection(this, false);
	}
	//Tell the object it has been selected
	if (!this->_visualization->IsSelected()) this->_visualization->OnSelection(false, std::list<WCVisualObject*>());
	//Mark the tree element as selected
	this->_visualization->TreeElement()->IsSelected(true);
}


void WCVisController::OnDeselection(const bool fromManager) {
	if (!fromManager) {
		//Remove the item from the selection list
		this->_visualization->Document()->ActiveWorkbench()->SelectionManager()->ForceDeselection(this, false);
	}
	//Tell the object it has been deselected
	if (this->_visualization->IsSelected()) this->_visualization->OnDeselection(false);
	//Mark the tree element as not selected
	this->_visualization->TreeElement()->IsSelected(false);
}


void WCVisController::OnContextClick(void) {
}


/***********************************************~***************************************************/

