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
#include <Sketcher/sketch.h>
#include <Sketcher/sketch_workbench.h>
#include <Sketcher/sketch_controller.h>
#include <Sketcher/sketch_feature.h>
#include <Constraint/sketch_constraint.h>
#include <Constraint/constraint_planner.h>
#include <Kernel/workbench_layer.h>
#include <PartDesign/part.h>
#include <PartDesign/part_plane.h>


/***********************************************~***************************************************/


void WCSketch::Initialize(void) {
	//Retain the plane
	this->_refPlane->Retain(*this);
	//Create event handler
	this->_controller = new WCSketchController(this);
	//Create constraint solver
	this->_planner = new WCConstraintPlanner();
	//Get tree element icons
	WSTexture* sketchIcon = this->_document->Scene()->TextureManager()->TextureFromName("sketch32");
	WSTexture* constraintIcon = this->_document->Scene()->TextureManager()->TextureFromName("constraint32");
	WSTexture* folderIcon = this->_document->Scene()->TextureManager()->TextureFromName("folder32");
	WSTexture* profileIcon = this->_document->Scene()->TextureManager()->TextureFromName("profile32");
	//Create tree elements
	this->_treeElement = new WCTreeElement(this->_document->TreeView(), this->_name, this->_controller, sketchIcon);
	this->_refTreeElement = new WCTreeElement(this->_document->TreeView(), "References", this->_controller, folderIcon);
	this->_featureTreeElement = new WCTreeElement(this->_document->TreeView(), "Features", this->_controller, folderIcon);
	this->_constraintTreeElement = new WCTreeElement(this->_document->TreeView(), "Constraints", this->_controller, constraintIcon);
	this->_profileTreeElement = new WCTreeElement(this->_document->TreeView(), "Profiles", this->_controller, profileIcon);
	//Add elements to sketch tree element
	this->_treeElement->AddLastChild(this->_refTreeElement);
	this->_treeElement->AddLastChild(this->_featureTreeElement);
	this->_treeElement->AddLastChild(this->_constraintTreeElement);
	this->_treeElement->AddLastChild(this->_profileTreeElement);
	//Add the refPlane as a child to the reference tree element
	this->_refTreeElement->AddLastChild(this->_refPlane->TreeElement());
	//Add tree view element to parent
	this->_creator->TreeElement()->AddLastChild(this->_treeElement);
	this->_treeElement->IsOpen(false);
	this->_refTreeElement->IsOpen(false);
	this->_featureTreeElement->IsOpen(false);
	this->_constraintTreeElement->IsOpen(false);
	this->_profileTreeElement->IsOpen(false);
	//Add the sketch to the part
	this->_part->AddFeature(this, false);
	//Create workbench (CreateAction enters the workbench)
	this->_workbench = new WCSketchWorkbench(this);
	//Setup h and v text labels
	this->_hText = new WCText(this->_document->Scene(), "H", WCColor(1.0f, 1.0f, 0.0f, 1.0f),
							  WCTextFont::Palatino(), WCTextStyle::Roman(), 15.0);
	this->_vText = new WCText(this->_document->Scene(), "V", WCColor(1.0f, 1.0f, 0.0f, 1.0f),
							  WCTextFont::Palatino(), WCTextStyle::Roman(), 15.0);
}


/***********************************************~***************************************************/


WCSketch::WCSketch(WCFeature *creator, const std::string &name, WCPartPlane* plane) : ::WCPartFeature(creator, name),
	_workbench(NULL), _refPlane(plane), _planner(NULL),
	_featureMap(), _featureList(), _pointMap(), _lineMap(), _curveMap(), _constraintList(), _profileList(),
	_refTreeElement(NULL), _featureTreeElement(NULL), 
	_constraintTreeElement(NULL), _profileTreeElement(NULL), _drawConstraints(true), _hText(NULL), _vText(NULL) {
	//Make sure plane is non-null
	if ((this->_refPlane == NULL) || (this->_creator == NULL)) { 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketch::WCSketch - NULL plane or creator passed.");
		//throw error
		return;
	}
	//Check feature name
	if (this->_name == "") this->_name = this->_part->GenerateFeatureName(this);
	//Initialize the object
	this->Initialize();
	//Create initial objects
	this->_workbench->CreateInitialObjects();
}


