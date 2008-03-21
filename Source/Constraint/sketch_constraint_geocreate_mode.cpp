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
#include "Constraint/sketch_constraint_modes.h"
#include "Constraint/sketch_constraint.h"
#include "Sketcher/sketch_workbench.h"
#include "Sketcher/sketch.h"
#include "Kernel/document.h"
#include "PartDesign/part_plane.h"


//Locally Defined Values
#define GEOMETRICCONSTRAINT_FIXED				0
#define GEOMETRICCONSTRAINT_VERTICAL			1
#define GEOMETRICCONSTRAINT_HORIZONTAL			2
#define GEOMETRICCONSTRAINT_COINCIDENT			3
#define GEOMETRICCONSTRAINT_TANGENT				4
#define GEOMETRICCONSTRAINT_SYMMETRIC			5
#define GEOMETRICCONSTRAINT_PERPENDICULAR		6
#define GEOMETRICCONSTRAINT_PARALLEL			7
#define GEOMETRICCONSTRAINT_MIDPOINT			8
#define GEOMETRICCONSTRAINT_EQUIDISTANT			9
#define GEOMETRICCONSTRAINT_CONCENTRIC			10


/***********************************************~***************************************************/


WCModeSketchConstraintGeometricCreate::WCModeSketchConstraintGeometricCreate(WCSketchWorkbench *wb) : 
	::WCDrawingMode(wb->Sketch(), SKETCHCONSTRAINT_DIMCREATE_NAME), _workbench(wb) {
	//Don't know what else for now
}


void WCModeSketchConstraintGeometricCreate::OnEntry(void) {
	//Clear any selected items
	this->_workbench->SelectionManager()->Clear(true);
	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Entering SketchConstraint Geometric Create Mode.");
}


void WCModeSketchConstraintGeometricCreate::OnExit(void) {
	//Delete anything?
	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Exiting SketchConstraint Geometric Create Mode.");

}


void WCModeSketchConstraintGeometricCreate::OnMouseDown(const WCMouseButton &button) {
	//Do something
}


void WCModeSketchConstraintGeometricCreate::OnMouseMove(const WPFloat &x, const WPFloat &y) {
	//Do something
}


void WCModeSketchConstraintGeometricCreate::OnMouseUp(const WCMouseButton &button) {
	//Do something
}


void WCModeSketchConstraintGeometricCreate::Render(void) {
	//Draw the crosshairs at the current position
	this->_workbench->RenderCrosshairs(this->_workbench->MouseX(), this->_workbench->MouseY());
}





/***********************************************~***************************************************/

