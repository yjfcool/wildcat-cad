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
#include "Sketcher/sketch_workbench.h"
#include "Sketcher/sketch.h"
#include "Sketcher/grid.h"
#include "Constraint/constraint_fix.h"
#include "Application/keymap.h"
#include "Kernel/document.h"
#include "Kernel/workbench_layer.h"
#include "Kernel/selection_mode.h"
#include "PartDesign/part_plane.h"
#include "PartDesign/part.h"


//Sketch Feature Included Headers
#include "Sketcher/sketch_arc.h"
#include "Sketcher/sketch_axis.h"
#include "Sketcher/sketch_circle.h"
#include "Sketcher/sketch_conic_twopoint.h"
#include "Sketcher/sketch_ellipse.h"
#include "Sketcher/sketch_line.h"
#include "Sketcher/sketch_point.h"
#include "Sketcher/sketch_rectangle.h"
#include "Sketcher/sketch_trim.h"


/***********************************************~***************************************************/


void WCSketchWorkbench::CalculateMousePosition() {
	//Get inverse projection matrices
	WCMatrix4 cameraInverse = this->_sketch->Document()->Scene()->ActiveCamera()->ViewInverseMatrix();
	WCMatrix4 planeInverse = this->_sketch->ReferencePlane()->InverseTransformMatrix();
	WCMatrix4 mat = planeInverse * cameraInverse;
	//Project scene coordinates back to plane
	WCVector4 screenCoords(	this->_sketch->Document()->Scene()->MouseX(), 
							this->_sketch->Document()->Scene()->MouseY(), 0.0, 1.0);
	WCVector4 planeCoords = mat * screenCoords;
	this->_mouseX = planeCoords.I();
	this->_mouseY = planeCoords.J();
	this->_snapMouseX = this->_mouseX;
	this->_snapMouseY = this->_mouseY;
	//If not, see about snap to point (and set snapMouseX and snapMouseY)
	if (this->_grid->Snap(this->_snapMouseX, this->_snapMouseY)) return;
}


void WCSketchWorkbench::RenderCoordinates(const WCColor &color) {
	std::cout << "Need to render coordinates\n";
}


/***********************************************~***************************************************/


