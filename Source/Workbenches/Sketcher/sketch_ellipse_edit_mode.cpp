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
#include <Sketcher/sketch_ellipse_modes.h>
#include <Sketcher/sketch_ellipse.h>
#include <Sketcher/sketch_workbench.h>
#include <Sketcher/sketch.h>
#include <Kernel/document.h>
#include <Kernel/selection_mode.h>
#include <PartDesign/part_plane.h>


/*** Locally Defined Values ***/
#define SKETCHELLIPSEEDIT_POSITION				0
#define SKETCHELLIPSEEDIT_MAJOR					1
#define SKETCHELLIPSEEDIT_MINOR					2


/***********************************************~***************************************************/


WCModeSketchEllipseEdit::WCModeSketchEllipseEdit(WCSketchEllipse *ellipse) : 
	::WCDrawingMode(ellipse->Sketch(), SKETCHELLIPSEMODE_EDIT_NAME), _ellipse(ellipse), 
	_xSuggest(0.0), _ySuggest(0.0), _suggestionType(WCSketchAlignmentSuggestion::None()) { 
	//Add in alignment suggestion rules
//	this->_alignRules.push_back(WCSketchAlignmentSuggestion::CoincidentToPoint());
//	this->_alignRules.push_back(WCSketchAlignmentSuggestion::CoincidentToLine());
//	this->_alignRules.push_back(WCSketchAlignmentSuggestion::CoincidentToCurve());
//	this->_alignRules.push_back(WCSketchAlignmentSuggestion::HorizontalToPoint());
//	this->_alignRules.push_back(WCSketchAlignmentSuggestion::VerticalToPoint());
}


void WCModeSketchEllipseEdit::OnEntry(void) {
//	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Entering SketchCircle Edit Mode.");
	//Record current configuration (get center and radius)
	this->_center = this->_ellipse->Center();
	this->_majorAxis = this->_ellipse->MajorAxis();
	this->_semiMajor = this->_ellipse->MajorRadius();
	this->_semiMinor = this->_ellipse->SemiMinor();
	//Get suggestion from workbench
	this->_xSuggest = this->_ellipse->Sketch()->Workbench()->SnapMouseX();
	this->_ySuggest = this->_ellipse->Sketch()->Workbench()->SnapMouseY();
	this->_suggestionType = this->_ellipse->Sketch()->Workbench()->SuggestAlignment(this->_alignRules, this->_ellipse->Base(), 
																				   this->_xSuggest, this->_ySuggest);
	//Mark the initial mouse location
	this->_position.Set(this->_xSuggest, this->_ySuggest, 0.0, 1.0);
	WCVector4 pos(this->_ellipse->Sketch()->Workbench()->MouseX(), this->_ellipse->Sketch()->Workbench()->MouseY(), 0.0, 1.0);
	//Determine major and minor point locations
	WCVector4 xUnit = this->_ellipse->MajorAxis();
	WCVector4 yUnit(xUnit.J(), -xUnit.I(), 0.0, 0.0);
	WCVector4 major = this->_center + xUnit * this->_semiMajor;
	WCVector4 minor = this->_center + yUnit * this->_semiMinor;
	//See if close to semiMajor
	if (pos.Distance(major) < 0.01) {
		//Set type to semiMajor
		this->_editMode = SKETCHELLIPSEEDIT_MAJOR;
	}
	//See if close to semiMinor
	else if (pos.Distance(minor) < 0.01) {
		//Set type to semiMinor
		this->_editMode = SKETCHELLIPSEEDIT_MINOR;
	}
	//Otherwise, must be position
	else {
		this->_editMode = SKETCHELLIPSEEDIT_POSITION;
	}
}


void WCModeSketchEllipseEdit::OnExit(void) {
//	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Exiting SketchEllipse Edit Mode.");
}


