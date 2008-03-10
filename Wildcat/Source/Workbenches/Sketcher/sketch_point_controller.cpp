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
#include "Sketcher/sketch_point.h"
#include "Sketcher/sketch_point_controller.h"
#include "Sketcher/sketch_point_modes.h"
#include "Sketcher/sketch.h"
#include "Kernel/workbench.h"
#include "Kernel/document.h"


/***********************************************~***************************************************/


WCSketchPointController::WCSketchPointController(WCSketchPoint *point) : ::WCEventController(NULL),
	_point(point) { 
	//Nothing else for now
}


WCSketchPointController::~WCSketchPointController() {
	//Make sure no longer selected
	this->_point->Sketch()->Workbench()->SelectionManager()->ForceDeselection(this, false);
}


inline WCObject* WCSketchPointController::Associate(void) {
	return this->_point;
}


void WCSketchPointController::OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) {
	this->_point->Sketch()->Document()->Status("Point " + this->_point->GetName() + " was selected.");
//	std::cout << "Point: " << this->_point->Name() << " was selected.\n";
	//See if in sketcher or in other workbench
	if (this->_point->Sketch()->Workbench()->IsActive()) {
		//Is this from the selection manager
		if (!fromManager) {
			//Do some things if multi-select is not on
			if (!this->_point->Sketch()->Workbench()->IsMultiSelect())
				this->_point->Sketch()->Workbench()->SelectionManager()->Clear(true);
			//Add this point to the selection manager
			this->_point->Sketch()->Workbench()->SelectionManager()->ForceSelection(this, false);
		}
		//Tell the point it has been selected
		if (!this->_point->IsSelected()) this->_point->OnSelection(false, std::list<WCVisualObject*>());
		//Go into editing mode if this is only item selected and mouse is still down
		if ((this->_point->Sketch()->Workbench()->SelectionManager()->Count() == 1) && 
			(this->_point->Sketch()->Document()->Scene()->MouseState(WCMouseButton::Left()))){
			this->_point->Sketch()->Workbench()->DrawingMode( WCSketchPoint::ModeEdit(this->_point) );
		}
	}
	//Otherwise, let the sketch handle it
	else {
		//Tell the sketch
		this->_point->Sketch()->Controller()->OnSelection(fromManager, objects);
	}
	//Mark the tree element as selected
	this->_point->TreeElement()->IsSelected(true);
}


void WCSketchPointController::OnDeselection(const bool fromManager) {
//	std::cout << "Point: " << this->_point->Name() << " was deselected.\n";
	//See if in sketcher or in other workbench
	if (this->_point->Sketch()->Workbench()->IsActive()) {
		//Remove the item from the selection list, if not from manager
		if (!fromManager) this->_point->Sketch()->Workbench()->SelectionManager()->ForceDeselection(this, false);
		//Tell the point it has been deselected
		if (this->_point->IsSelected()) this->_point->OnDeselection(false);
	}
	//Otherwise, let the sketch handle it
	else {
		//Tell the sketch
		this->_point->Sketch()->Controller()->OnDeselection(fromManager);
	}
	//Mark the tree element as not selected
	this->_point->TreeElement()->IsSelected(false);
}


void WCSketchPointController::OnContextClick(void) {
}


/***********************************************~***************************************************/
