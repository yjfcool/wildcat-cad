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


//Feature Types
#include "Sketcher/sketch_point.h"
#include "Sketcher/sketch_line.h"
#include "Sketcher/sketch_circle.h"
#include "Sketcher/sketch_arc.h"
#include "Sketcher/sketch_ellipse.h"


//Constraint Types
#include "Constraint/constraint_angle.h"
#include "Constraint/constraint_distance.h"
#include "Constraint/constraint_length.h"
#include "Constraint/constraint_radius.h"


//Locally Defined Values
#define DIMENSIONALCONSTRAINT_LENGTH			0
#define DIMENSIONALCONSTRAINT_RADIUS			1
#define DIMENSIONALCONSTRAINT_SEMIMAJOR			2
#define DIMENSIONALCONSTRAINT_SEMIMINOR			3
#define DIMENSIONALCONSTRAINT_ANGLE				4
#define DIMENSIONALCONSTRAINT_DISTANCE			5


/***********************************************~***************************************************/


WCModeSketchConstraintDimensionalCreate::WCModeSketchConstraintDimensionalCreate(WCSketchWorkbench *wb) : 
	::WCDrawingMode(wb->Sketch(), SKETCHCONSTRAINT_DIMCREATE_NAME), _workbench(wb), _stage(0),
	_type(DIMENSIONALCONSTRAINT_DISTANCE), _position(), _offset(), _labelOffset(),
	_first(NULL), _second(NULL), _posA(WCMeasurePosition::Closest()), _posB(WCMeasurePosition::Closest()),
	_measure(NULL), _displayLine(NULL) {
	//Nothing else for now
}


void WCModeSketchConstraintDimensionalCreate::OnEntry(void) {
	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Entering SketchConstraint Dimensional Create Mode.");
	//Clear any selected items
	this->_workbench->SelectionManager()->Clear(true);
}


void WCModeSketchConstraintDimensionalCreate::OnExit(void) {
	//Delete measure if needed
	if (this->_measure != NULL) {
		delete this->_measure;
		this->_measure = NULL;
	}
	//Delete the display line if needed
	if (this->_displayLine != NULL) {
		delete this->_displayLine;
		this->_displayLine = NULL;		
	}
	CLOGGER_DEBUG(WCLogManager::RootLogger(), "Exiting SketchConstraint Dimensional Create Mode.");
}