WCSketchWorkbench::WCSketchWorkbench(WCSketch *sketch) : ::WCWorkbench(sketch, "Sketcher"), 
	_sketch(sketch), _grid(NULL), _mouseX(0.0), _mouseY(0.0), _isConstruction(false), _isSuggest(true),
	_geoConstraints(true), _dimConstraints(true) {

	//Setup the key map
	this->_keyMap->AddMapping( WCKeyEvent('a'), WCUserMessage("twoPointArc") );
	this->_keyMap->AddMapping( WCKeyEvent('c'), WCUserMessage("centerCircle") );
	this->_keyMap->AddMapping( WCKeyEvent('d'), WCUserMessage("dimensionConstraint") );
	this->_keyMap->AddMapping( WCKeyEvent('e'), WCUserMessage("ellipse") );
	this->_keyMap->AddMapping( WCKeyEvent('f'), WCUserMessage("fix") );
	this->_keyMap->AddMapping( WCKeyEvent('g'), WCUserMessage("geometricConstraint") );
	this->_keyMap->AddMapping( WCKeyEvent('h'), WCUserMessage("hideConstraints") );
	this->_keyMap->AddMapping( WCKeyEvent('i'), WCUserMessage("trim") );
	this->_keyMap->AddMapping( WCKeyEvent('l'), WCUserMessage("line") );
	this->_keyMap->AddMapping( WCKeyEvent('o'), WCUserMessage("conic") );
	this->_keyMap->AddMapping( WCKeyEvent('p'), WCUserMessage("point") );
	this->_keyMap->AddMapping( WCKeyEvent('q'), WCUserMessage("exitWorkbench") );
	this->_keyMap->AddMapping( WCKeyEvent('r'), WCUserMessage("rectangle") );
	this->_keyMap->AddMapping( WCKeyEvent('s'), WCUserMessage("gridSnap") );
	this->_keyMap->AddMapping( WCKeyEvent('t'), WCUserMessage("threePointArc") );
	this->_keyMap->AddMapping( WCKeyEvent('w'), WCUserMessage("construction") );
	this->_keyMap->AddMapping( WCKeyEvent('x'), WCUserMessage("axisLine") );
	this->_keyMap->AddMapping( WCKeyEvent('b'), WCUserMessage("analyzeSketch") );
	this->_keyMap->AddMapping( WCKeyEvent('`'), WCUserMessage("select") );
	this->_keyMap->AddMapping( WCKeyEvent(127), WCUserMessage("delete") );
	this->_keyMap->AddMapping( WCKeyEvent('Z'), WCUserMessage("analyzeConstraint") );

	//Create the grid
	this->_grid = new WCGrid(*this->_sketch, 0.1, 0.1);
	
	//Add sketch and grid to the layer
	this->_layer->AddObject(this->_grid);
	this->_layer->AddObject(this->_sketch);
	//Set grid defaults
	this->_grid->IsSnap(true);
	
//	//Determine pan and zoom values
//	WCAlignedBoundingBox box = this->_sketch->Document()->Scene()->BoundingBox();
//	WCMatrix mat = box.ToMatrix();
//	//Project box to using rot
//	WCMatrix4 rot4 = this->_sketch->Document()->Scene()->ActiveCamera()->Quaternion().ToDCM();
//	WCMatrix rot = rot4.ToMatrix();
//	mat = rot * mat;
//	//Find projected min/max values
//	WPFloat xMin = mat.RowMinimum(0);
//	WPFloat xMax = mat.RowMaximum(0);
//	WPFloat yMin = mat.RowMinimum(1);
//	WPFloat yMax = mat.RowMaximum(1);
//	//Calculate the pan and zoom factors
//	WPFloat width =  this->_sketch->Document()->Scene()->Width() / ((xMax - xMin) + (2.0 * CAMERA_FITWINDOW_BOUNDRY));
//	WPFloat height = this->_sketch->Document()->Scene()->Height() / ((yMax - yMin) + (2.0 * CAMERA_FITWINDOW_BOUNDRY));
//	WPFloat zoom = std::min(width, height);
//	WPFloat panX = -0.5 * (xMin + xMax) * zoom;
//	WPFloat panY = -0.5 * (yMin + yMax) * zoom;
//	this->_defaultView.Zoom(zoom);
//	this->_defaultView.PanX(panX);
//	this->_defaultView.PanY(panY);

	//Determine view quaternion from reference plane
	this->_defaultView.Quaternion( WCQuaternion(this->_sketch->ReferencePlane()->TransformMatrix()) );
	this->_defaultView.Zoom(1.0);
}


WCSketchWorkbench::~WCSketchWorkbench() {
	//Remove the sketch and the grid from the layer
	this->_layer->RemoveObject(this->_grid);
	this->_layer->RemoveObject(this->_sketch);
	//Delete the grid
	if (this->_grid != NULL) delete this->_grid;
}


void WCSketchWorkbench::CreateInitialObjects(void) {
	WCVector4 p(0.0, 0.0, 0.0, 1.0);
	//Create construction center point
	WCSketchPoint *center = new WCSketchPoint(this->_sketch, "Origin", p);
	WCConstraintFix *constraint = new WCConstraintFix(this->_sketch, "", center);
	constraint->IsVisible(false);
	
	//Create construction infinite line along u Axis
//	WCSketchAxis *axis = new WCSketchAxis(this->_sketch, "u Axis", p, u);
//	constraint = new WCConstraintFix(this->_sketch, axis, p);
//	constraint->IsVisible(false);
	
	//Create construction infinite line along v Axis
//	axis = new WCSketchAxis(this->_sketch, "v Axis", p, v);
//	constraint = new WCConstraintFix(this->_sketch, axis, p);
//	constraint->IsVisible(false);
}


