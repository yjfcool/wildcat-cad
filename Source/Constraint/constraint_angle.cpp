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
#include <Constraint/constraint_angle.h>
#include <Sketcher/sketch.h>
#include <Sketcher/sketch_line.h>
#include <Kernel/document.h>
#include <Kernel/unit_types.h>
#include <PartDesign/part_plane.h>


/***********************************************~***************************************************/


void WCConstraintAngle::GenerateMeasure(const bool &genOffsets) {
	//Determine point of intersection (center) between lines
	WCVector4 center;
	WPFloat dummyA, dummyB;
	WPUInt retVal = RayRayIntersection(this->_lineA->Begin(), this->_lineA->End() - this->_lineA->Begin(),
									   this->_lineB->Begin(), this->_lineB->End() - this->_lineB->Begin(), 
									   dummyA, dummyB, center);
	//Make sure the lines intersect
	if (retVal != INTERSECTION_INTERSECTION) {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCConstraintAngle::GenerateMeasure - Non-itersecting lines selected");
		return;
	}
	center = this->_sketch->ReferencePlane()->TransformMatrix() * center;

	//Determine first and second point
	WCVector4 firstPt = this->_lineA->Base()->Evaluate(0.5);
	WCVector4 secondPt = this->_lineB->Base()->Evaluate(0.5);

	WPFloat offset, labelOffset;
	//Don't generate the offsets
	if (!genOffsets) {
		offset = this->_measure->Offset();
		labelOffset = this->_measure->LabelOffset();
	}
	//Generate offsets
	else {
		//Get distance from middle of lineA to center as offset
		offset = center.Distance(this->_lineA->Base()->Evaluate(0.5));
		labelOffset = 0.5;
	}

	//Determine angle between lines
	WCVector4 p2p1 = firstPt - center;
	WCVector4 p0p1 = secondPt - center;
	p2p1.L(0.0);
	p0p1.L(0.0);
	WPFloat num = p2p1.DotProduct(p0p1);
	WPFloat den = p2p1.Magnitude() * p0p1.Magnitude();
	this->_angle = acos( num / den );

	//Check to see if old measure needs to be deleted
	if (this->_measure != NULL) delete this->_measure;
	//Set the measure label
	std::string label = this->_lineA->Document()->AngleUnit()->DisplayString(this->_angle, 4);
	this->_measure = new WCConstraintMeasureAngle(this, label, center, firstPt, secondPt, true,
												  this->_sketch->ReferencePlane()->InverseTransformMatrix(),
												  this->_sketch->ReferencePlane()->TransformMatrix(),
												  offset, labelOffset);
}


void WCConstraintAngle::Initialize(void) {
	//Generate the measure
	this->GenerateMeasure(true);

	//Create the angle controller
	this->_controller = new WCConstraintAngleController(this);
	//Make sure base is not null
	if ((this->_lineA == NULL) || (this->_lineB == NULL) || (this->_controller == NULL)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConstraintAngle::Initialize - NULL arc or controller.");
		//throw error
		return;
	}

	//Check feature name
	if (this->_name == "") this->_name = this->_sketch->GenerateFeatureName(this);
	this->_color = WCSketchFeature::ConstraintColor;
	//Retain the lines
	this->_lineA->Retain(*this);
	this->_lineB->Retain(*this);
	//Add into sketch
	if (!this->_sketch->AddConstraint(this)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConstraintAngle::WCConstraintAngle - Problem adding constraint to sketch.");
		//Should delete base
		//throw error
		return;
	}

	//Create tree element and add into the tree (beneath the sketch features element)
	WSTexture* constraintIcon = this->_document->Scene()->TextureManager()->TextureFromName("constraint32");
	this->_treeElement = new WCTreeElement(	this->_sketch->Document()->TreeView(), this->_name, this->_controller, constraintIcon);
	this->_sketch->ConstraintsTreeElement()->AddLastChild(this->_treeElement);
	//Inject constraints into sketch planner
	this->InjectConstraints(this->_sketch->ConstraintPlanner());
}


/***********************************************~***************************************************/


WCConstraintAngle::WCConstraintAngle(WCSketch *sketch, const std::string &name, WCSketchLine *lineA, WCSketchLine *lineB) : 
	::WCSketchConstraint(sketch, name), _lineA(lineA), _lineB(lineB), _angle(0.0), _measure(NULL) {
	//Initialize the measure
	this->Initialize();
}


