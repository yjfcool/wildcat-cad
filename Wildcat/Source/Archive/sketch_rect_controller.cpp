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
#include "Sketcher/sketch_rect.h"
#include "Sketcher/sketch_rect_controller.h"
#include "Sketcher/sketch.h"
#include "Kernel/workbench.h"
#include "Kernel/document.h"


/***********************************************~***************************************************/


WCSketchRectangleController::WCSketchRectangleController(WCSketchRectangle *rect) : ::WCEventController(NULL),
	_rect(rect) { 
	//Nothing else for now
}


inline WCObject* WCSketchRectangleController::Associate(void) {
	return this->_rect;
}


void WCSketchRectangleController::OnSelection(const bool fromManager) {
	std::cout << "Rectangle: " << this->_rect->Name() << " was selected.\n";
	//See if in sketcher or in other workbench
	if (this->_rect->Sketch()->Workbench()->IsActive()) {
		//Is this from the selection manager
		if (!fromManager) {
			//Clear selection buffer if appropriate
			if (!this->_rect->Sketch()->Document()->ActiveWorkbench()->IsMultiSelect())
				this->_rect->Sketch()->Document()->ActiveWorkbench()->SelectionManager()->Clear(true);
			//Add this rectangle to the selection manager
			this->_rect->Sketch()->Document()->ActiveWorkbench()->SelectionManager()->ForceSelection(this, false);
		}
		//Tell the rectangle it has been selected
		if (!this->_rect->IsSelected()) this->_rect->OnSelection(false);
	}
	//Otherwise, let the sketch handle it
	else {
		//Tell the sketch
		this->_rect->Sketch()->Controller()->OnSelection(fromManager);
	}
	//Mark the tree element as selected
	this->_rect->TreeElement()->IsSelected(true);
}


void WCSketchRectangleController::OnDeselection(const bool fromManager) {
	std::cout << "Rectangle: " << this->_rect->Name() << " was deselected.\n";
	//See if in sketcher or in other workbench
	if (this->_rect->Sketch()->Workbench()->IsActive()) {
		//Remove the item from the selection list
		this->_rect->Sketch()->Document()->ActiveWorkbench()->SelectionManager()->ForceDeselection(this, false);
		//Tell the rectangle it has been deselected
		if (this->_rect->IsSelected()) this->_rect->OnDeselection(false);
	}
	//Otherwise, let the sketch handle it
	else {
		//Tell the sketch
		this->_rect->Sketch()->Controller()->OnDeselection(fromManager);
	}
	//Mark the tree element as not selected
	this->_rect->TreeElement()->IsSelected(false);
}


void WCSketchRectangleController::OnContextClick(void) {
}


/***********************************************~***************************************************/