WCSketch::WCSketch(xercesc::DOMElement *element, WCSerialDictionary *dictionary) : 
	::WCPartFeature( WCSerializeableObject::ElementFromName(element,"PartFeature"), dictionary),
	_workbench(NULL), _refPlane(NULL), _planner(NULL),
	_featureMap(), _featureList(), _pointMap(), _lineMap(), _curveMap(), _constraintList(), _profileList(),
	_refTreeElement(NULL), _featureTreeElement(NULL), 
	_constraintTreeElement(NULL), _profileTreeElement(NULL), _drawConstraints(true), _hText(NULL), _vText(NULL) {
	//Make sure element if not null
	if (element == NULL) return;
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);
	//Get the reference plane
	this->_refPlane = (WCPartPlane*)WCSerializeableObject::GetGUIDAttrib(element, "reference", dictionary);

	//Initialize the object
	this->Initialize();

	//Create sketch features
	XMLCh* xmlString = xercesc::XMLString::transcode("Features");
	xercesc::DOMNodeList *list = element->getElementsByTagName(xmlString)->item(0)->getChildNodes();
	xercesc::XMLString::release(&xmlString);
	xercesc::DOMNode *tmpNode;
	xercesc::DOMElement *featureElement;
	bool retVal;
	//Loop through all features
	for (WPUInt featureIndex=0; featureIndex < list->getLength(); featureIndex++) {
		//Get the indexed node
		tmpNode = list->item(featureIndex);
		//Make sure node is element
		if (tmpNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
			//Cast node to element
			featureElement = (xercesc::DOMElement*)tmpNode;
			//Pass to master feature switch
			retVal = WCSketchFeature::Deserialize(featureElement, dictionary);
		}
	}
	//Add all geometry into the part
	this->PublishGeometry();
}


WCSketch::~WCSketch() {
	//Remove the sketch to the part
	this->_part->RemoveFeature(this, false);
	//Need to delete all features (in reverse order)
	WCSketchFeature *feature;
	while (!this->_featureList.empty()) {
		//Get the last feature
		feature = this->_featureList.back();
		delete feature;
	}
	//If not null, release the plane
	if (this->_refPlane != NULL) this->_refPlane->Release(*this);
	//If workbench is still null delete it
	if (this->_workbench != NULL) delete this->_workbench;
	//Delete the extra tree elements
	if (this->_refTreeElement != NULL) delete this->_refTreeElement;
	if (this->_featureTreeElement != NULL) delete this->_featureTreeElement;
	if (this->_constraintTreeElement != NULL) delete this->_constraintTreeElement;
	if (this->_profileTreeElement != NULL) delete this->_profileTreeElement;
	//Delete the text labels
	if (this->_hText != NULL) delete this->_hText;
	if (this->_vText != NULL) delete this->_vText;
	//Delete the constraint planner
	if (this->_planner != NULL) delete this->_planner;
}


void WCSketch::PublishGeometry(void) {
	//Add all points
	std::map<WCGeometricPoint*,WCEventController*>::iterator pointIter;
	for (pointIter = this->_pointMap.begin(); pointIter != this->_pointMap.end(); pointIter++) {
		//Only publish non-construction elements
		if(!(*pointIter).first->IsConstruction()) {
			//Set the object color
			(*pointIter).first->Color(WCPartFeature::DefaultPointColor);
			//Add the object to the document
			this->_part->AddPoint((*pointIter).first, this->_controller);
		}
	}
	//Add all the lines
	std::map<WCGeometricLine*,WCEventController*>::iterator lineIter;
	for (lineIter = this->_lineMap.begin(); lineIter != this->_lineMap.end(); lineIter++) {
		//Only publish non-construction elements
		if(!(*lineIter).first->IsConstruction()) {
			//Set the object color
			(*lineIter).first->Color(WCPartFeature::DefaultCurveColor);
			//Add the object to the document
			this->_part->AddLine((*lineIter).first, this->_controller);
		}
	}
	//Add all the curves
	std::map<WCNurbsCurve*,WCEventController*>::iterator curveIter;
	for (curveIter = this->_curveMap.begin(); curveIter != this->_curveMap.end(); curveIter++) {
		//Only publish non-construction elements
		if(!(*curveIter).first->IsConstruction()) {
			//Set the object color
			(*curveIter).first->Color(WCPartFeature::DefaultCurveColor);
			//Add the object to the document
			this->_part->AddCurve((*curveIter).first, this->_controller);
		}
	}
}


