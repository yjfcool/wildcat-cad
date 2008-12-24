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
#include <Sketcher/sketch_conic_twopoint.h>
#include <Sketcher/sketch.h>
#include <Kernel/document.h>
#include <PartDesign/part_plane.h>


/***********************************************~***************************************************/


void WCSketchConicTwoPoint::Initialize(void) {
}


/***********************************************~***************************************************/


WCSketchConicTwoPoint::WCSketchConicTwoPoint(WCSketch *sketch, const std::string &name,	const WCVector4 &begin, 
	const WCVector4 &beginTangent, const WCVector4 &end, const WCVector4 &endTangent, const WCVector4 &fifthPoint) : ::WCSketchFeature(sketch),
	_begin(begin), _beginTangent(beginTangent), _end(end), _endTangent(endTangent), _fifthPoint(fifthPoint), 
	_curve(NULL), _beginPoint(NULL), _endPoint(NULL) {
	//Validate sketch and center
	if (this->_sketch == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchConicTwoPoint::WCSketchConicTwoPoint - NULL Sketch passed.\n");
		//throw error
		return;
	}
	//Create the arc
	WCVector4 p0( this->_sketch->ReferencePlane()->TransformMatrix() * this->_begin );
	WCVector4 t0( this->_sketch->ReferencePlane()->TransformMatrix() * this->_beginTangent );
	WCVector4 p2( this->_sketch->ReferencePlane()->TransformMatrix() *  this->_end );
	WCVector4 t2( this->_sketch->ReferencePlane()->TransformMatrix() *  this->_endTangent );
	WCVector4 p( this->_sketch->ReferencePlane()->TransformMatrix() *  this->_fifthPoint );		
	this->_curve = WCNurbsCurve::Conic(this->_document->Scene()->GeometryContext(), p0, t0, p2, t2, p);
	
	//Create the point controller
	this->_controller = new WCSketchConicController(this);
	//Make sure base is not null
	if ((this->_curve == NULL) || (this->_controller == NULL)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchConicTwoPoint::WCSketchConicTwoPoint - Not able to create conic or controller.");
		//throw error
		return;
	}
	
	//Check feature name
	if (this->_name == "") this->_name = this->_sketch->GenerateFeatureName(this);
	//Set the base color and thickness
	this->_curve->Color(WCSketchFeature::DefaultColor);
	this->_curve->Thickness(WCSketchFeature::LineThickness);
	//Retain the conic
	this->_curve->Retain(*this);
	//Add into sketch
	if ((!this->_sketch->AddFeature(this)) || (!this->_sketch->AddCurve(this->_curve, this->_controller))) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchConicTwoPoint::WCSketchConicTwoPoint - Problem adding feature or arc to sketch.");
		//Should delete base
		//throw error
		return;
	}
	//Create tree element and add into the tree (beneath the sketch features element)
	WSTexture *icon = this->_document->Scene()->TextureManager()->TextureFromName("conic32");
	this->_treeElement = new WCTreeElement(	this->_sketch->Document()->TreeView(), this->_name, this->_controller, icon);
	this->_sketch->FeaturesTreeElement()->AddLastChild(this->_treeElement);
}


WCSketchConicTwoPoint::WCSketchConicTwoPoint(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCSketchFeature( WCSerializeableObject::ElementFromName(element,"SketchFeature"), dictionary),
	_begin(), _beginTangent(), _end(), _endTangent(), _fifthPoint(), _curve(NULL), _beginPoint(NULL), _endPoint(NULL) {
	//Do something here
}


WCSketchConicTwoPoint::~WCSketchConicTwoPoint() {
	//Remove from the sketch
	if (!this->_sketch->RemoveFeature(this)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchConicTwoPoint::WCSketchConicTwoPoint - Problem removing feature from sketch.");	
	}
	this->_sketch->RemoveCurve(this->_curve);
	//Release the base
	this->_curve->Release(*this);
	//Delete the base and control points
	if (this->_curve->RefCount() == 0) delete this->_curve;
}

	
void WCSketchConicTwoPoint::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
}


bool WCSketchConicTwoPoint::Regenerate(void) {
	CLOGGER_WARN(WCLogManager::RootLogger(), "WCSketchConicTwoPoint::Regenerate - Not yet implemented.");
	return false;
}


void WCSketchConicTwoPoint::InjectConstraints(WCConstraintPlanner *planner) {
}


std::list<WCObject*> WCSketchConicTwoPoint::DeletionDependencies(void) {
	//Only downward dependency is the base curve
	return std::list<WCObject*>(1, this->_curve);
}


void WCSketchConicTwoPoint::IsConstruction(const bool state) {
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


void WCSketchConicTwoPoint::OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) {
	//Set the color of the base curve
	this->_curve->Color(WCSketchFeature::SelectedColor);
	//Mark this as selected
	this->_isSelected = true; 
}


void WCSketchConicTwoPoint::OnDeselection(const bool fromManager) {
	//Set the color of the base arc - depends on construction
	if (this->_isConstruction) this->_curve->Color(WCSketchFeature::ConstructionColor);
	else this->_curve->Color(WCSketchFeature::DefaultColor);
	//Mark this as not selected
	this->_isSelected = false; 
}
	

xercesc::DOMElement* WCSketchConicTwoPoint::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	CLOGGER_WARN(WCLogManager::RootLogger(), "WCSketchConicTwoPoint::Serialize - Not yet implemented.");
	return NULL;
}

	
/***********************************************~***************************************************/


WCDrawingMode* WCSketchConicTwoPoint::ModeCreate(WCSketchWorkbench *wb) {
	//Create a new drawing mode
	return new WCModeSketchConicTwoPointCreate(wb);
}


WCDrawingMode* WCSketchConicTwoPoint::ModeEdit(WCSketchConicTwoPoint *conic) {
	return NULL;
}

	
WCAction* WCSketchConicTwoPoint::ActionCreate(WCSketch *sketch, const std::string &conicName, const WCVector4 &start, 
	const WCVector4 &startTangent, const WCVector4 &end, const WCVector4 &endTangent, const WCVector4 &fifthPoint) {
	//Create action to create standard sketch arc
	return new WCActionSketchConicTwoPointCreate(sketch, conicName, start, startTangent, end, endTangent, fifthPoint);
}

	
/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCSketchConicTwoPoint &conic) {
	return out;
}


/***********************************************~***************************************************/

