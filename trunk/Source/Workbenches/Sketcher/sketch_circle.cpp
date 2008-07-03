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
#include "Sketcher/sketch_circle.h"
#include "Sketcher/sketch.h"
#include "Kernel/document.h"
#include "PartDesign/part_plane.h"


/***********************************************~***************************************************/


void WCSketchCircle::GenerateBase(void) {
	//Setup vectors
	WCVector4 pos = this->_sketch->ReferencePlane()->TransformMatrix() * this->_center;
	WCVector4 xUnit(1.0, 0.0, 0.0, 0.0);
	WCVector4 yUnit(0.0, 1.0, 0.0, 0.0);
	xUnit = this->_sketch->ReferencePlane()->TransformMatrix() * xUnit;
	yUnit = this->_sketch->ReferencePlane()->TransformMatrix() * yUnit;
	xUnit.Normalize(true);
	yUnit.Normalize(true);

	//Are the curve and center point already around
	if (this->_curve != NULL) {
		std::vector<WCVector4> controlPoints;
		std::vector<WPFloat> knotPoints;
		//Get updated control points and knot points vectors
		WCNurbs::CircularPoints(pos, xUnit, yUnit, this->_radius, 0.0, 360.0, controlPoints, knotPoints);
		//Update the curve
		this->_curve->ControlPoints(controlPoints);
		this->_curve->KnotPoints(knotPoints);
		//Update the point
		this->_centerPoint->Set(pos);
	}
	//Setup new curve and point
	else {
		//Create the curve
		this->_curve = WCNurbsCurve::CircularArc(this->_document->Scene()->GeometryContext(), pos, xUnit, yUnit, this->_radius, 0.0, 360.0);
		//Create the center point
		this->_centerPoint = new WCGeometricPoint(pos);

		//Set properties for curve and point
		this->_curve->Color(WCSketchFeature::DefaultColor);
		this->_curve->Thickness(WCSketchFeature::LineThickness);
		this->_centerPoint->Color(WCSketchFeature::ConstructionColor);
		this->_centerPoint->Size(WCSketchFeature::PointSize);
		this->_centerPoint->IsConstruction(true);
	}
}


void WCSketchCircle::Initialize(void) {
	//Create the point controller
	this->_controller = new WCSketchCircleController(this);
	//Check feature name
	if (this->_name == "") this->_name = this->_sketch->GenerateFeatureName(this);	
	//Add feature into sketch
	if ((!this->_sketch->AddFeature(this)) || (this->_controller == NULL)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchCircle::WCSketchCircle - Problem adding feature to sketch or creating controller.");
		//Should delete base
		//throw error
		return;
	}
	//Retain the new circle and point
	this->_sketch->AddCurve(this->_curve, this->_controller);
	this->_curve->Retain(*this);
	this->_sketch->AddPoint(this->_centerPoint, this->_controller);
	this->_centerPoint->Retain(*this);

	//Create tree element and add into the tree (beneath the sketch features element)
	WSTexture *circleIcon = this->_document->Scene()->TextureManager()->TextureFromName("centerCircle32");
	this->_treeElement = new WCTreeElement(	this->_sketch->Document()->TreeView(), this->_name, this->_controller, circleIcon);
	this->_sketch->FeaturesTreeElement()->AddLastChild(this->_treeElement);
}


/***********************************************~***************************************************/


WCSketchCircle::WCSketchCircle(WCSketch *sketch, const std::string &name, const WCVector4 &center, const WPFloat &radius) : 
	::WCSketchFeature(sketch), _center(center), _radius(radius), _curve(NULL), _centerPoint(NULL) {
	//Validate sketch
	if (this->_sketch == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchCircle::WCSketchCircle - NULL Sketch passed.\n");
		//throw error
		return;
	}
	//Generate circle and center
	this->GenerateBase();
	//Initialize
	this->Initialize();
}