void WCSketch::UnpublishGeometry(void) {
//	std::cout << "Pulling the geometry out!\n";
	//Remove all points
	std::map<WCGeometricPoint*,WCEventController*>::iterator pointIter;
	for (pointIter = this->_pointMap.begin(); pointIter != this->_pointMap.end(); pointIter++) {
		//Set the object color
		if ((*pointIter).first->IsConstruction())(*pointIter).first->Color(WCSketchFeature::ConstructionColor);
		else (*pointIter).first->Color(WCSketchFeature::DefaultColor);
		//Only remove published items
		if (!(*pointIter).first->IsConstruction()) this->_part->RemovePoint((*pointIter).first);
	}
	//Remove all the lines
	std::map<WCGeometricLine*,WCEventController*>::iterator lineIter;
	for (lineIter = this->_lineMap.begin(); lineIter != this->_lineMap.end(); lineIter++) {
		//Set the object color
		if ((*lineIter).first->IsConstruction())(*lineIter).first->Color(WCSketchFeature::ConstructionColor);
		else (*lineIter).first->Color(WCSketchFeature::DefaultColor);
		//Only remove published items
		if (!(*lineIter).first->IsConstruction()) this->_part->RemoveLine((*lineIter).first);
	}
	//Remove all the curves
	std::map<WCNurbsCurve*,WCEventController*>::iterator curveIter;
	for (curveIter = this->_curveMap.begin(); curveIter != this->_curveMap.end(); curveIter++) {
		//Set the object color
		if ((*curveIter).first->IsConstruction())(*curveIter).first->Color(WCSketchFeature::ConstructionColor);
		else (*curveIter).first->Color(WCSketchFeature::DefaultColor);
		//Only remove published items
		if (!(*curveIter).first->IsConstruction()) this->_part->RemoveCurve((*curveIter).first);
	}
}


bool WCSketch::AddFeature(WCSketchFeature *feature) {
	//Make sure feature is non-null
	if (feature == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketch::AddFeature - NULL feature passed.");
		return false;
	}
	//Check to see if feature already in list
	std::map<std::string,WCSketchFeature*>::iterator iter = this->_featureMap.find(feature->GetName());
	if (iter != this->_featureMap.end()) {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCSketch::AddFeature - " << feature->GetName() << " is already in the sketch.");
		return false;
	}
	//Add into the featureMap
	this->_featureMap.insert( std::make_pair(feature->GetName(), feature));
	//Add into featureList
	this->_featureList.push_back(feature);
	//Retain the feature
	feature->Retain(*this);
	//Set the construction state for the feature
	feature->IsConstruction(this->_workbench->IsConstruction());
	//Regenerate constraint solutions
	//...
	return true;
}


bool WCSketch::RemoveFeature(WCSketchFeature *feature) {
	//Make sure feature is non-null
	if (feature == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketch::RemoveFeature - NULL feature passed.");
		return false;
	}
	//Make sure feature is not a constraint
	WCSketchConstraint *constraint = dynamic_cast<WCSketchConstraint*>(feature);
	if (constraint != NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketch::RemoveFeature - Passed feature is a contraint.");
		return false;	
	}
	//Try to erase it
	if (this->_featureMap.erase(feature->GetName()) > 0) {
		//Release the object
		feature->Release(*this);
		std::list<WCSketchConstraint*>::iterator iter;
		std::list<WCSketchConstraint*> toBeDeleted;
		for (iter = this->_constraintList.begin(); iter != this->_constraintList.end(); iter++)
			if ((*iter)->DependencyCheck(feature)) toBeDeleted.push_back( *iter );
		//If there are any constraints to be deleted, get them
		if (toBeDeleted.size() > 0) {
			for (iter = toBeDeleted.begin(); iter != toBeDeleted.end(); iter++)
				delete *iter;
		}
		//Remove from feature list
		this->_featureList.remove(feature);
		//Return true
		return true;
	}
	//Otherwise, not found
	return false;
}


