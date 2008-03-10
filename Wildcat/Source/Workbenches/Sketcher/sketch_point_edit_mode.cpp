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
#include "Sketcher/sketch_point_modes.h"
#include "Sketcher/sketch_point.h"
#include "Sketcher/sketch.h"
#include "Sketcher/sketch_align_suggestion.h"
#include "Kernel/document.h"
#include "Kernel/selection_mode.h"
#include "PartDesign/part_plane.h"


/***********************************************~***************************************************/


WCModeSketchPointEdit::WCModeSketchPointEdit(WCSketchPoint *point) : ::WCDrawingMode(point->Sketch(), SKETCHPOINTMODE_EDIT_NAME), 
	_point(point), _position(), _xSuggest(0.0), _ySuggest(0.0), _suggestionType(WCSketchAlignmentSuggestion::None()) {
	//Add in alignment suggestion rules
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::CoincidentToPoint());
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::CoincidentToLine());
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::CoincidentToCurve());
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::HorizontalToPoint());
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::VerticalToPoint());
}


void WCModeSketchPointEdit::OnEntry(void) {
	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Entering SketchPoint Edit Mode.");
	//Record current location
	this->_position = this->_point->Position();
	//Get suggestion from workbench
	this->_xSuggest = this->_point->Sketch()->Workbench()->SnapMouseX();
	this->_ySuggest = this->_point->Sketch()->Workbench()->SnapMouseY();
	this->_suggestionType = this->_point->Sketch()->Workbench()->SuggestAlignment(this->_alignRules, this->_point->Base(), 
																				  this->_xSuggest, this->_ySuggest);
}


void WCModeSketchPointEdit::OnExit(void) {
	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Exiting SketchPoint Edit Mode.");
}


void WCModeSketchPointEdit::OnMouseMove(const WPFloat &x, const WPFloat &y) {
	//Make sure point is not fixed
	if (this->_point->IsFixed()) return;
	//Get suggestion from workbench
	this->_xSuggest = this->_point->Sketch()->Workbench()->SnapMouseX();
	this->_ySuggest = this->_point->Sketch()->Workbench()->SnapMouseY();
	this->_suggestionType = this->_point->Sketch()->Workbench()->SuggestAlignment(this->_alignRules, this->_point->Base(), 
																				  this->_xSuggest, this->_ySuggest);
	//Get new position for point
	WCVector4 pos(this->_xSuggest, this->_ySuggest, 0.0, 1.0);
	this->_point->Set(pos);
}


void WCModeSketchPointEdit::OnMouseUp(const WCMouseButton &button) {
	//Make sure point is not fixed
	if (!this->_point->IsFixed()) {
		//Move point back to initial position
		this->_point->Set(this->_position);
		//Get new position for point
		WCVector4 pos(this->_xSuggest, this->_ySuggest, 0.0, 1.0);
		//Make sure point has moved
		if (pos.Distance( this->_point->Position() ) > 0.001) {
			//Move point to current location with an action
			WCAction *action = WCSketchPoint::ActionMove(this->_point, pos);
			this->_point->Sketch()->Document()->ExecuteAction( action );
		}
	}
	//Revert back to old mode
	this->_point->Sketch()->Workbench()->DrawingMode( new WCSelectionMode(this->_point->Sketch()->Workbench()) );
}


void WCModeSketchPointEdit::OnArrowKeyPress(const WCArrowKey &key) {
}


void WCModeSketchPointEdit::Render(void) {
	//Draw any suggested references
	this->_point->Sketch()->Workbench()->RenderAlignmentSuggestion(this->_suggestionType, this->_xSuggest, this->_ySuggest);
}


/***********************************************~***************************************************/

