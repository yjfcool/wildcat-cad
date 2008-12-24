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
#include <Constraint/constraint_distance.h>
#include <Sketcher/sketch.h>
#include <Sketcher/sketch_point.h>
#include <Sketcher/sketch_line.h>
#include <Kernel/document.h>
#include <PartDesign/part_plane.h>


/***********************************************~***************************************************/


void WCConstraintDistance::PointPoint(void) {
	//Cast features to points
	WCSketchPoint *p0 = dynamic_cast<WCSketchPoint*>(this->_featureA);
	WCSketchPoint *p1 = dynamic_cast<WCSketchPoint*>(this->_featureB);
	//Setup parameters
	WCVector4 pos0 = p0->Base()->Data();
	WCVector4 pos1 = p1->Base()->Data();

	//Determine distance based on type
	if (this->_measureType == WCMeasureType::Absolute()) {
		this->_distance = pos0.Distance( pos1 );
	}
	else {
		//Project points back onto sketch plane
		WCVector4 invP0 = this->_sketch->ReferencePlane()->InverseTransformMatrix() * pos0;
		WCVector4 invP1 = this->_sketch->ReferencePlane()->InverseTransformMatrix() * pos1;
		//Determine horizontal distance
		if (this->_measureType == WCMeasureType::Horizontal()) this->_distance = invP0.I() - invP1.I();
		//Determine vertical distance
		else this->_distance = invP0.J() - invP1.J();
		//Get absolute value of value
		if (this->_distance < 0.0) this->_distance *= -1.0;
	}
	
	//Set the measure label
	std::string label = this->_document->LengthUnit()->DisplayString(this->_distance, 4);

	//Check to see if old measure needs to be updated
	if (this->_measure != NULL) {
		//Set the measure label
		this->_measure->Text()->Text(label);
		//Set the measure position
		this->_measure->Points(pos0, pos1);
	}
	//Or create the rendering measure
	else {
		this->_measure = new WCConstraintMeasureTwoPoint(this, label, this->_measureType, pos0, pos1,
														 this->_sketch->ReferencePlane()->InverseTransformMatrix(),
														 this->_sketch->ReferencePlane()->TransformMatrix(), 0.25 * this->_distance, 0.5);
	}
}


void WCConstraintDistance::PointCurve(void) {
	//Cast features to point and curve
	WCGeometricPoint *pt = dynamic_cast<WCGeometricPoint*>(this->_featureA->Base());
	WCGeometricCurve *curve = dynamic_cast<WCGeometricCurve*>(this->_featureB->Base());
	//Setup parameters
	WCVector4 pos0 = pt->Data();
	//Determine pos1 based on positionB
	WCVector4 pos1;
	if (this->_posB == WCMeasurePosition::Begin()) pos1 = curve->Evaluate(0.0);
	else if (this->_posB == WCMeasurePosition::End()) pos1 = curve->Evaluate(1.0);
	//Project pt onto curve
	else pos1 = curve->PointInversion(pos0).first;

	//Determine distance based on type (absolute case)
	if (this->_measureType == WCMeasureType::Absolute()) {
		this->_distance = pos0.Distance( pos1 );
	}
	else {
		//Project points back onto sketch plane
		WCVector4 invP0 = this->_sketch->ReferencePlane()->InverseTransformMatrix() * pos0;
		WCVector4 invP1 = this->_sketch->ReferencePlane()->InverseTransformMatrix() * pos1;
		//Determine horizontal distance
		if (this->_measureType == WCMeasureType::Horizontal()) this->_distance = invP0.I() - invP1.I();
		//Determine vertical distance
		else this->_distance = invP0.J() - invP1.J();
		//Get absolute value of value
		if (this->_distance < 0.0) this->_distance *= -1.0;
	}

	//Set the measure label
	std::string label = this->_document->LengthUnit()->DisplayString(this->_distance, 4);
	//Check to see if old measure needs to be updated
	if (this->_measure != NULL) {
		//Set the measure label
		this->_measure->Text()->Text(label);
		//Set the measure position
		this->_measure->Points(pos0, pos1);
	}
	//Or create the rendering measure
	else {
		this->_measure = new WCConstraintMeasureTwoPoint(this, label, this->_measureType, pos0, pos1,
														 this->_sketch->ReferencePlane()->InverseTransformMatrix(),
														 this->_sketch->ReferencePlane()->TransformMatrix(), 0.25 * this->_distance, 0.5);
	}
}


void WCConstraintDistance::CurveCurve(void) {
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConstraintDistance::CurveCurve - Not yet implemented.");
}


