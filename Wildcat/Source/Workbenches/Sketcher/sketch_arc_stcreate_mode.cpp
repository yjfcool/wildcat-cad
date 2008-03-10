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
#include "Sketcher/sketch_arc_modes.h"
#include "Sketcher/sketch_arc.h"
#include "Sketcher/sketch_workbench.h"
#include "Sketcher/sketch.h"
#include "Kernel/document.h"
#include "Kernel/workbench_layer.h"
#include "PartDesign/plane.h"


/***********************************************~***************************************************/


WCModeSketchArcStandardCreate::WCModeSketchArcStandardCreate(WCSketchWorkbench *wb) : 
	::WCDrawingMode(wb->Sketch(), SKETCHARCMODE_STANDARDCREATE_NAME), _workbench(wb),
	_stage(0), _first(), _second(), _centerPoint(NULL), _secondPoint(NULL), _thirdPoint(NULL), 
	_arc(NULL), _firstLine(NULL), _secondLine(NULL),
	_xSuggest(0.0), _ySuggest(0.0), _suggestionType(WCSketchAlignmentSuggestion::None()), _peepRight(true) {
	//Add in alignment suggestion rules
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::CoincidentToPoint());
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::CoincidentToLine());
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::CoincidentToCurve());
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::HorizontalToPoint());
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::VerticalToPoint());
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::HorizontalToLine());
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::VerticalToLine());
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::HorizontalToCurve());
	this->_alignRules.push_back(WCSketchAlignmentSuggestion::VerticalToCurve());
}


void WCModeSketchArcStandardCreate::OnEntry(void) {
	//Clear any selected items
	this->_workbench->SelectionManager()->Clear(true);
	this->_workbench->Sketch()->Document()->Status("Arc Creation Mode Entered");
//	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Entering SketchArc Draw Mode.");
}


void WCModeSketchArcStandardCreate::OnExit(void) {
	//Exit if nothing has been drawn
	if(this->_stage > 0) {
		//Delete center point, second point, and first line
		delete this->_centerPoint;
		delete this->_secondPoint;
		delete this->_firstLine;
		//See if in stage 2
		if (this->_stage > 1) {
			//Delete third point and arc
			delete this->_thirdPoint;
			delete this->_secondLine;
			delete this->_arc;
		}
	}
//	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Exiting SketchArc Draw Mode.");
}


void WCModeSketchArcStandardCreate::OnMouseDown(const WCMouseButton &button) {
	//Restart if right click
	if (button == WCMouseButton::Right()) {
		if(this->_stage == 0) return;
		//Delete center point, second point, and first line
		delete this->_centerPoint;
		delete this->_secondPoint;
		delete this->_firstLine;
		//See if in stage 2
		if (this->_stage > 1) {
			//Delete third point and arc
			delete this->_thirdPoint;
			delete this->_secondLine;
			if (this->_arc != NULL) {
				delete this->_arc;
				this->_arc = NULL;
			}
		}
		//Reset the stage
		this->_stage = 0;
		return;
	}
	else if (button != WCMouseButton::Left()) return;

	//Init some variables
	WCVector4 newPoint(this->_workbench->SnapMouseX(), this->_workbench->SnapMouseY(), 0.0, 1.0);
	newPoint = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * newPoint;

	//Switch on first click
	if (this->_stage == 0) {
		//Get the position
		this->_first.I( this->_workbench->SnapMouseX() );
		this->_first.J( this->_workbench->SnapMouseY() );
		//Create center point
		this->_centerPoint = new WCGeometricPoint(newPoint);
		//Create second point and first line
		this->_secondPoint = new WCGeometricPoint(newPoint);
		this->_firstLine = new WCGeometricLine(newPoint, newPoint);
		//Set the properties of the points and lines
		this->_centerPoint->Color(WCSketchFeature::InprocessColor);
		this->_centerPoint->Size(WCSketchFeature::PointSize);
		this->_secondPoint->Color(WCSketchFeature::InprocessColor);
		this->_secondPoint->Size(WCSketchFeature::PointSize);
		this->_firstLine->Color(WCSketchFeature::InprocessColor);
		this->_firstLine->Thickness(WCSketchFeature::LineThickness);
		//Increment stage
		this->_stage++;
		return;
	}
	//Second click
	else if (this->_stage == 1) {
		//Record second click
		this->_second.I( this->_workbench->SnapMouseX() );
		this->_second.J( this->_workbench->SnapMouseY() );
		//Create the third point and second line
		this->_thirdPoint = new WCGeometricPoint(newPoint);
		WCVector4 center = this->_centerPoint->Data();
		this->_secondLine = new WCGeometricLine(center, newPoint);
		//Set the properties of points and lines
		this->_thirdPoint->Color(WCSketchFeature::InprocessColor);
		this->_thirdPoint->Size(WCSketchFeature::PointSize);
		this->_secondLine->Color(WCSketchFeature::InprocessColor);
		this->_secondLine->Thickness(WCSketchFeature::LineThickness);
		//Increment the stage
		this->_stage++;
		return;
	}
	//Last click
	else {
		//Calculate the radius
		WPFloat radius = this->_second.Distance(this->_first);
		//Get direction of click
		WCVector4 unit(this->_workbench->SnapMouseX() - this->_first.I(), this->_workbench->SnapMouseY() - this->_first.J(), 0.0, 1.0);
		unit.Normalize(true);
		//Project click onto arc
		WCVector4 center = this->_workbench->Sketch()->ReferencePlane()->InverseTransformMatrix() * this->_centerPoint->Data();
		WCVector4 newPoint = center + (unit * radius);
		//Execute the action
		WPFloat firstAngle = WCSketchArc::PointsToDegrees(this->_first, this->_second);
		WPFloat secondAngle = WCSketchArc::PointsToDegrees(this->_first, newPoint);
		//Make sure angle is not too small
		if (abs(firstAngle - secondAngle ) < 0.01) {
			//Clean up and wait for good radius
			if (this->_arc != NULL) delete this->_arc;
			this->_arc = NULL;
		}
		else {
			if (this->_peepRight) std::swap(firstAngle, secondAngle);
			WCAction *action = WCSketchArc::ActionCreate(this->_workbench->Sketch(), "", center, radius, firstAngle, secondAngle);
			this->_workbench->Sketch()->Document()->ExecuteAction( action );
			//Clean up the arc
			delete this->_arc;
			this->_arc = NULL;
		}
		//Delete the drawing objects
		delete this->_centerPoint;
		delete this->_secondPoint;
		delete this->_firstLine;
		delete this->_thirdPoint;
		delete this->_secondLine;
		//Reset stage
		this->_stage = 0;
	}
}


