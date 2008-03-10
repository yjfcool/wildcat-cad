/*** Included Header Files ***/
#include "feature_point.h"
#include "document.h"
#include "tree_view.h"
#include "point_layer.h"
#include "feature_point_action.h"
#include "feature_point_controller.h"



/***********************************************~***************************************************/


WCFeaturePoint::WCFeaturePoint(WCFeature *creator, const std::string &name, const WPFloat x, const WPFloat y, const WPFloat z) : 
	::WCFeature(creator, name), _base(NULL) {
	//Create the base point
	this->_base = new WCGeometricPoint(x, y, z);
	//Retain the base
	this->_base->Retain(*this);
	//Check feature name
	if (this->_name == "") this->_name = this->_document->GenerateFeatureName(this);
	//Create event handler
	this->_controller = new WCFeaturePointController(this);
	//Create tree element and add into the tree (beneath the creator)
	this->_treeElement = new WCTreeElement(this->_document->TreeView(), this->_name, this->_controller, NULL);
	this->_creator->TreeElement()->AddLastChild(this->_treeElement);
	//Add into document
	this->_document->AddFeature(this, true);
	//Add to the points layer
	this->_document->PointsLayer()->AddPoint(this->_base);
}


WCFeaturePoint::~WCFeaturePoint() {
	//Remove the point from the layer
	this->_document->PointsLayer()->RemovePoint(this->_base);
	//Remove feature from document
	this->_document->RemoveFeature(this);
	//Release the base
	this->_base->Release(*this);
	//Delete the base
	delete this->_base;
}


void WCFeaturePoint::Set(const WCVector4 &position) {
	//Update the geometric point
	this->_base->Set(position);
	//Make sure all dependent objects know about it
	this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);
}


void WCFeaturePoint::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
	//Mark points layer as dirty
	this->_document->PointsLayer()->MarkDirty();
//	CLOGGER_WARN(WCLogManager::RootLogger(), "WCFeaturePoint::ReceiveNotice - Not yet implemented.\n");
}


bool WCFeaturePoint::Regenerate(void) {
	return false;
}

	
xercesc::DOMElement* WCFeaturePoint::Serialize(xercesc::DOMDocument *document) {
	return NULL; 
}


WCFeaturePoint* WCFeaturePoint::Deserialize(xercesc::DOMElement* obj) {
	return NULL;
}


bool WCFeaturePoint::Validate(xercesc::DOMElement* obj) {
	return false;
}

/*
WCVisualObject* WCFeaturePoint::HitTest(const WCRay &ray, const WPFloat tolerance) {
	return NULL; 
}


void WCFeaturePoint::ApplyTransform(const WCMatrix4 &transform) {
}


void WCFeaturePoint::ApplyTranslation(const WCVector4 &translation) {
}
*/

void WCFeaturePoint::Render(const GLuint defaultProg, const WCColor color) {
	//Make sure is visible
	if (!this->_isVisible) return;
	this->_base->Render(defaultProg, color);
}


/***********************************************~***************************************************/


WCFeaturePointAction* WCFeaturePoint::ActionCreate(WCFeature *creator, const std::string pointName, 
	const WPFloat x, const WPFloat y, const WPFloat z) {
	WCFeaturePointAction* action;
	action = new WCFeaturePointAction(creator, "", x, y, z);
	return action;
}


//WCFeaturePointAction* WCFeaturePoint::ActionModify() {
//}


WCFeaturePointAction* WCFeaturePoint::ActionDelete(WCFeaturePoint *point) {
	return NULL;
}

	
/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCFeaturePoint &point) {
	out << "FeaturePoint( " << &point << ")\n";
	return out;
}


/***********************************************~***************************************************/


