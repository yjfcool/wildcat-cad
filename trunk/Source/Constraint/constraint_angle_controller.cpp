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
#include "Constraint/constraint_angle_controller.h"
#include "Constraint/constraint_angle.h"
#include "Sketcher/sketch.h"
#include "Kernel/document.h"


/***********************************************~***************************************************/


WCConstraintAngleController::WCConstraintAngleController(WCConstraintAngle *constraint) : 
	::WCEventController(NULL), _constraint(constraint) { 
	//Nothing else for now
}


inline WCObject* WCConstraintAngleController::Associate(void) {
	return this->_constraint;
}


void WCConstraintAngleController::OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) {
//	std::cout << "Angle Constraint: " << this->_constraint->GetName() << " was selected.\n";
	this->_constraint->Sketch()->Document()->Status("Constraint " + this->_constraint->GetName() + " was selected.");
	//See if in sketcher or in other workbench
	if (this->_constraint->Sketch()->Workbench()->IsActive()) {
		//Is this from the selection manager
		if (!fromManager) {
			//Do some things if multi-select is not on
			if (!this->_constraint->Sketch()->Workbench()->IsMultiSelect())
				this->_constraint->Sketch()->Workbench()->SelectionManager()->Clear(true);
			//Add this object to the selection manager
			this->_constraint->Sketch()->Workbench()->SelectionManager()->ForceSelection(this, false);
		}
		//Tell the object it has been selected
		if (!this->_constraint->IsSelected()) this->_constraint->OnSelection(false, std::list<WCVisualObject*>());
		//Go into editing mode if this is only item selected and mouse is still down
		if ((this->_constraint->Sketch()->Workbench()->SelectionManager()->Count() == 1) && 
			(this->_constraint->Sketch()->Document()->Scene()->MouseState(WCMouseButton::Left()))) {
			this->_constraint->Sketch()->Workbench()->DrawingMode( WCConstraintAngle::ModeEdit(this->_constraint) );
		}
	}
	//Otherwise, let the sketch handle it
	else {
		//Tell the sketch
		this->_constraint->Sketch()->Controller()->OnSelection(fromManager, objects);
	}
	//Mark the tree element as selected
	this->_constraint->TreeElement()->IsSelected(true);
}


void WCConstraintAngleController::OnDeselection(const bool fromManager) {
//	std::cout << "Angle Constraint: " << this->_constraint->GetName() << " was deselected.\n";
	//See if in sketcher or in other workbench
	if (this->_constraint->Sketch()->Workbench()->IsActive()) {
		//Remove the item from the selection list, if not from manager
		if (!fromManager) this->_constraint->Sketch()->Workbench()->SelectionManager()->ForceDeselection(this, false);
		//Tell the object it has been deselected
		if (this->_constraint->IsSelected()) this->_constraint->OnDeselection(false);
	}
	//Otherwise, let the sketch handle it
	else {
		//Tell the sketch
		this->_constraint->Sketch()->Controller()->OnDeselection(fromManager);
	}
	//Mark the tree element as not selected
	this->_constraint->TreeElement()->IsSelected(false);
}


void WCConstraintAngleController::OnContextClick(void) {
	//Nothing for now
}


/***********************************************~***************************************************/