bool WCSketchWorkbench::OnEnter(void) {
	//Remove all geometry from the part
	this->_sketch->UnpublishGeometry();
	//Make treeView not visible
	this->_sketch->Document()->TreeView()->IsVisible(false);
	//Call to the base OnEnter
	this->WCWorkbench::OnEnter();
	//Show sketcher toolbars
	this->_sketch->Document()->ToolbarManager()->ToolbarFromName("Standard")->IsVisible(true);
	this->_sketch->Document()->ToolbarManager()->ToolbarFromName("View")->IsVisible(false);
	this->_sketch->Document()->ToolbarManager()->ToolbarFromName("Sketcher")->IsVisible(true);
	this->_sketch->Document()->ToolbarManager()->ToolbarFromName("Profiles")->IsVisible(true);
	this->_sketch->Document()->ToolbarManager()->ToolbarFromName("Operations")->IsVisible(true);
	//Update mouse tracker
	this->CalculateMousePosition();
	//Clear the selection buffer
	this->_selectionManager->Clear(true);
	//Set previous workbench to selection mode
//	this->_sketch->Part()->Workbench()->DrawingMode( WCDrawingMode::Selection( this->_sketch->Part()->Workbench()) );
	this->_sketch->Part()->Workbench()->DrawingMode( new WCSelectionMode( this->_sketch->Part()->Workbench()) );
	//Go into selection mode
	WCSelectionMode *mode = new WCSelectionMode(this);
	this->DrawingMode( mode );
	return true;
}


bool WCSketchWorkbench::OnExit(void) {
	//Clear the selection buffer
	this->_selectionManager->Clear(true);
	//Go into selection mode
//	this->DrawingMode( WCDrawingMode::Selection( this ));
	this->DrawingMode( new WCSelectionMode( this ));
	//Clear the selection buffer
	this->_sketch->Document()->ToolbarManager()->ToolbarFromName("Standard")->IsVisible(false);
	this->_sketch->Document()->ToolbarManager()->ToolbarFromName("View")->IsVisible(true);
	this->_sketch->Document()->ToolbarManager()->ToolbarFromName("Sketcher")->IsVisible(false);
	this->_sketch->Document()->ToolbarManager()->ToolbarFromName("Profiles")->IsVisible(false);
	this->_sketch->Document()->ToolbarManager()->ToolbarFromName("Operations")->IsVisible(false);
	//Hide sketcher toolbars
	this->_sketch->Document()->ToolbarManager()->PopState();
	//Call to the base OnExit
	this->WCWorkbench::OnExit();
	//Make treeView visible
	this->_sketch->Document()->TreeView()->IsVisible(true);
	//Analyze the sketch
	this->_sketch->Analyze();
	//Add all geometry into the part
	this->_sketch->PublishGeometry();
	return true;
}


void WCSketchWorkbench::OnMouseMove(const int x, const int y) {
	//Transform the movement to the plane
	this->CalculateMousePosition();
	//Call base workbench
	this->WCWorkbench::OnMouseMove(x, y);
}


WCSketchAlignmentSuggestion WCSketchWorkbench::SuggestAlignment(const std::list<WCSketchAlignmentSuggestion> &rules, 
	WCGeometricObject *object, WPFloat &x, WPFloat &y) {
	//Make sure a non-zero count of rules was passed
	if ((!this->_isSuggest) || (rules.size() == 0)) return WCSketchAlignmentSuggestion::None();
	//Prep an interator
	WCSketchAlignmentSuggestion suggestion = WCSketchAlignmentSuggestion::None();
	//Go through rules until valid one is found
	std::list<WCSketchAlignmentSuggestion>::const_iterator ruleIter;
	for (ruleIter = rules.begin(); ruleIter != rules.end(); ruleIter++) {
		//Evaluate the suggestion
		suggestion = (*ruleIter).Evaluate(this, object, x, y);
		//It a suggestion is found, return it
		if (suggestion != WCSketchAlignmentSuggestion::None()) return suggestion;
	}
	//No suggestion at this time
	return WCSketchAlignmentSuggestion::None();
}


