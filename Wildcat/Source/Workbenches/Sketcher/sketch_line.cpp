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
#include "Sketcher/sketch_line.h"
#include "Sketcher/sketch.h"
#include "Sketcher/sketch_point.h"
#include "Kernel/document.h"
#include "PartDesign/plane.h"


/***********************************************~***************************************************/


void WCSketchLine::Initialize(void) {
	//Retain the objects
	this->_line->Retain(*this);
	//Create the point controller
	this->_controller = new WCSketchLineController(this);
	//Add into sketch
	if ((!this->_sketch->AddFeature(this)) || 
		(!this->_sketch->AddLine(this->_line, this->_controller))) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchLine::Initialize - Problem adding feature or objects to sketch.");
		//Should delete base
		//throw error
		return;
	}
	//Create tree element and add into the tree (beneath the sketch features element)
	WSTexture *lineIcon = this->_document->Scene()->TextureManager()->TextureFromName("line32");
	this->_treeElement = new WCTreeElement(	this->_sketch->Document()->TreeView(), this->_name, this->_controller, lineIcon);
	this->_sketch->FeaturesTreeElement()->AddLastChild(this->_treeElement);
	//Inject constraints into sketch planner
	this->InjectConstraints(this->_sketch->ConstraintPlanner());
}


/***********************************************~***************************************************/


WCSketchLine::WCSketchLine(WCSketch *sketch, const std::string &name, const WCVector4 &begin, const WCVector4 &end) :
	::WCSketchFeature(sketch, name), _begin(begin), _end(end), _line(NULL), _isInfinite(false) {
	//Validate sketch
	if (sketch == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchLine::WCSketchLine - NULL references passed.\n");
		//throw error
		return;
	}
	//Calcuate begin and end points
	WCVector4 bVec = this->_sketch->ReferencePlane()->TransformMatrix() * this->_begin;
	WCVector4 eVec = this->_sketch->ReferencePlane()->TransformMatrix() * this->_end;
	//Create line
	this->_line = new WCGeometricLine(bVec, eVec);
	//Check feature name
	if (this->_name == "") this->_name = this->_sketch->GenerateFeatureName(this);
	//Set the object properties
	this->_line->Color(WCSketchFeature::DefaultColor);
	this->_line->Thickness(WCSketchFeature::LineThickness);

	//Initialize the object
	this->Initialize();
}


WCSketchLine::WCSketchLine(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCSketchFeature( WCSerializeableObject::ElementFromName(element,"SketchFeature"), dictionary ),
	_begin(), _end(), _line(NULL), _isInfinite(false) {
	//Make sure element if not null
	if (element == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchLine:: - NULL Element passed.");
		//throw error
		return;
	}
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);
	//Setup infinite attribute
	this->_isInfinite = WCSerializeableObject::GetBoolAttrib(element, "infinite");
	//Setup base
	this->_line = new WCGeometricLine( WCSerializeableObject::ElementFromName(element,"GeometricLine"), dictionary );
	//Restore begin
	this->_begin.FromElement( WCSerializeableObject::ElementFromName(element,"Begin") );
	//Restore end
	this->_end.FromElement( WCSerializeableObject::ElementFromName(element,"End") );
	//Initialize the object
	this->Initialize();
}


WCSketchLine::~WCSketchLine() {
	//Remove from the sketch
	if (!this->_sketch->RemoveFeature(this)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchLine::~WCSketchLine - Problem removing feature from sketch.");	
	}
	//Remove the objects from the sketch
	this->_sketch->RemoveLine(this->_line);
	//Release the objects
	this->_line->Release(*this);
	//Delete the objects
	if (this->_line->RefCount() == 0) delete this->_line;
}


void WCSketchLine::Set(const WCVector4 &begin, const WCVector4 &end) {
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

	
void WCSketchLine::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
	//Updates can be for position change, or color change
//	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchLine::ReceiveNotice - Invalid notice received.");
}


bool WCSketchLine::Regenerate(void) {
	CLOGGER_WARN(WCLogManager::RootLogger(), "WCSketchLine::Regenerate - Not yet implemented.");
	return false;
}


