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
#include <Sketcher/sketch_rect_mode.h>
#include <Sketcher/sketch_rectangle.h>
#include <Sketcher/sketch_line.h>
#include <Sketcher/sketch_workbench.h>
#include <Sketcher/sketch_feature.h>
#include <Sketcher/sketch.h>
#include <Constraint/constraint_horizontal.h>
#include <Constraint/constraint_vertical.h>
#include <Constraint/constraint_coincidence.h>
#include <Constraint/constraint_length.h>
#include <PartDesign/part_plane.h>
#include <Kernel/document.h>


/***********************************************~***************************************************/


WCModeSketchRectangleCreate::WCModeSketchRectangleCreate(WCSketchWorkbench *wb) : ::WCDrawingMode(wb->Sketch(), SKETCHRECTMODE_NAME), 
	_workbench(wb), _isDrawing(false), _begin(), _xSuggest(0.0), _ySuggest(0.0), _suggestionType(WCSketchAlignmentSuggestion::None()) {
	//Clear the lines
	this->_lines[0] = NULL;
	this->_lines[1] = NULL;
	this->_lines[2] = NULL;
	this->_lines[3] = NULL;
	//Add in alignment suggestion rules
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::CoincidentToPoint());
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::CoincidentToLine());
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::CoincidentToCurve());
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::HorizontalToPoint());
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::VerticalToPoint());
}


void WCModeSketchRectangleCreate::OnEntry(void) {
	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Entering SketchRectangle Draw Mode.");
	//Clear any selected items
	this->_workbench->SelectionManager()->Clear(true);
}


void WCModeSketchRectangleCreate::OnExit(void) {
	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Exiting SketchRectangle Draw Mode.");
	//Delete lines if drawing
	if (this->_isDrawing) {
		delete this->_lines[0];
		delete this->_lines[1];
		delete this->_lines[2];
		delete this->_lines[3];
	}
}


void WCModeSketchRectangleCreate::OnMouseDown(const WCMouseButton &button) {
	//Check mouse button
	if (button == WCMouseButton::Right()) {
		//Check if drawing
		if (!this->_isDrawing) return;
		//Delete lines if present
		delete this->_lines[0];
		delete this->_lines[1];
		delete this->_lines[2];
		delete this->_lines[3];
		//Reset drawing
		this->_isDrawing = false;
		return;
	}
	//Ignore all non-left buttons
	else if (button != WCMouseButton::Left()) return;
	
	//See if already drawing
	if (!this->_isDrawing) {
		//Get the position
		this->_begin.Set(this->_xSuggest, this->_ySuggest, 0.0, 1.0);
		//Get new corner positions for the rectangle
		WCVector4 pos = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * this->_begin;
		//Create new lines lines for display
		for(int i=0; i<4; i++) {
			this->_lines[i] = new WCGeometricLine(pos, pos);
			this->_lines[i]->Color(WCSketchFeature::InprocessColor); 
			this->_lines[i]->Thickness(WCSketchFeature::LineThickness);
		}
		//Mark as drawing
		this->_isDrawing = true;
	}
	//Otherwise, create the rectangle and reset
	else {
		//Create a new SketchRect
		WCVector4 pos(this->_xSuggest, this->_ySuggest, 0.0, 1.0);
		WCActionSketchRectangleCreate *rectAction = WCSketchRectangle::ActionCreate(this->_workbench->Sketch(), this->_begin, pos);
		this->_workbench->Sketch()->Document()->ExecuteAction( rectAction );
					
		//Create constraints
		if (this->_workbench->IsGeometricConstraint()) {
			//Create four coincidence constraints (UL corner)
			WCAction* coincidenceAction = WCConstraintCoincidence::ActionCreate(this->_workbench->Sketch(), "",
				rectAction->LeftLine(), WCMeasurePosition::End(), rectAction->TopLine(), WCMeasurePosition::Begin());
			this->_workbench->Sketch()->Document()->ExecuteAction( coincidenceAction );
			//Coincidence (UR corner)
			coincidenceAction = WCConstraintCoincidence::ActionCreate(this->_workbench->Sketch(), "",
				rectAction->TopLine(), WCMeasurePosition::End(), rectAction->RightLine(), WCMeasurePosition::Begin());
			this->_workbench->Sketch()->Document()->ExecuteAction( coincidenceAction );
			//Coincidence (LR corner)
			coincidenceAction = WCConstraintCoincidence::ActionCreate(this->_workbench->Sketch(), "",
				rectAction->RightLine(), WCMeasurePosition::End(), rectAction->BottomLine(), WCMeasurePosition::Begin());
			this->_workbench->Sketch()->Document()->ExecuteAction( coincidenceAction );
			//Coincidence (LL corner)
			coincidenceAction = WCConstraintCoincidence::ActionCreate(this->_workbench->Sketch(), "",
				rectAction->BottomLine(), WCMeasurePosition::End(), rectAction->LeftLine(), WCMeasurePosition::Begin());
			this->_workbench->Sketch()->Document()->ExecuteAction( coincidenceAction );

			//Create two horizontal constraints
			WCAction* horizAction = WCConstraintHorizontal::ActionCreate(this->_workbench->Sketch(), "", rectAction->TopLine());
			this->_workbench->Sketch()->Document()->ExecuteAction( horizAction );
			horizAction = WCConstraintHorizontal::ActionCreate(this->_workbench->Sketch(), "", rectAction->BottomLine());
			this->_workbench->Sketch()->Document()->ExecuteAction( horizAction );

			//Create two vertical constraints
			WCAction* vertAction = WCConstraintVertical::ActionCreate(this->_workbench->Sketch(), "", rectAction->LeftLine());
			this->_workbench->Sketch()->Document()->ExecuteAction( vertAction );
			vertAction = WCConstraintVertical::ActionCreate(this->_workbench->Sketch(), "", rectAction->RightLine());
			this->_workbench->Sketch()->Document()->ExecuteAction( vertAction );

			//Create one vertical length
			//...

			//Create one horizontal length
			//...
		}
		
		//Delete lines
		for(int i=0; i<4; i++) {
			delete this->_lines[i];
			this->_lines[i] = NULL;
		}
		//Mark as no longer drawing
		this->_isDrawing = false;
	}
}


