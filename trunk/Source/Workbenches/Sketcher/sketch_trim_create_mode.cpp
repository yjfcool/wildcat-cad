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
#include "Sketcher/sketch_trim_modes.h"
#include "Sketcher/sketch.h"
#include "Sketcher/sketch_feature.h"
#include "Kernel/document.h"


/***********************************************~***************************************************/


WCModeSketchTrimCreate::WCModeSketchTrimCreate(WCSketchWorkbench *wb) :
	::WCDrawingMode(wb->Sketch(), SKETCHTRIMMODE_CREATE_NAME), _workbench(wb), _curve(NULL) { 
}


void WCModeSketchTrimCreate::OnEntry(void) {
	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Entering SketchTrim Draw Mode.");
	//Clear any selected items
	this->_workbench->SelectionManager()->Clear(true);
}


void WCModeSketchTrimCreate::OnExit(void) {
	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Exiting SketchTrim Draw Mode.");
	//Need to make sure nothing is selected
	//...
}


void WCModeSketchTrimCreate::OnMouseDown(const WCMouseButton &button) {
}


void WCModeSketchTrimCreate::OnMouseMove(const WPFloat &x, const WPFloat &y) {
	WCScene *scene = this->_workbench->Feature()->Document()->Scene();
	//Clear if appropriate
	if (!this->_workbench->IsMultiSelect()) this->_workbench->SelectionManager()->Clear(true);
	//Get pixel position
	WPUInt xPos = WPUInt((x + (scene->Width()/2.0)) * (scene->WindowWidth() / scene->Width()));
	WPUInt yPos = WPUInt(scene->WindowHeight() - (y - scene->Height() / 2.0) * (-1.0 * scene->WindowHeight() / scene->Height()));
	//Call single select
	this->_workbench->SelectionManager()->Select(xPos, xPos, yPos, yPos, false);
	if (this->_workbench->SelectionManager()->Count() != 1) return;
	std::pair<WCVisualObject*,WCSelectionObject*> selObj = this->_workbench->SelectionManager()->SelectedVisuals().front();
	//Cast associate to GeometricCurve
	WCGeometricCurve *curve =  dynamic_cast<WCGeometricCurve*>(selObj.first);
	//Make sure we have a curve
	if (curve != NULL) {
		std::list<WCIntersectionResult> hitList, tmpList;
		std::list<WCIntersectionResult>::iterator hitIter;
		
		//Loop through all points and try to intersect
		std::map<WCGeometricPoint*,WCEventController*> pointMap = this->_workbench->Sketch()->PointMap();
		std::map<WCGeometricPoint*,WCEventController*>::iterator pointIter;
		for(pointIter = pointMap.begin(); pointIter != pointMap.end(); pointIter++) {
			tmpList = GeometricIntersection((WCNurbsCurve*)curve, (*pointIter).first, 0.0001);
			hitList.splice(hitList.begin(), tmpList);
		}

		//Loop through all lines and try to intersect
		std::map<WCGeometricLine*,WCEventController*> lineMap = this->_workbench->Sketch()->LineMap();
		std::map<WCGeometricLine*,WCEventController*>::iterator lineIter;
		for(lineIter = lineMap.begin(); lineIter != lineMap.end(); lineIter++) {
			//Make sure not to do self-intersection
			if ( (*lineIter).first != curve ) {
				tmpList = GeometricIntersection((WCNurbsCurve*)curve, (*lineIter).first, 0.0001);
				//Add hit points into hitList
				hitList.splice(hitList.begin(), tmpList);
			}
		}
		
		//Loop through all curves and try to intersect
		std::map<WCNurbsCurve*,WCEventController*> curveMap = this->_workbench->Sketch()->CurveMap();
		std::map<WCNurbsCurve*,WCEventController*>::iterator curveIter;
		for(curveIter = curveMap.begin(); curveIter != curveMap.end(); curveIter++) {
			//Make sure not to do self-intersection
			if ( (*curveIter).first != curve ) {
				tmpList = GeometricIntersection((WCNurbsCurve*)curve, (*curveIter).first, 0.0001);
				//Add hit points into hitList
				hitList.splice(hitList.begin(), tmpList);
			}
		}
		//Sort the list and
//		hitList.sort();
		for (hitIter = hitList.begin(); hitIter != hitList.end(); hitIter++) {
			std::cout << "Hit: " << (*hitIter).leftParam << std::endl;
		}
	
	}
}


void WCModeSketchTrimCreate::Render(void) {
}


/***********************************************~***************************************************/

