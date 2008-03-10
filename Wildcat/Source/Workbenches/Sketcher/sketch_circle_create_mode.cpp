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
#include "Sketcher/sketch_circle_modes.h"
#include "Sketcher/sketch_circle.h"
#include "Sketcher/sketch_workbench.h"
#include "Sketcher/sketch.h"
#include "Kernel/document.h"
#include "Kernel/workbench_layer.h"
#include "PartDesign/plane.h"


/***********************************************~***************************************************/


WCModeSketchCircleCreate::WCModeSketchCircleCreate(WCSketchWorkbench *wb) : 
	::WCDrawingMode(wb->Sketch(), SKETCHCIRCLEMODE_CREATE_NAME), _workbench(wb),
	_isDrawing(false), _beginX(0.0), _beginY(0.0), _radius(0.0), _center(NULL), _circle(NULL),
	_xSuggest(0.0), _ySuggest(0.0), _suggestionType(WCSketchAlignmentSuggestion::None()) {
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


void WCModeSketchCircleCreate::OnEntry(void) {
	//Clear any selected items
	this->_workbench->SelectionManager()->Clear(true);
	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Entering SketchCircle Draw Mode.");
}


void WCModeSketchCircleCreate::OnExit(void) {
	//Exit if nothing has been drawn
	if(this->_isDrawing) {
		//Delete circle
		delete this->_circle;
		//Delete center point
		delete this->_center;
	}
	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Exiting SketchCircle Draw Mode.");
}


void WCModeSketchCircleCreate::OnMouseDown(const WCMouseButton &button) {
	//Restart if right click
	if (button == WCMouseButton::Right()) {
		if(!this->_isDrawing) return;
		//Delete first point, second point, and first line
		delete this->_center;
		delete this->_circle;
		//Reset the drawing flag
		this->_isDrawing = false;
		return;
	}
	else if (button != WCMouseButton::Left()) return;

	//See if already drawing
	if (!this->_isDrawing) {
		//Get the position
		this->_beginX = this->_xSuggest;
		this->_beginY = this->_ySuggest;
		//Get the center
		WCVector4 center(this->_beginX, this->_beginY, 0.0, 1.0);
		WCVector4 xUnit(1.0, 0.0, 0.0, 0.0);
		WCVector4 yUnit(0.0, 1.0, 0.0, 0.0);
		center = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * center;
		xUnit = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * xUnit;
		yUnit = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * yUnit;
		xUnit.Normalize(true);
		yUnit.Normalize(true);
		//Create display circle and point
		this->_circle = WCNurbsCurve::CircularArc(this->_creator->Document()->Scene()->GeometryContext(),
			center, xUnit, yUnit, 0.0001, 0.0, 360.0);
		this->_center = new WCGeometricPoint(center);
		//Set the center and circle color and size
		this->_circle->Color(WCSketchFeature::InprocessColor);
		this->_circle->Thickness(WCSketchFeature::LineThickness);
		this->_center->Color(WCSketchFeature::InprocessColor);
		this->_center->Size(WCSketchFeature::PointSize);
		//Mark as drawing
		this->_isDrawing = true;
	}
	//Otherwise, create the circle and reset
	else {
		//Delete circle and point
		delete this->_center;
		delete this->_circle;
		//Execute the action
		WCVector4 begin(this->_beginX, this->_beginY, 0.0, 1.0);
		WCActionSketchCircleCreate *action = WCSketchCircle::ActionCreate(this->_workbench->Sketch(), "", 
																		  begin, this->_radius);
		this->_workbench->Sketch()->Document()->ExecuteAction( action );
		//Mark as no longer drawing
		this->_isDrawing = false;
	}
}


void WCModeSketchCircleCreate::OnMouseMove(const WPFloat &x, const WPFloat &y) {
	//Get suggestion from workbench
	this->_xSuggest = this->_workbench->SnapMouseX();
	this->_ySuggest = this->_workbench->SnapMouseY();
	this->_suggestionType = this->_workbench->SuggestAlignment(this->_alignRules, NULL, this->_xSuggest, this->_ySuggest);
	
	//Move p1 if we are drawing
	if (this->_isDrawing) {
		//Delete the current circle
		delete this->_circle;		
		//Calculate the radius
		this->_radius = sqrt( (this->_xSuggest - this->_beginX) * (this->_xSuggest - this->_beginX) + 
							   (this->_ySuggest - this->_beginY) * (this->_ySuggest - this->_beginY) );
		this->_radius = std::max(0.0001, this->_radius);
		//Update the circle
		WCVector4 center(this->_beginX, this->_beginY, 0.0, 1.0);
		WCVector4 xUnit(1.0, 0.0, 0.0, 0.0);
		WCVector4 yUnit(0.0, 1.0, 0.0, 0.0);
		center = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * center;
		xUnit = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * xUnit;
		yUnit = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * yUnit;
		xUnit.Normalize(true);
		yUnit.Normalize(true);
		//Create display circle and point
		this->_circle = WCNurbsCurve::CircularArc(this->_creator->Document()->Scene()->GeometryContext(),
			center, xUnit, yUnit, this->_radius, 0.0, 360.0);
		//Set the center and circle color and size
		this->_circle->Color(WCSketchFeature::InprocessColor);
		this->_circle->Thickness(WCSketchFeature::LineThickness);
	}
}


void WCModeSketchCircleCreate::Render(void) {
	//Draw the crosshairs at the current position
	this->_workbench->RenderCrosshairs(this->_xSuggest, this->_ySuggest);
	//Draw any suggested references
	this->_workbench->RenderAlignmentSuggestion(this->_suggestionType, this->_xSuggest, this->_ySuggest);
	//Draw the circle if editing
	if (this->_isDrawing) {
		//Turn off depth testing
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(WCSketchFeature::LineStippleFactor, WCSketchFeature::LineStipplePattern);
		WPFloat zoom = this->_workbench->Feature()->Document()->Scene()->ActiveCamera()->Zoom();
		//Draw the center point
		this->_center->Render(0, WCColor::Default(), zoom);
		//Draw pending curve
		this->_circle->Render(0, WCColor::Default(), zoom);
		//Renable depth test
		glDisable(GL_LINE_STIPPLE);
		glEnable(GL_DEPTH_TEST);
	}
}


/***********************************************~***************************************************/

