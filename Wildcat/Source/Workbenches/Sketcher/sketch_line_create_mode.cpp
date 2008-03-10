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
#include "Sketcher/sketch_point.h"
#include "Sketcher/sketch_workbench.h"
#include "Sketcher/sketch.h"
#include "Constraint/constraint_length.h"
#include "Constraint/constraint_distance.h"
#include "Constraint/constraint_coincidence.h"
#include "Constraint/constraint_horizontal.h"
#include "Constraint/constraint_vertical.h"
#include "Kernel/document.h"
#include "PartDesign/plane.h"


/***********************************************~***************************************************/


WCModeSketchLineCreate::WCModeSketchLineCreate(WCSketchWorkbench *wb) : 
	::WCDrawingMode(wb->Sketch(), SKETCHLINEMODE_CREATE_NAME), _workbench(wb), _isDrawing(false),
	_begin(), _p0(NULL), _p1(NULL), _line(NULL), _xSuggest(0.0), _ySuggest(0.0), _suggestionType(WCSketchAlignmentSuggestion::None()) { 
	//Add in alignment suggestion rules
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::CoincidentToPoint());
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::CoincidentToLine());
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::CoincidentToCurve());
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::HorizontalToPoint());
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::VerticalToPoint());
}


void WCModeSketchLineCreate::OnEntry(void) {
	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Entering SketchLine Draw Mode.");
	//Clear any selected items
	this->_workbench->SelectionManager()->Clear(true);
}


void WCModeSketchLineCreate::OnExit(void) {
	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Exiting SketchLine Draw Mode.");
	//Delete line is present
	if(this->_isDrawing) {
		delete this->_p0;
		delete this->_p1;
		delete this->_line;
	}
}


void WCModeSketchLineCreate::OnMouseDown(const WCMouseButton &button) {
	//Cancel if right click
	if (button == WCMouseButton::Right()) {
		//If not drawing, do nothing
		if (!this->_isDrawing) return;
		//Delete the visualization objects
		delete this->_line;
		delete this->_p0;
		delete this->_p1;
		//Reset drawing state
		this->_isDrawing = false;
		return;
	}
	//Otherwise, only left button
	else if (button != WCMouseButton::Left()) return;

	//If not already drawing
	if (!this->_isDrawing) {
		//Get the position
		this->_begin.Set(this->_xSuggest, this->_ySuggest, 0.0, 1.0);
		WCVector4 pos = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * this->_begin;
		//Create the visualization points
		this->_p0 = new WCGeometricPoint(pos);
		this->_p0->Color(WCSketchFeature::InprocessColor);
		this->_p0->Size(WCSketchFeature::PointSize);
		this->_p1 = new WCGeometricPoint(pos);
		this->_p1->Color(WCSketchFeature::InprocessColor);
		this->_p1->Size(WCSketchFeature::PointSize);
		//Create line
		this->_line = new WCGeometricLine(pos, pos);
		this->_line->Color(WCSketchFeature::InprocessColor);
		this->_line->Thickness(WCSketchFeature::LineThickness);
		//Mark as drawing
		this->_isDrawing = true;
	}
	//If already drawing
	else {
		//Get the current coordinates
		WCVector4 pos(this->_xSuggest, this->_ySuggest, 0.0, 1.0);
		//Make sure line is long enough
		if (pos.Distance(this->_begin) < SKETCHLINE_MIN_LENGTH) {
			CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCModeSketchLineCreate::OnMouseDown - Line length below minimum.");
			return;
		}
		//Create a new SketchLine
		WCActionSketchLineCreate *action = WCSketchLine::ActionCreate(this->_workbench->Sketch(), "", this->_begin, pos);
		this->_workbench->Sketch()->Document()->ExecuteAction( action );
		WCSketchLine *line = action->Line();

		//Create two new SketchPoints
//		WCActionSketchPointCreate *p0Action = WCSketchPoint::ActionCreate(this->_workbench->Sketch(), "", this->_begin);
//		this->_workbench->Sketch()->Document()->ExecuteAction( p0Action );
//		WCSketchPoint *p0 = p0Action->Point();
//		WCActionSketchPointCreate *p1Action = WCSketchPoint::ActionCreate(this->_workbench->Sketch(), "", pos);
//		this->_workbench->Sketch()->Document()->ExecuteAction( p1Action );
//		WCSketchPoint *p1 = p1Action->Point();
		
		//See if auto-dimensional constraints are on
		if (this->_workbench->IsDimensionalConstraint()) {
			//Create distance constraints from origin to p0
//			WCSketchPoint* origin = dynamic_cast<WCSketchPoint*>( this->_workbench->Sketch()->FeatureFromName("Origin") );
			//Create the horizontal distance constraint
//			WCAction *p0ConstraintAction = WCConstraintDistance::ActionCreate( this->_workbench->Sketch(), "", p0, 
//				origin, WCMeasureType::Horizontal() );
//			this->_workbench->Sketch()->Document()->ExecuteAction( p0ConstraintAction );
			//Create the vertical distance constraint
//			p0ConstraintAction = WCConstraintDistance::ActionCreate( this->_workbench->Sketch(), "", p0,
//				origin, WCMeasureType::Vertical() );
//			this->_workbench->Sketch()->Document()->ExecuteAction( p0ConstraintAction );
			//Create a length constraint on line
			WCAction *a2 = WCConstraintLength::ActionCreate(this->_workbench->Sketch(), "", line);
			this->_workbench->Sketch()->Document()->ExecuteAction( a2 );
		}

		//See if auto-geometric constraints are on
		if (this->_workbench->IsGeometricConstraint()) {		
			//Create coincidence constraints for p0-line and p1-line
//			WCAction *p0LineCoincidenceAction = WCConstraintCoincidence::ActionCreate( this->_workbench->Sketch(), "", p0, line);
//			this->_workbench->Sketch()->Document()->ExecuteAction( p0LineCoincidenceAction );
//			WCAction *p1LineCoincidenceAction = WCConstraintCoincidence::ActionCreate( this->_workbench->Sketch(), "", p1, line);
//			this->_workbench->Sketch()->Document()->ExecuteAction( p1LineCoincidenceAction );
			
			//Determine the slope of the line
			WPFloat slope = (pos.J() - this->_begin.J()) / (pos.I() - this->_begin.I());
			//Check for horizontal or vertical constraints
			if ((slope < 0.1) && (slope > -0.1)) {
				WCAction *horizAction = WCConstraintHorizontal::ActionCreate( this->_workbench->Sketch(), "", line);
				this->_workbench->Sketch()->Document()->ExecuteAction( horizAction );
			}
			else if ((slope > 10) || (slope < -10)) {
				WCAction *vertAction = WCConstraintVertical::ActionCreate( this->_workbench->Sketch(), "", line);
				this->_workbench->Sketch()->Document()->ExecuteAction( vertAction );
			}
			//Otherwise, look for angle constraint compared to uAxis
			else {
				//...
//				std::cout << "Making angle constraint\n";
			}
		}
		
		//Reset for the next line
		this->_begin.Set(this->_xSuggest, this->_ySuggest, 0.0, 1.0);
		pos = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * this->_begin;
		this->_line->Begin(pos);
		this->_line->End(pos);
		this->_p0->Set(pos);
		this->_p1->Set(pos);
	}
}