void WCModeSketchRectangleCreate::OnMouseMove(const WPFloat &x, const WPFloat &y) {
	//Get suggestion from workbench
	this->_xSuggest = this->_workbench->SnapMouseX();
	this->_ySuggest = this->_workbench->SnapMouseY();
	this->_suggestionType = this->_workbench->SuggestAlignment(this->_alignRules, NULL, this->_xSuggest, this->_ySuggest);
	
	//Move p1 if we are drawing
	if (this->_isDrawing) {
		//Get new corner positions for the rectangle
		WCVector4 ll = this->_begin;
		WCVector4 ul(this->_begin.I(), this->_ySuggest, 0.0, 1.0);
		WCVector4 ur(this->_xSuggest, this->_ySuggest, 0.0, 1.0);
		WCVector4 lr(this->_xSuggest, this->_begin.J(), 0.0, 1.0);
		ll = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * ll;
		ul = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * ul;
		ur = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * ur;
		lr = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * lr;
		//Set the end points of the lines appropriately
		this->_lines[0]->Begin(ll); this->_lines[0]->End(ul);
		this->_lines[1]->Begin(ul); this->_lines[1]->End(ur);
		this->_lines[2]->Begin(ur); this->_lines[2]->End(lr);
		this->_lines[3]->Begin(lr); this->_lines[3]->End(ll);
	}
}


void WCModeSketchRectangleCreate::Render(void) {
	//Draw the crosshairs at the current position
	this->_workbench->RenderCrosshairs(this->_xSuggest, this->_ySuggest);
	//Draw any suggested references
	this->_workbench->RenderAlignmentSuggestion(this->_suggestionType, this->_xSuggest, this->_ySuggest);

	//Draw the lines if appropriate
	if (!this->_isDrawing) return;

	//Turn off depth testing
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(WCSketchFeature::LineStippleFactor, WCSketchFeature::LineStipplePattern);
	WPFloat zoom = this->_workbench->Feature()->Document()->Scene()->ActiveCamera()->Zoom();
	//Draw pending line
	for(int i=0; i<4; i++) this->_lines[i]->Render(0, WCColor::Default(), zoom);
	//Renable depth test
	glDisable(GL_LINE_STIPPLE);
	glEnable(GL_DEPTH_TEST);
}


/***********************************************~***************************************************/

