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
#include "Sketcher/sketch_conic_modes.h"
#include "Sketcher/sketch_conic_twopoint.h"
#include "Sketcher/sketch_workbench.h"
#include "Sketcher/sketch.h"
#include "Kernel/document.h"
#include "PartDesign/part_plane.h"


/***********************************************~***************************************************/


WCModeSketchConicTwoPointCreate::WCModeSketchConicTwoPointCreate(WCSketchWorkbench *wb) : 
	::WCDrawingMode(wb->Sketch(), SKETCHCONIC_TWOPOINT_CREATE_MODENAME), _workbench(wb), _stage(0),
	_first(), _second(), _third(), _fourth(),
	_p1(NULL), _p2(NULL), _p3(NULL), _p4(NULL), _p5(NULL), _line1(NULL), _line2(NULL), _conic(NULL),
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


void WCModeSketchConicTwoPointCreate::OnEntry(void) {
	//Clear any selected items
	this->_workbench->SelectionManager()->Clear(true);
	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Entering Sketch Conic(Two-Point) Draw Mode.");
}


void WCModeSketchConicTwoPointCreate::OnExit(void) {
	//Post-first click
	if (this->_stage > 0) {
		//Delete first and second points, and first line
		delete this->_p1;
		delete this->_p2;
		delete this->_line1;
		//Post second click
		if (this->_stage > 1) {
			//Delete third point
			delete this->_p3;
			//Post third click
			if (this->_stage > 2) {
				//Delete fourth points, and second line
				delete this->_p4;
				delete this->_line2;
				//Post fourth click
				if (this->_stage > 3) {
					//Delete fifth point and conic
					delete this->_p5;
					if (this->_conic != NULL) delete this->_conic;
				}
			}
		}
	}
	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Exiting Sketch Conic(Two-Point) Draw Mode.");
}


void WCModeSketchConicTwoPointCreate::OnMouseDown(const WCMouseButton &button) {
	//Restart if right click
	if (button == WCMouseButton::Right()) {
		if(this->_stage == 0) return;
		//Delete first and second points, and first line
		delete this->_p1;
		delete this->_p2;
		delete this->_line1;
		//Post second click
		if (this->_stage > 1) {
			//Delete third point
			delete this->_p3;
			//Post third click
			if (this->_stage > 2) {
				//Delete fourth points, and second line
				delete this->_p4;
				delete this->_line2;
				//Post fourth click
				if (this->_stage > 3) {
					//Delete fifth point and conic
					delete this->_p5;
					if (this->_conic != NULL) {
						delete this->_conic;
						this->_conic = NULL;
					}
				}
			}
		}
		//Reset the stage
		this->_stage = 0;
		return;
	}
	else if (button != WCMouseButton::Left()) return;
	
	if (this->_stage == 0) {
		//Record location
		this->_first = WCVector4(this->_xSuggest, this->_ySuggest, 0.0, 1.0);
		WCVector4 pos( this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * this->_first );
		//Create first and second point
		this->_p1 = new WCGeometricPoint(pos);
		this->_p1->Color(WCSketchFeature::InprocessColor);
		this->_p1->Size(WCSketchFeature::PointSize);
		this->_p2 = new WCGeometricPoint(pos);
		this->_p2->Color(WCSketchFeature::InprocessColor);
		this->_p2->Size(WCSketchFeature::PointSize);
		//Create the first line
		this->_line1 = new WCGeometricLine(pos, pos);
		this->_line1->Color(WCSketchFeature::InprocessColor);
		this->_line1->Thickness(WCSketchFeature::LineThickness);
		//Increment the stage
		this->_stage++;
	}
	else if (this->_stage == 1) {
		//Record location
		this->_second = WCVector4(this->_xSuggest, this->_ySuggest, 0.0, 1.0);
		WCVector4 pos( this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * this->_second );
		//Create third point
		this->_p3 = new WCGeometricPoint(pos);
		this->_p3->Color(WCSketchFeature::InprocessColor);
		this->_p3->Size(WCSketchFeature::PointSize);
		//Increment the stage
		this->_stage++;	
	}
	else if (this->_stage == 2) {
		//Record location
		this->_third = WCVector4(this->_xSuggest, this->_ySuggest, 0.0, 1.0);
		WCVector4 pos( this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * this->_third );
		//Create fourth point and second line
		this->_p4 = new WCGeometricPoint(pos);
		this->_p4->Color(WCSketchFeature::InprocessColor);
		this->_p4->Size(WCSketchFeature::PointSize);
		this->_line2 = new WCGeometricLine(pos, pos);
		this->_line2->Color(WCSketchFeature::InprocessColor);
		this->_line2->Thickness(WCSketchFeature::LineThickness);
		//Increment the stage
		this->_stage++;
	}
	else if (this->_stage == 3) {
		//Record location
		this->_fourth = WCVector4(this->_xSuggest, this->_ySuggest, 0.0, 1.0);
		WCVector4 pos( this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * this->_fourth );
		//Create fifth point
		this->_p5 = new WCGeometricPoint(pos);
		this->_p5->Color(WCSketchFeature::InprocessColor);
		this->_p5->Size(WCSketchFeature::PointSize);
		//Create the visualization conic
		WCVector4 firstTangent = (this->_p2->Data() - this->_p1->Data()).Normalize(true);
		WCVector4 secondTangent = (this->_p4->Data() - this->_p3->Data()).Normalize(true);
		this->_conic = WCNurbsCurve::Conic(this->_creator->Document()->Scene()->GeometryContext(),
			this->_p1->Data(), firstTangent, this->_p3->Data(), secondTangent, this->_p5->Data());
		//Check to make sure non-null return
		if (this->_conic != NULL) {
			this->_conic->Color(WCSketchFeature::InprocessColor);
			this->_conic->Thickness(WCSketchFeature::LineThickness);
		}		
		//Increment the stage
		this->_stage++;
	}
	else if (this->_stage == 4) {
		//Only execute if there is a valid conic currently
		if (this->_conic != NULL) {
			//Get the last click
			WCVector4 fifth(this->_xSuggest, this->_ySuggest, 0.0, 1.0);
			//Find the tangents
			WCVector4 firstTangent = (this->_second - this->_first).Normalize(true);
			WCVector4 secondTangent = (this->_fourth - this->_third).Normalize(true);
			//Create and execute action
			WCAction *action = WCSketchConicTwoPoint::ActionCreate(this->_workbench->Sketch(), "", this->_first, firstTangent,
				this->_third, secondTangent, fifth);
			this->_workbench->Sketch()->Document()->ExecuteAction(action);
			//Destroy the visualization conic
			delete this->_conic;
			this->_conic = NULL;
		}
		//Destroy all objects
		delete this->_p1;
		delete this->_p2;
		delete this->_line1;
		delete this->_p3;
		delete this->_p4;
		delete this->_line2;
		delete this->_p5;
		//Reset the stage
		this->_stage = 0;
	}
}