void WCSketchWorkbench::RenderAlignmentSuggestion(const WCSketchAlignmentSuggestion suggest, const WPFloat x, const WPFloat y) {
	//If type == none - do nothing
	if (suggest == WCSketchAlignmentSuggestion::None()) return;
	//Otherwise, draw it
	suggest.Render(this, x, y);
}


void WCSketchWorkbench::RenderPoint(const WPFloat x, const WPFloat y, const WCColor &color) {
	//Calculate horizontal line start and stop
	WCVector4 pt(x, y, 0.0, 1.0);
	pt = this->_sketch->ReferencePlane()->TransformMatrix() * pt;
	GLfloat data[3] = { (GLfloat)pt.I(), (GLfloat)pt.J(), (GLfloat)pt.K() };

	//Enable mode color
	if (color == WCColor::Default()) {
		if (this->_isConstruction) glColor4f(SKETCHWORKBENCH_POINTER_CONSTRUCTION_COLOR);
		else glColor4f(SKETCHWORKBENCH_POINTER_DEFAULT_COLOR);
	}
	else color.Enable();
	//Set point size
	glPointSize(WCSketchFeature::PointSize);
	//Disable depth testing
	glDisable(GL_DEPTH_TEST);
	glEnableClientState(GL_VERTEX_ARRAY);
	//Draw the line
	glVertexPointer(3, GL_FLOAT, 0, data);
	glDrawArrays(GL_POINTS, 0, 1);
	//Clean up state
	glDisableClientState(GL_VERTEX_ARRAY);
	//Renable depth testing
	glEnable(GL_DEPTH_TEST);
}


void WCSketchWorkbench::RenderCrosshairs(const WPFloat x, const WPFloat y, const WCColor &color) {
	//Calculate base and h-v pointers
	WCVector4 base(x, y, 0.0, 1.0);
	WCVector4 hAxis(1.0, 0.0, 0.0, 0.0);
	WCVector4 vAxis(0.0, 1.0, 0.0, 0.0);
	WCMatrix4 mat = this->_sketch->ReferencePlane()->TransformMatrix();
	//And apply inverse zoom factor
	GLfloat invZoom = 1.0f / (GLfloat)this->_sketch->Document()->Scene()->ActiveCamera()->Zoom();
	GLfloat zoom = invZoom * SKETCHWORKBENCH_CROSSHAIR_SIZE;
	//Calculate final offsets
	base = mat * base;
	hAxis = mat * hAxis;
	vAxis = mat * vAxis;
	//Determine line start/stop points
	WCVector4 left = base - hAxis*zoom;
	WCVector4 right = base + hAxis*zoom;
	WCVector4 up = base + vAxis*zoom;
	WCVector4 down = base - vAxis*zoom;
	//Set up render array
	GLfloat data[12] = { (GLfloat)left.I(), (GLfloat)left.J(), (GLfloat)left.K(),
						 (GLfloat)right.I(), (GLfloat)right.J(), (GLfloat)right.K(),
						 (GLfloat)down.I(), (GLfloat)down.J(), (GLfloat)down.K(),
						 (GLfloat)up.I(), (GLfloat)up.J(), (GLfloat)up.K() };
	
	//Enable mode color
	if (color == WCColor::Default()) {
		if (this->_isConstruction) glColor4f(SKETCHWORKBENCH_POINTER_CONSTRUCTION_COLOR);
		else glColor4f(SKETCHWORKBENCH_POINTER_DEFAULT_COLOR);
	}
	else color.Enable();
	
	//Set up render state
	glLineWidth(1.0);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisable(GL_DEPTH_TEST);
	//Draw the line
	glVertexPointer(3, GL_FLOAT, 0, data);
	glDrawArrays(GL_LINES, 0, 4);
	//Clean up state
	glDisableClientState(GL_VERTEX_ARRAY);
	glEnable(GL_DEPTH_TEST);
}