void WCModeSketchArcStandardCreate::OnMouseMove(const WPFloat &x, const WPFloat &y) {
	//Switch on click stage
	if (this->_stage == 1) {
		//Init some variables
		WCVector4 newPoint(this->_workbench->SnapMouseX(), this->_workbench->SnapMouseY(), 0.0, 1.0);
		//Get the updated position
		newPoint = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * newPoint;	
		//Update second point
		this->_secondPoint->Set(newPoint);
		//Update end of firstLine
		this->_firstLine->End(newPoint);
	}
	//Second click has occured
	else if (this->_stage == 2) {
		//Calculate the radius
		WPFloat radius = this->_second.Distance(this->_first);
		//Get direction of click
		WCVector4 unit(this->_workbench->SnapMouseX() - this->_first.I(), this->_workbench->SnapMouseY() - this->_first.J(), 0.0, 1.0);
		unit.Normalize(true);
		//Project click onto arc
		WCVector4 center = this->_workbench->Sketch()->ReferencePlane()->InverseTransformMatrix() * this->_centerPoint->Data();
		WCVector4 newPoint = center + (unit * radius);
		newPoint.L(1.0);
		WCVector4 newPointProj = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * newPoint;
		//Update third point
		this->_thirdPoint->Set(newPointProj);			
		//Update second line
		this->_secondLine->End(newPointProj);
		//Check angles
		WPFloat firstAngle = WCSketchArc::PointsToDegrees(this->_first, this->_second);		
		WPFloat secondAngle = WCSketchArc::PointsToDegrees(this->_first, newPoint);
		if (abs(firstAngle - secondAngle) < 0.01) {
			//Clean up and wait for good times
			if (this->_arc != NULL) delete this->_arc;
			this->_arc = NULL;
		}
		else {
			//Check peep
			if (this->_arc == NULL)
				this->_peepRight = IsOnRight2D(this->_first.I(), this->_first.J(), this->_second.I(), this->_second.J(), 
												this->_workbench->SnapMouseX(), this->_workbench->SnapMouseY());
			//Recreate arc
			WCVector4 xUnit(1.0, 0.0, 0.0, 0.0);
			WCVector4 yUnit(0.0, 1.0, 0.0, 0.0);
			xUnit = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * xUnit;
			yUnit = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * yUnit;
			xUnit.Normalize(true);
			yUnit.Normalize(true);
			if (this->_peepRight) std::swap(firstAngle, secondAngle);
			//Delete the old arc
			if (this->_arc != NULL) delete this->_arc;
			//Create the new one
			this->_arc = WCNurbsCurve::CircularArc(this->_creator->Document()->Scene()->GeometryContext(), 
												   this->_centerPoint->Data(), xUnit, yUnit, radius, firstAngle, secondAngle);
			this->_arc->Color(WCSketchFeature::InprocessColor);
			this->_arc->Thickness(WCSketchFeature::LineThickness);
		}
	}
	//Get suggestion from workbench
	this->_xSuggest = this->_workbench->SnapMouseX();
	this->_ySuggest = this->_workbench->SnapMouseY();
	this->_suggestionType = this->_workbench->SuggestAlignment(this->_alignRules, NULL, this->_xSuggest, this->_ySuggest);
}


void WCModeSketchArcStandardCreate::Render(void) {
	//Draw the crosshairs at the current position
	this->_workbench->RenderCrosshairs(this->_xSuggest, this->_ySuggest);
	//Draw any suggested references
	this->_workbench->RenderAlignmentSuggestion(this->_suggestionType, this->_xSuggest, this->_ySuggest);
	
	//Draw the arc if editing
	if (this->_stage > 0) {
		//Turn off depth testing
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(WCSketchFeature::LineStippleFactor, WCSketchFeature::LineStipplePattern);
		//Draw the first click objects
		WPFloat zoom = this->_workbench->Feature()->Document()->Scene()->ActiveCamera()->Zoom();
		this->_firstLine->Render(0, WCColor::Default(), zoom);
		this->_centerPoint->Render(0, WCColor::Default(), zoom);
		this->_secondPoint->Render(0, WCColor::Default(), zoom);
		
		//See if 2nd click has occured
		if (this->_stage > 1) {
			//Draw second click objects
			if (this->_arc != NULL) this->_arc->Render(0, WCColor::Default(), zoom);
			this->_secondLine->Render(0, WCColor::Default(), zoom);
			this->_thirdPoint->Render(0, WCColor::Default(), zoom);
		}
		//Renable depth test
		glDisable(GL_LINE_STIPPLE);
		glEnable(GL_DEPTH_TEST);
	}
}


/***********************************************~***************************************************/