void WCModeSketchConicTwoPointCreate::OnMouseMove(const WPFloat &x, const WPFloat &y) {
	//Get suggestion from workbench
	this->_xSuggest = this->_workbench->SnapMouseX();
	this->_ySuggest = this->_workbench->SnapMouseY();
	this->_suggestionType = this->_workbench->SuggestAlignment(this->_alignRules, NULL, this->_xSuggest, this->_ySuggest);

	//Init some variables
	WCVector4 newPoint(this->_xSuggest, this->_ySuggest, 0.0, 1.0);
	//Get the updated position
	newPoint = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * newPoint;

	if (this->_stage == 1) {
		//Move point2 and line1-end
		this->_p2->Set(newPoint);
		this->_line1->End(newPoint);
	}
	else if (this->_stage == 2) {
		//Move point 3
		this->_p3->Set(newPoint);
	}
	else if (this->_stage == 3) {
		//Move point4 and line2-end
		this->_p4->Set(newPoint);
		this->_line2->End(newPoint);
	}
	else if (this->_stage == 4) {
		//Move point5 and regenerate conic
		this->_p5->Set(newPoint);
		if (this->_conic != NULL) {
			//Delete the existing conic
			delete this->_conic;
		}
		//Try to create the visualization conic
		WCVector4 firstTangent = (this->_p2->Data() - this->_p1->Data()).Normalize(true);
		WCVector4 secondTangent = (this->_p4->Data() - this->_p3->Data()).Normalize(true);
		this->_conic = WCNurbsCurve::Conic(this->_creator->Document()->Scene()->GeometryContext(),
			this->_p1->Data(), firstTangent, this->_p3->Data(), secondTangent, this->_p5->Data());
		//Check to make sure non-null return
		if (this->_conic != NULL) {
			this->_conic->Color(WCSketchFeature::InprocessColor);
			this->_conic->Thickness(WCSketchFeature::LineThickness);
		}
	}
}


void WCModeSketchConicTwoPointCreate::Render(void) {
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
	this->_line1->Render(0, WCColor::Default(), zoom);
	//If post second click
	if (this->_stage > 1) {
		//Draw point3
		this->_p3->Render(0, WCColor::Default(), zoom);
		//if post third click
		if (this->_stage > 2) {
			//Draw point4 and line2
			this->_p4->Render(0, WCColor::Default(), zoom);
			this->_line2->Render(0, WCColor::Default(), zoom);
			//If pose fourth click
			if (this->_stage > 3) {
				//Draw point5 and regenerate conic
				this->_p5->Render(0, WCColor::Default(), zoom);
				if (this->_conic != NULL) this->_conic->Render(0, WCColor::Default(), zoom);
			}
		}
	}

	//Renable depth test
	glDisable(GL_LINE_STIPPLE);
	glEnable(GL_DEPTH_TEST);
}


/***********************************************~***************************************************/