void WCConstraintDistance::Initialize(void) {
	//Create the distance controller
	this->_controller = new WCConstraintDistanceController(this);
	//Make sure base is not null
	if (this->_controller == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConstraintDistance::Initialize - NULL controller.");
		//throw error
		return;
	}
	//Retain the features
	this->_featureA->Retain(*this);
	this->_featureB->Retain(*this);
	//Set the color
	this->_color = WCSketchFeature::ConstraintColor;
	//Check feature name
	if (this->_name == "") this->_name = this->_sketch->GenerateFeatureName(this);
	//Add into sketch
	if (!this->_sketch->AddConstraint(this)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConstraintDistance::Initialize - Problem adding constraint to sketch.");
		//Should delete base
		//throw error
		return;
	}
	//Create tree element and add into the tree (beneath the sketch features element)
	WSTexture* constraintIcon = this->_document->Scene()->TextureManager()->TextureFromName("constraint32");
	this->_treeElement = new WCTreeElement(	this->_sketch->Document()->TreeView(), this->_name, this->_controller, constraintIcon);
	this->_sketch->ConstraintsTreeElement()->AddLastChild(this->_treeElement);
}


/***********************************************~***************************************************/


/*** Point Point Constructor ***/
WCConstraintDistance::WCConstraintDistance(WCSketch *sketch, const std::string &name, const WCMeasureType &type,
	WCSketchPoint *p0, WCSketchPoint *p1) : ::WCSketchConstraint(sketch, name), _type(CONSTRAINTDISTANCE_POINTPOINT),
	_measureType(type), _featureA(p0), _featureB(p1),
	_posA(WCMeasurePosition::Closest()), _posB(WCMeasurePosition::Closest()), _distance(0.0), _measure(NULL) {
	//Perform remaining initialization
	this->Initialize();
	//Setup measure
	this->PointPoint();
	//Inject constraints into sketch planner
	this->InjectConstraints(this->_sketch->ConstraintPlanner());
}


/*** Point Curve Constructor ***/
WCConstraintDistance::WCConstraintDistance(WCSketch *sketch, const std::string &name, const WCMeasureType &type,
	WCSketchPoint *pt, WCSketchFeature *curve, const WCMeasurePosition &pos) : ::WCSketchConstraint(sketch, name),
	_type(CONSTRAINTDISTANCE_POINTCURVE), _measureType(type), _featureA(pt), _featureB(curve),
	_posA(WCMeasurePosition::Closest()), _posB(pos), _distance(0.0), _measure(NULL) {
	//Perform remaining initialization
	this->Initialize();
	//Setup measure
	this->PointCurve();
	//Inject constraints into sketch planner
	this->InjectConstraints(this->_sketch->ConstraintPlanner());
}


/*** Curve Curve Constructor ***/
WCConstraintDistance::WCConstraintDistance(WCSketch *sketch, const std::string &name, const WCMeasureType &type,
	WCSketchFeature *curveA, const WCMeasurePosition &posA, WCSketchFeature *curveB, const WCMeasurePosition &posB) :
	::WCSketchConstraint(sketch, name), _type(CONSTRAINTDISTANCE_CURVECURVE), _measureType(type), _featureA(curveA), _featureB(curveB),
	_posA(posA), _posB(posB), _distance(0.0), _measure(NULL) {
	//Perform remaining initialization
	this->Initialize();
	//Setup measure
	this->CurveCurve();
	//Inject constraints into sketch planner
	this->InjectConstraints(this->_sketch->ConstraintPlanner());
}


/*** Persistance Constructor ***/
WCConstraintDistance::WCConstraintDistance(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCSketchConstraint( WCSerializeableObject::ElementFromName(element,"SketchConstraint"), dictionary ),
	_type(CONSTRAINTDISTANCE_POINTPOINT), _measureType(WCMeasureType::Absolute()), _featureA(NULL), _featureB(NULL),
	_posA(WCMeasurePosition::Closest()), _posB(WCMeasurePosition::Closest()), _distance(0.0), _measure(NULL) {
	//Make sure element if not null
	if (element == NULL) return;
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

	//Setup type
	this->_type = (WPUInt)WCSerializeableObject::GetFloatAttrib(element, "type");
	//Setup distance
	this->_distance = WCSerializeableObject::GetFloatAttrib(element, "distance");
	//Setup feature A
	this->_featureA = (WCSketchFeature*)WCSerializeableObject::GetGUIDAttrib(element, "featureA", dictionary);
	//Setup feature B
	this->_featureB = (WCSketchFeature*)WCSerializeableObject::GetGUIDAttrib(element, "featureB", dictionary);
	//Setup measure type
	this->_measureType.FromElement( WCSerializeableObject::ElementFromName(element, "MeasureType") );
	//Setup pos A
	this->_posA.FromElement( WCSerializeableObject::ElementFromName(element,"PosA") );
	//Setup pos B
	this->_posB.FromElement( WCSerializeableObject::ElementFromName(element,"PosB") );

	//Finish initialization
	switch(this->_type) {
		case CONSTRAINTDISTANCE_POINTPOINT: this->PointPoint(); break;
		case CONSTRAINTDISTANCE_POINTCURVE: this->PointCurve(); break;
		case CONSTRAINTDISTANCE_CURVECURVE: this->CurveCurve(); break;
	}
}


WCConstraintDistance::~WCConstraintDistance() {
	//Release the points
	this->_featureA->Release(*this);
	this->_featureB->Release(*this);
	//Remove from the sketch
	if (!this->_sketch->RemoveConstraint(this)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConstraintDistance::~WCConstraintDistance - Problem removing constraint from sketch.");	
	}
	//Check to see if old measure needs to be deleted
	if (this->_measure != NULL) delete this->_measure;
}


void  WCConstraintDistance::InjectConstraints(WCConstraintPlanner *planner) {
	//Inject distance constraint based on type of distance
	if (this->_type == CONSTRAINTDISTANCE_POINTPOINT) {
		WCConstraintNode *p0Node = this->_featureA->ConstraintNodes().front();
		WCConstraintNode *p1Node = this->_featureB->ConstraintNodes().front();
		planner->AddEdge(this, p0Node, p1Node, WCConstraintEdgeType::AbsoluteDistance(), 1, WCVector4(this->_distance));
	}
}


bool WCConstraintDistance::DependencyCheck(WCSketchFeature *feature) {
	//Return true if feature equals A or B, false otherwise
	return (feature == this->_featureA) || (feature == this->_featureB);
}


void WCConstraintDistance::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
	//Check type of distance
	switch(this->_type) {
		//Reinitialize based on type
		case CONSTRAINTDISTANCE_POINTPOINT: this->PointPoint(); break;
		case CONSTRAINTDISTANCE_POINTCURVE: this->PointCurve(); break;
		case CONSTRAINTDISTANCE_CURVECURVE: this->CurveCurve(); break;
	}
}


