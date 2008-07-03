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
#include "Sketcher/sketch_circle.h"
#include "Sketcher/sketch_circle_controller.h"
#include "Sketcher/sketch.h"
#include "Kernel/workbench.h"
#include "Kernel/document.h"


/***********************************************~***************************************************/


WCSketchCircleController::WCSketchCircleController(WCSketchCircle *circle) : ::WCEventController(NULL),
	_circle(circle) { 
	//Nothing else for now
}


WCSketchCircleController::~WCSketchCircleController() {
	//Make sure no longer selected
	this->_circle->Sketch()->Workbench()->SelectionManager()->ForceDeselection(this, false);
}


inline WCObject* WCSketchCircleController::Associate(void) {
	return this->_circle;
}


void WCSketchCircleController::OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) {
//	std::cout << "Circle: " << this->_circle->GetName() << " was selected.\n";
	this->_circle->Sketch()->Document()->Status("Circle " + this->_circle->GetName() + " was selected.");
	//See if in sketcher or in other workbench
	if (this->_circle->Sketch()->Workbench()->IsActive()) {
		//Is this from the selection manager
		if (!fromManager) {
			//Clear selection buffer if appropriate
			if (!this->_circle->Sketch()->Workbench()->IsMultiSelect())
				this->_circle->Sketch()->Workbench()->SelectionManager()->Clear(true);
			//Add this object to the selection manager
			this->_circle->Sketch()->Workbench()->SelectionManager()->ForceSelection(this, false);
		}
		//Tell the object it has been selected
		if (!this->_circle->IsSelected()) this->_circle->OnSelection(false, objects);
		//Go into editing mode if this is only item selected and mouse is still down
		if ((this->_circle->Sketch()->Workbench()->SelectionManager()->Count() == 1) && 
			(this->_circle->Sketch()->Document()->Scene()->MouseState(WCMouseButton::Left()))){
			this->_circle->Sketch()->Workbench()->DrawingMode( WCSketchCircle::ModeEdit(this->_circle) );
		}
	}
	//Otherwise, let the sketch handle it
	else {
		//Tell the sketch
		this->_circle->Sketch()->Controller()->OnSelection(fromManager, objects);
	}
	//Mark the tree element as selected
	this->_circle->TreeElement()->IsSelected(true);
}


void WCSketchCircleController::OnDeselection(const bool fromManager) {
//	std::cout << "Circle: " << this->_circle->GetName() << " was deselected.\n";
	//See if in sketcher or in other workbench
	if (this->_circle->Sketch()->Workbench()->IsActive()) {
		//Remove the item from the selection list, if not from manager
		if (!fromManager) this->_circle->Sketch()->Workbench()->SelectionManager()->ForceDeselection(this, false);
		//Tell the object it has been deselected
		if (this->_circle->IsSelected()) this->_circle->OnDeselection(false);
	}
	//Otherwise, let the sketch handle it
	else {
		//Tell the sketch
		this->_circle->Sketch()->Controller()->OnDeselection(fromManager);
	}
	//Mark the tree element as not selected
	this->_circle->TreeElement()->IsSelected(false);
}


void WCSketchCircleController::OnContextClick(void) {
}


/***********************************************~***************************************************/
