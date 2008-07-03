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
#include "Sketcher/sketch_axis.h"
#include "Sketcher/sketch.h"
#include "Kernel/document.h"
#include "Kernel/workbench_layer.h"
#include "PartDesign/part.h"
#include "PartDesign/part_plane.h"


/***********************************************~***************************************************/


void WCSketchAxis::Initialize(void) {
	//Create the point controller
	this->_controller = new WCSketchAxisController(this);
	//Make sure base is not null
	if ((this->_line == NULL) || (this->_controller == NULL)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchAxis::WCSketchAxis - Not able to create objects or controller.");
		//throw error
		return;
	}
	//Check feature name
	if (this->_name == "") this->_name = this->_sketch->GenerateFeatureName(this);
	//Add into sketch
	if (!this->_sketch->AddFeature(this)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchPoint::WCSketchPoint - Problem adding feature to sketch.");
		//Should delete base
		//throw error
		return;
	}

	//Add into sketch layer
	this->_sketch->Workbench()->Layer()->AddObject(this);
	//Add into the sketch selector
	this->_sketch->Workbench()->SelectionManager()->AddObject(this, this->_controller);

	//Add into part workbench layer
	WCPart *part = dynamic_cast<WCPart*>(this->_document);
	part->Workbench()->Layer()->AddObject(this);
	//Add into the part workbench selector
	part->Workbench()->SelectionManager()->AddObject(this, this->_controller);

	//Create tree element and add into the tree (beneath the sketch features element)
	WSTexture *axisIcon = this->_document->Scene()->TextureManager()->TextureFromName("axisLine32");
	this->_treeElement = new WCTreeElement(	this->_sketch->Document()->TreeView(), this->_name, this->_controller, axisIcon);
	this->_sketch->FeaturesTreeElement()->AddLastChild(this->_treeElement);
}
	

/***********************************************~***************************************************/


WCSketchAxis::WCSketchAxis(WCSketch *sketch, const std::string &name, const WCVector4 &begin, const WCVector4 &end) :
	::WCSketchFeature(sketch, name), _begin(begin), _end(end), _line(NULL) {
	//Validate sketch
	if (sketch == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchAxis::WCSketchAxis - NULL Reference passed.\n");
		//throw error
		return;
	}
	
	//Calcuate begin and end points
	WCVector4 bVec = this->_sketch->ReferencePlane()->TransformMatrix() * this->_begin;
	WCVector4 eVec = this->_sketch->ReferencePlane()->TransformMatrix() * this->_end;
	//Create line
	this->_line = new WCGeometricLine(bVec, eVec);
	//Set the base color and thickness
	this->_color = WCSketchFeature::DefaultColor;
	this->_line->Thickness(SKETCHAXIS_LINE_THICKNESS);
	//Perform remaining initialization
	this->Initialize();
}


WCSketchAxis::WCSketchAxis(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCSketchFeature( WCSerializeableObject::ElementFromName(element,"SketchFeature"), dictionary),
	_begin(), _end(), _line(NULL) {
	//Do something here
}


WCSketchAxis::~WCSketchAxis() {
	//Remove from the sketch
	if (!this->_sketch->RemoveFeature(this)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchAxis::WCSketchAxis - Problem removing feature from sketch.");	
	}
	//Remove from layer
	this->_sketch->Workbench()->Layer()->RemoveObject(this);
	//Remove from selection manager
	this->_sketch->Workbench()->SelectionManager()->RemoveObject(this);
	//Delete the base object
	if (this->_line->RefCount() == 0) delete this->_line;
}


void WCSketchAxis::Set(const WCVector4 &begin, const WCVector4 &end) {
	//Update the point
	this->_begin = begin;
	this->_end = end;
	//Calculate global x, y, z
	WCVector4 bVec = this->_sketch->ReferencePlane()->TransformMatrix() * this->_begin;
	WCVector4 eVec = this->_sketch->ReferencePlane()->TransformMatrix() * this->_end;
	//Set the base line
	this->_line->Set(bVec, eVec);
	//Make sure all dependent objects know about it
	this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);
}


void WCSketchAxis::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
	//Updates can be for position change, or color change
//	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchLine::ReceiveNotice - Invalid notice received.");
}


bool WCSketchAxis::Regenerate(void) {
	CLOGGER_WARN(WCLogManager::RootLogger(), "WCSketchAxis::Regenerate - Not yet implemented.");
	return false;
}


void WCSketchAxis::InjectConstraints(WCConstraintPlanner *planner) {
	//Not yet implemented
}


std::list<WCObject*> WCSketchAxis::DeletionDependencies(void) {
	//Only downward dependency is the base line
	return std::list<WCObject*>(1, this->_line);
}


void WCSketchAxis::OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) {
	//See if in sketcher workbench
	if (this->_sketch->Workbench()->IsActive()) {
		//Set the color of the base line
		this->_color = WCSketchFeature::SelectedColor;
	}
	//Must be in part designer
	else {
		//Set the color of the base line
		this->_color = WCPartFeature::SelectedColor;
	}
	//Mark this as selected
	this->_isSelected = true; 
}


void WCSketchAxis::OnDeselection(const bool fromManager) {
	//Set the color of the base line
	this->_color = WCSketchFeature::DefaultColor;
	//Mark this as not selected
	this->_isSelected = false; 
}


xercesc::DOMElement* WCSketchAxis::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict) {
	CLOGGER_WARN(WCLogManager::RootLogger(), "WCSketchAxis::Serialize - Not yet implemented.");
	return NULL;
}


void WCSketchAxis::Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom) {
	//Force no depth test if in sketch workbench
	if (this->_sketch->Workbench()->IsActive()) glDisable(GL_DEPTH_TEST);
	//Turn on line stippling
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(SKETCHAXIS_LINESTIPPLE_FACTOR, SKETCHAXIS_LINESTIPPLE_PATTERN);
	if (color == WCColor::Default()) {
		//Draw line	
		if (this->_line != NULL) this->_line->Render(0, this->_color, zoom);
	} else {
		//Draw line
		if (this->_line != NULL) this->_line->Render(0, color, zoom);
	}
	//Clean up
	glDisable(GL_LINE_STIPPLE);
	//Restore depth test if in sketch workbench
	if (this->_sketch->Workbench()->IsActive()) glEnable(GL_DEPTH_TEST);
}

	
/***********************************************~***************************************************/


WCDrawingMode* WCSketchAxis::ModeCreate(WCSketchWorkbench *wb) {
	//Create a new drawing mode
	return new WCModeSketchAxisCreate(wb);
}


WCDrawingMode* WCSketchAxis::ModeEdit(WCSketchAxis *axis) {
	//Create a new editing mode
	return new WCModeSketchAxisEdit(axis);
}


WCActionSketchAxisCreate* WCSketchAxis::ActionCreate(WCSketch *sketch, const std::string &axisName, 
	const WCVector4 &begin, const WCVector4 &end) {
	//Create new action to create axis
	return new WCActionSketchAxisCreate(sketch, axisName, begin, end);
}


WCActionSketchAxisMove* WCSketchAxis::ActionMove(WCSketchAxis *axis, const WCVector4 &begin, const WCVector4 &end) {
	//Create new action to move the axis
	return new WCActionSketchAxisMove(axis, begin, end);
}


/***********************************************~***************************************************/

	
std::ostream& operator<<(std::ostream& out, const WCSketchAxis &axis) {
	out << "Sketch Axis\n";
	return out;
}


/***********************************************~***************************************************/

