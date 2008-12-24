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
#include <Sketcher/sketch_arc_modes.h>
#include <Sketcher/sketch_arc.h>
#include <Sketcher/sketch_workbench.h>
#include <Sketcher/sketch.h>
#include <Kernel/document.h>
#include <Kernel/selection_mode.h>
#include <PartDesign/part_plane.h>


/*** Locally Defined Values ***/
#define SKETCHARCEDIT_POSITION					0
#define SKETCHARCEDIT_RADIUS					1


/***********************************************~***************************************************/


WCModeSketchArcEdit::WCModeSketchArcEdit(WCSketchArc *arc) : 
	::WCDrawingMode(arc->Sketch(), SKETCHARCMODE_EDIT_NAME), _arc(arc), _center(), _position(), _radius(0.0),
	_xSuggest(0.0), _ySuggest(0.0), _suggestionType(WCSketchAlignmentSuggestion::None()), _alignRules(), _editMode(SKETCHARCEDIT_POSITION) { 
	//Add in alignment suggestion rules
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::CoincidentToPoint());
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::CoincidentToLine());
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::CoincidentToCurve());
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::HorizontalToPoint());
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::VerticalToPoint());
}


void WCModeSketchArcEdit::OnEntry(void) {
	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Entering SketchArc Edit Mode.");
	//Record current configuration (get center and radius)
	this->_center = this->_arc->Center();
	this->_radius = this->_arc->Radius();
	//Get suggestion from workbench
	this->_xSuggest = this->_arc->Sketch()->Workbench()->SnapMouseX();
	this->_ySuggest = this->_arc->Sketch()->Workbench()->SnapMouseY();
	this->_suggestionType = this->_arc->Sketch()->Workbench()->SuggestAlignment(this->_alignRules, this->_arc->Base(), 
																				   this->_xSuggest, this->_ySuggest);
	//Mark the initial mouse location
	this->_position.Set(this->_xSuggest, this->_ySuggest, 0.0, 1.0);
	//See if we are close to the center of the circle
	WPFloat distance = this->_center.Distance(this->_position);
	//See if far from center
	if (distance > 0.1) this->_editMode = SKETCHARCEDIT_RADIUS;
}


void WCModeSketchArcEdit::OnExit(void) {
	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Exiting SketchArc Edit Mode.");
}


void WCModeSketchArcEdit::OnMouseMove(const WPFloat &x, const WPFloat &y) {
	//Make sure feature is not fixed
	if (this->_arc->IsFixed()) return;
	//Get suggestion from workbench
	this->_xSuggest = this->_arc->Sketch()->Workbench()->SnapMouseX();
	this->_ySuggest = this->_arc->Sketch()->Workbench()->SnapMouseY();
	this->_suggestionType = this->_arc->Sketch()->Workbench()->SuggestAlignment(this->_alignRules, this->_arc->Base(), 
																				 this->_xSuggest, this->_ySuggest);
	//Calculate new locations for begin and end
	WCVector4 move(this->_xSuggest, this->_ySuggest, 0.0, 1.0);

	//See if changing position
	if (this->_editMode == SKETCHARCEDIT_POSITION) {
		//Move center to new location
		this->_arc->Center(move);
	}
	//Change the radius
	else {
		//Determine distance from center to move
		WPFloat radius = this->_center.Distance(move);
		//Set the new radius
		this->_arc->Radius(radius);
	}
}


void WCModeSketchArcEdit::OnMouseUp(const WCMouseButton &button) {
	//Make sure object is not fixed
	if (!this->_arc->IsFixed()) {
		//One path for position
		if (this->_editMode == SKETCHARCEDIT_POSITION) {
			//Get modified center position
			WCVector4 center = this->_arc->Center();
			//Make sure point has moved
			if (this->_center != center) {
				//Move object back to initial position
				this->_arc->Center(this->_center);
				//Move object to current location with an action
				WCAction *action = WCSketchArc::ActionMove(this->_arc, center);
				this->_arc->Sketch()->Document()->ExecuteAction( action );
			}
		}
		//Otherwise, different path for radius
		else {
			//Get modified radius value
			WPFloat radius = this->_arc->Radius();
			//Make sure radius has changed
			if (this->_radius != radius) {
				//Change radius back to original value
				this->_arc->Radius(this->_radius);
				//Set radius with an action
				WCAction *action = WCSketchArc::ActionRadius(this->_arc, radius);
				this->_arc->Sketch()->Document()->ExecuteAction( action );
			}
		}
	}
	//Revert back to selection mode
	this->_arc->Sketch()->Workbench()->DrawingMode( new WCSelectionMode(this->_arc->Sketch()->Workbench()) );
	//Set document status
//	this->_arc->Sketch()->Document()->Status("Arc " + this->_arc->GetName() + " was selected.");
}


void WCModeSketchArcEdit::OnArrowKeyPress(const WCArrowKey &key) {
	CLOGGER_WARN(WCLogManager::RootLogger(), "WCModeSketchArcEdit::OnArrowKeyPress - Not yet implemented.");
}


void WCModeSketchArcEdit::Render(void) {
	//Draw any suggested references
	this->_arc->Sketch()->Workbench()->RenderAlignmentSuggestion(this->_suggestionType, this->_xSuggest, this->_ySuggest);
}


/***********************************************~***************************************************/