bool WCSketch::DeleteFeatures(std::list<WCSketchFeature*> features) {
	std::list<WCSketchFeature*>::iterator iter;
	bool found;
	//Loop through all requested objects
	for (iter = features.begin(); iter != features.end(); iter++) {
		if ( (*iter)->Sketch() != this) {
			CLOGGER_WARN(WCLogManager::RootLogger(), "WCSketch::DeleteFeatures - Feature not-from this sketch found."); 
		} if ((*iter) == NULL) {
			CLOGGER_WARN(WCLogManager::RootLogger(), "WCSketch::DeleteFeatures - NULL Feature found."); 
		} else {
			//Iterate through the list of features
			std::map<std::string,WCSketchFeature*>::iterator featureIter;
			found = false;
			//Check to see if feature is still in sketch
			for (featureIter = this->_featureMap.begin(); featureIter != this->_featureMap.end(); featureIter++) {
				if ((*iter) == (*featureIter).second) found = true;
			}
			//If the feature was found, it was still in the sketch
			if ((found) && ((*iter)->GetName() != "Origin")) {
				std::cout << "Deleting: " << (*iter)->GetName() << std::endl;
				delete *iter;
			}
		}
	}
	//Regenerate profiles
	this->AnalyzeProfiles();
	//Regenerate constraint solutions
	//...
	//All is good for now
	return true;
}


bool WCSketch::AddConstraint(WCSketchConstraint *constraint) {
	//Make sure constraint is non-null
	if (constraint == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketch::AddConstraint - NULL constraint passed.");
		return false;
	}
	//Check to see if constraint already in feature list
	std::map<std::string,WCSketchFeature*>::iterator iter = this->_featureMap.find(constraint->GetName());
	if (iter != this->_featureMap.end()) {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCSketch::AddConstraint - " << constraint->GetName() << " is already in the sketch.");
		return false;
	}
	//Add into the sketch featureMap, featureList, and constraintList
	this->_featureMap.insert( std::make_pair(constraint->GetName(), constraint) );
	this->_featureList.push_back( constraint );
	this->_constraintList.push_back( constraint );
	//Add constraint into selector
	this->_workbench->SelectionManager()->AddObject(constraint, constraint->Controller());
	//Retain the constraint
	constraint->Retain(*this);
	//All is good
	return true;
}


bool WCSketch::RemoveConstraint(WCSketchConstraint *constraint) {
	//Make sure constraint is non-null
	if (constraint == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketch::RemoveConstraint - NULL point passed.");
		return false;
	}
	//Try to erase it
	if (this->_featureMap.erase(constraint->GetName()) > 0) {
		//Remove from featureList
		this->_featureList.remove(constraint);
		//Remove from constraintList
		this->_constraintList.remove(constraint);
		//Remove from selection manager
		this->_workbench->SelectionManager()->RemoveObject(constraint);
		//Release the object
		constraint->Release(*this);
		//Return true
		return true;
	}
	//Otherwise, not found
	return false;
}	


WCSketchFeature* WCSketch::FeatureFromName(const std::string &name) {
	std::map<std::string, WCSketchFeature*>::iterator iter = this->_featureMap.find(name);
	if (iter == this->_featureMap.end()) return NULL;
	return iter->second;
}


std::string WCSketch::GenerateFeatureName(WCSketchFeature *feature) {
	std::string root = feature->GetName(); 
	//Catch empty case
	if (root == "") {
		root = feature->RootName();
	}
	WPUInt index = 1;
	std::string id, newString;
	//Loop until a good name is found
	do {
		//Get index into a string
		std::stringstream ss;
		ss << index;
		ss >> id;
		newString = root + "." + id;
		index++;
//		std::cout << "Index = " << index << " Name: " << newString << std::endl;
	} while (!this->CheckName(newString));
	//Found a good name
	return newString;
}


