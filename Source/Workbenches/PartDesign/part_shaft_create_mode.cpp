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
#include "PartDesign/part_shaft_modes.h"
#include "PartDesign/part_shaft.h"
#include "PartDesign/part_workbench.h"
#include "PartDesign/part.h"
#include "PartDesign/part_plane.h"
#include "Kernel/document.h"
#include "Kernel/selection_mode.h"
#include "Kernel/workbench_layer.h"
#include "Sketcher/sketch.h"
#include "Sketcher/sketch_axis.h"
#include "Sketcher/sketch_profile.h"


/***********************************************~***************************************************/


void WCModePartShaftCreate::CheckSelections(void) {
	//Check to see if a sketch is selected
	std::list<WCSketch*> sketchList = this->_workbench->SelectionManager()->FilterSelected<WCSketch>(true);
	std::list<WCSketchProfile*> profiles;
	WCSketch* sketch;
	//If there is one sketch selected
	if (sketchList.size() == 1) {
		sketch = sketchList.front();
		//Get the list of profiles
		profiles = sketch->Profiles();
		//See if there is also an axis
		WCSketchFeature *featureAxis = sketch->FeatureFromName("Axis.1");
		this->_axis = dynamic_cast<WCSketchAxis*>(featureAxis);
		//Make sure axis is valid
		if (this->_axis == NULL) {
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCModeShaftCreate::CheckSelections - No valid axis in sketch");
			return;
		}
	}
	//Or check to see if profiles are selected
	else {
		profiles = this->_workbench->SelectionManager()->FilterSelected<WCSketchProfile>(true);
		//Make sure there is at least one profile
		if (profiles.size() == 0) {
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCModeShaftCreate::CheckSelections - No valid profiles selected");
			return;
		}
		sketch = profiles.front()->Sketch();
		//See if there is also an axis
		WCSketchFeature *featureAxis = sketch->FeatureFromName("Axis.1");
		this->_axis = dynamic_cast<WCSketchAxis*>(featureAxis);
		//Make sure axis is valid
		if (this->_axis == NULL) {
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCModeShaftCreate::CheckSelections - No valid axis in sketch");
			return;
		}		
	}
	//If there are any valid profiles
	if (profiles.size() > 0)
		//Process the profiles
		this->ProcessProfiles(profiles);
	//If any valid profiles remain, create surfaces and curves
	if (this->_profiles.size() > 0) {
		//Get axis base
		WCVector4 base = sketch->ReferencePlane()->TransformMatrix() * this->_axis->Begin();
		//Get axis direction
		WCVector4 direction = (sketch->ReferencePlane()->TransformMatrix() * this->_axis->End()) - base;
		//Force direction upwards
		WCVector4 dirOnPlane = this->_axis->Sketch()->ReferencePlane()->InverseTransformMatrix() * direction;
		//If slope is neg, invert dirOnPlane
		if (dirOnPlane.J() < 0.0) dirOnPlane.Set( -dirOnPlane.I(), -dirOnPlane.J(), 0.0, 0.0);
		direction = this->_axis->Sketch()->ReferencePlane()->TransformMatrix() * dirOnPlane;
		//Create ray
		this->_ray = new WCRay(base, direction);
		//Generate surfaces
		this->GenerateSurfaces();
	}
}


void WCModePartShaftCreate::ProcessProfiles(const std::list<WCSketchProfile*> &profiles) {
	//Qualify all of the selected profiles
	this->_profilesOnRight = WCPartShaft::QualifyProfiles(profiles, this->_profiles, this->_axis);
	//Make sure there is atleast one profile remains
	if (this->_profiles.size() == 0) {
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCModePartShaftCreate::ProcessProfiles - No valid profiles remain.");
			//throw error
			return;
	}

	//Build list of boundary points for exterior profiles
	std::list<WCVector4> inputList, tmpList;
	std::list<WCSketchProfile*>::iterator profileIter;
	for (profileIter = this->_profiles.begin(); profileIter != this->_profiles.end(); profileIter++) {
		//Only use exterior profiles
		if ((*profileIter) == this->_profiles.front()) {
			//Get list of boundary points (using control points is fine)
			tmpList = (*profileIter)->BoundaryList(false);
			//Merge all lists together
			inputList.splice(inputList.begin(), tmpList);
		}
	}
	
	//Project all points onto the plane
	std::list<WCVector4>::iterator pointIter;
	for (pointIter = inputList.begin(); pointIter != inputList.end(); pointIter++)
		(*pointIter) = this->_axis->Sketch()->ReferencePlane()->InverseTransformMatrix() * (*pointIter);
	//Determine convex hull
	this->_convexHull = ConvexHull2D(inputList, 0.0001);
	//Convert convexHull back to 3D space
	for (pointIter = this->_convexHull.begin(); pointIter != this->_convexHull.end(); pointIter++)
		(*pointIter) = this->_axis->Sketch()->ReferencePlane()->TransformMatrix() * (*pointIter);

	//Otherwise set stage to 1 (valid profiles)
	this->_stage = 1;
	//Mark the mouse location
	WCScene* scene = this->_workbench->Feature()->Document()->Scene();
	this->_mark.Set(scene->MouseX(), scene->MouseY(), 0.0, 1.0);
}


