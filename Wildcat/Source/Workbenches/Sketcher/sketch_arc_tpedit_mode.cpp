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


WCModeSketchArcThreePointEdit::WCModeSketchArcThreePointEdit(WCSketchArc *arc) : 
	::WCDrawingMode(arc->Sketch(), SKETCHARCMODE_STANDARDCREATE_NAME), _arc(arc),
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


void WCModeSketchArcThreePointEdit::OnEntry(void) {
	//Log some info
	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Entering SketchArc Edit Mode.");
}


void WCModeSketchArcThreePointEdit::OnExit(void) {
	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Exiting SketchArc Edit Mode.");
}


void WCModeSketchArcThreePointEdit::OnMouseMove(const WPFloat &x, const WPFloat &y) {
}


void WCModeSketchArcThreePointEdit::OnMouseUp(const WCMouseButton &button) {
}


void WCModeSketchArcThreePointEdit::Render(void) {
	//Draw the crosshairs at the current position
//	this->_workbench->RenderCrosshairs(this->_xSuggest, this->_ySuggest);
	//Draw any suggested references
//	this->_workbench->RenderAlignmentSuggestion(this->_suggestionType, this->_xSuggest, this->_ySuggest);
}


/***********************************************~***************************************************/

