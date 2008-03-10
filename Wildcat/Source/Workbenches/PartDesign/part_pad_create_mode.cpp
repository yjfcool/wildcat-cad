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
#include "PartDesign/part_pad_modes.h"
#include "PartDesign/part_pad.h"
#include "PartDesign/part_plane.h"
#include "PartDesign/part_workbench.h"
#include "PartDesign/part.h"
#include "Kernel/document.h"
#include "Kernel/selection_mode.h"
#include "Kernel/workbench_layer.h"
#include "Sketcher/sketch_profile.h"
#include "Sketcher/sketch.h"


/***********************************************~***************************************************/


bool WCModePartPadCreate::CheckSelections(void) {
	//Check to see if a sketch is selected
	std::list<WCSketch*> sketchList = this->_workbench->SelectionManager()->FilterSelected<WCSketch>(true);
	std::list<WCSketchProfile*> profiles;
	WCSketch* sketch;
	//If there is one sketch selected
	if (sketchList.size() == 1) {
		sketch = sketchList.front();
		profiles = sketch->Profiles();
	}
	//Or check to see if profiles are selected
	else {
		profiles = this->_workbench->SelectionManager()->FilterSelected<WCSketchProfile>(true);
		sketch = profiles.front()->Sketch();
	}
	//If there are any valid profiles
	if (profiles.size() > 0) {
		this->_plane = sketch->ReferencePlane()->TransformMatrix();
		this->ProcessProfiles(profiles);
		//Return all good
		return true;
	}
	//Return not so good
	return false;
}


void WCModePartPadCreate::ProcessProfiles(const std::list<WCSketchProfile*> &profiles) {
	WCSketch *sketch;
	//Validate each profile
	std::list<WCSketchProfile*>::const_iterator iter;
	std::list<WCSketchProfile*> profileList;
	for (iter = profiles.begin(); iter != profiles.end(); iter++) {
		//Make sure profile is linear, closed, and not self-intersecting
		if ((!(*iter)->IsLinear()) || (!(*iter)->IsClosed()) || ((*iter)->IsSelfIntersecting())) {
			//Validate that the sketch is closed, linear, and non-intersecting
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCModePadCreate::WCModePadCreate - Invalid profile in list of profiles.");
		}
		else {
			//Get first valid profile's sketch
			if (this->_profiles.size() == 0) sketch = (*iter)->Sketch();
			if ((*iter)->Sketch() != sketch) {
				CLOGGER_ERROR(WCLogManager::RootLogger(), "WCModePadCreate::WCModePadCreate - Profile sketches to not match.");
			}
			else {
				//Put good profiles into local list
				profileList.push_back(*iter);
			}
		}
	}
	
	//Categorize the profiles
	std::list<WCProfileTreeNode*> rootList = WCSketchProfile::CategorizeIntoTree(profileList);
	this->_profiles = WCSketchProfile::FlattenCategorizationTree(rootList);
	//Make sure there is atleast one profile remains
	if (this->_profiles.size() == 0) {
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCModePadCreate::WCModePadCreate - No valid profiles remain.");
			//throw error
			return;
	}

	//Build list of boundary points for exterior profiles
	std::list<WCVector4> inputList, tmpList;
	std::list< std::pair<WCSketchProfile*,bool> >::iterator profileIter;
	for (profileIter = this->_profiles.begin(); profileIter != this->_profiles.end(); profileIter++) {
		//Only use exterior profiles
		if ((*profileIter).second) {
			//Get list of boundary points
			tmpList = (*profileIter).first->BoundaryList();
			//Merge all lists together
			inputList.splice(inputList.begin(), tmpList);
		}
	}
	//Find minimum bounding rectangle for bounding points
	WCPartPlane *refPlane = this->_profiles.front().first->Sketch()->ReferencePlane();
	std::list<WCVector4> baseCorners = MinimumBoundingRectangle(inputList, refPlane->InverseTransformMatrix(), refPlane->TransformMatrix());
	std::list<WCVector4>::iterator baseIter = baseCorners.begin();
	//Load the corners array - (clockwise starting at lower-left)
	this->_corners[0] = *baseIter++;		// Lower-left
	this->_corners[1] = *baseIter++;		// Upper-left
	this->_corners[2] = *baseIter++;		// Upper-right
	this->_corners[3] = *baseIter;			// Lower-right

	//Determine direction vector
	WCVector4 dir(0.0, 0.0, 1.0, 0.0);
	dir = this->_plane * dir;
	dir.Normalize(true);
	this->_direction = dir;

	//Otherwise set stage to 1 (valid profiles)
	this->_stage = 1;
	//Mark the mouse location
	WCScene* scene = this->_workbench->Feature()->Document()->Scene();
	this->_mark.Set(scene->MouseX(), scene->MouseY(), 0.0, 1.0);
}