bool WCSketch::AddPoint(WCGeometricPoint *point, WCEventController *controller) {
	//Make sure point is non-null
	if (point == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketch::AddPoint - NULL point passed.");
		return false;
	}
	//Check to see if point already in list
	std::map<WCGeometricPoint*,WCEventController*>::iterator iter = this->_pointMap.find(point);
	if (iter != this->_pointMap.end()) {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCSketch::AddPoint - Point is already in the sketch.");
		return false;
	}
	//Add into the sketch list
	this->_pointMap.insert( std::make_pair(point, controller) );
	//Add point into selector
	this->_workbench->SelectionManager()->AddObject(point, controller);
	//Set layer of point
//	point->Layer(this->_workbench->Layer());
	//All is good
	return true;
}


bool WCSketch::RemovePoint(WCGeometricPoint *point) {
	//Make sure point is non-null
	if (point == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketch::RemovePoint - NULL point passed.");
		return false;
	}
	//Try to erase it
	if (this->_pointMap.erase(point) > 0) {
		//Remove point from selector
		this->_workbench->SelectionManager()->RemoveObject(point);
		//Return true
		return true;
	}
	//Otherwise, not found
	return false;
}


bool WCSketch::AddLine(WCGeometricLine *line, WCEventController *controller) {
	//Make sure line is non-null
	if (line == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketch::AddLine - NULL line passed.");
		return false;
	}
	//Check to see if line already in list
	std::map<WCGeometricLine*,WCEventController*>::iterator iter = this->_lineMap.find(line);
	if (iter != this->_lineMap.end()) {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCSketch::AddLine - Line is already in the sketch.");
		return false;
	}
	//Add into the sketch list
	this->_lineMap.insert( std::make_pair(line, controller) );
	//Add line into selector
	this->_workbench->SelectionManager()->AddObject(line, controller);
	//Set layer of line
//	line->Layer(this->_workbench->Layer());
	//All is good
	return true;
}


bool WCSketch::RemoveLine(WCGeometricLine *line) {
	//Make sure line is non-null
	if (line == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketch::RemoveLine - NULL line passed.");
		return false;
	}
	//Try to erase it
	if (this->_lineMap.erase(line) > 0) {
		//Remove line from selector
		this->_workbench->SelectionManager()->RemoveObject(line);
		//Return true
		return true;
	}
	//Otherwise, not found
	return false;
}



bool WCSketch::AddCurve(WCNurbsCurve *curve, WCEventController *controller) {
	//Make sure curve is non-null
	if (curve == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketch::AddCurve - NULL curve passed.");
		return false;
	}
	//Check to see if curve already in list
	std::map<WCNurbsCurve*,WCEventController*>::iterator iter = this->_curveMap.find(curve);
	if (iter != this->_curveMap.end()) {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCSketch::AddCurve - Curve is already in the sketch.");
		return false;
	}
	//Add into the sketch list
	this->_curveMap.insert( std::make_pair(curve, controller) );
	//Add curve into selector
	this->_workbench->SelectionManager()->AddObject(curve, controller);
	//Set layer of curve
//	curve->Layer(this->_workbench->Layer());
	//All is good
	return true;
}


bool WCSketch::RemoveCurve(WCNurbsCurve *curve) {
	//Make sure curve is non-null
	if (curve == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketch::RemoveCurve - NULL curve passed.");
		return false;
	}
	//Try to erase it
	if (this->_curveMap.erase(curve) > 0) {
		//Remove curve from selector
		this->_workbench->SelectionManager()->RemoveObject(curve);
		//Return true
		return true;
	}
	//Otherwise, not found
	return false;
}


void WCSketch::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
//	CLOGGER_WARN(WCLogManager::RootLogger(), "WCSketch::ReceiveNotice - Not yet implemented.");
//	this->_isDirty = true;
}