void WCModePartShaftCreate::GenerateSurfaces(void) {
	//Check to make sure there is a depth
	if (fabs(this->_cwAngle - this->_ccwAngle) < 0.01) return;
	WCNurbsSurface *surface;
	std::list<WCVector4>::iterator cvIter;
	WCVector4 start, end;

	/*** Generate all surfaces based on convex hull in CW direction***/

	//Make sure there is some CW angle
	if (this->_cwAngle > 0.0) {
		//Loop through all points in the convexHull
		for (cvIter = this->_convexHull.begin(); cvIter != this->_convexHull.end(); cvIter++) {
			//Find start and end
			start = (*cvIter++);
			//Check for wrapping case
			if (cvIter == this->_convexHull.end()) end = this->_convexHull.front();
			else end = (*cvIter);
			//Roll back cvIter
			cvIter--;
			//Create temp line from corners cvIter[i] to cvIter[i+1]
			WCGeometricLine *line = new WCGeometricLine(start, end);
			//Create surface from line
			surface = WCNurbsSurface::RevolveCurve(this->_creator->Document()->Scene()->GeometryContext(),
				line, this->_ray, this->_cwAngle, this->_profilesOnRight );
			//See about adding line
			if (surface != NULL) this->_surfaces.push_back(surface);
			//Delete the line
			delete line;
		}
	}

	/*** Generate all surfaces based on convex hull in CCW direction***/

	//Make sure there is some CCW angle
	if (this->_ccwAngle < 0.0) {
		//Loop through all points in the convexHull
		for (cvIter = this->_convexHull.begin(); cvIter != this->_convexHull.end(); cvIter++) {
			//Find start and end
			start = (*cvIter++);
			//Check for wrapping case
			if (cvIter == this->_convexHull.end()) end = this->_convexHull.front();
			else end = (*cvIter);
			//Roll back cvIter
			cvIter--;
			//Create temp line from corners cvIter[i] to cvIter[i+1]
			WCGeometricLine *line = new WCGeometricLine(start, end);
			//Create surface from line
			surface = WCNurbsSurface::RevolveCurve(this->_creator->Document()->Scene()->GeometryContext(),
				line, this->_ray, this->_ccwAngle, !this->_profilesOnRight );
			//See about adding line
			if (surface != NULL) this->_surfaces.push_back(surface);
			//Delete the line
			delete line;
		}
	}

	/*** Set properties for all surfaces ***/

	std::list<WCNurbsSurface*>::iterator surfIter;
	for (surfIter = this->_surfaces.begin(); surfIter != this->_surfaces.end(); surfIter++) {
		(*surfIter)->Color( WCPartFeature::ConstructionColor );
		(*surfIter)->RenderProgram( this->_workbench->Part()->Scene()->ShaderManager()->ProgramFromName("scn_basiclight"));
	}
}


/***********************************************~***************************************************/


WCModePartShaftCreate::WCModePartShaftCreate(WCPartWorkbench *wb) :	::WCDrawingMode(wb->Part(), PARTSHAFTMODE_CREATE_NAME), 
	_workbench(wb), _stage(0), _mark(), _axis(NULL), _ray(NULL), _profiles(), _cwAngle(0.0), _ccwAngle(0.0), _surfaces() {
	//Nothing else for now
}


void WCModePartShaftCreate::OnEntry(void) {
	this->_workbench->Part()->Status("Shaft Creation Mode Entered");
//	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Entering Shaft Create Mode.");
	//Check to see what is selected
	this->CheckSelections();
	//If no valid profiles, clear selection
	this->_workbench->SelectionManager()->Clear(true);
}


void WCModePartShaftCreate::OnExit(void) {
//	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Exiting Shaft Create Mode.");
	//Delete appropriate objects
	if (this->_stage == 0) return;
	
	//Delete the ray
	delete this->_ray;
	//Delete surfaces and curves
	std::list<WCNurbsSurface*>::iterator surfaceIter;
	for (surfaceIter = this->_surfaces.begin(); surfaceIter != this->_surfaces.end(); surfaceIter++) delete (*surfaceIter);
	//Clear the list
	this->_surfaces.clear();
}


