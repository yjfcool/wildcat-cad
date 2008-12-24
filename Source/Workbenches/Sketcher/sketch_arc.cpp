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
#include <Sketcher/sketch_arc.h>
#include <Sketcher/sketch.h>
#include <Kernel/document.h>
#include <PartDesign/part_plane.h>


/***********************************************~***************************************************/


void WCSketchArc::GenerateBase(void) {
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
		WCNurbs::CircularPoints(pos, xUnit, yUnit, this->_radius, this->_startAngle, this->_endAngle, controlPoints, knotPoints);
		//Update the curve
		this->_curve->ControlPoints(controlPoints);
		this->_curve->KnotPoints(knotPoints);
		//Update the point
		this->_centerPoint->Set(pos);
	}
	//Setup new curve and point
	else {
		//Create the curve
		this->_curve = WCNurbsCurve::CircularArc(this->_document->Scene()->GeometryContext(), pos, xUnit, yUnit, 
			this->_radius, this->_startAngle, this->_endAngle);
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


void WCSketchArc::Initialize(void) {
	//Create the point controller
	this->_controller = new WCSketchArcController(this);
	//Check feature name
	if (this->_name == "") this->_name = this->_sketch->GenerateFeatureName(this);
	//Add feature into sketch
	if ((!this->_sketch->AddFeature(this)) || (this->_controller == NULL)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchArc::WCSketchArc - Problem adding feature to sketch or creating controller.");
		//Should delete base
		//throw error
		return;
	}
	//Retain the arc and point
	this->_sketch->AddCurve(this->_curve, this->_controller);
	this->_curve->Retain(*this);
	this->_sketch->AddPoint(this->_centerPoint, this->_controller);
	this->_centerPoint->Retain(*this);

	//Get icon texture (depends on arc type)
	WSTexture *arcIcon;
	if (this->_type == WCSketchArcType::ThreePoint())
		arcIcon = this->_document->Scene()->TextureManager()->TextureFromName("threePointArc32");
	else
		arcIcon = this->_document->Scene()->TextureManager()->TextureFromName("twoPointArc32");
	//Create tree element and add into the tree (beneath the sketch features element)
	this->_treeElement = new WCTreeElement(	this->_sketch->Document()->TreeView(), this->_name, this->_controller, arcIcon);
	this->_sketch->FeaturesTreeElement()->AddLastChild(this->_treeElement);
}



/***********************************************~***************************************************/


WCSketchArc::WCSketchArc(WCSketch *sketch, const std::string &name, const WCVector4 &center,
	const WPFloat &radius, const WPFloat &startAngle, const WPFloat &endAngle) : ::WCSketchFeature(sketch), 
	_type(WCSketchArcType::Standard()), _center(center), _radius(radius),
	_startAngle(startAngle), _endAngle(endAngle), _curve(NULL), _centerPoint(NULL) {
	//Validate sketch and center
	if (this->_sketch == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchArc::WCSketchArc - NULL Sketch passed.\n");
		//throw error
		return;
	}
	//Generate arc and center
	this->GenerateBase();
	//Initialize arc info
	this->Initialize();
}


WCSketchArc::WCSketchArc(WCSketch *sketch, const std::string &name, const WCVector4 &center,
	const WPFloat &radius, const WPFloat &startAngle, const WPFloat &endAngle, const WCVector4 &threePoint) : 
	::WCSketchFeature(sketch), _type(WCSketchArcType::ThreePoint()), _center(center), _radius(radius),
	_startAngle(startAngle), _endAngle(endAngle), _curve(NULL), _centerPoint(NULL) {
	//Validate sketch and center
	if (this->_sketch == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchArc::WCSketchArc - NULL Sketch passed.\n");
		//throw error
		return;
	}
	//Generate arc and center
	this->GenerateBase();
	//Initialize arc info
	this->Initialize();
}


WCSketchArc::WCSketchArc(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	WCSketchFeature(  WCSerializeableObject::ElementFromName(element,"SketchFeature"), dictionary ),
	_type(WCSketchArcType::Standard()), _center(), _radius(0.0), _startAngle(0.0), _endAngle(0.0), _curve(NULL), _centerPoint(NULL) {
	//Make sure element if not null
	if (element == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchArc::WCSketchArc - NULL Element passed.");
		//throw error
		return;
	}
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

	//Restore the type
	this->_type.FromElement(element, "ArcType");
	//Setup radius and start/end angles
	this->_radius = WCSerializeableObject::GetBoolAttrib(element, "radius");
	this->_startAngle = WCSerializeableObject::GetBoolAttrib(element, "startAngle");
	this->_endAngle = WCSerializeableObject::GetBoolAttrib(element, "endAngle");
	//Setup center
	this->_center.FromElement( WCSerializeableObject::ElementFromName(element,"Center") );
	//Setup base
	this->_curve = new WCNurbsCurve( WCSerializeableObject::ElementFromName(element,"NURBSCurve"), dictionary );
	//Setup center point
	this->_centerPoint = new WCGeometricPoint( WCSerializeableObject::ElementFromName(element,"GeometricPoint"), dictionary );
	//Initialize the object
	this->Initialize();
}


WCSketchArc::~WCSketchArc() {
	//Remove from the sketch
	if (!this->_sketch->RemoveFeature(this)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchArc::WCSketchArc - Problem removing feature from sketch.");	
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


void WCSketchArc::Center(const WCVector4 &center) {
	//Set the center
	this->_center = center;
	//Generate circle and center
	this->GenerateBase();
}


void WCSketchArc::Radius(const WPFloat &radius) {
	//Set the radius
	this->_radius = radius;
	//Generate circle and center
	this->GenerateBase();
}


void WCSketchArc::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
//	CLOGGER_WARN(WCLogManager::RootLogger(), "WCSketchArc::ReceiveNotice - Not yet implemented.");
}


bool WCSketchArc::Regenerate(void) {
	CLOGGER_WARN(WCLogManager::RootLogger(), "WCSketchArc::Regenerate - Not yet implemented.");
	return false;
}


void WCSketchArc::InjectConstraints(WCConstraintPlanner *planner) {
	CLOGGER_WARN(WCLogManager::RootLogger(), "WCSketchArc::InjectConstraints - Not yet implemented.");
}


std::list<WCObject*> WCSketchArc::DeletionDependencies(void) {
	//Only downward dependency is the base curve
	return std::list<WCObject*>(1, this->_curve);
}


void WCSketchArc::IsConstruction(const bool state) {
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


void WCSketchArc::OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) {
	//Set the color of the base curve, and center point
	this->_curve->Color(WCSketchFeature::SelectedColor);
	this->_centerPoint->Color(WCSketchFeature::SelectedColor);
	//Mark this as selected
	this->_isSelected = true; 
}


void WCSketchArc::OnDeselection(const bool fromManager) {
	//Set the color of the base arc - depends on construction
	if (this->_isConstruction) 
		this->_curve->Color(WCSketchFeature::ConstructionColor);
	else 
		this->_curve->Color(WCSketchFeature::DefaultColor);

	//Set the color of the center point
	if (this->_centerPoint->IsConstruction()) 
		this->_centerPoint->Color(WCSketchFeature::ConstructionColor);
	else 
		this->_centerPoint->Color(WCSketchFeature::DefaultColor);
	//Mark this as not selected
	this->_isSelected = false; 
}

	
xercesc::DOMElement* WCSketchArc::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create primary element for this object
	XMLCh* xmlString = xercesc::XMLString::transcode("SketchArc");
	xercesc::DOMElement* element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Include the parent element
	xercesc::DOMElement* featureElement = this->WCSketchFeature::Serialize(document, dictionary);
	element->appendChild(featureElement);
	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);

	//Add type
	this->_type.ToElement(element, "ArcType");
	//Add attributes
	WCSerializeableObject::AddFloatAttrib(element, "radius", this->_radius);
	WCSerializeableObject::AddFloatAttrib(element, "startAngle", this->_startAngle);
	WCSerializeableObject::AddFloatAttrib(element, "endAngle", this->_endAngle);
	//Add center vector
	this->_center.ToElement(element, "Center");
	//Add child geometric curve
	xercesc::DOMElement *curveElement = this->_curve->Serialize(document, dictionary);
	element->appendChild(curveElement);
	xercesc::DOMElement *pointElement = this->_centerPoint->Serialize(document, dictionary);
	element->appendChild(pointElement);
	
	//Return the new element
	return element;
}

	
/***********************************************~***************************************************/


WCDrawingMode* WCSketchArc::ModeCreate(WCSketchWorkbench *wb, const WCSketchArcType &type) {
	//Create drawing mode based on type
	if (type == WCSketchArcType::Standard())	return new WCModeSketchArcStandardCreate(wb);
	if (type == WCSketchArcType::ThreePoint())	return new WCModeSketchArcThreePointCreate(wb);
	//Unrecognized type
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchArc::ModeCreate - Unknown arc type passed.");
	//throw error
	return NULL;
}


WCDrawingMode* WCSketchArc::ModeEdit(WCSketchArc *arc) {
	//Create drawing mode based on type
	if (arc->Type() == WCSketchArcType::Standard())		return new WCModeSketchArcEdit(arc);
	if (arc->Type() == WCSketchArcType::ThreePoint())	return new WCModeSketchArcEdit(arc);
	//Unrecognized type
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchArc::ModeEdit - Unknown arc type passed.");
	//throw error
	return NULL;
}


WCActionSketchArcCreate* WCSketchArc::ActionCreate(WCSketch *sketch, const std::string &arcName, const WCVector4 &center, 
	const WPFloat &radius, const WPFloat &startAngle, const WPFloat &endAngle) {
	//Create action to create standard sketch arc
	return new WCActionSketchArcCreate(sketch, arcName, center, radius, startAngle, endAngle);
}


WCActionSketchArcCreate* WCSketchArc::ActionCreate(WCSketch *sketch, const std::string &arcName, const WCVector4 &center, 
	const WPFloat &radius, const WPFloat &startAngle, const WPFloat &endAngle, const WCVector4 &threePoint) {
	//Create action to create three-point sketch arc
	return new WCActionSketchArcCreate(sketch, arcName, center, radius, startAngle, endAngle, threePoint);
}


WCActionSketchArcMove* WCSketchArc::ActionMove(WCSketchArc *arc, const WCVector4 &centerPos) {
	//Create new action to move arc
	return new WCActionSketchArcMove(arc, centerPos);
}


WCActionSketchArcRadius* WCSketchArc::ActionRadius(WCSketchArc *arc, const WPFloat &radius) {
	//Create new action to change radius of arc
	return new WCActionSketchArcRadius(arc, radius);
}


/***********************************************~***************************************************/


WPFloat WCSketchArc::PointsToDegrees(const WCVector4 rotAxis, const WCVector4 point) {
//	std::cout << "RotAxis: " << rotAxis << std::endl;
//	std::cout << "Point: " << point << std::endl;
	WCVector4 dir = point - rotAxis;
	WPFloat angle = atan(dir.I() / dir.J()) * 180.0 / M_PI;
	
	//Switch by quadrant - Upperquadrants
	if (dir.J() >= 0.0) angle = 90.0 - angle;
	//Lower quadrants
	else angle = 270.0 - angle;	
//	std::cout << "Angle: " << angle << std::endl;;
	return angle;
}


WPFloat WCSketchArc::CalculateCenterAndRadius(const WCVector4 &p1, const WCVector4 &p2, const WCVector4 &p3, WCVector4 &center) {
	WCVector4 p1p2 = p1 - p2;
	WCVector4 p2p3 = p2 - p3;
	WPFloat denomBase = (p1p2.CrossProduct(p2p3)).Magnitude();
	//Check on denomBase
	if (denomBase < 0.001) {
//		CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCSketchArc::CalculateCenterAndRadius - Denominator is zero.");
		return 0.0;
	}

	WPFloat radius = 0.0;
	WCVector4 p1p3 = p1 - p3;
	WCVector4 p2p1 = p2 - p1;
	WCVector4 p3p1 = p3 - p1;
	WCVector4 p3p2 = p3 - p2;
	WPFloat p1p2Mag = p1p2.Magnitude();
	WPFloat p1p3Mag = p1p3.Magnitude();
	WPFloat p2p3Mag = p2p3.Magnitude();
	//Calculate radius
	radius = (p1p2Mag * p2p3Mag * p1p3Mag) / (2.0 * denomBase);
	//Calculate arc center coefficients
	denomBase *= 2.0 * denomBase;
	WPFloat tmp = p1p2.DotProduct(p1p3);
	WPFloat a = (p2p3Mag * p2p3Mag * tmp) / denomBase;
	tmp = p2p1.DotProduct(p2p3);
	WPFloat b = (p1p3Mag * p1p3Mag * tmp) / denomBase;
	tmp = p3p1.DotProduct(p3p2);
	WPFloat c = (p1p2Mag * p1p2Mag * tmp) / denomBase;
	//Calculate center point
	center = (p1 * a) + (p2 * b) + (p3 *c);
	return radius;
}


/***********************************************~***************************************************/

	
std::ostream& operator<<(std::ostream& out, const WCSketchArc &arc) {
	return out;
}


/***********************************************~***************************************************/