void WCModeSketchConstraintDimensionalCreate::OnMouseDown(const WCMouseButton &button) {
	WCObject *top;
	WCSketchConstraint *constraint;

	//Clear the selection (with no signal)
	this->_workbench->SelectionManager()->Clear(false);
	//See if something is selected (one-hit only)
	WCScene *scene = this->_workbench->Feature()->Document()->Scene();
	//Convert scene position to screen position
	WPUInt xPos = (WPUInt)((scene->MouseX() + (scene->Width()/2.0)) * (scene->WindowWidth() / scene->Width()));
	WPUInt yPos = (WPUInt)(scene->WindowHeight() - (scene->MouseY() - scene->Height() / 2.0) * (-1.0 * scene->WindowHeight() / scene->Height()));
	//Call selection
	this->_workbench->SelectionManager()->Select(xPos, xPos, yPos, yPos, false);

	//Switch on stage (stage 0 == no clicks)
	if (this->_stage == 0) {
		//See how many geometric items selected (must have something to start with)
		if (this->_workbench->SelectionManager()->CountVisuals() == 0) return;
		//Get the mouse position
		this->_position.Set(this->_workbench->MouseX(), this->_workbench->MouseY(), 0.0, 1.0);
		//Get top selection
		top = this->_workbench->SelectionManager()->Top()->Associate();
		//See if top is a constraint
		constraint = dynamic_cast<WCSketchConstraint*>(top);
		//Set first marker
		this->_first = dynamic_cast<WCSketchFeature*>(top);
		//Make sure is sketch object and not a constraint
		if ((constraint != NULL) || (this->_first == NULL)) {
			//Clear the selection again
			this->_workbench->SelectionManager()->Clear(false);
			return;
		}

		//See if is line
		WCSketchLine* line = dynamic_cast<WCSketchLine*>(this->_first);
		if (line != NULL) {
			//Set the current constraint type
			this->_type = DIMENSIONALCONSTRAINT_LENGTH;
			//Determine values for measure
			WCVector4 p0( line->Base()->Begin() );
			WCVector4 p1( line->Base()->End() );
			//Calcuate length of line
			WPFloat length = p0.Distance(p1);
			//Determine offsets
			this->_offset = 0.0;
			this->_labelOffset = p0.Distance(this->_position) / length;
			//Set the measure label
			std::string label = line->Document()->LengthUnit()->DisplayString(length, 4);
			//Create the rendering measure
			this->_measure = new WCConstraintMeasureTwoPoint(line, label, WCMeasureType::Absolute(), p0, p1, 
															 this->_workbench->Sketch()->ReferencePlane()->InverseTransformMatrix(),
															 this->_workbench->Sketch()->ReferencePlane()->TransformMatrix(), 
															 this->_offset, this->_labelOffset);
			this->_measure->Text()->Color(WCSketchFeature::SelectedColor);
			//Go to the next stage
			this->_stage++;
			return;
		}

		//See if it is an arc
		WCSketchArc* arc = dynamic_cast<WCSketchArc*>(this->_first);
		if (arc != NULL) {
			this->_type = DIMENSIONALCONSTRAINT_RADIUS;
			//Get the location in 3D space
			WCVector4 position = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * this->_position;
			WCGeometricPoint *point = new WCGeometricPoint(position);
			//Project position onto curve to get label offset
			this->_offset = 0.0;
			std::list<WPFloat> hitList = arc->Base()->Intersect(point, 0.01);
			if (hitList.empty()) {
				CLOGGER_ERROR(WCLogManager::RootLogger(), "WCModeSketchConstraintDimensionalCreate::OnMouseDown - No arc hits returned");
				return;
			}
			this->_labelOffset = hitList.front();
			delete point;
			//Calcuate radius of arc
			WPFloat radius = arc->Radius();
			//Set the measure label
			std::string label = "R " + arc->Document()->LengthUnit()->DisplayString(radius, 4);
			//Create the rendering measure
			WCVector4 center = arc->Center();
			center = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * center;
			this->_measure = new WCConstraintMeasureRadius(arc, label, arc->Base(), center,
														   this->_workbench->Sketch()->ReferencePlane()->InverseTransformMatrix(),
														   this->_workbench->Sketch()->ReferencePlane()->TransformMatrix(), 
														   this->_offset, this->_labelOffset);
			this->_measure->Text()->Color(WCSketchFeature::SelectedColor);
			//Go to the next stage
			this->_stage++;
			return;
		}

		//See if it is a circle
		WCSketchCircle* circle = dynamic_cast<WCSketchCircle*>(this->_first);
		if (circle != NULL) {
			//Need to check if center point is selected
			if (this->_workbench->SelectionManager()->TopVisual() == circle->CenterPoint()) {
				std::cout << "Center point selected\n";
			}

			this->_type = DIMENSIONALCONSTRAINT_RADIUS;
			//Get the location in 3D space
			WCVector4 position = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * this->_position;
			WCGeometricPoint *point = new WCGeometricPoint(position);
			//Project position onto curve to get label offset
			this->_offset = 0.0;
			std::list<WPFloat> hitList = circle->Base()->Intersect(point, 0.01);
			if (hitList.empty()) {
				CLOGGER_ERROR(WCLogManager::RootLogger(), "WCModeSketchConstraintDimensionalCreate::OnMouseDown - No circle hits returned");
				return;
			}
			this->_labelOffset = hitList.front();
			delete point;
			//Calcuate radius of arc
			WPFloat radius = circle->Radius();
			//Set the measure label
			std::string label = "R " + circle->Document()->LengthUnit()->DisplayString(radius, 4);
			//Create the rendering measure
			WCVector4 center = circle->Center();
			center = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * center;
			this->_measure = new WCConstraintMeasureRadius(circle, label, circle->Base(), center,
														   this->_workbench->Sketch()->ReferencePlane()->InverseTransformMatrix(),
														   this->_workbench->Sketch()->ReferencePlane()->TransformMatrix(), 
														   this->_offset, this->_labelOffset);
			this->_measure->Text()->Color(WCSketchFeature::SelectedColor);
			//Go to the next stage
			this->_stage++;
			return;
		}

		//See if it is an ellipse
		WCSketchEllipse* ellipse = dynamic_cast<WCSketchEllipse*>(this->_first);
		if (ellipse != NULL) {
			std::cout << "1st = semi-major or semi-minor!\n";
//			//Set the current constraint type
//			this->_type = DIMENSIONALCONSTRAINT_SEMIMAJOR;
//			//Project position onto curve to get label offset
//			//...
//			this->_labelOffset = 0.5;
//			WCVector4 p0( ellipse->Center() );
//			WCVector4 p1( ellipse->SemiMajor() );
//			//Calcuate length of line
//			WPFloat length = p0.Distance(p1);
//			//Determine offset
//			this->_offset = 0.0;
//			//Set the measure label
//			std::string label = ellipse->Document()->LengthUnit()->DisplayString(length, 4);
//			//Create the rendering measure
//			this->_measure = new WCConstraintMeasureTwoPoint(ellipse, label, WCMeasureType::Absolute(), p0, p1, 
//															 this->_workbench->Sketch()->ReferencePlane()->InverseTransformMatrix(),
//															 this->_workbench->Sketch()->ReferencePlane()->TransformMatrix(),
//															 this->_offset, this->_labelOffset);
//			this->_measure->Text()->Color(WCSketchFeature::SelectedColor);
			//Go to the next stage
			this->_stage++;
			return;
		}

		//Otherwise, just sticking with distance
		this->_type = DIMENSIONALCONSTRAINT_DISTANCE;
		WCVector4 position = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * this->_position;
/*** DEBUG ***/
		//Set the first measure position
		this->_posA = WCMeasurePosition::Closest();
/*** DEBUG ***/
		//Create display line
		this->_displayLine = new WCGeometricLine(position, position);
		//Go to the next stage
		this->_stage++;
		return;
	}

	/*** On second click ***/

	else if (this->_stage == 1) {
		//If type is length
		if (this->_type == DIMENSIONALCONSTRAINT_LENGTH) {
			//See if any non-constraint features are selected
			if ((this->_workbench->SelectionManager()->Count() == 0) ||
				(this->_workbench->SelectionManager()->FilterSelected<WCSketchConstraint>(true).size() != 0)) {
				//Finishing with length
				this->_stage = 3;
				return;
			}
			//Switching to angle or distance
			else {
				//Delete the measure
				delete this->_measure;
				this->_measure = NULL;
				//Set second marker
				top = this->_workbench->SelectionManager()->Top()->Associate();
				this->_second = dynamic_cast<WCSketchFeature*>(top);

				//Determine if switching to angle (second selection must be a line)
				WCSketchLine *lineB = dynamic_cast<WCSketchLine*>(this->_second);
				if (lineB != NULL) {
					this->_type = DIMENSIONALCONSTRAINT_ANGLE;
					//Cast first object to a line
					WCSketchLine *lineA = dynamic_cast<WCSketchLine*>(this->_first);
					//Set the position vector to new location (slew from here)
					this->_position.Set(this->_workbench->MouseX(), this->_workbench->MouseY(), 0.0, 1.0);
					//Determine point of intersection (center) between lines
					WCVector4 center;
					WPFloat dummyA, dummyB;
					WPUInt retVal = RayRayIntersection(lineA->Begin(), lineA->End() - lineA->Begin(), 
													   lineB->Begin(), lineB->End() - lineB->Begin(), 
													   dummyA, dummyB, center);
					//Make sure the lines intersect
					if (retVal != INTERSECTION_INTERSECTION) {
						CLOGGER_WARN(WCLogManager::RootLogger(), "WCModeSketchConstraintDimensionalCreate::OnMouseDown - Non-itersecting lines selected");
						return;
					}
					center = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * center;

					//Determine first point
					WCVector4 firstPt = lineA->Base()->Evaluate(0.5);
					//Determine second point (in 3D space project click onto lineB)
					WCVector4 secondPt = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * this->_position;
					secondPt = lineB->Base()->PointInversion(secondPt).first;

					//Get the angle (for some reason Angle returns in degrees)
					WPFloat angle = Angle(firstPt, center, secondPt) / 180.0 * M_PI;
					//Determine the offset (distance from center to click)
					this->_offset = center.Distance(secondPt);
					//Label offset will be 1.0 because we just clicked on lineB
					this->_labelOffset = 1.0;
					//Set the measure label
					std::string label = this->_first->Document()->AngleUnit()->DisplayString(angle, 4);
					this->_measure = new WCConstraintMeasureAngle(this->_first, label, center, firstPt, secondPt, true,
																  this->_workbench->Sketch()->ReferencePlane()->InverseTransformMatrix(),
																  this->_workbench->Sketch()->ReferencePlane()->TransformMatrix(),
																  this->_offset, this->_labelOffset);
					//Moving to next stage
					this->_stage++;
					return;
				}
				//Otherwise, switch to distance
				std::cout << "2nd switch to distance\n";
				this->_type = DIMENSIONALCONSTRAINT_DISTANCE;
			}
		}

		//If type is radius
		if (this->_type == DIMENSIONALCONSTRAINT_RADIUS) {
			//See if any non-constraint features are selected
			if ((this->_workbench->SelectionManager()->Count() == 0) ||
				(this->_workbench->SelectionManager()->FilterSelected<WCSketchConstraint>(true).size() != 0)) {
				//Finishing with radius
				this->_stage = 3;
				return;
			}
			//Switching to distance
			else {
				std::cout << "2nd switch to distance\n";
				this->_type = DIMENSIONALCONSTRAINT_DISTANCE;
				//Create distance measure between features
				//...
			}
			//Moving to next stage
			this->_stage++;
			return;
		}

		//If type is semi
		if (this->_type == DIMENSIONALCONSTRAINT_SEMIMAJOR) {
			//See if any non-constraint features are selected
			if ((this->_workbench->SelectionManager()->Count() == 0) ||
				(this->_workbench->SelectionManager()->FilterSelected<WCSketchConstraint>(true).size() != 0)) {
				//Finishing with radius
				this->_stage = 3;
				return;
			}
			//Switching to distance
			else {
				std::cout << "2nd switch to distance\n";
				this->_type = DIMENSIONALCONSTRAINT_DISTANCE;
				//Create distance measure between features
				//...
			}
			//Moving to next stage
			this->_stage++;
			return;
		}

		//If type is distance
		if (this->_type == DIMENSIONALCONSTRAINT_DISTANCE) {
			//See if any non-constraint features are selected
			if ((this->_workbench->SelectionManager()->Count() == 0) ||
				(this->_workbench->SelectionManager()->FilterSelected<WCSketchConstraint>(true).size() != 0)) {
				//Need to select something, so move on
				return;
			}
			//Delete the measure (if present)
			if (this->_measure != NULL) {
				delete this->_measure;
				this->_measure = NULL;
			}
			//Set second marker
			top = this->_workbench->SelectionManager()->Top()->Associate();
			this->_second = dynamic_cast<WCSketchFeature*>(top);
			//Set the position vector to new location (slew from here)
			this->_position.Set(this->_workbench->MouseX(), this->_workbench->MouseY(), 0.0, 1.0);

			std::cout << "In distance creation\n";
			//Try to categorize the measure
			WCSketchPoint *p0 = dynamic_cast<WCSketchPoint*>(this->_first);
			WCSketchPoint *p1 = dynamic_cast<WCSketchPoint*>(this->_second);

			/*** Point-point case ***/
			if ((p0 != NULL) && (p1 != NULL)) {
				std::cout << "Point-point case\n";
				//Get the points
				WCVector4 pt0 = p0->Base()->Data();
				WCVector4 pt1 = p1->Base()->Data();
				//Get the absolute distance between points
				WPFloat distance = pt0.Distance(pt1);
				//Get label for the measure
				std::string label = this->_first->Document()->LengthUnit()->DisplayString(distance, 4);
				//Create the measure
				this->_measure = new WCConstraintMeasureTwoPoint(this->_first, label, WCMeasureType::Absolute(), pt0, pt1,
																 this->_first->Sketch()->ReferencePlane()->InverseTransformMatrix(),
																 this->_first->Sketch()->ReferencePlane()->TransformMatrix(), 0.0, 1.0);
			}

			/*** Point-line case ***/
			else if (((p0 != NULL) || (p1 != NULL)) && ((p0 == NULL) || (p1 == NULL))) {
				std::cout << "Point-Curve case\n";
				WCGeometricCurve *curve;
				//Make sure p0 is the point and curveA is the curve
				if (p1 != NULL) {
					p0 = p1;
					//Get the underlying curve
					curve = dynamic_cast<WCGeometricCurve*>( this->_first->Base() );
				}
				//Otherwise, p0 is the point already
				else {
					//Get the underlying curve
					curve = dynamic_cast<WCGeometricCurve*>( this->_second->Base() );
				}

				//Set the measure position for the second
				this->_posB = WCMeasurePosition::Closest();
				WCVector4 position = this->_first->Sketch()->ReferencePlane()->TransformMatrix() * this->_position;
				WCVector4 pt0;
				WCVector4 pt1;
				WCMeasureType type = WCMeasureType::Absolute();
				//Set pt0
				pt0 = p0->Base()->Data();
				//Find where clicked
				std::pair<WCVector4,WPFloat> piResult = curve->PointInversion(position);
				pt1 = piResult.first;
				WPFloat u = piResult.second;

				//Begin case (first 10% of curve)
				if (u < 0.1) {
					//Get very beginning of line
					pt1 = curve->Evaluate(0.0);
					//Set position
					this->_posB = WCMeasurePosition::Begin();
				}
				//End case (last 10% of curve)
				else if (u > 0.9) {
					//Get very beginning of line
					pt1 = curve->Evaluate(1.0);
					//Set position
					this->_posB = WCMeasurePosition::End();
				}
				//Closest case (middle 80% of curve)
				else {
					//Project onto the line
					piResult = curve->PointInversion(pt0);
					pt1 = piResult.first;
					u = piResult.second;

					//Set position
					this->_posB = WCMeasurePosition::Closest();
				}

				//Get the absolute distance between points
				WPFloat distance = pt0.Distance(pt1);
				//Get label for the measure
				std::string label = this->_first->Document()->LengthUnit()->DisplayString(distance, 4);
				//Create the measure
				this->_measure = new WCConstraintMeasureTwoPoint(this->_first, label, type, pt0, pt1,
																 this->_first->Sketch()->ReferencePlane()->InverseTransformMatrix(),
																 this->_first->Sketch()->ReferencePlane()->TransformMatrix(), 0.0, 1.0);
			}

			/*** Default to curve-curve case ***/
			else {
				std::cout << "Curve-curve case\n";
			}

			//Delete the line if present
			if (this->_displayLine != NULL) {
				delete this->_displayLine;
				this->_displayLine = NULL;
			}
			//Moving to next stage
			this->_stage++;
			return;
		}
	}
	
	/*** On third click ***/

	else {
		//Finishing with either angle or distance
		this->_stage = 3;
		return;
	}
}


