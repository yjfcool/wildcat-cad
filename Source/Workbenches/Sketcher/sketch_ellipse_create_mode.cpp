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
#include <PartDesign/part_plane.h>


/***********************************************~***************************************************/


WCModeSketchEllipseCreate::WCModeSketchEllipseCreate(WCSketchWorkbench *wb) : 
	::WCDrawingMode(wb->Sketch(), SKETCHELLIPSEMODE_CREATE_NAME), _workbench(wb), _stage(0),
	_center(), _first(), _p1(NULL), _p2(NULL), _p3(NULL), _l1(NULL), _l2(NULL), _curve(NULL), 
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


void WCModeSketchEllipseCreate::OnEntry(void) {
	//Clear any selected items
	this->_workbench->SelectionManager()->Clear(true);
	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Entering SketchEllipse Draw Mode.");
}


void WCModeSketchEllipseCreate::OnExit(void) {
	//Post-first click
	if (this->_stage > 0) {
		//Delete first and second points, and first line
		delete this->_p1;
		delete this->_p2;
		delete this->_l1;
		//Post second click
		if (this->_stage > 1) {
			//Delete third point
			delete this->_p3;
			delete this->_l2;
			if (this->_curve != NULL) delete this->_curve;
		}
	}
	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Exiting SketchEllipse Draw Mode.");
}


void WCModeSketchEllipseCreate::OnMouseDown(const WCMouseButton &button) {
	//Restart if right click
	if (button == WCMouseButton::Right()) {
		if(this->_stage == 0) return;
		//Delete first and second points, and first line
		delete this->_p1;
		delete this->_p2;
		delete this->_l1;
		//Post second click
		if (this->_stage > 1) {
			//Delete third point
			delete this->_p3;
			delete this->_l2;
			if (this->_curve != NULL) {
				delete this->_curve;
				this->_curve = NULL;
			}
		}
		//Reset the stage
		this->_stage = 0;
		return;
	}
	else if (button != WCMouseButton::Left()) return;
	
	if (this->_stage == 0) {
		//Record location
		this->_center = WCVector4(this->_xSuggest, this->_ySuggest, 0.0, 1.0);
		WCVector4 pos = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * this->_center ;
		//Create first and second point
		this->_p1 = new WCGeometricPoint(pos);
		this->_p1->Color(WCSketchFeature::InprocessColor);
		this->_p1->Size(WCSketchFeature::PointSize);
		this->_p2 = new WCGeometricPoint(pos);
		this->_p2->Color(WCSketchFeature::InprocessColor);
		this->_p2->Size(WCSketchFeature::PointSize);
		//Create the first line
		this->_l1 = new WCGeometricLine(pos, pos);
		this->_l1->Color(WCSketchFeature::InprocessColor);
		this->_l1->Thickness(WCSketchFeature::LineThickness);
		//Increment the stage
		this->_stage++;
	}
	else if (this->_stage == 1) {
		//Record location
		this->_first = WCVector4(this->_xSuggest, this->_ySuggest, 0.0, 1.0);
		WCVector4 pos = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * this->_first;
		//Create third point
		this->_p3 = new WCGeometricPoint(pos);
		this->_p3->Color(WCSketchFeature::InprocessColor);
		this->_p3->Size(WCSketchFeature::PointSize);
		this->_l2 = new WCGeometricLine(this->_p1->Data(), pos);
		this->_l2->Color(WCSketchFeature::InprocessColor);
		this->_l2->Thickness(WCSketchFeature::LineThickness);
		//Increment the stage
		this->_stage++;
	}
	else if (this->_stage == 2) {
		//Only execute if there is a valid conic currently
		if (this->_curve != NULL) {
			//Find the semiMajor radius
			WPFloat semiMajor = this->_first.Distance(this->_center);
			//Get distance from mouse cursor to primary line (semiMinor radius)
			WPFloat semiMinor = PointToLineDistance2D(this->_center.I(), this->_center.J(), this->_first.I(), this->_first.J(), 
													  this->_xSuggest, this->_ySuggest, false);
			//Get unit direction of primary line
			WCVector4 dir = WCVector4(this->_first.I() - this->_center.I(), this->_first.J()-this->_center.J(), 0.0, 0.0) / semiMajor;
			//Create and execute action
			WCAction *action = WCSketchEllipse::ActionCreate(this->_workbench->Sketch(), "", this->_center, dir, semiMajor, semiMinor);
															 this->_workbench->Sketch()->Document()->ExecuteAction(action);
			//Destroy the visualization conic
			if (this->_curve != NULL) delete this->_curve;
			this->_curve = NULL;
		}
		//Destroy all objects
		delete this->_p1;
		delete this->_p2;
		delete this->_l1;
		delete this->_l2;
		//Reset the stage
		this->_stage = 0;
	}
}


