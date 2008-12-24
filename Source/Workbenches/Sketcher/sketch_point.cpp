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
#include <Sketcher/sketch_point.h>
#include <Sketcher/sketch.h>
#include <Kernel/document.h>
#include <PartDesign/part_plane.h>


/***********************************************~***************************************************/


void WCSketchPoint::Initialize(void) {
	//Retain the base
	this->_base->Retain(*this);
	//Create the point controller
	this->_controller = new WCSketchPointController(this);
	//Set the base color and size
	this->_base->Color(WCSketchFeature::ConstructionColor);
	this->_base->Size(WCSketchFeature::PointSize);
	//Add into sketch
	if ((!this->_sketch->AddFeature(this)) || (!this->_sketch->AddPoint(this->_base, this->_controller))) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchPoint::Initialize - Problem adding feature or point to sketch.");
		//Should delete base
	}

	//Create tree element and add into the tree (beneath the sketch features element)
	WSTexture *pointIcon = this->_document->Scene()->TextureManager()->TextureFromName("point32");
	this->_treeElement = new WCTreeElement(this->_sketch->Document()->TreeView(), this->_name, this->_controller, pointIcon);
	this->_sketch->FeaturesTreeElement()->AddLastChild(this->_treeElement);
	//Inject constraints into sketch planner
	this->InjectConstraints(this->_sketch->ConstraintPlanner());
}


/***********************************************~***************************************************/


WCSketchPoint::WCSketchPoint(WCSketch *sketch, const std::string &name, const WCVector4 &position) : 
	::WCSketchFeature(sketch, name), _base(NULL), _position(position) {
	//Calculate global x, y, z from sketch plane
	WCVector4 pos = this->_sketch->ReferencePlane()->TransformMatrix() * position;
	//Create the base point
	this->_base = new WCGeometricPoint(pos);
	//Check feature name
	if (this->_name == "") this->_name = this->_sketch->GenerateFeatureName(this);
	//Make point construction by default
	this->_isConstruction = SKETCHPOINT_CONSTRUCTION_DEFAULT;
	this->_base->IsConstruction(SKETCHPOINT_CONSTRUCTION_DEFAULT);
	//Initialize the object
	this->Initialize();
}


WCSketchPoint::WCSketchPoint(xercesc::DOMElement *element, WCSerialDictionary *dictionary) : 
	::WCSketchFeature( WCSerializeableObject::ElementFromName(element,"SketchFeature"), dictionary),
	_base(NULL), _position() {
	//Make sure element if not null
	if (element == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchPoint::WCSketchPoint - NULL Element passed.");
		//throw error
		return;
	}
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);
	//Setup base
	this->_base = new WCGeometricPoint( WCSerializeableObject::ElementFromName(element,"GeometricPoint"), dictionary );
	//Setup position
	this->_position.FromElement(WCSerializeableObject::ElementFromName(element,"Position"));
	//Initialize the object
	this->Initialize();
}


WCSketchPoint::~WCSketchPoint() {
	//Remove from the sketch
	if (!this->_sketch->RemoveFeature(this)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchPoint::~WCSketchPoint - Problem removing feature from sketch.");	
	}
	this->_sketch->RemovePoint(this->_base);
	//Release the base
	this->_base->Release(*this);
	//Delete the base
	if (this->_base != NULL) delete this->_base;
	//Remove the constraint node from planner
	this->_sketch->ConstraintPlanner()->RemoveNode( this->_constraintNodes.front() );
	//If there are any remaining references, send out a broadcast
	if (this->_refSet.size() > 0) this->SendBroadcastNotice(OBJECT_NOTIFY_DELETE);
}


void WCSketchPoint::Set(const WCVector4 &position) {
	//Update the point
	this->_position = position;
	//Calculate global x, y, z
	WCVector4 pos = this->_sketch->ReferencePlane()->TransformMatrix() * position;
	//Set the base point
	this->_base->Set(pos);
	//Make sure all dependent objects know about it
	this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);
}