WCAlignedBoundingBox WCSketch::BoundingBox(void) {
	//Center initial box about origin
	WCVector4 ll(-1.0, -1.0, 0.0, 1.0);
	ll = this->_refPlane->TransformMatrix() * ll;
	WCVector4 ur(1.0, 1.0, 0.0, 1.0);
	ur = this->_refPlane->TransformMatrix() * ur;
	WCVector4 pts[2] = {ll, ur};
	WCAlignedBoundingBox box(pts, 2);
	
	//Add all points
	std::map<WCGeometricPoint*,WCEventController*>::iterator pointIter;
	for (pointIter = this->_pointMap.begin(); pointIter != this->_pointMap.end(); pointIter++)
		box += (*pointIter).first->BoundingBox();
	//Add all lines
	std::map<WCGeometricLine*,WCEventController*>::iterator lineIter;
	for (lineIter = this->_lineMap.begin(); lineIter != this->_lineMap.end(); lineIter++)
		box += (*lineIter).first->BoundingBox();
	//Add all curves
	std::map<WCNurbsCurve*,WCEventController*>::iterator curveIter;
	for (curveIter = this->_curveMap.begin(); curveIter != this->_curveMap.end(); curveIter++)
		box += (*curveIter).first->BoundingBox();
	return box;
}


bool WCSketch::Regenerate(void) {
	bool status = true;
	//Go through all features and constraints and regenerate
	//...
	return status;
}

void WCSketch::OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) {
	this->_isSelected = true;
	this->_color.Set(WCPartFeature::SelectedColor);
	//Color all points
	std::map<WCGeometricPoint*,WCEventController*>::iterator pointIter;
	for (pointIter = this->_pointMap.begin(); pointIter != this->_pointMap.end(); pointIter++)
		(*pointIter).first->Color(this->_color);
	//Color all lines
	std::map<WCGeometricLine*,WCEventController*>::iterator lineIter;
	for (lineIter = this->_lineMap.begin(); lineIter != this->_lineMap.end(); lineIter++)
		(*lineIter).first->Color(this->_color);
	//Color all curves
	std::map<WCNurbsCurve*,WCEventController*>::iterator curveIter;
	for (curveIter = this->_curveMap.begin(); curveIter != this->_curveMap.end(); curveIter++)
		(*curveIter).first->Color(this->_color);
}


void WCSketch::OnDeselection(const bool fromManager) {
	this->_isSelected = false;
	//Color all points
	std::map<WCGeometricPoint*,WCEventController*>::iterator pointIter;
	for (pointIter = this->_pointMap.begin(); pointIter != this->_pointMap.end(); pointIter++)
		(*pointIter).first->Color(WCPartFeature::DefaultPointColor);
	//Color all lines
	std::map<WCGeometricLine*,WCEventController*>::iterator lineIter;
	for (lineIter = this->_lineMap.begin(); lineIter != this->_lineMap.end(); lineIter++)
		(*lineIter).first->Color(WCPartFeature::DefaultCurveColor);
	//Color all curves
	std::map<WCNurbsCurve*,WCEventController*>::iterator curveIter;
	for (curveIter = this->_curveMap.begin(); curveIter != this->_curveMap.end(); curveIter++)
		(*curveIter).first->Color(WCPartFeature::DefaultCurveColor);
}