void WCModeSketchConstraintDimensionalCreate::OnMouseMove(const WPFloat &x, const WPFloat &y) {
	//Exit if stage 0 or no measure
	if (this->_stage == 0) return;
	//Get current position
	WPFloat xSlew = this->_workbench->MouseX() - this->_position.I();
	WPFloat ySlew = this->_workbench->MouseY() - this->_position.J();

	//If type is length, radius, semi
	if ((this->_type == DIMENSIONALCONSTRAINT_LENGTH) ||
		(this->_type == DIMENSIONALCONSTRAINT_RADIUS) ||
		(this->_type == DIMENSIONALCONSTRAINT_SEMIMAJOR) ||
		(this->_type == DIMENSIONALCONSTRAINT_SEMIMINOR) ||
		(this->_type == DIMENSIONALCONSTRAINT_ANGLE) ) {
		//Move measure back to initial position
		this->_measure->Offsets(this->_offset, this->_labelOffset);
		//Slew the measure offsets
		this->_measure->SlewOffsets(xSlew, ySlew);
	}
	//If type is distance
	else if (this->_type == DIMENSIONALCONSTRAINT_DISTANCE) {
		//If stage == 1
		if (this->_stage == 1) {
			//Get the current position
			WCVector4 position(this->_workbench->MouseX(), this->_workbench->MouseY(), 0.0, 1.0);
			position = this->_workbench->Sketch()->ReferencePlane()->TransformMatrix() * position;
			//Reposition the in-process line

			this->_displayLine->End(position);
			return;
		}

		//Cast to a twopoint measure
		WCConstraintMeasureTwoPoint *measure = dynamic_cast<WCConstraintMeasureTwoPoint*>(this->_measure);
		//Determine angle from second to current location (use slew)
		WPFloat angle = atan2(ySlew, xSlew);
		WPFloat distance;
		std::string label;
		WCVector4 pt0 = measure->FirstPoint();
		WCVector4 pt1 = measure->SecondPoint();
		//See if should be vertical ()
		if (fabs(angle) > (5.0 * M_PI / 6.0) || fabs(angle) < (M_PI / 6.0)) {
			//Switch measure type (if needed)
			if (measure->Type() != WCMeasureType::Vertical()) {
				//Change the measure
				measure->Type(WCMeasureType::Vertical());
				//Get new distance measure
				distance = pt0.J() - pt1.J();
				//Set label for the measure
				label = this->_first->Document()->LengthUnit()->DisplayString(distance, 4);
				this->_measure->Text()->Text(label);
			}
		}

		//See if horizontal
		else if (fabs(angle) > (M_PI / 3.0) && fabs(angle) < (4.0 * M_PI / 6.0)) {
			//Switch measure type (if needed)
			if (measure->Type() != WCMeasureType::Horizontal()) {
				measure->Type(WCMeasureType::Horizontal());
				//Update measure label
				distance = pt0.I() - pt1.I();
				//Set label for the measure
				label = this->_first->Document()->LengthUnit()->DisplayString(distance, 4);
				this->_measure->Text()->Text(label);
			}
		}

		//Default to absolute
		else {
			if (measure->Type() != WCMeasureType::Absolute()) {
				//Switch measure type
				measure->Type(WCMeasureType::Absolute());
				//Update measure label
				distance = pt0.Distance( pt1 );
				//Set label for the measure
				label = this->_first->Document()->LengthUnit()->DisplayString(distance, 4);
				this->_measure->Text()->Text(label);
			}
		}

		//Move measure back to initial position
		this->_measure->Offsets(this->_offset, this->_labelOffset);
		//Slew the measure offsets
		this->_measure->SlewOffsets(xSlew, ySlew);
	}
	//Error case
	else CLOGGER_ERROR(WCLogManager::RootLogger(), "WCModeSketchConstraintDimensionalCreate::OnMouseMove - Invalid type");
}