void WCModePartPadCreate::GenerateSurfaces(void) {
	//Check to make sure there is a depth
	if (fabs(this->_posDepth - this->_negDepth) < 0.01) return;
	
	WCNurbsSurface* surface;
	//Calculate all eight points
	WCVector4 tll = this->_corners[0] + this->_direction * this->_posDepth;
	WCVector4 bll = this->_corners[0] + this->_direction * this->_negDepth;
	WCVector4 tul = this->_corners[1] + this->_direction * this->_posDepth;
	WCVector4 bul = this->_corners[1] + this->_direction * this->_negDepth;
	WCVector4 tur = this->_corners[2] + this->_direction * this->_posDepth;
	WCVector4 bur = this->_corners[2] + this->_direction * this->_negDepth;
	WCVector4 tlr = this->_corners[3] + this->_direction * this->_posDepth;
	WCVector4 blr = this->_corners[3] + this->_direction * this->_negDepth;

	/*** Wall Surfaces ***/

	std::vector<WCVector4> controlPoints;
	//Left Wall
	controlPoints.push_back(bul);
	controlPoints.push_back(bll);
	controlPoints.push_back(tul);
	controlPoints.push_back(tll);
	surface = new WCNurbsSurface(this->_creator->Document()->Scene()->GeometryContext(), 1, 1, 2, 2, controlPoints,
		WCNurbsMode::Default(), WCNurbsMode::Default());
	this->_surfaces.push_back( surface );
	controlPoints.clear();

	//Front Wall
	controlPoints.push_back(bll);
	controlPoints.push_back(blr);
	controlPoints.push_back(tll);
	controlPoints.push_back(tlr);
	surface = new WCNurbsSurface(this->_creator->Document()->Scene()->GeometryContext(), 1, 1, 2, 2, controlPoints,
		WCNurbsMode::Default(), WCNurbsMode::Default());
	this->_surfaces.push_back( surface );
	controlPoints.clear();

	//Right Wall
	controlPoints.push_back(blr);
	controlPoints.push_back(bur);
	controlPoints.push_back(tlr);
	controlPoints.push_back(tur);
	surface = new WCNurbsSurface(this->_creator->Document()->Scene()->GeometryContext(), 1, 1, 2, 2, controlPoints,
		WCNurbsMode::Default(), WCNurbsMode::Default());
	this->_surfaces.push_back( surface );
	controlPoints.clear();

	//Back Wall
	controlPoints.push_back(bur);
	controlPoints.push_back(bul);
	controlPoints.push_back(tur);
	controlPoints.push_back(tul);
	surface = new WCNurbsSurface(this->_creator->Document()->Scene()->GeometryContext(), 1, 1, 2, 2, controlPoints,
		WCNurbsMode::Default(), WCNurbsMode::Default());
	this->_surfaces.push_back( surface );
	controlPoints.clear();

	//Top Wall
	controlPoints.push_back(tll);
	controlPoints.push_back(tlr);
	controlPoints.push_back(tul);
	controlPoints.push_back(tur);
	surface = new WCNurbsSurface(this->_creator->Document()->Scene()->GeometryContext(), 1, 1, 2, 2, controlPoints,
		WCNurbsMode::Default(), WCNurbsMode::Default());
	this->_surfaces.push_back( surface );
	controlPoints.clear();
	
	//Bottom Wall
	controlPoints.push_back(bul);
	controlPoints.push_back(bur);
	controlPoints.push_back(bll);
	controlPoints.push_back(blr);
	surface = new WCNurbsSurface(this->_creator->Document()->Scene()->GeometryContext(), 1, 1, 2, 2, controlPoints,
		WCNurbsMode::Default(), WCNurbsMode::Default());
	this->_surfaces.push_back( surface );
	controlPoints.clear();
	
	/*** Set properties for surfaces ***/

	std::list<WCNurbsSurface*>::iterator surfIter;
	for (surfIter = this->_surfaces.begin(); surfIter != this->_surfaces.end(); surfIter++) {
		(*surfIter)->Color( WCPartFeature::ConstructionColor );
		(*surfIter)->RenderProgram(this->_workbench->Part()->Scene()->ShaderManager()->ProgramFromName("scn_basiclight"));
//		(*surfIter)->Layer(this->_workbench->Layer());
	}
}