void WCModeSketchLineCreate::OnMouseMove(const WPFloat &x, const WPFloat &y) {
	//Get suggestion from workbench
	this->_xSuggest = this->_workbench->SnapMouseX();
	this->_ySuggest = this->_workbench->SnapMouseY();
	this->_suggestionType = this->_workbench->SuggestAlignment(this->_alignRules, NULL, this->_xSuggest, this->_ySuggest);
	
	//Move p1 if we are drawing
	if (this->_isDrawing) {
		//Get the new position
		WCVector4 pos(this->_xSuggest, this->_ySuggest, 0.0, 1.0);
		pos = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * pos;
		//Set the end point and line appropriately
		this->_p1->Set(pos);
		this->_line->End(pos);
	}		
}


void WCModeSketchLineCreate::Render(void) {
	//Draw the crosshairs at the current position
	this->_workbench->RenderCrosshairs(this->_xSuggest, this->_ySuggest);
	//Draw any suggested references
	this->_workbench->RenderAlignmentSuggestion(this->_suggestionType, this->_xSuggest, this->_ySuggest);

	//Check to see if drawing
	if (!this->_isDrawing) return;

	//Turn off depth testing
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(WCSketchFeature::LineStippleFactor, WCSketchFeature::LineStipplePattern);
	WPFloat zoom = this->_workbench->Feature()->Document()->Scene()->ActiveCamera()->Zoom();
	//Draw visualization points and line
	this->_p0->Render(0, WCColor::Default(), zoom);
	this->_p1->Render(0, WCColor::Default(), zoom);
	this->_line->Render(0, WCColor::Default(), zoom);
	//Renable depth test
	glDisable(GL_LINE_STIPPLE);
	glEnable(GL_DEPTH_TEST);
}


/***********************************************~***************************************************/