void WCSketchWorkbench::RenderCrossTarget(const WPFloat x, const WPFloat y, const WCColor &color) {
	//Calculate base and h-v pointers
	WCVector4 base(x, y, 0.0, 1.0);
	WCVector4 hAxis(1.0, 0.0, 0.0, 0.0);
	WCVector4 vAxis(0.0, 1.0, 0.0, 0.0);
	WCMatrix4 mat = this->_sketch->ReferencePlane()->TransformMatrix();
	//And apply inverse zoom factor
	GLfloat invZoom = 1.0f / (GLfloat)this->_sketch->Document()->Scene()->ActiveCamera()->Zoom();
	GLfloat zoom = invZoom * SKETCHWORKBENCH_CROSSHAIR_SIZE;
	//Calculate final offsets
	base = mat * base;
	hAxis = mat * hAxis;
	vAxis = mat * vAxis;
	//Determine line start/stop points
	WCVector4 left = base - hAxis*zoom;
	WCVector4 right = base + hAxis*zoom;
	WCVector4 up = base + vAxis*zoom;
	WCVector4 down = base - vAxis*zoom;
	//Determine box points
	WCVector4 ll = base - hAxis*zoom*0.45 - vAxis*zoom*0.45;
	WCVector4 ul = base - hAxis*zoom*0.45 + vAxis*zoom*0.45;
	WCVector4 ur = base + hAxis*zoom*0.45 + vAxis*zoom*0.45;
	WCVector4 lr = base + hAxis*zoom*0.45 - vAxis*zoom*0.45;
	//Set up render array
	GLfloat data[36] = { (GLfloat)left.I(), (GLfloat)left.J(), (GLfloat)left.K(),
						 (GLfloat)right.I(), (GLfloat)right.J(), (GLfloat)right.K(),
						 (GLfloat)down.I(), (GLfloat)down.J(), (GLfloat)down.K(),
						 (GLfloat)up.I(), (GLfloat)up.J(), (GLfloat)up.K(),
						 (GLfloat)ll.I(), (GLfloat)ll.J(), (GLfloat)ll.K(),
						 (GLfloat)ul.I(), (GLfloat)ul.J(), (GLfloat)ul.K(),
						 (GLfloat)ul.I(), (GLfloat)ul.J(), (GLfloat)ul.K(),
						 (GLfloat)ur.I(), (GLfloat)ur.J(), (GLfloat)ur.K(), 
						 (GLfloat)ur.I(), (GLfloat)ur.J(), (GLfloat)ur.K(),
						 (GLfloat)lr.I(), (GLfloat)lr.J(), (GLfloat)lr.K(),
						 (GLfloat)lr.I(), (GLfloat)lr.J(), (GLfloat)lr.K(),
						 (GLfloat)ll.I(), (GLfloat)ll.J(), (GLfloat)ll.K() };

	//Enable mode color
	if (color == WCColor::Default()) {
		if (this->_isConstruction) glColor4f(SKETCHWORKBENCH_POINTER_CONSTRUCTION_COLOR);
		else glColor4f(SKETCHWORKBENCH_POINTER_DEFAULT_COLOR);
	}
	else color.Enable();

	//Set up render state
	glLineWidth(1.0);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisable(GL_DEPTH_TEST);
	//Draw the line
	glVertexPointer(3, GL_FLOAT, 0, data);
	glDrawArrays(GL_LINES, 0, 12);
	//Clean up state
	glDisableClientState(GL_VERTEX_ARRAY);
	glEnable(GL_DEPTH_TEST);
}


/***********************************************~***************************************************/


