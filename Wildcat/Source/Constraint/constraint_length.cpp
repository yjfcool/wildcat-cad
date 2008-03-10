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
#include "Constraint/constraint_length.h"
#include "Sketcher/sketch.h"
#include "Sketcher/sketch_line.h"
#include "Kernel/document.h"
#include "Kernel/unit_types.h"
#include "PartDesign/plane.h"


/***********************************************~***************************************************/


void WCConstraintLength::Initialize(const WPFloat &offset, const WPFloat &labelOffset) {
	//Set the measure label
	std::string label = this->_line->Document()->LengthUnit()->DisplayString(this->_length, 4);
	
	//Check to see if old measure needs to be deleted
	if (this->_measure != NULL) delete this->_measure;
	//Create the rendering measure
	WCVector4 p0( this->_line->Base()->Begin() );
	WCVector4 p1( this->_line->Base()->End() );
	this->_measure = new WCConstraintMeasureTwoPoint(this, label, WCMeasureType::Absolute(), p0, p1, 
		this->_sketch->ReferencePlane()->InverseTransformMatrix(),
		this->_sketch->ReferencePlane()->TransformMatrix(), offset, labelOffset);
}


/***********************************************~***************************************************/


WCConstraintLength::WCConstraintLength(WCSketch *sketch, const std::string &name, WCSketchLine *line) : ::WCSketchConstraint(sketch, name),
	_line(line), _length(0.0), _measure(NULL) {
	//Create the length controller
	this->_controller = new WCConstraintLengthController(this);
	//Make sure base is not null
	if ((this->_line == NULL) || (this->_controller == NULL)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConstraintLength::WCConstraintLength - NULL Line or controller.");
		//throw error
		return;
	}
	
	//Calcuate length of line
	WCVector4 p0( this->_line->Base()->Begin() );
	WCVector4 p1( this->_line->Base()->End() );
	this->_length = p0.Distance(p1);
	//Initialize the measure
	this->Initialize(0.25*this->_length, 0.5);

	//Check feature name
	if (this->_name == "") this->_name = this->_sketch->GenerateFeatureName(this);
	this->_color = WCSketchFeature::ConstraintColor;
	//Retain the line
	this->_line->Retain(*this);
	//Add into sketch
	if (!this->_sketch->AddConstraint(this)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConstraintLength::WCConstraintLength - Problem adding constraint to sketch.");
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


WCConstraintLength::WCConstraintLength(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCSketchConstraint( WCSerializeableObject::ElementFromName(element,"SketchConstraint"), dictionary ), 
	_line(NULL), _length(0.0), _measure(NULL) {
	//Make sure element if not null
	if (element == NULL) return;
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	//Recreate the line
	this->_line = (WCSketchLine*)WCSerializeableObject::GetGUIDAttrib(element, "sketchline", dictionary);
	//Retain the line
	this->_line->Retain(*this);

	//Initialize the measure
	this->Initialize(0.25*this->_length, 0.5);

	//Add into sketch
	if (!this->_sketch->AddConstraint(this)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConstraintLength::WCConstraintLength - Problem adding constraint to sketch.");
		//Should delete base
		//throw error
		return;
	}

	//Create the length controller
	this->_controller = new WCConstraintLengthController(this);
	//Create tree element and add into the tree (beneath the sketch features element)
	WSTexture* constraintIcon = this->_document->Scene()->TextureManager()->TextureFromName("constraint32");
	this->_treeElement = new WCTreeElement(	this->_sketch->Document()->TreeView(), this->_name, this->_controller, constraintIcon);
	this->_sketch->ConstraintsTreeElement()->AddLastChild(this->_treeElement);
	//Inject constraints into sketch planner
	this->InjectConstraints(this->_sketch->ConstraintPlanner());
}


WCConstraintLength::~WCConstraintLength() {
	//Release the line
	this->_line->Release(*this);
	//Remove from the sketch
	if (!this->_sketch->RemoveConstraint(this)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConstraintLength::~WCConstraintLength - Problem removing constraint from sketch.");	
	}
	//Check to see if old measure needs to be deleted
	if (this->_measure != NULL) delete this->_measure;
}


void WCConstraintLength::InjectConstraints(WCConstraintPlanner *planner) {
	//Get the list of constraint nodes from the line
	std::list<WCConstraintNode*> nodeList = this->_line->ConstraintNodes();
	//Make sure there are three nodes
	if (nodeList.size() != 3) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConstraintLength::InjectConstraints - Not three nodes in line's list.");
		return;
	}
	std::list<WCConstraintNode*>::iterator iter = nodeList.begin();
	//Get the starting point (second node in list) and ending point (third node in list) of the line
	iter++;
	WCConstraintNode *p0Node = *iter++;
	WCConstraintNode *p1Node = *iter;
	//Add a constraint edge to the planner
	planner->AddEdge(this, p0Node, p1Node, WCConstraintEdgeType::AbsoluteDistance(), 1, WCVector4(0.0));
}


void WCConstraintLength::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
	//Determine values for measure
	WCVector4 p0( this->_line->Base()->Begin() );
	WCVector4 p1( this->_line->Base()->End() );
	//Calcuate length of line
	this->_length = p0.Distance(p1);

	//Set the measure label
	std::string label = this->_line->Document()->LengthUnit()->DisplayString(this->_length, 4);
	this->_measure->Text()->Text(label);
	//Set the measure position
	this->_measure->Points(p0, p1);
}


bool WCConstraintLength::Regenerate(void) {
	return false;
}


std::list<WCObject*> WCConstraintLength::DeletionDependencies(void) {
	return std::list<WCObject*>();
}


void WCConstraintLength::OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) {
	//Set the color
	this->_color.Set(WCSketchFeature::SelectedColor);
	this->_measure->Text()->Color(WCSketchFeature::SelectedColor);
	//Mark this as selected
	this->_isSelected = true;
}


void WCConstraintLength::OnDeselection(const bool fromManager) {
	//Set the color
	this->_color.Set(WCSketchFeature::ConstraintColor);
	this->_measure->Text()->Color(WCSketchFeature::DefaultTextColor);
	//Mark this as not selected
	this->_isSelected = false; 
}


void WCConstraintLength::Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom) {
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

	
xercesc::DOMElement* WCConstraintLength::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	return NULL;
}


/***********************************************~***************************************************/


WCDrawingMode* WCConstraintLength::ModeEdit(WCConstraintLength *constraint) {
	//Create a new editing mode
	return new WCModeConstraintLengthEdit(constraint);
}


WCActionConstraintLengthCreate* WCConstraintLength::ActionCreate(WCSketch *sketch, const std::string &constraintName, WCSketchLine *line) {
	//Create new action to create constraint
	return new WCActionConstraintLengthCreate(sketch, constraintName, line);
}


WCActionConstraintLengthMove* WCConstraintLength::ActionMove(WCConstraintLength *constraint, const WPFloat &offset, const WPFloat &labelOffset) {
	//Create new action to move the constraint
	return new WCActionConstraintLengthMove(constraint, offset, labelOffset);
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCConstraintLength &constraint) {
	std::cout << "Constraint Length(" << &constraint << ")\n";
	return out;
}


/***********************************************~***************************************************/

