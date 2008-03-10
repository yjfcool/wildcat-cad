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
#include "Sketcher/sketch_axis_modes.h"
#include "Sketcher/sketch_axis.h"
#include "Sketcher/sketch_workbench.h"
#include "Sketcher/sketch.h"
#include "Kernel/document.h"
#include "Kernel/selection_mode.h"
#include "PartDesign/plane.h"


/*** Locally Defined Values ***/
#define SKETCHAXISEDIT_WHOLE					0
#define SKETCHAXISEDIT_BEGIN					1
#define SKETCHAXISEDIT_END						2


/***********************************************~***************************************************/


WCModeSketchAxisEdit::WCModeSketchAxisEdit(WCSketchAxis *axis) : 
	::WCDrawingMode(axis->Sketch(), SKETCHAXISMODE_EDIT_NAME), _axis(axis), _position(), _p0(), _p1(),
	_xSuggest(0.0), _ySuggest(0.0), _suggestionType(WCSketchAlignmentSuggestion::None()), _editMode(SKETCHAXISEDIT_WHOLE) { 
	//Add in alignment suggestion rules
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::CoincidentToPoint());
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::CoincidentToLine());
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::CoincidentToCurve());
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::HorizontalToPoint());
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::VerticalToPoint());
}


void WCModeSketchAxisEdit::OnEntry(void) {
//	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Entering SketchAxis Edit Mode.");
	//Record current location
	this->_p0 = this->_axis->Begin();
	this->_p1 = this->_axis->End();
	//Get suggestion from workbench
	this->_xSuggest = this->_axis->Sketch()->Workbench()->SnapMouseX();
	this->_ySuggest = this->_axis->Sketch()->Workbench()->SnapMouseY();
	this->_suggestionType = this->_axis->Sketch()->Workbench()->SuggestAlignment(this->_alignRules, this->_axis->Base(), 
																				  this->_xSuggest, this->_ySuggest);
	//Mark the initial mouse location
	this->_position.Set(this->_xSuggest, this->_ySuggest, 0.0, 1.0);

	//See if we are close to the end of the line
	WCVector4 pos = this->_axis->Sketch()->ReferencePlane()->TransformMatrix() * this->_position;
	WPFloat length = this->_p0.Distance(this->_p1);
	WPFloat distBegin = this->_axis->Begin().Distance(pos) / length;
	//See if moving begin point
	if (distBegin < 0.05) this->_editMode = SKETCHAXISEDIT_BEGIN;
	else if (distBegin > 0.95) this->_editMode = SKETCHAXISEDIT_END;
}


void WCModeSketchAxisEdit::OnExit(void) {
//	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Exiting SketchAxis Edit Mode.");
}


void WCModeSketchAxisEdit::OnMouseMove(const WPFloat &x, const WPFloat &y) {
	//Make sure feature is not fixed
	if (this->_axis->IsFixed()) return;
	//Get suggestion from workbench
	this->_xSuggest = this->_axis->Sketch()->Workbench()->SnapMouseX();
	this->_ySuggest = this->_axis->Sketch()->Workbench()->SnapMouseY();
	this->_suggestionType = this->_axis->Sketch()->Workbench()->SuggestAlignment(this->_alignRules, this->_axis->Base(), 
																				 this->_xSuggest, this->_ySuggest);
	//Calculate new locations for begin and end
	WCVector4 move(this->_xSuggest, this->_ySuggest, 0.0, 1.0);
	WCVector4 begin( this->_p0 + (move - this->_position) );
	WCVector4 end( this->_p1 + (move - this->_position) );
	//Move based on edit mode
	if (this->_editMode == SKETCHAXISEDIT_WHOLE) {
		//Move axis to new location
		this->_axis->Set(begin, end);
	}
	else if (this->_editMode == SKETCHAXISEDIT_BEGIN) {
		//Move axis to new location
		this->_axis->Set(begin, this->_p1);
	}
	else {
		//Move axis to new location
		this->_axis->Set(this->_p0, end);
	}
}


void WCModeSketchAxisEdit::OnMouseUp(const WCMouseButton &button) {
	//Make sure object is not fixed
	if (!this->_axis->IsFixed()) {
		//Move object back to initial position
		WCVector4 p0 = this->_axis->Begin();
		WCVector4 p1 = this->_axis->End();
		this->_axis->Set(this->_p0, this->_p1);
		//Make sure point has moved
		if ((this->_p0 != p0) || (this->_p1 != p1)) {
			WCAction *action;
			//Move based on edit mode
			if (this->_editMode == SKETCHAXISEDIT_WHOLE) {
				//Move object to current location with an action
				action = WCSketchAxis::ActionMove(this->_axis, p0, p1);
			}
			else if (this->_editMode == SKETCHAXISEDIT_BEGIN) {
				//Move object to current location with an action
				action = WCSketchAxis::ActionMove(this->_axis, p0, this->_p1);
			}
			else {
				//Move object to current location with an action
				action = WCSketchAxis::ActionMove(this->_axis, this->_p0, p1);
			}
			//Execute the action
			this->_axis->Sketch()->Document()->ExecuteAction( action );
		}
	}
	//Revert back to selection mode
	this->_axis->Sketch()->Workbench()->DrawingMode( new WCSelectionMode(this->_axis->Sketch()->Workbench()) );
	//Set document status
//	this->_axis->Sketch()->Document()->Status("Axis " + this->_axis->GetName() + " was selected.");
}


void WCModeSketchAxisEdit::OnArrowKeyPress(const WCArrowKey &key) {
	std::cout << "Hit that key\n";
}


void WCModeSketchAxisEdit::Render(void) {
	//Draw any suggested references
	this->_axis->Sketch()->Workbench()->RenderAlignmentSuggestion(this->_suggestionType, this->_xSuggest, this->_ySuggest);
}


/***********************************************~***************************************************/