void WCSketchPoint::Set(const WPFloat &x, const WPFloat &y) {
	//Update the point
	this->_position.Set(x, y, 0.0, 1.0);
	//Calculate global x, y, z
	WCVector4 pos(x, y, 0.0, 1.0);
	pos = this->_sketch->ReferencePlane()->TransformMatrix() * pos;
	//Set the base point
	this->_base->Set(pos);
	//Make sure all dependent objects know about it
	this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);
}

	
void WCSketchPoint::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
	//See if notice is from sketch or base point
	if (sender == this->_sketch) {
		//Reposition base point
		//...
	}
	//Base FeaturePoint was moved
	else {
		WCVector4 pos = this->_sketch->ReferencePlane()->InverseTransformMatrix() * this->_base->Data();
		//Update the values
		this->_position.Set(pos.I(), pos.J(), 0.0, 1.0);
	}
}


bool WCSketchPoint::Regenerate(void) {
	CLOGGER_WARN(WCLogManager::RootLogger(), "WCSketchPoint::Regenerate - Not yet implemented.");
	return false;
}


void WCSketchPoint::InjectConstraints(WCConstraintPlanner *planner) {
	//Clear out any existing nodes
	this->_constraintNodes.clear();
	//Add the feature into the planner
	WCConstraintNode *node = planner->AddNode(this, WCConstraintNodeType::Point(), 2, this->_position);
	//Add the constraint node into the list
	this->_constraintNodes.push_back(node);
}


std::list<WCObject*> WCSketchPoint::DeletionDependencies(void) {
	//Only downward dependency is the base point
	return std::list<WCObject*>(1, this->_base);
}


void WCSketchPoint::IsConstruction(const bool state) {
	//See if something is changing
	if (state == this->_isConstruction) return;
	//Set state and base state
	this->_isConstruction = state;
	this->_base->IsConstruction(state);
	
	//If not selected
	if (!this->_isSelected) {
		//Otherwise, set color appropriately
		if (this->_isConstruction) this->_base->Color(WCSketchFeature::ConstructionColor);
		else this->_base->Color(WCSketchFeature::DefaultColor);
	}
}


void WCSketchPoint::OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) {
	//Set the color of the base point
	this->_base->Color(WCSketchFeature::SelectedColor);
	//Mark this as selected
	this->_isSelected = true;
}


void WCSketchPoint::OnDeselection(const bool fromManager) {
	//Set the color of the base point - depends on construction
	if (this->_isConstruction) this->_base->Color(WCSketchFeature::ConstructionColor);
	else this->_base->Color(WCSketchFeature::DefaultColor);
	//Mark this as not selected
	this->_isSelected = false; 
}

	
xercesc::DOMElement* WCSketchPoint::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create primary element for this object
	XMLCh* xmlString = xercesc::XMLString::transcode("SketchPoint");
	xercesc::DOMElement* element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Include the parent element
	xercesc::DOMElement* featureElement = this->WCSketchFeature::Serialize(document, dictionary);
	element->appendChild(featureElement);

	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);
	//Add child geometric point
	xercesc::DOMElement *pointElement = this->_base->Serialize(document, dictionary);
	element->appendChild(pointElement);
	//Add position vector
	this->_position.ToElement(element, "Position");

	//Return the new element
	return element;
}
	
	
/***********************************************~***************************************************/


WCDrawingMode* WCSketchPoint::ModeCreate(WCSketchWorkbench *wb) {
	//Create a new drawing mode
	return new WCModeSketchPointCreate(wb);
}


WCDrawingMode* WCSketchPoint::ModeEdit(WCSketchPoint *point) {
	//Create a new editing mode
	return new WCModeSketchPointEdit(point);
}


WCActionSketchPointCreate* WCSketchPoint::ActionCreate(WCSketch *sketch, const std::string &pointName, const WCVector4 &pos) {
	//Create new action for creating a point
	return new WCActionSketchPointCreate(sketch, pointName, pos);
}


WCAction* WCSketchPoint::ActionMove(WCSketchPoint *point, const WCVector4 &pos) {
	//Create new action for moving a point
	return new WCActionSketchPointMove(point, pos);
}


/***********************************************~***************************************************/


std::ostream& __WILDCAT_NAMESPACE__::operator<<(std::ostream& out, const WCSketchPoint &point) {
	std::cout << "SketchPoint(" << &point << ") - X: " << point._position.I() << ", Y: " << point._position.J();
	return out;
}


/***********************************************~***************************************************/