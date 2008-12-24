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
#include <Sketcher/sketch_conic_controller.h>
#include <Sketcher/sketch_conic_twopoint.h>
#include <Sketcher/sketch.h>
#include <Kernel/workbench.h>
#include <Kernel/document.h>


/***********************************************~***************************************************/


WCSketchConicController::WCSketchConicController(WCSketchConicTwoPoint *conic) : ::WCEventController(NULL),
	_conic(conic) { 
	//Nothing else for now
}


WCSketchConicController::~WCSketchConicController() {
	//Make sure no longer selected
	this->_conic->Sketch()->Workbench()->SelectionManager()->ForceDeselection(this, false);
}


inline WCObject* WCSketchConicController::Associate(void) {
	return this->_conic;
}


void WCSketchConicController::OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) {
//	std::cout << "Conic: " << this->_conic->GetName() << " was selected.\n";
	this->_conic->Sketch()->Document()->Status("Conic " + this->_conic->GetName() + " was selected.");
	//See if in sketcher or in other workbench
	if (this->_conic->Sketch()->Workbench()->IsActive()) {
		//Is this from the selection manager
		if (!fromManager) {
			//Clear selection buffer if appropriate
			if (!this->_conic->Sketch()->Workbench()->IsMultiSelect())
				this->_conic->Sketch()->Workbench()->SelectionManager()->Clear(true);
			//Add this object to the selection manager
			this->_conic->Sketch()->Workbench()->SelectionManager()->ForceSelection(this, false);
		}
		//Tell the object it has been selected
		if (!this->_conic->IsSelected()) this->_conic->OnSelection(false, objects);
		//Go into editing mode if this is only item selected and mouse is still down
		if ((this->_conic->Sketch()->Workbench()->SelectionManager()->Count() == 1) && 
			(this->_conic->Sketch()->Document()->Scene()->MouseState(WCMouseButton::Left()))){
//			this->_conic->Sketch()->Workbench()->DrawingMode( WCSketchConic::ModeEdit(this->_circle) );
		}
	}
	//Otherwise, let the sketch handle it
	else {
		//Tell the sketch
		this->_conic->Sketch()->Controller()->OnSelection(fromManager, objects);
	}
	//Mark the tree element as selected
	this->_conic->TreeElement()->IsSelected(true);
}


void WCSketchConicController::OnDeselection(const bool fromManager) {
//	std::cout << "Conic: " << this->_conic->GetName() << " was deselected.\n";
	//See if in sketcher or in other workbench
	if (this->_conic->Sketch()->Workbench()->IsActive()) {
		//Remove the item from the selection list, if not from manager
		if (!fromManager) this->_conic->Sketch()->Workbench()->SelectionManager()->ForceDeselection(this, false);
		//Tell the object it has been deselected
		if (this->_conic->IsSelected()) this->_conic->OnDeselection(false);
	}
	//Otherwise, let the sketch handle it
	else {
		//Tell the sketch
		this->_conic->Sketch()->Controller()->OnDeselection(fromManager);
	}
	//Mark the tree element as not selected
	this->_conic->TreeElement()->IsSelected(false);
}


void WCSketchConicController::OnContextClick(void) {
}


/***********************************************~***************************************************/
