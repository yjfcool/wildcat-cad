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
#include "Sketcher/sketch_ellipse.h"
#include "Sketcher/sketch.h"
#include "Sketcher/sketch_point.h"
#include "Kernel/document.h"
#include "PartDesign/part_plane.h"


/***********************************************~***************************************************/


void WCSketchEllipse::GenerateBase(void) {
	//Get unit direction of minor line
	WCVector4 xUnit = this->_majorAxis;
	WCVector4 yUnit(xUnit.J(), -xUnit.I(), 0.0, 0.0);
	WCVector4 center = this->_sketch->ReferencePlane()->TransformMatrix() * this->_center;
	xUnit = this->_sketch->ReferencePlane()->TransformMatrix() * xUnit;
	yUnit = this->_sketch->ReferencePlane()->TransformMatrix() * yUnit;
	//Get position of 3rd point
	WCVector4 major = center + (xUnit * this->_semiMajor);
	WCVector4 minor = center + (yUnit * this->_semiMinor);

	//Are the curve and center point already around
	if (this->_curve != NULL) {
		std::vector<WCVector4> controlPoints;
		std::vector<WPFloat> knotPoints;
		//Get updated control points and knot points vectors
		WCNurbs::EllipsePoints(center, xUnit, yUnit, this->_semiMajor, this->_semiMinor, controlPoints, knotPoints);
		//Update the curve
		this->_curve->ControlPoints(controlPoints);
		//Update the points
		this->_centerPoint->Set(center);
		this->_majorPoint->Set(major);
		this->_minorPoint->Set(minor);
	}
	//Setup new curve and point
	else {
		//Create ellipse
		this->_curve = WCNurbsCurve::Ellipse(this->_document->Scene()->GeometryContext(), center, xUnit, yUnit,
											 this->_semiMajor, this->_semiMinor);
		//Create the center point
		this->_centerPoint = new WCGeometricPoint(center);
		//Create the semiMajor point
		this->_majorPoint = new WCGeometricPoint(major);
		//Create the semiMinor point
		this->_minorPoint = new WCGeometricPoint(minor);

		//Set properties for curve and points
		this->_curve->Color(WCSketchFeature::DefaultColor);
		this->_curve->Thickness(WCSketchFeature::LineThickness);
		this->_centerPoint->Color(WCSketchFeature::ConstructionColor);
		this->_centerPoint->Size(WCSketchFeature::PointSize);
		this->_centerPoint->IsConstruction(true);
		this->_majorPoint->Color(WCSketchFeature::ConstructionColor);
		this->_majorPoint->Size(WCSketchFeature::PointSize);
		this->_majorPoint->IsConstruction(true);
		this->_minorPoint->Color(WCSketchFeature::ConstructionColor);
		this->_minorPoint->Size(WCSketchFeature::PointSize);
		this->_minorPoint->IsConstruction(true);
	}
}


void WCSketchEllipse::Initialize(void) {
	//Create the point controller
	this->_controller = new WCSketchEllipseController(this);
	//Check feature name
	if (this->_name == "") this->_name = this->_sketch->GenerateFeatureName(this);
	//Add into sketch
	if ((!this->_sketch->AddFeature(this)) || (this->_controller == NULL)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchEllipse::WCSketchEllipse - Problem adding feature or curve to sketch.");
		//Should delete base
		//throw error
		return;
	}

	//Retain the curve, and center, major and minor points
	this->_sketch->AddCurve(this->_curve, this->_controller);
	this->_curve->Retain(*this);
	this->_sketch->AddPoint(this->_centerPoint, this->_controller);
	this->_centerPoint->Retain(*this);
	this->_sketch->AddPoint(this->_majorPoint, this->_controller);
	this->_majorPoint->Retain(*this);
	this->_sketch->AddPoint(this->_minorPoint, this->_controller);
	this->_minorPoint->Retain(*this);

	//Create tree element and add into the tree (beneath the sketch features element)
	WSTexture *ellipseIcon = this->_document->Scene()->TextureManager()->TextureFromName("ellipse32");
	this->_treeElement = new WCTreeElement(	this->_sketch->Document()->TreeView(), this->_name, this->_controller, ellipseIcon);
	this->_sketch->FeaturesTreeElement()->AddLastChild(this->_treeElement);
}


/***********************************************~***************************************************/


