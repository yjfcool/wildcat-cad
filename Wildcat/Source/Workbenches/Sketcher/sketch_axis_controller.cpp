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
#include "Sketcher/sketch_axis_controller.h"
#include "Sketcher/sketch_axis.h"
#include "Sketcher/sketch.h"
#include "Kernel/document.h"
#include "PartDesign/part.h"


/***********************************************~***************************************************/


WCSketchAxisController::WCSketchAxisController(WCSketchAxis *axis) : ::WCEventController(NULL),
	_axis(axis) { 
	//Nothing else for now
}


WCSketchAxisController::~WCSketchAxisController() {
	//Make sure no longer selected
	this->_axis->Sketch()->Workbench()->SelectionManager()->ForceDeselection(this, false);
}


inline WCObject* WCSketchAxisController::Associate(void) {
	return this->_axis;
}


void WCSketchAxisController::OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) {
	this->_axis->Sketch()->Document()->Status("Axis " + this->_axis->GetName() + " was selected.");
//	std::cout << "Axis: " << this->_axis->GetName() << " was selected.\n";
	//See if in sketcher or in other workbench
	if (this->_axis->Sketch()->Workbench()->IsActive()) {
		//Is this from the selection manager
		if (!fromManager) {
			//Clear selection buffer if appropriate
			if (!this->_axis->Sketch()->Workbench()->IsMultiSelect())
				this->_axis->Sketch()->Workbench()->SelectionManager()->Clear(true);
		}
		//Tell the axis it has been selected
		if (!this->_axis->IsSelected()) this->_axis->OnSelection(false, objects);
		//Go into editing mode if this is only item selected and mouse is still down
		if ((this->_axis->Sketch()->Workbench()->SelectionManager()->Count() == 1) && 
			(this->_axis->Sketch()->Document()->Scene()->MouseState(WCMouseButton::Left()))){
			this->_axis->Sketch()->Workbench()->DrawingMode( WCSketchAxis::ModeEdit(this->_axis) );
		}
	}
	//Otherwise, let the sketch handle it
	else {
		//Force selection on the sketch
		WCPart *part = dynamic_cast<WCPart*>(this->_axis->Document());
		part->Workbench()->SelectionManager()->ForceSelection(this->_axis->Sketch()->Controller(), true);
	}
	//Tell the axis it has been selected
	if (!this->_axis->IsSelected()) this->_axis->OnSelection(false, objects);
	//Mark the tree element as selected
	this->_axis->TreeElement()->IsSelected(true);
}


void WCSketchAxisController::OnDeselection(const bool fromManager) {
//	std::cout << "Axis: " << this->_axis->GetName() << " was deselected.\n";
	//See if in sketcher or in other workbench
	if (this->_axis->Sketch()->Workbench()->IsActive()) {
		//Remove the item from the selection list, if not from manager
		if (!fromManager) this->_axis->Sketch()->Workbench()->SelectionManager()->ForceDeselection(this, false);
	}
	//Otherwise, let the sketch handle it
	else {
		//Tell the sketch
		this->_axis->Sketch()->Controller()->OnDeselection(fromManager);
	}
	//Tell the axis it has been deselected
	if (this->_axis->IsSelected()) this->_axis->OnDeselection(false);
	//Mark the tree element as not selected
	this->_axis->TreeElement()->IsSelected(false);
}


void WCSketchAxisController::OnContextClick(void) {
}


/***********************************************~***************************************************/
