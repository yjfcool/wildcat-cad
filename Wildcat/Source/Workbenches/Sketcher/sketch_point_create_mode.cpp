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
#include "Sketcher/sketch_workbench.h"
#include "Sketcher/sketch_point.h"
#include "Sketcher/sketch.h"
#include "Sketcher/sketch_align_suggestion.h"
#include "Constraint/constraint_distance.h"
#include "Kernel/document.h"


/***********************************************~***************************************************/


WCModeSketchPointCreate::WCModeSketchPointCreate(WCSketchWorkbench *wb) : ::WCDrawingMode(wb->Sketch(), SKETCHPOINTMODE_CREATE_NAME), 
	_workbench(wb), _xSuggest(0.0), _ySuggest(0.0), _suggestionType(WCSketchAlignmentSuggestion::None()) { 
	//Add in alignment suggestion rules
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::CoincidentToPoint());
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::CoincidentToLine());
//	this->_alignRules.push_back(WCSketchAlignmentSuggestion::CoincidentToCurve());
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::HorizontalToPoint());
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::VerticalToPoint());
}


void WCModeSketchPointCreate::OnEntry(void) {
	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Entering SketchPoint Draw Mode.");
	//Clear any selected items
	this->_workbench->SelectionManager()->Clear(true);
	//Get suggestion from workbench
	this->_xSuggest = this->_workbench->SnapMouseX();
	this->_ySuggest = this->_workbench->SnapMouseY();
	this->_suggestionType = this->_workbench->SuggestAlignment(this->_alignRules, NULL, this->_xSuggest, this->_ySuggest);
}


void WCModeSketchPointCreate::OnExit(void) {
	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Exiting SketchPoint Draw Mode.");
}


void WCModeSketchPointCreate::OnMouseMove(const WPFloat &x, const WPFloat &y) {
	//Get suggestion from workbench
	this->_xSuggest = this->_workbench->SnapMouseX();
	this->_ySuggest = this->_workbench->SnapMouseY();
	this->_suggestionType = this->_workbench->SuggestAlignment(this->_alignRules, NULL, this->_xSuggest, this->_ySuggest);
}


void WCModeSketchPointCreate::OnMouseUp(const WCMouseButton &button) {
	//Make sure button is left-mouse
	if (button != WCMouseButton::Left()) return;

	//Check to see if a point already exists at this location
	if (this->_suggestionType == WCSketchAlignmentSuggestion::CoincidentToPoint()) {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCSketchPointMode::OnMouseUp - Point already exists at this location.");
		return;
	}
	//Create new point at location (via action)
	WCVector4 pos(this->_xSuggest, this->_ySuggest, 0.0, 1.0);
	WCActionSketchPointCreate *action = WCSketchPoint::ActionCreate(this->_workbench->Sketch(), "", pos);
	this->_workbench->Sketch()->Document()->ExecuteAction( action );
	
	//Get the origin point
	WCSketchPoint* origin = dynamic_cast<WCSketchPoint*>( this->_workbench->Sketch()->FeatureFromName("Origin") );
	
	//See if auto-dimensional constraints are on
	if (this->_workbench->IsDimensionalConstraint()) {
		//Create the horizontal distance constraint
		WCAction *constraintAction = WCConstraintDistance::ActionCreate(this->_workbench->Sketch(), "", WCMeasureType::Horizontal(),
																		action->Point(), origin);
		this->_workbench->Sketch()->Document()->ExecuteAction( constraintAction);
		//Create the vertical distance constraint
		constraintAction = WCConstraintDistance::ActionCreate(this->_workbench->Sketch(), "", WCMeasureType::Vertical(),
															  action->Point(), origin);
		this->_workbench->Sketch()->Document()->ExecuteAction( constraintAction);
	}
	
	//Fake that there a point has been suggested
	this->_suggestionType = WCSketchAlignmentSuggestion::CoincidentToPoint();
}


void WCModeSketchPointCreate::Render(void) {
	//Draw the crosshairs at the current position
	this->_workbench->RenderCrosshairs(this->_xSuggest, this->_ySuggest);
	//Draw any suggested references
	this->_workbench->RenderAlignmentSuggestion(this->_suggestionType, this->_xSuggest, this->_ySuggest);
}


/***********************************************~***************************************************/