bool WCConstraintDistance::Regenerate(void) {
	//Not yet implemented
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConstraintDistance::Regenerate - Not yet implemented.");
	return false;
}


std::list<WCObject*> WCConstraintDistance::DeletionDependencies(void) {
	return std::list<WCObject*>();
}


void WCConstraintDistance::OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) {
	//Set the color
	this->_color.Set(WCSketchFeature::SelectedColor);
	this->_measure->Text()->Color(WCSketchFeature::SelectedColor);
	//Mark this as selected
	this->_isSelected = true;
}


void WCConstraintDistance::OnDeselection(const bool fromManager) {
	//Set the color
	this->_color.Set(WCSketchFeature::ConstraintColor);
	this->_measure->Text()->Color(WCSketchFeature::DefaultTextColor);
	//Mark this as not selected
	this->_isSelected = false; 
}


void WCConstraintDistance::Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom) {
	//Make sure is visible
	if (!this->_isVisible) return;
	//Check the color (see if in selection mode)
	if (color == WCColor::Default()) {
		//Just call measure to render
		this->_measure->Render(this->_color);
	}
	//Not in selection mode
	else {
		//Call to measure in selection mode
		this->_measure->Render(color, true);
	}
}

	
xercesc::DOMElement* WCConstraintDistance::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create primary element for this object
	XMLCh* xmlString = xercesc::XMLString::transcode("ConstraintDistance");
	xercesc::DOMElement* element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Include the parent element
	xercesc::DOMElement* featureElement = this->WCSketchConstraint::Serialize(document, dictionary);
	element->appendChild(featureElement);
	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);
	dictionary->InsertGUID(guid, this);

	//Add type attribute
	WCSerializeableObject::AddFloatAttrib(element, "type", this->_type);
	//Add distance attribute
	WCSerializeableObject::AddFloatAttrib(element, "distance", this->_distance);
	//Add line A attribute
	WCSerializeableObject::AddGUIDAttrib(element, "featureA", this->_featureA, dictionary);
	//Add line B attribute
	WCSerializeableObject::AddGUIDAttrib(element, "featureB", this->_featureB, dictionary);

	//Add type
	xmlString = xercesc::XMLString::transcode("MeasureType");
	xercesc::DOMElement* typeElement = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	this->_posA.ToElement(typeElement);
	element->appendChild(typeElement);
	//Add pos A
	xmlString = xercesc::XMLString::transcode("PosA");
	xercesc::DOMElement* posElement = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	this->_posA.ToElement(posElement);
	element->appendChild(posElement);
	//Add pos B
	xmlString = xercesc::XMLString::transcode("PosB");
	posElement = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	this->_posB.ToElement(posElement);
	element->appendChild(posElement);
	
	//Return the new element
	return element;
}



/***********************************************~***************************************************/


WCDrawingMode* WCConstraintDistance::ModeEdit(WCConstraintDistance *constraint) {
	//Create a new editing mode
	return new WCModeConstraintDistanceEdit(constraint);
}


WCActionConstraintDistanceCreate* WCConstraintDistance::ActionCreate(WCSketch *sketch, const std::string &constraintName,
	const WCMeasureType &type, WCSketchFeature *first, WCSketchFeature *second, 
	const WCMeasurePosition &posA, const WCMeasurePosition &posB) {
	//Create new action to create constraint
	return new WCActionConstraintDistanceCreate(sketch, constraintName, type, first, second, posA, posB);
}


WCActionConstraintDistanceMove* WCConstraintDistance::ActionMove(WCConstraintDistance *constraint, const WPFloat &offset, 
	const WPFloat &labelOffset) {
	//Create new action to move the constraint
	return new WCActionConstraintDistanceMove(constraint, offset, labelOffset);
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCConstraintDistance &constraint) {
	std::cout << "Constraint Distance(" << &constraint << ")\n";
	return out;
}


/***********************************************~***************************************************/