void WCSketch::Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom) {
	//Calculate base and h-v pointers
	WCVector4 base(0.0, 0.0, 0.0, 1.0);
	WCVector4 uUnit(1.0, 0.0, 0.0, 0.0);
	WCVector4 vUnit(0.0, 1.0, 0.0, 0.0);
	WCMatrix4 mat = this->_refPlane->TransformMatrix();

	//Calculate final locations
	base = mat * base;
	uUnit = (mat * uUnit) / zoom;
	vUnit = (mat * vUnit) / zoom;
	WCVector4 hAxis = base + uUnit * SKETCH_AXIS_SCALE;
	WCVector4 vAxis = base + vUnit * SKETCH_AXIS_SCALE;

	//Disable depth testing
	glDisable(GL_DEPTH_TEST);

	//Draw the two axis
	glColor4f(SKETCH_AXIS_COLOR);
	GLfloat data[9] = { (GLfloat)vAxis.I(), (GLfloat)vAxis.J(), (GLfloat)vAxis.K(),
						(GLfloat)base.I(), (GLfloat)base.J(), (GLfloat)base.K(),
						(GLfloat)hAxis.I(), (GLfloat)hAxis.J(), (GLfloat)hAxis.K() };
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, data);
	glDrawArrays(GL_LINE_STRIP, 0, 3);
	glDisableClientState(GL_VERTEX_ARRAY);

	//Label the axis'
	this->_hText->DrawAtPoint(hAxis, uUnit, vUnit);
	this->_vText->DrawAtPoint(vAxis, uUnit, vUnit);
	
	//Draw all the points
	std::map<WCGeometricPoint*,WCEventController*>::iterator pointIter;
	for (pointIter = this->_pointMap.begin(); pointIter != this->_pointMap.end(); pointIter++) {
		//Render the points
		(*pointIter).first->Render(0, WCColor::Default(), this->_document->Scene()->ActiveCamera()->Zoom());
	}
	//Draw all the lines
	std::map<WCGeometricLine*,WCEventController*>::iterator lineIter;
	for (lineIter = this->_lineMap.begin(); lineIter != this->_lineMap.end(); lineIter++) {
		//Render the lines
		(*lineIter).first->Render(0, WCColor::Default(), this->_document->Scene()->ActiveCamera()->Zoom());
	}
	//Draw all the curves
	std::map<WCNurbsCurve*,WCEventController*>::iterator curveIter;
	for (curveIter = this->_curveMap.begin(); curveIter != this->_curveMap.end(); curveIter++) {
		//Render the curves
		(*curveIter).first->Render(0, WCColor::Default(), this->_document->Scene()->ActiveCamera()->Zoom());
	}
	
	//Draw the contraints
	if (this->_drawConstraints) {
		std::list<WCSketchConstraint*>::iterator constraintIter;
		for (constraintIter = this->_constraintList.begin(); constraintIter != this->_constraintList.end(); constraintIter++) {
			//Render the curves
			(*constraintIter)->Render(0, WCColor::Default(), this->_document->Scene()->ActiveCamera()->Zoom());
		}
	}
	//Enable depth testing
	glEnable(GL_DEPTH_TEST);
}


xercesc::DOMElement* WCSketch::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create primary element for this object
	XMLCh* xmlString = xercesc::XMLString::transcode("Sketch");
	xercesc::DOMElement* element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Include the parent element
	xercesc::DOMElement* featureElement = this->WCPartFeature::Serialize(document, dictionary);
	element->appendChild(featureElement);

	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);
	//Set reference plane attribute
	WCSerializeableObject::AddGUIDAttrib(element, "reference", this->_refPlane, dictionary);

	//Loop through all sketch features and add them
	xmlString = xercesc::XMLString::transcode("Features");
	xercesc::DOMElement *features = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	element->appendChild(features);
	xercesc::DOMElement *featureElem;
	std::list<WCSketchFeature*>::iterator featureIter;
	WPUInt index = 0;
	for (featureIter = this->_featureList.begin(); featureIter != this->_featureList.end(); featureIter++) {
		//Get the feature element
		featureElem = (*featureIter)->Serialize(document, dictionary);
		//Add the index of the feature
		WCSerializeableObject::AddFloatAttrib(featureElem, "index", index++);
		//Add the feature element as a child to this one
		if (featureElem != NULL) features->appendChild(featureElem);
	}

	//Return the element
	return element;
}


/***********************************************~***************************************************/


WCAction* WCSketch::ActionCreate(WCFeature *creator, const std::string sketchName, WCPartPlane *refPlane) {
	//Create new action for sketch creation
	return new WCActionSketchCreate(creator, sketchName, refPlane);
}


WCAction* WCSketch::ActionMarkConstruction(WCSketch *sketch, std::list<WCSketchFeature*> features) {
	//Create new action to mark sketch features as construction elements
	return new WCActionSketchMarkConstruction(sketch, features);
}


WCAction* WCSketch::ActionMarkNonConstruction(WCSketch *sketch, std::list<WCSketchFeature*> features) {
	//Create new action to mark sketch features as not construction elements
	return new WCActionSketchMarkNonConstruction(sketch, features);
}


WCAction* WCSketch::ActionDelete(WCSketch *sketch, std::list<WCSketchFeature*> features) {
	//Create new action to delete selected objects from the sketch
	return new WCActionSketchDelete(sketch, features);
}

	
/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCSketch &sketch) {
	return out;
}


/***********************************************~***************************************************/