void WCSketchLine::InjectConstraints(WCConstraintPlanner *planner) {
	//Clear out any existing nodes
	this->_constraintNodes.clear();
	//Add the feature nodes into the planner
	WCConstraintNode *primaryNode = planner->AddNode(this, WCConstraintNodeType::Line(), 2, WCVector4());
	WCConstraintNode *beginNode = planner->AddNode(this, WCConstraintNodeType::Point(), 2, this->_begin);
	WCConstraintNode *endNode = planner->AddNode(this, WCConstraintNodeType::Point(), 2, this->_end);
	//Add the constraint nodes into the list
	this->_constraintNodes.push_back(primaryNode);
	this->_constraintNodes.push_back(beginNode);
	this->_constraintNodes.push_back(endNode);
}


std::list<WCObject*> WCSketchLine::DeletionDependencies(void) {
	//Only downward dependency are the base objects
	std::list<WCObject*> list;
	list.push_back(this->_line);
	return list;
}


void WCSketchLine::OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) {
	//Set the color of the objects
	this->_line->Color(WCSketchFeature::SelectedColor);
	//Mark this as selected
	this->_isSelected = true; 
}


void WCSketchLine::OnDeselection(const bool fromManager) {
	//Set the color of the objects - depends on construction
	if (this->_isConstruction) this->_line->Color(WCSketchFeature::ConstructionColor);
	else this->_line->Color(WCSketchFeature::DefaultColor);
	//Mark this as not selected
	this->_isSelected = false;  
}


void WCSketchLine::IsConstruction(const bool state) {
	//See if something is changing
	if (state == this->_isConstruction) return;
	//Set state and base state
	this->_isConstruction = state;
	this->_line->IsConstruction(state);
	
	//Exit if selected
	if (this->_isSelected) return;
	//Otherwise, set color (of line only) appropriately
	if (this->_isConstruction) this->_line->Color(WCSketchFeature::ConstructionColor);
	else this->_line->Color(WCSketchFeature::DefaultColor);
}


xercesc::DOMElement* WCSketchLine::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create primary element for this object
	XMLCh* xmlString = xercesc::XMLString::transcode("SketchLine");
	xercesc::DOMElement* element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Include the parent element
	xercesc::DOMElement* featureElement = this->WCSketchFeature::Serialize(document, dictionary);
	element->appendChild(featureElement);
	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);
	//Add infinite attribute
	WCSerializeableObject::AddBoolAttrib(element, "infinite", this->_isInfinite);

	//Add child geometric line
	xercesc::DOMElement *lineElement = this->_line->Serialize(document, dictionary);
	element->appendChild(lineElement);
	//Add begin vector
	xmlString = xercesc::XMLString::transcode("Begin");
	xercesc::DOMElement* beginElement = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	this->_begin.ToElement(beginElement);
	element->appendChild(beginElement);
	//Add end vector
	xmlString = xercesc::XMLString::transcode("End");
	xercesc::DOMElement* endElement = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	this->_end.ToElement(endElement);
	element->appendChild(endElement);

	//Return the new element
	return element;
}

	
/***********************************************~***************************************************/


WCDrawingMode* WCSketchLine::ModeCreate(WCSketchWorkbench *wb) {
	//Create a new drawing mode
	return new WCModeSketchLineCreate(wb);
}


WCDrawingMode* WCSketchLine::ModeEdit(WCSketchLine *line) {
	//Create a new editing mode
	return new WCModeSketchLineEdit(line);
}


WCActionSketchLineCreate* WCSketchLine::ActionCreate(WCSketch *sketch, const std::string &lineName, 
	const WCVector4 &begin, const WCVector4 &end) {
	//Create new action to create line
	return new WCActionSketchLineCreate(sketch, lineName, begin, end);
}


WCActionSketchLineMove* WCSketchLine::ActionMove(WCSketchLine *line, const WCVector4 &begin, const WCVector4 &end) {
	//Create new action to move the line
	return new WCActionSketchLineMove(line, begin, end);
}


/***********************************************~***************************************************/

	
std::ostream& operator<<(std::ostream& out, const WCSketchLine &line) {
	//Print some basic info
	out << "SketchLine(" << &line << ") - Infinite:";
	if (line._isInfinite) out << "True\n";
	else out << "False\n";
	//Print out begin and end info
	out << "\tBegin: " << line._begin << std::endl;
	out << "\tEnd: " << line._end << std::endl;
	//Maybe print geometric line info
	//...
	//Return
	return out;
}


/***********************************************~***************************************************/

