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
#include "PartDesign/part_plane.h"


/***********************************************~***************************************************/


WCModeSketchArcThreePointCreate::WCModeSketchArcThreePointCreate(WCSketchWorkbench *wb) : 
	::WCDrawingMode(wb->Sketch(), SKETCHARCMODE_THREEPOINTCREATE_NAME), _workbench(wb),
	_stage(0), _first(), _second(), _firstPoint(NULL), _secondPoint(NULL), _thirdPoint(NULL), _arc(NULL), _firstLine(NULL),
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


void WCModeSketchArcThreePointCreate::OnEntry(void) {
	//Clear any selected items
	this->_workbench->SelectionManager()->Clear(true);
	this->_workbench->Sketch()->Document()->Status("Three-Point Arc Creation Mode Entered");
//	CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCSketchArcThreePointCreate::OnEntry - Entering drawing mode.");
}


void WCModeSketchArcThreePointCreate::OnExit(void) {
	//Exit if nothing has been drawn
	if(this->_stage > 0) {
		//Delete first point, second point, and first line
		delete this->_firstPoint;
		delete this->_secondPoint;
		delete this->_firstLine;
		//See if in stage 2
		if (this->_stage > 1) {
			//Delete third point and arc
			delete this->_thirdPoint;
			if (this->_arc != NULL) delete this->_arc;
		}
	}
//	CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCSketchArcThreePointCreate::OnExit - Exiting drawing mode.");
}


