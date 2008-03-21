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
#include "Sketcher/sketch_line_modes.h"
#include "Sketcher/sketch_line.h"
#include "Sketcher/sketch_workbench.h"
#include "Sketcher/sketch.h"
#include "Kernel/document.h"
#include "Kernel/selection_mode.h"
#include "PartDesign/part_plane.h"


/*** Locally Defined Values ***/
#define SKETCHLINEEDIT_WHOLE					0
#define SKETCHLINEEDIT_BEGIN					1
#define SKETCHLINEEDIT_END						2


/***********************************************~***************************************************/


WCModeSketchLineEdit::WCModeSketchLineEdit(WCSketchLine *line) : 
	::WCDrawingMode(line->Sketch(), SKETCHLINEMODE_EDIT_NAME), _line(line), _position(), _p0(), _p1(),
	_xSuggest(0.0), _ySuggest(0.0), _suggestionType(WCSketchAlignmentSuggestion::None()), _alignRules(), _editMode(SKETCHLINEEDIT_WHOLE) { 
	//Add in alignment suggestion rules
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::CoincidentToPoint());
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::CoincidentToLine());
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::CoincidentToCurve());
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::HorizontalToPoint());
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::VerticalToPoint());
}


void WCModeSketchLineEdit::OnEntry(void) {
//	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Entering SketchLine Edit Mode.");
	//Record current location
	this->_p0 = this->_line->Begin();
	this->_p1 = this->_line->End();
	//Get suggestion from workbench
	this->_xSuggest = this->_line->Sketch()->Workbench()->SnapMouseX();
	this->_ySuggest = this->_line->Sketch()->Workbench()->SnapMouseY();
	this->_suggestionType = this->_line->Sketch()->Workbench()->SuggestAlignment(this->_alignRules, this->_line->Base(), 
																				  this->_xSuggest, this->_ySuggest);
	//Mark the initial mouse location
	this->_position.Set(this->_xSuggest, this->_ySuggest, 0.0, 1.0);

	//See if we are close to the end of the line
	WCVector4 pos = this->_line->Sketch()->ReferencePlane()->TransformMatrix() * this->_position;
	WPFloat length = this->_p0.Distance(this->_p1);
	WPFloat distBegin = this->_line->Begin().Distance(pos) / length;
	//See if moving begin point
	if (distBegin < 0.05) this->_editMode = SKETCHLINEEDIT_BEGIN;
	else if (distBegin > 0.95) this->_editMode = SKETCHLINEEDIT_END;
}


void WCModeSketchLineEdit::OnExit(void) {
//	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Exiting SketchLine Edit Mode.");
}


void WCModeSketchLineEdit::OnMouseMove(const WPFloat &x, const WPFloat &y) {
	//Make sure feature is not fixed
	if (this->_line->IsFixed()) return;
	//Get suggestion from workbench
	this->_xSuggest = this->_line->Sketch()->Workbench()->SnapMouseX();
	this->_ySuggest = this->_line->Sketch()->Workbench()->SnapMouseY();
	this->_suggestionType = this->_line->Sketch()->Workbench()->SuggestAlignment(this->_alignRules, this->_line->Base(), 
																				 this->_xSuggest, this->_ySuggest);
	//Calculate new locations for begin and end
	WCVector4 move(this->_xSuggest, this->_ySuggest, 0.0, 1.0);
	WCVector4 begin( this->_p0 + (move - this->_position) );
	WCVector4 end( this->_p1 + (move - this->_position) );
	//Move based on edit mode
	if (this->_editMode == SKETCHLINEEDIT_WHOLE) {
		//Move line to new location
		this->_line->Set(begin, end);
	}
	else if (this->_editMode == SKETCHLINEEDIT_BEGIN) {
		//Move line to new location
		this->_line->Set(begin, this->_p1);
	}
	else {
		//Move line to new location
		this->_line->Set(this->_p0, end);
	}
}


void WCModeSketchLineEdit::OnMouseUp(const WCMouseButton &button) {
	//Make sure object is not fixed
	if (!this->_line->IsFixed()) {
		//Move object back to initial position
		WCVector4 p0 = this->_line->Begin();
		WCVector4 p1 = this->_line->End();
		this->_line->Set(this->_p0, this->_p1);
		//Make sure point has moved
		if ((this->_p0 != p0) || (this->_p1 != p1)) {
			WCAction *action;
			//Move based on edit mode
			if (this->_editMode == SKETCHLINEEDIT_WHOLE) {
				//Move object to current location with an action
				action = WCSketchLine::ActionMove(this->_line, p0, p1);
			}
			else if (this->_editMode == SKETCHLINEEDIT_BEGIN) {
				//Move object to current location with an action
				action = WCSketchLine::ActionMove(this->_line, p0, this->_p1);
			}
			else {
				//Move object to current location with an action
				action = WCSketchLine::ActionMove(this->_line, this->_p0, p1);
			}
			//Execute the action
			this->_line->Sketch()->Document()->ExecuteAction( action );
		}
	}
	//Revert back to selection mode
	this->_line->Sketch()->Workbench()->DrawingMode( new WCSelectionMode(this->_line->Sketch()->Workbench()) );
	//Set document status
//	this->_line->Sketch()->Document()->Status("Line " + this->_line->GetName() + " was selected.");
}


void WCModeSketchLineEdit::OnArrowKeyPress(const WCArrowKey &key) {
	std::cout << "Hit that key\n";
}


void WCModeSketchLineEdit::Render(void) {
	//Draw any suggested references
	this->_line->Sketch()->Workbench()->RenderAlignmentSuggestion(this->_suggestionType, this->_xSuggest, this->_ySuggest);
}


/***********************************************~***************************************************/