/***********************************************~***************************************************/


WCModePartPadCreate::WCModePartPadCreate(WCPartWorkbench *wb) :	::WCDrawingMode(wb->Part(), PARTPADMODE_CREATE_NAME), 
	_workbench(wb), _stage(0), _mark(), _plane(), _direction(), _profiles(), _posDepth(0.0), _negDepth(0.0), _surfaces() {
	//Nothing else for now
}


void WCModePartPadCreate::OnEntry(void) {
	this->_workbench->Part()->Status("Pad Creation Mode Entered");
//	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Entering Pad Create Mode.");
	//Check to see what is selected
	if (!this->CheckSelections()) {
		//If no valid profiles, clear selection
		this->_workbench->SelectionManager()->Clear(true);
	}
}


void WCModePartPadCreate::OnExit(void) {
//	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Exiting Pad Create Mode.");
	//Delete appropriate objects
	if (this->_stage == 0) return;
	
	//Delete construction surfaces
	std::list<WCNurbsSurface*>::iterator surfaceIter;
	for (surfaceIter = this->_surfaces.begin(); surfaceIter != this->_surfaces.end(); surfaceIter++) delete (*surfaceIter);
	//Clear the list
	this->_surfaces.clear();
}


void WCModePartPadCreate::OnMouseDown(const WCMouseButton &button) {
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
		WPUInt x = (scene->MouseX() + (scene->Width()/2.0)) * (scene->WindowWidth() / scene->Width());
		WPUInt y = scene->WindowHeight() - (scene->MouseY() - scene->Height() / 2.0) * (-1.0 * scene->WindowHeight() / scene->Height());
		//Call single select
		this->_workbench->SelectionManager()->Select(x, x, y, y, true);
		//Check selections
		if (!this->CheckSelections()) {
			//Clear selection
			this->_workbench->SelectionManager()->Clear(true);
		}
		return;
	}
	//If in stage 1 (valid profiles, set posDepth)
	if (this->_stage == 1) {
		//Mark the mouse location
		WCScene* scene = this->_workbench->Feature()->Document()->Scene();
		this->_mark.Set(scene->MouseX(), scene->MouseY(), 0.0, 1.0);
		this->_stage++;
	}
	//Otherwise, in stage 2 (set negDepth, execute action, reset to nothing selected)
	else {
		//Increment the stage (just need it to exit on mouse up)
		this->_stage++;
		//Delete construction surfaces
		std::list<WCNurbsSurface*>::iterator surfaceIter;
		for (surfaceIter = this->_surfaces.begin(); surfaceIter != this->_surfaces.end(); surfaceIter++) delete (*surfaceIter);
		//Clear the list
		this->_surfaces.clear();
		//Create action to create pad
		WCActionPartPadCreate *action = WCPartPad::ActionCreate(this->_workbench->Part()->Body(), "",
			this->_profiles, this->_direction, this->_posDepth, this->_negDepth);
		//Execute action
		this->_workbench->Part()->Document()->ExecuteAction( action );
	}
}


