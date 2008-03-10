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
#include "Constraint/constraint_coincidence.h"
#include "Sketcher/sketch.h"
#include "Sketcher/sketch_point.h"
#include "Kernel/document.h"
#include "PartDesign/part_plane.h"


/***********************************************~***************************************************/


void WCConstraintCoincidence::PointPoint(void) {
	//Cast features to points
	WCSketchPoint *p0 = dynamic_cast<WCSketchPoint*>(this->_featureA);
	WCVector4 pt0 = p0->Base()->Data();
	WCSketchPoint *p1 = dynamic_cast<WCSketchPoint*>(this->_featureB);
	
	//Make sure points are within tolerance
	if (pt0.Distance( p1->Base()->Data() ) > 0.001) {
//		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConstraintCoincidence::PointPoint - Points too far apart.");
//		return;		
	}
	//Set data to pt0
	this->_data[0] = pt0.I();
	this->_data[1] = pt0.J();
	this->_data[2] = pt0.K();
}


void WCConstraintCoincidence::PointCurve(void) {
	//Cast both features
	WCSketchPoint *p0 = dynamic_cast<WCSketchPoint*>(this->_featureA);
	WCVector4 pt = p0->Base()->Data(), ptC;
	WCGeometricCurve *curve = dynamic_cast<WCGeometricCurve*>( this->_featureB->Base() );

	//Get position on curve
	if (this->_posB == WCMeasurePosition::Begin()) ptC = curve->Evaluate(0.0);
	else if (this->_posB == WCMeasurePosition::End()) ptC = curve->Evaluate(1.0);
	//Need to determine closest point
	else {
		std::cout << "WCConstraintCoincidence::PointCurve - Need to determine closest point\n";
	}
	
	//If distance from pt to ptC > tol there is a problem
	if (pt.Distance(ptC) > 0.001) {
//		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConstraintCoincidence::CurveCurve - Points too far apart.");
//		return;
	}
	//Set data to ptA
	this->_data[0] = pt.I();
	this->_data[1] = pt.J();
	this->_data[2] = pt.K();
}


void WCConstraintCoincidence::CurveCurve(void) {
	//Cast both to curves
	WCGeometricCurve *curveA = dynamic_cast<WCGeometricCurve*>( this->_featureA->Base() );
	WCGeometricCurve *curveB = dynamic_cast<WCGeometricCurve*>( this->_featureB->Base() );

	WCVector4 ptA, ptB;
	//Get position on curve A
	if (this->_posA == WCMeasurePosition::Begin()) ptA = curveA->Evaluate(0.0);
	else if (this->_posA == WCMeasurePosition::End()) ptA = curveA->Evaluate(1.0);
	//Need to determine closest point
	else {
		std::cout << "WCConstraintCoincidence::CurveCurve - Need to determine closest point\n";
	}

	//Get position on curve B
	if (this->_posB == WCMeasurePosition::Begin()) ptB = curveB->Evaluate(0.0);
	else if (this->_posB == WCMeasurePosition::End()) ptB = curveB->Evaluate(1.0);
	//Need to determine closest point
	else {
		std::cout << "WCConstraintCoincidence::CurveCurve - Need to determine closest point\n";
	}
	
	//If distance from ptA to ptB > tol there is a problem
	if (ptA.Distance(ptB) > 0.001) {
//		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConstraintCoincidence::CurveCurve - Points too far apart.");
//		return;
	}
	//Set data to ptA
	this->_data[0] = ptA.I();
	this->_data[1] = ptA.J();
	this->_data[2] = ptA.K();
}


void WCConstraintCoincidence::Initialize(void) {
	//Create the length controller
	this->_controller = new WCConstraintCoincidenceController(this);
	//Make sure controller is not null
	if (this->_controller == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConstraintLength::WCConstraintLength - NULL controller.");
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
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConstraintCoincidence::Initialize - Problem adding constraint to sketch.");
		//Should delete base
		//throw error
		return;
	}
	//Create tree element and add into the tree (beneath the sketch features element)
	WSTexture* contactIcon = this->_document->Scene()->TextureManager()->TextureFromName("contact32");
	this->_treeElement = new WCTreeElement(	this->_sketch->Document()->TreeView(), this->_name, this->_controller, contactIcon);
	this->_sketch->ConstraintsTreeElement()->AddLastChild(this->_treeElement);
}


/***********************************************~***************************************************/


WCConstraintCoincidence::WCConstraintCoincidence(WCSketch *sketch, const std::string &name, WCSketchFeature *featureA,
	const WCMeasurePosition &posA, WCSketchFeature* featureB, const WCMeasurePosition &posB) : 
	::WCSketchConstraint(sketch, name), _type(CONSTRAINTCOINCIDENCE_POINTPOINT),
	_featureA(featureA), _featureB(featureB), _posA(posA), _posB(posB) {
	//Cast features to points
	WCSketchPoint *p0 = dynamic_cast<WCSketchPoint*>(this->_featureA);
	WCSketchPoint *p1 = dynamic_cast<WCSketchPoint*>(this->_featureB);
	//Point-point case
	if ((p0 != NULL) && (p1 != NULL)) {
		//Initialize some
		this->PointPoint();
	}
	//Curve-curve case
	else if ((p0 == NULL) && (p1 == NULL)) {
		//Initialize some
		this->_type = CONSTRAINTCOINCIDENCE_CURVECURVE;
		this->CurveCurve();
	}
	//Point-curve case
	else {
		//Initialize some
		this->_type = CONSTRAINTCOINCIDENCE_POINTCURVE;
		//Need to swap if p1 is a point
		if (p1 != NULL) {
			std::swap(this->_featureA, this->_featureB);
			std::swap(this->_posA, this->_posB);
		}
		//Get rendering data
		this->PointCurve();
	}
	//Perform remaining initialization
	this->Initialize();
}


