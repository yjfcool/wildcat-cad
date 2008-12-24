/*** Included Header Files ***/
#include <feature_line.h>
#include <document.h>
#include <feature_point.h>
#include <tree_view.h>
#include <line_layer.h>
#include <feature_line_action.h>
#include <feature_line_controller.h>



/***********************************************~***************************************************/


WCFeatureLine::WCFeatureLine(WCFeature *creator, const std::string &name, WCFeaturePoint *p0, WCFeaturePoint *p1) : 
	::WCFeature(creator, name), _base(NULL), _p0(p0), _p1(p1) {
	//Make sure p0 and p1 are not null
	if ((p0 == NULL) || (p1 == NULL)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCFeatureLine::WCFeatureLine - NULL points passed.\n");
		//throw error
		return;
	}
	//Create the base line
	this->_base = new WCGeometricLine(p0->Base(), p1->Base());
	this->_base->Retain(*this);
	//Retain both p0 and p1
	this->_p0->Retain(*this);
	this->_p1->Retain(*this);
	//Check feature name
	if (this->_name == "") this->_name = this->_document->GenerateFeatureName(this);
	//Create event handler
	this->_controller = new WCFeatureLineController(this);
	//Create tree element and add into the tree (beneath the creator)
	this->_treeElement = new WCTreeElement(this->_document->TreeView(), this->_name, this->_controller, NULL);
	//Add p0 and p1 as a children in the tree view
	this->_treeElement->AddLastChild(this->_p0->TreeElement());
	this->_treeElement->AddLastChild(this->_p1->TreeElement());
	//Mark as closed
	this->_treeElement->IsOpen(false);
	//Add to the creator	
	this->_creator->TreeElement()->AddLastChild(this->_treeElement);
	//Add into document
	this->_document->AddFeature(this);
	//Add to the lines layer
	this->_document->LinesLayer()->AddLine(this->_base);
}


WCFeatureLine::~WCFeatureLine() {
	//Remove the line from the layer
	this->_document->LinesLayer()->RemoveLine(this->_base);
	//Remove the line from the document
	this->_document->RemoveFeature(this);
	//Release and delete the base
	this->_base->Release(*this);
	delete this->_base;
	//Release the points
	this->_p0->Release(*this);
	this->_p1->Release(*this);
}


void WCFeatureLine::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
	//Just need to mark line layer as dirty
	this->_document->LinesLayer()->MarkDirty();
//	CLOGGER_WARN(WCLogManager::RootLogger(), "WCFeatureLine::ReceiveNotice - Not yet implemented.\n");
}


bool WCFeatureLine::Regenerate(void) {
	return false;
}

	
xercesc::DOMElement* WCFeatureLine::Serialize(xercesc::DOMDocument *document) {
	return NULL; 
}


WCFeatureLine* WCFeatureLine::Deserialize(xercesc::DOMElement* obj) {
	return NULL;
}


bool WCFeatureLine::Validate(xercesc::DOMElement* obj) {
	return false;
}

/*	
WCVisualObject* WCFeatureLine::HitTest(const WCRay &ray, const WPFloat tolerance) {
	return NULL; 
}


void WCFeatureLine::ApplyTransform(const WCMatrix4 &transform) {
}


void WCFeatureLine::ApplyTranslation(const WCVector4 &translation) {
}
*/

void WCFeatureLine::Render(const GLuint defaultProg, const WCColor color) {
	//Make sure is visible
	if (!this->_isVisible) return;
	this->_base->Render(defaultProg, color);
}


/***********************************************~***************************************************/


WCFeatureLineAction* WCFeatureLine::ActionCreate(WCFeature *creator, const std::string lineName, WCFeaturePoint *p0, WCFeaturePoint *p1) {
	WCFeatureLineAction* action;
	action = new WCFeatureLineAction(creator, lineName, p0, p1);
	return action;
}


//WCFeatureLineAction* WCFeatureLine::ActionModify() {
//}


WCFeatureLineAction* WCFeatureLine::ActionDelete(WCFeatureLine *line) {
	return NULL;
}

	
/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCFeatureLine &line) {
	out << "FeatureLine( " << &line << ")\n";
	return out;
}


/***********************************************~***************************************************/


