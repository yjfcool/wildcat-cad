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
#include "Sketcher/sketch_axis_modes.h"
#include "Sketcher/sketch_axis.h"
#include "Sketcher/sketch_workbench.h"
#include "Sketcher/sketch.h"
#include "Kernel/document.h"
#include "PartDesign/plane.h"


/***********************************************~***************************************************/


WCModeSketchAxisCreate::WCModeSketchAxisCreate(WCSketchWorkbench *wb) : 
	::WCDrawingMode(wb->Sketch(), SKETCHAXISMODE_CREATE_NAME), _workbench(wb), _isDrawing(false),
	_begin(), _p0(NULL), _p1(NULL), _line(NULL), _xSuggest(0.0), _ySuggest(0.0), _suggestionType(WCSketchAlignmentSuggestion::None()) { 
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


void WCModeSketchAxisCreate::OnEntry(void) {
	this->_workbench->Sketch()->Document()->Status("Axis Creation Mode Entered");
//	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Entering SketchAxis Draw Mode.");
	//Check to see if line is already selected
	//Clear any selected items
	this->_workbench->SelectionManager()->Clear(true);
}


void WCModeSketchAxisCreate::OnExit(void) {
//	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Exiting SketchAxis Draw Mode.");
	//Delete objcets if drawing
	if(this->_isDrawing) {
		delete this->_p0;
		delete this->_p1;
		delete this->_line;
	}
}


void WCModeSketchAxisCreate::OnMouseDown(const WCMouseButton &button) {
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
		//Create a new SketchAxis
		WCAction *action = WCSketchAxis::ActionCreate(this->_workbench->Sketch(), "", this->_begin, pos);
		this->_workbench->Sketch()->Document()->ExecuteAction( action );
		//Delete the visualization objects
		delete this->_line;
		delete this->_p0;
		delete this->_p1;
		//Reset drawing state
		this->_isDrawing = false;				
	}
}


void WCModeSketchAxisCreate::OnMouseMove(const WPFloat &x, const WPFloat &y) {
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


void WCModeSketchAxisCreate::Render(void) {
	//Draw the crosshairs at the current position
	this->_workbench->RenderCrosshairs(this->_xSuggest, this->_ySuggest);
	//Draw any suggested references
	this->_workbench->RenderAlignmentSuggestion(this->_suggestionType, this->_xSuggest, this->_ySuggest);
		
	//Check to see if drawing
	if (!this->_isDrawing) return;
	//Turn off depth testing
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(SKETCHAXIS_LINESTIPPLE_FACTOR, SKETCHAXIS_LINESTIPPLE_PATTERN);
	//Draw visualization points and line
	WPFloat zoom = this->_workbench->Feature()->Document()->Scene()->ActiveCamera()->Zoom();
	this->_p0->Render(0, WCColor::Default(), zoom);
	this->_p1->Render(0, WCColor::Default(), zoom);
	this->_line->Render(0, WCColor::Default(), zoom);
	//Clean up
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_LINE_STIPPLE);
}


/***********************************************~***************************************************/