void WCModeSketchEllipseCreate::OnMouseMove(const WPFloat &x, const WPFloat &y) {
	//Get suggestion from workbench
	this->_xSuggest = this->_workbench->SnapMouseX();
	this->_ySuggest = this->_workbench->SnapMouseY();
	this->_suggestionType = this->_workbench->SuggestAlignment(this->_alignRules, NULL, this->_xSuggest, this->_ySuggest);

	//If just the first click has occured
	if (this->_stage == 1) {
		//Init some variables
		WCVector4 newPoint(this->_xSuggest, this->_ySuggest, 0.0, 1.0);
		//Get the updated position
		newPoint = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * newPoint;
		//Move point2 and line1-end
		this->_p2->Set(newPoint);
		this->_l1->End(newPoint);
	}
	//If the second click has happened
	else if (this->_stage == 2) {
		WCVector4 center = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * this->_center;
		//Find the semiMajor radius
		WPFloat semiMajor = this->_center.Distance(this->_first);
		//Get distance from mouse cursor to primary line (semiMinor radius)
		WPFloat semiMinor = PointToLineDistance2D(this->_center.I(), this->_center.J(), this->_first.I(), this->_first.J(), 
												  this->_xSuggest, this->_ySuggest, false);
		//Get unit direction of primary line and aux line
		WCVector4 xUnit = WCVector4(this->_first.I() - this->_center.I(), this->_first.J() - this->_center.J(), 0.0, 0.0) / semiMajor;
		WCVector4 yUnit(xUnit.J(), -xUnit.I(), 0.0, 0.0);
		xUnit = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * xUnit;
		yUnit = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * yUnit;
		//Get position of 3rd point
		WCVector4 pos = center + (yUnit * semiMinor);
		//Move point 3
		this->_p3->Set(pos);
		this->_l2->End(pos);
		//Delete exsiting curve if present
		if (this->_curve != NULL) {
			delete this->_curve;
			this->_curve = NULL;
		}
		//Do a bit of error checking
		if ((semiMajor > 0.001) && (fabs(semiMinor) > 0.001)) {
			//Create the visualization ellipse
			this->_curve = WCNurbsCurve::Ellipse(this->_creator->Document()->Scene()->GeometryContext(),
												 center, xUnit, yUnit, semiMajor, semiMinor);
			//Check to make sure non-null return
			if (this->_curve != NULL) {
				this->_curve->Color(WCSketchFeature::InprocessColor);
				this->_curve->Thickness(WCSketchFeature::LineThickness);
			}
		}
	}
}


void WCModeSketchEllipseCreate::Render(void) {
	//Draw the crosshairs at the current position
	this->_workbench->RenderCrosshairs(this->_xSuggest, this->_ySuggest);
	//Draw any suggested references
	this->_workbench->RenderAlignmentSuggestion(this->_suggestionType, this->_xSuggest, this->_ySuggest);
	
	if (this->_stage == 0) return;
	//Turn off depth testing
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(WCSketchFeature::LineStippleFactor, WCSketchFeature::LineStipplePattern);
	WPFloat zoom = this->_workbench->Feature()->Document()->Scene()->ActiveCamera()->Zoom();

	//Draw point1, point2, and line1
	this->_p1->Render(0, WCColor::Default(), zoom);
	this->_p2->Render(0, WCColor::Default(), zoom);
	this->_l1->Render(0, WCColor::Default(), zoom);
	//If post second click
	if (this->_stage > 1) {
		//Draw point3
		this->_p3->Render(0, WCColor::Default(), zoom);
		this->_l2->Render(0, WCColor::Default(), zoom);
		if (this->_curve != NULL) this->_curve->Render(0, WCColor::Default(), zoom);
	}

	//Renable depth test
	glDisable(GL_LINE_STIPPLE);
	glEnable(GL_DEPTH_TEST);
}


/***********************************************~***************************************************/

