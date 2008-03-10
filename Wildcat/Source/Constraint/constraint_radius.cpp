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
#include "Constraint/constraint_radius.h"
#include "Sketcher/sketch.h"
#include "Sketcher/sketch_arc.h"
#include "Sketcher/sketch_circle.h"
#include "Kernel/document.h"
#include "Kernel/unit_types.h"
#include "PartDesign/part_plane.h"


/***********************************************~***************************************************/


void WCConstraintRadius::Initialize(const WPFloat &offset, const WPFloat &labelOffset) {
	//Set the measure label
	std::string label = "R " + this->_feature->Document()->LengthUnit()->DisplayString(this->_radius, 4);
	
	//Check to see if old measure needs to be deleted
	if (this->_measure != NULL) delete this->_measure;
	//Get the underlying curve
	WCGeometricCurve *curve;
	WCVector4 center;
	if (this->_isArc) {
		WCSketchArc *arc = dynamic_cast<WCSketchArc*>(this->_feature);
		curve = arc->Base();
		center = arc->Center();
	}
	else {
		WCSketchCircle *circle = dynamic_cast<WCSketchCircle*>(this->_feature);
		curve = circle->Base();
		center = circle->Center();
	}
	center = this->_sketch->ReferencePlane()->TransformMatrix() * center;
	//Create the rendering measure
	this->_measure = new WCConstraintMeasureRadius(this, label, curve, center,
		this->_sketch->ReferencePlane()->InverseTransformMatrix(),
		this->_sketch->ReferencePlane()->TransformMatrix(), offset, labelOffset);

	//Create the radius controller
	this->_controller = new WCConstraintRadiusController(this);
	//Make sure base is not null
	if ((this->_feature == NULL) || (this->_controller == NULL)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConstraintRadius::Initialize - NULL arc or controller.");
		//throw error
		return;
	}

	//Check feature name
	if (this->_name == "") this->_name = this->_sketch->GenerateFeatureName(this);
	this->_color = WCSketchFeature::ConstraintColor;
	//Retain the line
	this->_feature->Retain(*this);
	//Add into sketch
	if (!this->_sketch->AddConstraint(this)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConstraintRadius::Initialize - Problem adding constraint to sketch.");
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


WCConstraintRadius::WCConstraintRadius(WCSketch *sketch, const std::string &name, WCSketchArc *arc) : ::WCSketchConstraint(sketch, name),
	_feature(arc), _isArc(true), _radius(arc->Radius()), _measure(NULL) {
	//Initialize the measure
	this->Initialize(0.25*this->_radius, 0.5);
}


WCConstraintRadius::WCConstraintRadius(WCSketch *sketch, const std::string &name, WCSketchCircle *circle) : ::WCSketchConstraint(sketch, name),
	_feature(circle), _isArc(false), _radius(circle->Radius()), _measure(NULL) {
	//Initialize the measure
	this->Initialize(0.25*this->_radius, 0.5);
}


WCConstraintRadius::WCConstraintRadius(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCSketchConstraint( WCSerializeableObject::ElementFromName(element,"SketchConstraint"), dictionary ), 
	_feature(NULL), _isArc(false), _radius(0.0), _measure(NULL) {
	//Make sure element if not null
	if (element == NULL) return;
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");

	//Recreate the feature
	this->_feature = (WCSketchFeature*)WCSerializeableObject::GetGUIDAttrib(element, "sketchfeature", dictionary);
	//Retain the line
	this->_feature->Retain(*this);
	//Initialize the measure
	this->Initialize(0.25*this->_radius, 0.5);

	//Add into sketch
	if (!this->_sketch->AddConstraint(this)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConstraintRadius::WCConstraintRadius - Problem adding constraint to sketch.");
		//Should delete base
		//throw error
		return;
	}

	//Create the length controller
	this->_controller = new WCConstraintRadiusController(this);
	//Create tree element and add into the tree (beneath the sketch features element)
	WSTexture* constraintIcon = this->_document->Scene()->TextureManager()->TextureFromName("constraint32");
	this->_treeElement = new WCTreeElement(	this->_sketch->Document()->TreeView(), this->_name, this->_controller, constraintIcon);
	this->_sketch->ConstraintsTreeElement()->AddLastChild(this->_treeElement);
	//Inject constraints into sketch planner
	this->InjectConstraints(this->_sketch->ConstraintPlanner());
}


WCConstraintRadius::~WCConstraintRadius() {
	//Release the feature
	this->_feature->Release(*this);
	//Remove from the sketch
	if (!this->_sketch->RemoveConstraint(this)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConstraintRadius::~WCConstraintRadius - Problem removing constraint from sketch.");	
	}
	//Check to see if old measure needs to be deleted
	if (this->_measure != NULL) delete this->_measure;
}


void WCConstraintRadius::InjectConstraints(WCConstraintPlanner *planner) {
	//Get the list of constraint nodes from the line
	std::list<WCConstraintNode*> nodeList = this->_feature->ConstraintNodes();
	//Make sure there are four nodes
	if (nodeList.size() != 4) {
//		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConstraintRadius::InjectConstraints - Not four nodes in line's list.");
		return;
	}
	std::list<WCConstraintNode*>::iterator iter = nodeList.begin();
	//Get the center point (second node in list), starting point and ending point (third and fourth nodes in list)
	iter++;
	WCConstraintNode *center = *iter++;
	WCConstraintNode *p0Node = *iter++;
//	WCConstraintNode *p1Node = *iter;
	//Add a constraint edge to the planner
	planner->AddEdge(this, center, p0Node, WCConstraintEdgeType::AbsoluteDistance(), 1, this->_radius);
}


bool WCConstraintRadius::DependencyCheck(WCSketchFeature *feature) {
	return (feature == this->_feature);
}


void WCConstraintRadius::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
	//Mark as dirty
	this->_measure->MarkDirty();
}


bool WCConstraintRadius::Regenerate(void) {
	return false;
}


std::list<WCObject*> WCConstraintRadius::DeletionDependencies(void) {
	return std::list<WCObject*>();
}


void WCConstraintRadius::OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) {
	//Set the color
	this->_color.Set(WCSketchFeature::SelectedColor);
	this->_measure->Text()->Color(WCSketchFeature::SelectedColor);
	//Mark this as selected
	this->_isSelected = true;
}


void WCConstraintRadius::OnDeselection(const bool fromManager) {
	//Set the color
	this->_color.Set(WCSketchFeature::ConstraintColor);
	this->_measure->Text()->Color(WCSketchFeature::DefaultTextColor);
	//Mark this as not selected
	this->_isSelected = false; 
}


void WCConstraintRadius::Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom) {
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

	
xercesc::DOMElement* WCConstraintRadius::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	return NULL;
}


/***********************************************~***************************************************/


WCDrawingMode* WCConstraintRadius::ModeEdit(WCConstraintRadius *constraint) {
	//Create a new editing mode
	return new WCModeConstraintRadiusEdit(constraint);
}


WCActionConstraintRadiusCreate* WCConstraintRadius::ActionCreate(WCSketch *sketch, const std::string &constraintName, WCSketchArc *arc) {
	//Create new action to create constraint
	return new WCActionConstraintRadiusCreate(sketch, constraintName, arc);
}


WCActionConstraintRadiusCreate* WCConstraintRadius::ActionCreate(WCSketch *sketch, const std::string &constraintName, WCSketchCircle *circle) {
	//Create new action to create constraint
	return new WCActionConstraintRadiusCreate(sketch, constraintName, circle);
}


WCActionConstraintRadiusMove* WCConstraintRadius::ActionMove(WCConstraintRadius *constraint, const WPFloat &offset, const WPFloat &labelOffset) {
	//Create new action to move the constraint
	return new WCActionConstraintRadiusMove(constraint, offset, labelOffset);
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCConstraintRadius &constraint) {
	std::cout << "Constraint Radius(" << &constraint << ")\n";
	return out;
}


/***********************************************~***************************************************/
