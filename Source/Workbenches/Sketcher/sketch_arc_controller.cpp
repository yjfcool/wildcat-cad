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
#include "Sketcher/sketch_arc_controller.h"
#include "Sketcher/sketch_arc.h"
#include "Sketcher/sketch.h"
#include "Kernel/workbench.h"
#include "Kernel/document.h"


/***********************************************~***************************************************/


WCSketchArcController::WCSketchArcController(WCSketchArc *arc) : ::WCEventController(NULL),
	_arc(arc) { 
	//Nothing else for now
}


WCSketchArcController::~WCSketchArcController() {
	//Make sure no longer selected
	this->_arc->Sketch()->Workbench()->SelectionManager()->ForceDeselection(this, false);
}


inline WCObject* WCSketchArcController::Associate(void) {
	return this->_arc;
}


void WCSketchArcController::OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) {
//	std::cout << "Arc: " << this->_arc->GetName() << " was selected.\n";
	this->_arc->Document()->Status("Arc " + this->_arc->GetName() + " was selected.");
	//See if in sketcher or in other workbench
	if (this->_arc->Sketch()->Workbench()->IsActive()) {
		//Is this from the selection manager
		if (!fromManager) {
			//Clear selection buffer if appropriate
			if (!this->_arc->Sketch()->Workbench()->IsMultiSelect())
				this->_arc->Sketch()->Workbench()->SelectionManager()->Clear(false);
			//Add this object to the selection manager
			this->_arc->Sketch()->Workbench()->SelectionManager()->ForceSelection(this, false);
		}
		//Tell the object it has been selected
		if (!this->_arc->IsSelected()) this->_arc->OnSelection(false, objects);
		//Go into editing mode if this is only item selected and mouse is still down
		if ((this->_arc->Sketch()->Workbench()->SelectionManager()->Count() == 1) && 
			(this->_arc->Sketch()->Document()->Scene()->MouseState(WCMouseButton::Left()))){
			this->_arc->Sketch()->Workbench()->DrawingMode( WCSketchArc::ModeEdit(this->_arc) );
		}
	}
	//Otherwise, let the sketch handle it
	else {
		//Tell the sketch
		this->_arc->Sketch()->Controller()->OnSelection(fromManager, objects);
	}
	//Mark the tree element as selected
	this->_arc->TreeElement()->IsSelected(true);
}


void WCSketchArcController::OnDeselection(const bool fromManager) {
//	std::cout << "Arc: " << this->_arc->GetName() << " was deselected.\n";
	//See if in sketcher or in other workbench
	if (this->_arc->Sketch()->Workbench()->IsActive()) {
		//Remove the item from the selection list, if not from manager
		if (!fromManager) this->_arc->Sketch()->Workbench()->SelectionManager()->ForceDeselection(this, false);
		//Tell the line it has been deselected
		if (this->_arc->IsSelected()) this->_arc->OnDeselection(false);
	}
	//Otherwise, let the sketch handle it
	else {
		//Tell the sketch
		this->_arc->Sketch()->Controller()->OnDeselection(fromManager);
	}
	//Mark the tree element as not selected
	this->_arc->TreeElement()->IsSelected(false);
}


void WCSketchArcController::OnContextClick(void) {
}


/***********************************************~***************************************************/