void WCModeSketchConstraintDimensionalCreate::OnMouseUp(const WCMouseButton &button) {
	//See if complete
	if (this->_stage == 3) {
		//If type is length
		if (this->_type == DIMENSIONALCONSTRAINT_LENGTH) {
			//Cast to a line
			WCSketchLine* line = dynamic_cast<WCSketchLine*>(this->_first);
			//Create a length constraint on line
			WCActionConstraintLengthCreate *lengthConstraint = WCConstraintLength::ActionCreate(this->_workbench->Sketch(), "", line);
			this->_workbench->Sketch()->Document()->ExecuteAction( lengthConstraint );
			//Set its offsets appropriately
			WCConstraintLength* constraint = lengthConstraint->Constraint();
			constraint->Measure()->Offsets(this->_measure->Offset(), this->_measure->LabelOffset());
		}

		//If type is radius
		if (this->_type == DIMENSIONALCONSTRAINT_RADIUS) {
			//See if we can cast to an arc
			WCSketchArc* arc = dynamic_cast<WCSketchArc*>(this->_first);
			WCSketchCircle* circle = dynamic_cast<WCSketchCircle*>(this->_first);
			WCActionConstraintRadiusCreate *radiusConstraint;
			//Arc path
			if (arc != NULL) {		
				//Create a length constraint on arc
				radiusConstraint = WCConstraintRadius::ActionCreate(this->_workbench->Sketch(), "", arc);
			}
			else {
				//Create a length constraint on circle
				radiusConstraint = WCConstraintRadius::ActionCreate(this->_workbench->Sketch(), "", circle);
			}
			this->_workbench->Sketch()->Document()->ExecuteAction( radiusConstraint );
			//Set its offsets appropriately
			WCConstraintRadius* constraint = radiusConstraint->Constraint();
			constraint->Measure()->Offsets(this->_measure->Offset(), this->_measure->LabelOffset());
		}

		//If type is semi-major
		//...

		//If type is semi-minor
		//...

		//If type is angle
		if (this->_type == DIMENSIONALCONSTRAINT_ANGLE) {
			//Cast to lines
			WCSketchLine* lineA = dynamic_cast<WCSketchLine*>(this->_first);
			WCSketchLine* lineB = dynamic_cast<WCSketchLine*>(this->_second);
			//Create an angle constraint on lines
			WCActionConstraintAngleCreate *angleConstraint = WCConstraintAngle::ActionCreate(this->_workbench->Sketch(), "", lineA, lineB);
			this->_workbench->Sketch()->Document()->ExecuteAction( angleConstraint );
			//Set its offsets appropriately
			WCConstraintAngle* constraint = angleConstraint->Constraint();
			constraint->Measure()->Offsets(this->_measure->Offset(), this->_measure->LabelOffset());
		}

		//If type is distance
		if (this->_type == DIMENSIONALCONSTRAINT_DISTANCE) {
			//Cast measure to distance measure
			WCConstraintMeasureTwoPoint *measure = dynamic_cast<WCConstraintMeasureTwoPoint*>(this->_measure);
			//Create a distance constraint
			WCActionConstraintDistanceCreate *distanceConstraint = WCConstraintDistance::ActionCreate(this->_workbench->Sketch(), "", 
																		measure->Type(), this->_first, this->_second, this->_posA, this->_posB);
			this->_workbench->Sketch()->Document()->ExecuteAction( distanceConstraint );
			//Set its offsets appropriately
			WCConstraintDistance* constraint = distanceConstraint->Constraint();
			constraint->Measure()->Offsets(this->_measure->Offset(), this->_measure->LabelOffset());			
		}

		//Reset all of the state
		this->_stage = 0;
		this->_type = DIMENSIONALCONSTRAINT_DISTANCE;
		this->_first = NULL;
		this->_second = NULL;
		//Reset the measure
		if (this->_measure != NULL) {
			delete this->_measure;
			this->_measure = NULL;
		}
		//Reset the display line
		if (this->_displayLine != NULL) {
			delete this->_displayLine;
			this->_displayLine = NULL;		
		}
	}
}


void WCModeSketchConstraintDimensionalCreate::Render(void) {
	//Only render valid measures
	if (this->_measure != NULL)
		this->_measure->Render(WCSketchFeature::SelectedColor);
	//Only render valid line
	if (this->_displayLine != NULL)
		this->_displayLine->Render(0, WCSketchFeature::SelectedColor, 1.0);
}


/***********************************************~***************************************************/