void WCModeSketchArcThreePointCreate::OnMouseDown(const WCMouseButton &button) {
	//Restart if right click
	if (button == WCMouseButton::Right()) {
		if(this->_stage == 0) return;
		//Delete first point, second point, and first line
		delete this->_firstPoint;
		delete this->_secondPoint;
		delete this->_firstLine;
		//See if in stage 2
		if (this->_stage > 1) {
			//Delete third point and arc
			delete this->_thirdPoint;
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
		//Create first point
		this->_firstPoint = new WCGeometricPoint(newPoint);
		//Create second point and first line
		this->_secondPoint = new WCGeometricPoint(newPoint);
		this->_firstLine = new WCGeometricLine(newPoint, newPoint);
		//Set the properties of the points and lines
		this->_firstPoint->Color(WCSketchFeature::InprocessColor);
		this->_firstPoint->Size(WCSketchFeature::PointSize);
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
		//Increment the stage
		this->_stage++;
		return;
	}
	//Last click
	else {
		//Calculate center and radius
		WCVector4 center;
		WPFloat radius = WCSketchArc::CalculateCenterAndRadius(this->_firstPoint->Data(), this->_secondPoint->Data(), newPoint, center);
		if (radius == 0.0) {
			//Clean up and wait for good radius
			if (this->_arc != NULL) delete this->_arc;
			this->_arc = NULL;
		}
		//All is good
		else {
			//Recreate arc
			WCVector4 xUnit(1.0, 0.0, 0.0, 0.0);
			WCVector4 yUnit(0.0, 1.0, 0.0, 0.0);
			xUnit = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * xUnit;
			yUnit = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * yUnit;
			xUnit.Normalize(true);
			yUnit.Normalize(true);
			//Back project center onto the plane to determine rotation angles
			WCVector4 planeCenter = this->_workbench->Sketch()->ReferencePlane()->InverseTransformMatrix() * center;
			WPFloat firstAngle = WCSketchArc::PointsToDegrees(planeCenter, this->_first);		
			WPFloat secondAngle = WCSketchArc::PointsToDegrees(planeCenter, this->_second);
			//If not on right, swap angles
			if (!IsOnRight2D(this->_first.I(), this->_first.J(), this->_second.I(), this->_second.J(), this->_workbench->SnapMouseX(), this->_workbench->SnapMouseY()))
				std::swap(firstAngle, secondAngle);
			//Determine center planar coordinates
			center = this->_workbench->Sketch()->ReferencePlane()->InverseTransformMatrix() * center;
			//Create the arc creation action
			WCAction *action = WCSketchArc::ActionCreate(this->_workbench->Sketch(), "", center, radius, firstAngle, secondAngle, 
														 WCVector4(this->_workbench->MouseX(), this->_workbench->MouseY(), 0.0, 1.0));
			//Execute the action
			this->_workbench->Sketch()->Document()->ExecuteAction( action );
			//Clean up the arc
			delete this->_arc;
			this->_arc = NULL;
		}
		//Delete the drawing objects
		delete this->_firstPoint;
		delete this->_secondPoint;
		delete this->_firstLine;
		delete this->_thirdPoint;
		//Reset stage
		this->_stage = 0;
	}
}


void WCModeSketchArcThreePointCreate::OnMouseMove(const WPFloat &x, const WPFloat &y) {
	//Get suggestion from workbench
	this->_xSuggest = this->_workbench->SnapMouseX();
	this->_ySuggest = this->_workbench->SnapMouseY();
	this->_suggestionType = this->_workbench->SuggestAlignment(this->_alignRules, NULL, this->_xSuggest, this->_ySuggest);

	//Init some variables
	WCVector4 newPoint(this->_xSuggest, this->_ySuggest, 0.0, 1.0);
	//Get the updated position
	newPoint = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * newPoint;
	
	//Switch on click stage
	if (this->_stage == 1) {
		//Update second point
		this->_secondPoint->Set(newPoint);
		//Update end of firstLine
		this->_firstLine->End(newPoint);
	}
	//Second click has occured
	else if (this->_stage == 2) {
		//Update third point
		this->_thirdPoint->Set(newPoint);	
		//Calculate center and radius
		WCVector4 center;
		WPFloat radius = WCSketchArc::CalculateCenterAndRadius(this->_firstPoint->Data(), this->_secondPoint->Data(), this->_thirdPoint->Data(), center);
		if (radius == 0.0) {
			//Clean up and wait for good radius
			if (this->_arc != NULL) delete this->_arc;
			this->_arc = NULL;
			//Make first line visible
			this->_firstLine->IsVisible(true);
		}
		else {
			//Make first line not visible
			this->_firstLine->IsVisible(false);
			//Recreate arc
			WCVector4 xUnit(1.0, 0.0, 0.0, 0.0);
			WCVector4 yUnit(0.0, 1.0, 0.0, 0.0);
			xUnit = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * xUnit;
			yUnit = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * yUnit;
			xUnit.Normalize(true);
			yUnit.Normalize(true);
			//Back project center onto the plane to determine rotation angles
			WCVector4 planeCenter = this->_workbench->Sketch()->ReferencePlane()->InverseTransformMatrix() * center;
			WPFloat firstAngle = WCSketchArc::PointsToDegrees(planeCenter, this->_first);		
			WPFloat secondAngle = WCSketchArc::PointsToDegrees(planeCenter, this->_second);
			//If not on right, then swap first and second
			if (!IsOnRight2D(this->_first.I(), this->_first.J(), this->_second.I(), this->_second.J(), this->_workbench->SnapMouseX(), this->_workbench->SnapMouseY()))
				std::swap(firstAngle, secondAngle);
			//Delete the old arc
			if (this->_arc != NULL) delete this->_arc;
			//Create the new one
			this->_arc = WCNurbsCurve::CircularArc(this->_creator->Document()->Scene()->GeometryContext(), 
				center, xUnit, yUnit, radius, firstAngle, secondAngle);
			this->_arc->Color(WCSketchFeature::InprocessColor);
			this->_arc->Thickness(WCSketchFeature::LineThickness);
//			this->_arc->Layer( this->_workbench->Layer() );
		}
	}
}


void WCModeSketchArcThreePointCreate::Render(void) {
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
		this->_firstPoint->Render(0, WCColor::Default(), zoom);
		this->_secondPoint->Render(0, WCColor::Default(), zoom);
		
		//See if 2nd click has occured
		if (this->_stage > 1) {
			//Draw second click objects
			if (this->_arc != NULL) this->_arc->Render(0, WCColor::Default(), zoom);
			this->_thirdPoint->Render(0, WCColor::Default(), zoom);
		}
		//Renable depth test
		glDisable(GL_LINE_STIPPLE);
		glEnable(GL_DEPTH_TEST);
	}
}


/***********************************************~***************************************************/