WCSketchCircle::WCSketchCircle(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCSketchFeature( WCSerializeableObject::ElementFromName(element,"SketchFeature"), dictionary),
	_center(), _radius(0.0), _curve(NULL), _centerPoint(NULL) {
	//Make sure element if not null
	if (element == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchCircle::WCSketchCircle - NULL Element passed.");
		//throw error
		return;
	}
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

	//Setup center
	this->_center.FromElement( WCSerializeableObject::ElementFromName(element,"Center") );
	//Setup radius
	this->_radius = WCSerializeableObject::GetBoolAttrib(element, "radius");
	//Setup base
	this->_curve = new WCNurbsCurve( WCSerializeableObject::ElementFromName(element,"NURBSCurve"), dictionary );
	//Setup center point
	this->_centerPoint = new WCGeometricPoint( WCSerializeableObject::ElementFromName(element,"GeometricPoint"), dictionary );
	//Initialize the object
	this->Initialize();
}


WCSketchCircle::~WCSketchCircle() {
	//Remove from the sketch
	if (!this->_sketch->RemoveFeature(this)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchCircle::~WCSketchCircle - Problem removing feature from sketch.");	
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
}


void WCSketchCircle::Center(const WCVector4 &center) {
	//Set the center
	this->_center = center;
	//Generate circle and center
	this->GenerateBase();
}


void WCSketchCircle::Radius(const WPFloat &radius) {
	//Set the radius
	this->_radius = radius;
	//Generate circle and center
	this->GenerateBase();
}


void WCSketchCircle::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
//	CLOGGER_WARN(WCLogManager::RootLogger(), "WCSketchCircle::ReceiveNotice - Not yet implemented.");
}


bool WCSketchCircle::Regenerate(void) {
	CLOGGER_WARN(WCLogManager::RootLogger(), "WCSketchCircle::Regenerate - Not yet implemented.");
	return false;
}


void WCSketchCircle::InjectConstraints(WCConstraintPlanner *planner) {
	CLOGGER_WARN(WCLogManager::RootLogger(), "WCSketchCircle::InjectConstraints - Not yet implemented.");
}


std::list<WCObject*> WCSketchCircle::DeletionDependencies(void) {
	//Only downward dependency is the base curve
	return std::list<WCObject*>(1, this->_curve);
}


void WCSketchCircle::IsConstruction(const bool state) {
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


void WCSketchCircle::OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) {
	//Set the color of the base curve
	this->_curve->Color(WCSketchFeature::SelectedColor);
	//Set color and show center point
	this->_centerPoint->Color(WCSketchFeature::SelectedColor);
	//Mark this as selected
	this->_isSelected = true; 
}


void WCSketchCircle::OnDeselection(const bool fromManager) {
	//Set the color of the base curve - depends on construction
	if (this->_isConstruction) this->_curve->Color(WCSketchFeature::ConstructionColor);
	else this->_curve->Color(WCSketchFeature::DefaultColor);
	//Set color and show center point
	if (this->_centerPoint->IsConstruction()) this->_centerPoint->Color(WCSketchFeature::ConstructionColor);
	else this->_centerPoint->Color(WCSketchFeature::DefaultColor);
	//Mark this as not selected
	this->_isSelected = false; 
}

	
xercesc::DOMElement* WCSketchCircle::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create primary element for this object
	XMLCh* xmlString = xercesc::XMLString::transcode("SketchCircle");
	xercesc::DOMElement* element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Include the parent element
	xercesc::DOMElement* featureElement = this->WCSketchFeature::Serialize(document, dictionary);
	element->appendChild(featureElement);
	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);

	//Add radius attribute
	WCSerializeableObject::AddFloatAttrib(element, "radius", this->_radius);
	//Add center vector
	this->_center.ToElement(element, "Center");
	xercesc::XMLString::release(&xmlString);
	//Add child geometric curve
	xercesc::DOMElement *curveElement = this->_curve->Serialize(document, dictionary);
	element->appendChild(curveElement);
	xercesc::DOMElement *pointElement = this->_centerPoint->Serialize(document, dictionary);
	element->appendChild(pointElement);

	//Return the new element
	return element;
}

	
/***********************************************~***************************************************/


WCDrawingMode* WCSketchCircle::DrawingMode(WCSketchWorkbench *wb) {
	//Create a new drawing mode
	return new WCModeSketchCircleCreate(wb);
}


WCDrawingMode* WCSketchCircle::ModeEdit(WCSketchCircle *circle) {
	//Create a new editing mode
	return new WCModeSketchCircleEdit(circle);
}


WCActionSketchCircleCreate* WCSketchCircle::ActionCreate(WCSketch *sketch, const std::string &circleName, 
	const WCVector4 &center, const WPFloat &radius) {
	//Create new action to create circle
	WCActionSketchCircleCreate *action = new WCActionSketchCircleCreate(sketch, circleName, center, radius);
	return action;
}


WCActionSketchCircleMove* WCSketchCircle::ActionMove(WCSketchCircle *circle, const WCVector4 &centerPos) {
	//Create new action to move circle
	WCActionSketchCircleMove *action = new WCActionSketchCircleMove(circle, centerPos);
	return action;
}


WCActionSketchCircleRadius* WCSketchCircle::ActionRadius(WCSketchCircle *circle, const WPFloat &radius) {
	//Create new action to change radius circle
	WCActionSketchCircleRadius *action = new WCActionSketchCircleRadius(circle, radius);
	return action;
}


/***********************************************~***************************************************/

	
std::ostream& operator<<(std::ostream& out, const WCSketchCircle &circle) {
	return out;
}


/***********************************************~***************************************************/