WCConstraintAngle::WCConstraintAngle(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCSketchConstraint( WCSerializeableObject::ElementFromName(element,"SketchConstraint"), dictionary ), 
	_lineA(NULL), _lineB(NULL), _angle(0.0), _measure(NULL) {
	//Make sure element if not null
	if (element == NULL) return;
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

	//Setup lineA
	this->_lineA = (WCSketchLine*)WCSerializeableObject::GetGUIDAttrib(element, "lineA", dictionary);
	//Setup lineB
	this->_lineB = (WCSketchLine*)WCSerializeableObject::GetGUIDAttrib(element, "lineB", dictionary);
	//Setup angle
	this->_angle = WCSerializeableObject::GetFloatAttrib(element, "angle");

	//Initialize the measure
	this->Initialize();
}


WCConstraintAngle::~WCConstraintAngle() {
	//Release the features
	this->_lineA->Release(*this);
	this->_lineB->Release(*this);
	//Remove from the sketch
	if (!this->_sketch->RemoveConstraint(this)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConstraintAngle::~WCConstraintAngle - Problem removing constraint from sketch.");	
	}
	//Check to see if old measure needs to be deleted
	if (this->_measure != NULL) delete this->_measure;
}


void WCConstraintAngle::InjectConstraints(WCConstraintPlanner *planner) {
	//Not yet implemented
}


bool WCConstraintAngle::DependencyCheck(WCSketchFeature *feature) {
	//If feature matches either lineA or lineB return true, false otherwise
	return (feature == this->_lineA) || (feature == this->_lineB);
}


void WCConstraintAngle::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
	//Update measure
	this->GenerateMeasure(false);
}


bool WCConstraintAngle::Regenerate(void) {
	//Not yet implemented
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConstraintAngle::Regenerate - Not yet implemented.");
	return false;
}


std::list<WCObject*> WCConstraintAngle::DeletionDependencies(void) {
	//No deletion dependencies
	return std::list<WCObject*>();
}


void WCConstraintAngle::OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) {
	//Set the color
	this->_color.Set(WCSketchFeature::SelectedColor);
	this->_measure->Text()->Color(WCSketchFeature::SelectedColor);
	//Mark this as selected
	this->_isSelected = true;
}


void WCConstraintAngle::OnDeselection(const bool fromManager) {
	//Set the color
	this->_color.Set(WCSketchFeature::ConstraintColor);
	this->_measure->Text()->Color(WCSketchFeature::DefaultTextColor);
	//Mark this as not selected
	this->_isSelected = false; 
}


void WCConstraintAngle::Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom) {
	//Make sure is visible
	if (!this->_isVisible) return;
	//Check the color (see if in selection mode)
	if (color == WCColor::Default()) {
		//Just call measure to render
		this->_measure->Render(this->_color);
	}
	//Not in selection mode
	else {
		//Call measure to render for selection
		this->_measure->Render(color, true);
	}
}

	
xercesc::DOMElement* WCConstraintAngle::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create primary element for this object
	XMLCh* xmlString = xercesc::XMLString::transcode("ConstraintAngle");
	xercesc::DOMElement* element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Include the parent element
	xercesc::DOMElement* featureElement = this->WCSketchConstraint::Serialize(document, dictionary);
	element->appendChild(featureElement);
	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);

	//Add line A attribute
	WCSerializeableObject::AddGUIDAttrib(element, "lineA", this->_lineA, dictionary);
	//Add line B attribute
	WCSerializeableObject::AddGUIDAttrib(element, "lineB", this->_lineB, dictionary);
	//Add angle attribute
	WCSerializeableObject::AddFloatAttrib(element, "angle", this->_angle);
	
	//Return the new element
	return element;
}


/***********************************************~***************************************************/


WCDrawingMode* WCConstraintAngle::ModeEdit(WCConstraintAngle *constraint) {
	//Create a new editing mode
	return new WCModeConstraintAngleEdit(constraint);
}


WCActionConstraintAngleCreate* WCConstraintAngle::ActionCreate(WCSketch *sketch, const std::string &constraintName,
	WCSketchLine *lineA, WCSketchLine *lineB) {
	//Create new action to create constraint
	return new WCActionConstraintAngleCreate(sketch, constraintName, lineA, lineB);
}


WCActionConstraintAngleMove* WCConstraintAngle::ActionMove(WCConstraintAngle *constraint, const WPFloat &offset, const WPFloat &labelOffset) {
	//Create new action to move the constraint
	return new WCActionConstraintAngleMove(constraint, offset, labelOffset);
}


/***********************************************~***************************************************/


std::ostream& __WILDCAT_NAMESPACE__::operator<<(std::ostream& out, const WCConstraintAngle &constraint) {
	std::cout << "Constraint Angle(" << &constraint << "): " << constraint._angle << std::endl;
	return out;
}


/***********************************************~***************************************************/