WCSketchEllipse::WCSketchEllipse(WCSketch *sketch, const std::string &name, const WCVector4 &center,
	const WCVector4 &majorAxis, const WPFloat &semiMajor, const WPFloat &semiMinor) : ::WCSketchFeature(sketch),
	_center(center), _majorAxis(majorAxis), _semiMajor(semiMajor), _semiMinor(semiMinor), _curve(NULL) {
	//Validate sketch
	if (this->_sketch == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchEllipse::WCSketchEllipse - NULL Sketch passed.\n");
		//throw error
		return;
	}
	//Generate ellipse and points
	this->GenerateBase();
	//Initialize
	this->Initialize();
}


WCSketchEllipse::WCSketchEllipse(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCSketchFeature( WCSerializeableObject::ElementFromName(element,"SketchFeature"), dictionary),
	_center(), _majorAxis(), _semiMajor(), _semiMinor(0.0), _curve(NULL) {
	//Make sure element if not null
	if (element == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchEllipse::WCSketchEllipse - NULL Element passed.");
		//throw error
		return;
	}
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

//	//Setup center
//	this->_center.FromElement( WCSerializeableObject::ElementFromName(element,"Center") );
//	//Setup radius
//	this->_radius = WCSerializeableObject::GetBoolAttrib(element, "radius");
//	//Setup base
//	this->_curve = new WCNurbsCurve( WCSerializeableObject::ElementFromName(element,"NURBSCurve"), dictionary );
//	//Setup center point
//	this->_centerPoint = new WCGeometricPoint( WCSerializeableObject::ElementFromName(element,"GeometricPoint"), dictionary );
//	//Initialize the object
//	this->Initialize();
}


WCSketchEllipse::~WCSketchEllipse() {
	//Remove from the sketch
	if (!this->_sketch->RemoveFeature(this)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchEllipse::WCSketchEllipse - Problem removing feature from sketch.");	
	}
	//Remove the curve, release it, and try to delete it
	if (this->_curve != NULL) {
		this->_sketch->RemoveCurve(this->_curve);
		this->_curve->Release(*this);
		if (this->_curve->RefCount() == 0) delete this->_curve;
	}
	//Release the center point, release it, and try to delete it
	if (this->_centerPoint != NULL) {
		this->_sketch->RemovePoint(this->_centerPoint);
		this->_centerPoint->Release(*this);
		if (this->_centerPoint->RefCount() == 0) delete this->_centerPoint;
	}
	//Release the major point, release it, and try to delete it
	if (this->_majorPoint != NULL) {
		this->_sketch->RemovePoint(this->_majorPoint);
		this->_majorPoint->Release(*this);
		if (this->_majorPoint->RefCount() == 0) delete this->_majorPoint;
	}
	//Release the minor point, release it, and try to delete it
	if (this->_minorPoint != NULL) {
		this->_sketch->RemovePoint(this->_minorPoint);
		this->_minorPoint->Release(*this);
		if (this->_minorPoint->RefCount() == 0) delete this->_minorPoint;
	}
}
	

void WCSketchEllipse::Center(const WCVector4 &center) {
	//Set the center
	this->_center = center;
	//Generate ellipse and points
	this->GenerateBase();
}


void WCSketchEllipse::SemiMajor(const WCVector4 &axis, const WPFloat &radius) {
	//Set the major axis value
	this->_majorAxis = axis;
	//Set the semiMajor radius
	this->_semiMajor = radius;
	//Generate ellipse and points
	this->GenerateBase();
}


void WCSketchEllipse::SemiMinor(const WPFloat &minor) {
	//Set the semiminor point
	this->_semiMinor = minor;
	//Generate ellipse and points
	this->GenerateBase();
}


void WCSketchEllipse::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
	//Updates can be for position change, or color change
//	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchLine::ReceiveNotice - Invalid notice received.");

}


bool WCSketchEllipse::Regenerate(void) {
	CLOGGER_WARN(WCLogManager::RootLogger(), "WCSketchLine::Regenerate - Not yet implemented.");
	return false;
}


void WCSketchEllipse::InjectConstraints(WCConstraintPlanner *planner) {
	//Nothing for now
}


std::list<WCObject*> WCSketchEllipse::DeletionDependencies(void) {
	//Only downward dependency is the base curve
	return std::list<WCObject*>(1, this->_curve);
}