bool WCSketchWorkbench::OnUserMessage(const WCUserMessage &message) {
	//See if super wants to handle
	if (this->WCWorkbench::OnUserMessage(message)) return true;

	//Get list of selected sketch features
	std::list<WCSketchFeature*> features = this->SelectionManager()->FilterSelected<WCSketchFeature>(true);

	WCDrawingMode *mode;
	WCAction *action;
	
/*** -- Primitives -- ***/
	//Set the drawing mode to two-point arc
	if (message == "twoPointArc") {
		//Create a new drawing mode
		mode = WCSketchArc::ModeCreate(this, WCSketchArcType::Standard());
		//Go into arc mode
		this->DrawingMode(mode);
	}
	//Set the drawing mode to three-point arc
	else if (message == "threePointArc") {
		//Create a new drawing mode
		mode = WCSketchArc::ModeCreate(this, WCSketchArcType::ThreePoint() );
		//Go into three-point arc mode
		this->DrawingMode(mode);
	}
	//Set the drawing mode to axis line
	else if (message == "axisLine") {
		//Create a new drawing mode
		mode = WCSketchAxis::ModeCreate(this);
		//Go into axis mode
		this->DrawingMode(mode);
	}
	//Set the drawing mode to center circle
	else if (message == "centerCircle") {
		//Create a new drawing mode
		mode = WCSketchCircle::DrawingMode(this);
		//Go into circle mode
		this->DrawingMode(mode);
	}
	//Set the drawing mode to conic
	else if (message == "conic") {
		//Create a new drawing mode
		mode = WCSketchConicTwoPoint::ModeCreate(this);
		//Go into two-point conic mode
		this->DrawingMode(mode);
	}
	//Set the drawing mode to ellipse
	else if (message == "ellipse") {
		//Create a new drawing mode
		mode = WCSketchEllipse::ModeCreate(this);
		//Go into ellipse mode
		this->DrawingMode(mode);
	}
	//Set the drawing mode to line
	else if (message == "line") {
		//Create a new drawing mode
		mode = WCSketchLine::ModeCreate(this);
		//Go into line mode
		this->DrawingMode(mode);
	}
	//Set the drawing mode to point
	else if (message == "point") {
		//Create a new drawing mode
		mode = WCSketchPoint::ModeCreate(this);
		//Go into point mode
		this->DrawingMode(mode);
	}
	
/*** -- Profiles -- ***/

	//Set the drawing mode to rectangle
	else if (message == "rectangle") {
		//Create a new drawing mode
		mode = WCSketchRectangle::ModeCreate(this);
		//Go into rectangle mode
		this->DrawingMode(mode);
	}
	//Set the drawing mode to oriented rectangle
	else if (message == "orientedRect") {
		//Create a new drawing mode
//		mode = WCSketchOrientedRectangle::ModeCreate(this);
//		//Go into oriented rectangle mode
//		this->DrawingMode(mode);
	}
	//Set the drawing mode to centered rectangle
	else if (message == "centeredRect") {
		//Create a new drawing mode
//		mode = WCSketchCenteredRectangle::ModeCreate(this);
//		//Go into centered rectangle mode
//		this->DrawingMode(mode);
	}
	//Set the drawing mode to parallelogram
	else if (message == "parallelogram") {
		//Create a new drawing mode
//		mode = WCSketchParallelogram::ModeCreate(this);
//		//Go into parallelogram mode
//		this->DrawingMode(mode);
	}
	//Set the drawing mode to centered parallelogram
	else if (message == "centeredParallelogram") {
		//Create a new drawing mode
//		mode = WCSketchCenteredParallelogram::ModeCreate(this);
//		//Go into centered parallelogram mode
//		this->DrawingMode(mode);
	}
	//Set the drawing mode to oblong hole
	else if (message == "oblongHole") {
		//Create a new drawing mode
//		mode = WCSketchOblongHole::ModeCreate(this);
//		//Go into oblong hole mode
//		this->DrawingMode(mode);
	}
	//Set the drawing mode to oblong arc
	else if (message == "rectangle") {
		//Create a new drawing mode
//		mode = WCSketchOblongArc::ModeCreate(this);
//		//Go into oblong arc mode
//		this->DrawingMode(mode);
	}
	//Set the drawing mode to keyhole
	else if (message == "keyhole") {
		//Create a new drawing mode
//		mode = WCSketchKeyhole::ModeCreate(this);
//		//Go into keyhole mode
//		this->DrawingMode(mode);
	}
	//Set the drawing mode to hexagon
	else if (message == "hexagon") {
		//Create a new drawing mode
//		mode = WCSketchHexagon::ModeCreate(this);
//		//Go into hexagon mode
//		this->DrawingMode(mode);
	}

/*** -- Operations -- ***/

	//Set the drawing mode to trim
	else if (message == "trim") {
		//Create a new drawing mode
		mode = WCSketchTrim::ModeCreate(this);
		//Go into trim mode
		this->DrawingMode(mode);
	}

/*** -- Constraints -- ***/

	//Set the drawing mode to ellipse
	else if (message == "geometricConstraint") {
		//Create a new drawing mode
		mode = WCSketchConstraint::ModeCreateGeometric(this);
		//Go into geometric constraint mode
		this->DrawingMode(mode);
	}
	//Toggle the constraint visibility
	else if (message == "hideConstraints") {
		//Hide constraints
		this->_sketch->DrawConstraints( !_sketch->DrawConstraints() );
	}
	//Fix the selected feature
	else if (message == "fix") {
		//Fix the sketch feature
		if (features.size() == 1) {
			action = WCConstraintFix::ActionCreate(_sketch, "", features.front());
			this->_sketch->Document()->ExecuteAction( action );
		}
	}
	//Set the drawing mode to dimensional constraint
	else if (message == "dimensionConstraint") {
		//Create a new drawing mode
		mode = WCSketchConstraint::ModeCreateDimensional(this);
		//Go into dimensional constraint mode
		this->DrawingMode(mode);
	}

/*** -- Other -- ***/

	//Constraint analysis mode
	else if (message == "analyzeConstraint") {
		//Analyze the sketch constraints
		this->_sketch->AnalyzeConstraints();
	}
	//Analyze the sketch
	else if (message == "analyzeSketch") {
		//Analyze the sketch
		this->_sketch->Analyze();
	}
	//Toggle construction mode
	else if (message == "construction") {
		//Switch the construction mode
		this->IsConstruction( !this->IsConstruction() );
	}
	//Delete the selected elements
	else if (message == "delete") {
		//Delete selected sketch features
		action = WCSketch::ActionDelete(_sketch, this->SelectionManager()->FilterSelected<WCSketchFeature>(true) );
		this->_sketch->Document()->ExecuteAction( action );	
	}
	//Exit the workbench
	else if (message == "exitWorkbench") {
		//Exit sketcher
		this->_sketch->Document()->ExitWorkbench();
	}
	//Toggle grid snap
	else if (message == "gridSnap") {
		//Swap suggestion snap
		if (this->IsSuggestionSnap()) {
			this->_sketch->Document()->Status("Snap Off");
			this->IsSuggestionSnap(false);
			this->_grid->IsSnap(false);
		}
		else {	
			this->_sketch->Document()->Status("Snap On");
			this->IsSuggestionSnap(true);
			this->_grid->IsSnap(true);				
		}
	}
	//Selection mode
	else if (message == "select") {
		//Revert to default drawing mode
//		this->DrawingMode( WCDrawingMode::Selection(this) );
		this->DrawingMode( new WCSelectionMode(this) );
	}
	//Set grid visibility
	else if (message == "showGrid") {
		//Toggle grid visibility
		this->_grid->IsVisible( !this->_grid->IsVisible() );
		//Toggle showGrid button
		this->_sketch->Document()->ToolbarManager()->ButtonFromName("showGrid")->IsActive( this->_grid->IsVisible() );
	}
	//Otherwise this is an unknowe message
	else {
		CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCSketchWorkbench::OnUserMessage - Unknown user message received: " << message);
		return false;
	}
	//Message captured
	return true;
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCSketchWorkbench &wb) {
	return out;
}


/***********************************************~***************************************************/