WCConstraintCoincidence::WCConstraintCoincidence(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCSketchConstraint( WCSerializeableObject::ElementFromName(element,"SketchConstraint"), dictionary ),
	_type(CONSTRAINTCOINCIDENCE_POINTPOINT), _featureA(NULL), _featureB(NULL),
	_posA(WCMeasurePosition::Closest()), _posB(WCMeasurePosition::Closest()) {
	//Make sure element if not null
	if (element == NULL) return;
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
		dictionary->InsertGUID(guid, this);

	//Setup feature A
	this->_featureA = (WCSketchFeature*)WCSerializeableObject::GetGUIDAttrib(element, "featureA", dictionary);
	//Setup feature B
	this->_featureB = (WCSketchFeature*)WCSerializeableObject::GetGUIDAttrib(element, "featureB", dictionary);
	//Setup pos A
	this->_posA.FromElement( WCSerializeableObject::ElementFromName(element,"PosA") );
	//Setup pos B
	this->_posB.FromElement( WCSerializeableObject::ElementFromName(element,"PosB") );

	//Perform remaining initialization
	this->Initialize();
}


WCConstraintCoincidence::~WCConstraintCoincidence() {
	//Release the features
	this->_featureA->Release(*this);
	this->_featureB->Release(*this);
	//Remove from the sketch
	if (!this->_sketch->RemoveConstraint(this)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConstraintCoincidence::~WCConstraintCoincidence - Problem removing constraint from sketch.");	
	}
}


void  WCConstraintCoincidence::InjectConstraints(WCConstraintPlanner *planner) {
}


void WCConstraintCoincidence::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
	//Update based on type
	switch(this->_type) {
		case CONSTRAINTCOINCIDENCE_POINTPOINT: this->PointPoint(); break;
		case CONSTRAINTCOINCIDENCE_POINTCURVE: this->PointCurve(); break;
		case CONSTRAINTCOINCIDENCE_CURVECURVE: this->CurveCurve(); break;
	}
}


bool WCConstraintCoincidence::Regenerate(void) {
	//Not yet implemented
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCConstraintCoincidence::Regenerate - Not yet implemented.");
	return false;
}


std::list<WCObject*> WCConstraintCoincidence::DeletionDependencies(void) {
	//No deletion dependencies
	return std::list<WCObject*>();
}


void WCConstraintCoincidence::OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) {
	//Nothing for now
}


void WCConstraintCoincidence::OnDeselection(const bool fromManager) {
	//Nothing for now
}


void WCConstraintCoincidence::Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom) {
	//Do a visibility check
	if (!this->_isVisible) return;
	//For now, don't allow click selection
	if (color != WCColor::Default()) return;

	//Enable state
	glDisable(GL_DEPTH_TEST);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(4.0);
	glVertexPointer(3, GL_FLOAT, 0, this->_data);

	//Set the color
	WCSketchFeature::ConstraintColor.Enable();
	//Draw rounded point
	glDrawArrays(GL_POINTS, 0, 1);

	//Clean up state
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_POINT_SMOOTH);
	glDisableClientState(GL_VERTEX_ARRAY);
}

	
xercesc::DOMElement* WCConstraintCoincidence::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create primary element for this object
	XMLCh* xmlString = xercesc::XMLString::transcode("ConstraintCoincidence");
	xercesc::DOMElement* element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Include the parent element
	xercesc::DOMElement* featureElement = this->WCSketchConstraint::Serialize(document, dictionary);
	element->appendChild(featureElement);
	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);
	dictionary->InsertGUID(guid, this);

	//Add line A attribute
	WCSerializeableObject::AddGUIDAttrib(element, "featureA", this->_featureA, dictionary);
	//Add line B attribute
	WCSerializeableObject::AddGUIDAttrib(element, "featureB", this->_featureB, dictionary);

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


WCActionConstraintCoincidenceCreate* WCConstraintCoincidence::ActionCreate(WCSketch *sketch, const std::string &constraintName,
	WCSketchFeature *featureA, const WCMeasurePosition &posA, WCSketchFeature *featureB, const WCMeasurePosition &posB) {
	//Create new action to create constraint
	return new WCActionConstraintCoincidenceCreate(sketch, constraintName, featureA, posA, featureB, posB);
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCConstraintCoincidence &constraint) {
	std::cout << "Constraint Coincidence(" << &constraint << ")\n";
	return out;
}


/***********************************************~***************************************************/