void WCModePartPadCreate::OnMouseMove(const WPFloat &x, const WPFloat &y) {
	//If in stage 0, do nothing
	if (this->_stage == 0) return;

	//Delete surfaces
	std::list<WCNurbsSurface*>::iterator surfaceIter;
	for (surfaceIter = this->_surfaces.begin(); surfaceIter != this->_surfaces.end(); surfaceIter++) delete (*surfaceIter);
	//Clear the list
	this->_surfaces.clear();

	//If in stage 1...
	if (this->_stage == 1) {
		WCScene* scene = this->_workbench->Feature()->Document()->Scene();
		WCVector4 viewDir( scene->ActiveCamera()->ViewMatrix() * this->_direction );
		WCVector4 nextMark(scene->MouseX(), scene->MouseY(), 0.0, 1.0);
		WCVector4 markDir = nextMark - this->_mark;
		viewDir.K( 0.0 );
		viewDir.Normalize(true);
		//Calculate posDepth (Make sure is positive)
		this->_posDepth = std::max(0.0, viewDir.DotProduct(markDir));
	}
	//If in stage 2...
	else {
		WCScene* scene = this->_workbench->Feature()->Document()->Scene();
		WCVector4 viewDir( scene->ActiveCamera()->ViewMatrix() * this->_direction );
		WCVector4 nextMark(scene->MouseX(), scene->MouseY(), 0.0, 1.0);
		WCVector4 markDir = nextMark - this->_mark;
		viewDir.K( 0.0 );
		viewDir.Normalize(true);
		//Calculate negDepth (Make sure is negative)
		this->_negDepth = std::min(0.0, viewDir.DotProduct(markDir));
	}
	//Regenerate surfaces
	this->GenerateSurfaces();
}


void WCModePartPadCreate::OnMouseUp(const WCMouseButton &button) {
	//If stage == 3, exit the mode
	if (this->_stage == 3) {
		//Clear selection
		this->_workbench->SelectionManager()->Clear(true);
		//Return to selection mode
		this->_workbench->DrawingMode( WCDrawingMode::Selection(this->_workbench) );		
	}
}


void WCModePartPadCreate::Render(void) {
	//If stage is above zero, render surfaces and curves
	if (this->_stage == 0) return;
	
	//Determine the magnitude arrow
	WCVector4 base(0.0, 0.0, 0.0, 1.0), end;
	base = this->_profiles.front().first->Sketch()->ReferencePlane()->TransformMatrix() * base;
	if (this->_stage == 1) end = base + this->_direction * this->_posDepth;
	else end = base + this->_direction * this->_negDepth;
	GLfloat arrow[6] = { base.I(), base.J(), base.K(), end.I(), end.J(), end.K() };
	
	//Render arrow
	glColor4f(1.0, 0.0, 0.0, 1.0);
	glLineWidth(4.0);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, arrow);
	glDrawArrays(GL_LINES, 0, 2);
	glDisableClientState(GL_VERTEX_ARRAY);
	
	//Render all of the surfaces
	std::list<WCNurbsSurface*>::iterator surfIter;
	for (surfIter = this->_surfaces.begin(); surfIter != this->_surfaces.end(); surfIter++) {
		(*surfIter)->Render(0, WCColor::Default(), this->_workbench->Part()->Document()->Scene()->ActiveCamera()->Zoom());
	}
}


/***********************************************~***************************************************/