void WCModeSketchEllipseEdit::OnMouseMove(const WPFloat &x, const WPFloat &y) {
	//Make sure feature is not fixed
	if (this->_ellipse->IsFixed()) return;
	//Get suggestion from workbench
	this->_xSuggest = this->_ellipse->Sketch()->Workbench()->SnapMouseX();
	this->_ySuggest = this->_ellipse->Sketch()->Workbench()->SnapMouseY();
	this->_suggestionType = this->_ellipse->Sketch()->Workbench()->SuggestAlignment(this->_alignRules, this->_ellipse->Base(), 
																				 this->_xSuggest, this->_ySuggest);
	//Calculate new locations for begin and end
	WCVector4 position(this->_xSuggest, this->_ySuggest, 0.0, 1.0);

	//See if changing position
	if (this->_editMode == SKETCHELLIPSEEDIT_POSITION) {
		WCVector4 center = this->_center + position - this->_position;
		//Move center to new location
		this->_ellipse->Center(center);
	}
	//See if changing major
	else if(this->_editMode == SKETCHELLIPSEEDIT_MAJOR) {
		//Determine new radius
		WPFloat radius = this->_center.Distance(position);
		//Get unit direction of primary line (make sure to normalize with div by radius)
		WCVector4 axis = (position - this->_center) / radius;
		//Set the new semimajor axis and position
		this->_ellipse->SemiMajor(axis, radius);
	}
	//Otherwise, changing minor
	else {
		//Get distance from mouse cursor to primary line
		WCVector4 major = this->_center + this->_majorAxis * this->_semiMajor;
		WPFloat radius = PointToLineDistance2D(this->_center.I(), this->_center.J(), major.I(), major.J(), 
											   this->_xSuggest, this->_ySuggest, false);
		//Set the new semiminor distance
		this->_ellipse->SemiMinor(radius);
	}
}


void WCModeSketchEllipseEdit::OnMouseUp(const WCMouseButton &button) {
	//Make sure object is not fixed
	if (!this->_ellipse->IsFixed()) {
		//Path for position
		if (this->_editMode == SKETCHELLIPSEEDIT_POSITION) {
			//Get current center
			WCVector4 center = this->_ellipse->Center();
			//Make sure point has moved
			if (this->_center != center) {
				//Move object back to initial position
				this->_ellipse->Center(this->_center);
				//Move object to current location with an action
				WCAction *action = WCSketchEllipse::ActionMove(this->_ellipse, center);
				this->_ellipse->Sketch()->Document()->ExecuteAction( action );
			}
		}
		//Path for semiMajor
		else if (this->_editMode == SKETCHELLIPSEEDIT_MAJOR) {
			//Get current major axis and radius
			WCVector4 axis = this->_ellipse->MajorAxis();
			WPFloat radius = this->_ellipse->MajorRadius();
			//Make sure radius has changed
			if ((this->_majorAxis != axis) || (this->_semiMajor != radius)) {
				//Set major back to original value
				this->_ellipse->SemiMajor(this->_majorAxis, this->_semiMajor);
				//Set major with an action
				WCAction *action = WCSketchEllipse::ActionMajor(this->_ellipse, axis, radius);
				this->_ellipse->Sketch()->Document()->ExecuteAction( action );
			}
		}
		//Path for semiMinor
		else {
			//Get current minor radius
			WPFloat radius = this->_ellipse->SemiMinor();
			//Make sure radius has changed
			if (this->_semiMinor != radius) {
				//Set minor back to original value
				this->_ellipse->SemiMinor(this->_semiMinor);
				//Set minor with an action
				WCAction *action = WCSketchEllipse::ActionMinor(this->_ellipse, radius);
				this->_ellipse->Sketch()->Document()->ExecuteAction( action );
			}
		}
	}

	//Revert back to selection mode
	this->_ellipse->Sketch()->Workbench()->DrawingMode( new WCSelectionMode(this->_ellipse->Sketch()->Workbench()) );
	//Set document status
//	this->_ellipse->Sketch()->Document()->Status("Ellipse " + this->_ellipse->GetName() + " was selected.");
}


void WCModeSketchEllipseEdit::OnArrowKeyPress(const WCArrowKey &key) {
	CLOGGER_WARN(WCLogManager::RootLogger(), "WCModeSketchEllipseEdit::OnArrowKeyPress - Not yet implemented.");
}


void WCModeSketchEllipseEdit::Render(void) {
	//Draw any suggested references
	this->_ellipse->Sketch()->Workbench()->RenderAlignmentSuggestion(this->_suggestionType, this->_xSuggest, this->_ySuggest);
}


/***********************************************~***************************************************/