void WCSketchEllipse::IsConstruction(const bool state) {
	//See if something is changing
	if (state == this->_isConstruction) return;
	//Set state and base state
	this->_isConstruction = state;
	this->_curve->IsConstruction(state);
	
	//Exit if selected
	if (this->_isSelected) return;
	//Otherwise, set color appropriately
	if (this->_isConstruction) this->_curve->Color(WCSketchFeature::ConstructionColor);
	else this->_curve->Color(WCSketchFeature::DefaultColor);
}


void WCSketchEllipse::OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) {
	//Set the color of the base curve
	this->_curve->Color(WCSketchFeature::SelectedColor);
	//Set color and show center point
	this->_centerPoint->Color(WCSketchFeature::SelectedColor);
	//Set color and show major point
	this->_majorPoint->Color(WCSketchFeature::SelectedColor);
	//Set color and show minor point
	this->_minorPoint->Color(WCSketchFeature::SelectedColor);
	//Mark this as selected
	this->_isSelected = true; 
}


void WCSketchEllipse::OnDeselection(const bool fromManager) {
	//Set the color of the base curve - depends on construction
	if (this->_isConstruction) this->_curve->Color(WCSketchFeature::ConstructionColor);
	else this->_curve->Color(WCSketchFeature::DefaultColor);

	//Set color and show center point
	if (this->_centerPoint->IsConstruction()) this->_centerPoint->Color(WCSketchFeature::ConstructionColor);
	else this->_centerPoint->Color(WCSketchFeature::DefaultColor);

	//Set color and show major point
	if (this->_majorPoint->IsConstruction()) this->_majorPoint->Color(WCSketchFeature::ConstructionColor);
	else this->_majorPoint->Color(WCSketchFeature::DefaultColor);

	//Set color and show minor point
	if (this->_minorPoint->IsConstruction()) this->_minorPoint->Color(WCSketchFeature::ConstructionColor);
	else this->_minorPoint->Color(WCSketchFeature::DefaultColor);

	//Mark this as not selected
	this->_isSelected = false; 
}

	
xercesc::DOMElement* WCSketchEllipse::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	CLOGGER_WARN(WCLogManager::RootLogger(), "WCSketchEllipse::Serialize - Not yet implemented.");
	return NULL;
}

	
/***********************************************~***************************************************/


WCDrawingMode* WCSketchEllipse::ModeCreate(WCSketchWorkbench *wb) {
	//Create a new drawing mode
	return new WCModeSketchEllipseCreate(wb);
}


WCDrawingMode* WCSketchEllipse::ModeEdit(WCSketchEllipse *ellipse) {
	//Create a new editing mode
	return new WCModeSketchEllipseEdit(ellipse);
}


WCActionSketchEllipseCreate* WCSketchEllipse::ActionCreate(WCSketch *sketch, const std::string &ellipseName, 
	const WCVector4 &center, const WCVector4 &majorAxis, const WPFloat &semiMajor, const WPFloat &semiMinor) {
	//Create new action to create feature
	return new WCActionSketchEllipseCreate(sketch, ellipseName, center, majorAxis, semiMajor, semiMinor);
}


WCActionSketchEllipseMove* WCSketchEllipse::ActionMove(WCSketchEllipse *ellipse, const WCVector4 &centerPos) {
	//Create new action to move feature
	WCActionSketchEllipseMove *action = new WCActionSketchEllipseMove(ellipse, centerPos);
	return action;
}


WCActionSketchEllipseMajor* WCSketchEllipse::ActionMajor(WCSketchEllipse *ellipse, const WCVector4 &majorAxis, const WPFloat &major) {
	//Create new action to move major position
	WCActionSketchEllipseMajor *action = new WCActionSketchEllipseMajor(ellipse, majorAxis, major);
	return action;
}


WCActionSketchEllipseMinor* WCSketchEllipse::ActionMinor(WCSketchEllipse *ellipse, const WPFloat &minor) {
	//Create new action to change feature minor radius
	WCActionSketchEllipseMinor *action = new WCActionSketchEllipseMinor(ellipse, minor);
	return action;
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCSketchEllipse &ellipse) {
	//Print some basic info
	out << "SketchEllipse(" << &ellipse << ")\n";
	//Print out axis, major radius and minor radius info
	out << "\tAxis: " << ellipse._majorAxis << std::endl;
	out << "\tMajor Radius: " << ellipse._semiMajor << std::endl;
	out << "\tMinor Radius: " << ellipse._semiMinor << std::endl;
	//Maybe print geometric curve info
	//...
	//Return
	return out;
}


/***********************************************~***************************************************/