void WCModePartShaftCreate::OnMouseDown(const WCMouseButton &button) {
	//Cancel if right click
	if (button == WCMouseButton::Right()) {
		//If not drawing, do nothing
		if (this->_stage == 0) return;
		//Reset drawing state
		this->_stage = 1;
		return;
	}
	//Otherwise, only left button
	else if (button != WCMouseButton::Left()) return;
	
	//If nothing selected, check to see if something selected
	if (this->_stage == 0) {
		//Get the scene
		WCScene* scene = this->_workbench->Feature()->Document()->Scene();
		//Get pixel position
		WPUInt x = WPUInt((scene->MouseX() + (scene->Width()/2.0)) * (scene->WindowWidth() / scene->Width()));
		WPUInt y = WPUInt(scene->WindowHeight() - (scene->MouseY() - scene->Height() / 2.0) * (-1.0 * scene->WindowHeight() / scene->Height()));
		//Call single select
		this->_workbench->SelectionManager()->Select(x, x, y, y, true);
		//Check selections
		this->CheckSelections();
		//If no valid profiles, clear selection
		this->_workbench->SelectionManager()->Clear(true);
		return;
	}
	//If in stage 1 (valid profiles, set cwAngle)
	if (this->_stage == 1) {
		//Mark the mouse location
		WCScene* scene = this->_workbench->Feature()->Document()->Scene();
		this->_mark.Set(scene->MouseX(), scene->MouseY(), 0.0, 0.0);
		this->_stage++;
	}
	//Otherwise, in stage 2 (set ccwAngle, execute action, reset to nothing selected)
	else {
		//Increment the stage (just need it to exit on mouse up)
		this->_stage++;
		//Create action to create shaft
		WCActionPartShaftCreate *action = WCPartShaft::ActionCreate(this->_workbench->Part()->Body(), "",
			this->_profiles, this->_axis, this->_profilesOnRight, this->_cwAngle, this->_ccwAngle);
		//Execute action
		this->_workbench->Part()->Document()->ExecuteAction( action );
	}
}


void WCModePartShaftCreate::OnMouseMove(const WPFloat &x, const WPFloat &y) {
	//If in stage 0, do nothing
	if (this->_stage == 0) return;

	//Delete surfaces
	std::list<WCNurbsSurface*>::iterator surfaceIter;
	for (surfaceIter = this->_surfaces.begin(); surfaceIter != this->_surfaces.end(); surfaceIter++) delete (*surfaceIter);
	//Clear the list
	this->_surfaces.clear();

	WCScene* scene = this->_workbench->Feature()->Document()->Scene();
	//Determine clockwise rotation direction (based on which side profiles are on)
	WCVector4 rotDir;
	if (this->_profilesOnRight)
		rotDir = this->_axis->Sketch()->ReferencePlane()->TransformMatrix() * WCVector4(0.0, 0.0, 1.0, 0.0);
	else
		rotDir = this->_axis->Sketch()->ReferencePlane()->TransformMatrix() * WCVector4(0.0, 0.0, -1.0, 0.0);
	//Project rotDir onto the screen
	rotDir = scene->ActiveCamera()->ViewMatrix() * rotDir;
	//Determine user movement dir
	WCVector4 markDir = WCVector4(scene->MouseX(), scene->MouseY(), 0.0, 0.0) - this->_mark;
	WPFloat angle = rotDir.DotProduct(markDir);

	//If in stage 1 (determine cwAngle)
	if (this->_stage == 1) {
		//Set the clockwise angle
		this->_cwAngle = STDMIN(2.0*M_PI, STDMAX(0.0, angle));
	}
	//If in stage 2...
	else {
		this->_ccwAngle = STDMAX(-2.0 * M_PI, angle);
		//Don't let cw-ccw be greater than 2pi
		this->_ccwAngle = STDMAX(-2.0 * M_PI + this->_cwAngle, this->_ccwAngle);
	}
	//Regenerate surfaces
	this->GenerateSurfaces();
}


void WCModePartShaftCreate::OnMouseUp(const WCMouseButton &button) {
	//If stage == 3, exit the mode
	if (this->_stage == 3) {
		//Return to selection mode
//		this->_workbench->DrawingMode( WCDrawingMode::Selection(this->_workbench) );
		this->_workbench->DrawingMode( new WCSelectionMode(this->_workbench) );
	}
}


void WCModePartShaftCreate::Render(void) {
	//If stage is above zero, render surfaces and curves
	if (this->_stage == 0) return;
	
	//Determine the magnitude arrow
//	WCVector4 base(0.0, 0.0, 0.0, 1.0), end;
//	base = this->_profiles.front().first->Sketch()->ReferencePlane()->TransformMatrix() * base;
//	if (this->_stage == 1) end = base + this->_direction * this->_posDepth;
//	else end = base + this->_direction * this->_negDepth;
//	GLfloat arrow[6] = { base.I(), base.J(), base.K(), end.I(), end.J(), end.K() };
	
	//Render arrow
//	glColor4f(1.0, 0.0, 0.0, 1.0);
//	glLineWidth(4.0);
//	glEnableClientState(GL_VERTEX_ARRAY);
//	glVertexPointer(3, GL_FLOAT, 0, arrow);
//	glDrawArrays(GL_LINES, 0, 2);
//	glDisableClientState(GL_VERTEX_ARRAY);
	
	//Render all of the surfaces
	std::list<WCNurbsSurface*>::iterator surfIter;
	for (surfIter = this->_surfaces.begin(); surfIter != this->_surfaces.end(); surfIter++) {
		(*surfIter)->Render(0, WCColor::Default(), this->_workbench->Part()->Document()->Scene()->ActiveCamera()->Zoom());
	}
}


/***********************************************~***************************************************/

