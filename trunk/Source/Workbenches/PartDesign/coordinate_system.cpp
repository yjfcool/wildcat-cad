/*** Included Header Files ***/
#include <coordinate_system.h>
#include <document.h>
#include <tree_view.h>
#include <coordinate_system_action.h>
#include <coordinate_system_controller.h>
#include <feature_point.h>


/***********************************************~***************************************************/


WCCoordinateSystem::WCCoordinateSystem(WCFeature *creator, const std::string name, WCFeaturePoint *refPoint,
	const WCVector4 &xAxis, const WCVector4 &yAxis, const WCVector4 &zAxis) : ::WCFeature(creator, name), 
	_refPoint(refPoint), _xAxis(xAxis), _yAxis(yAxis), _zAxis(zAxis), _color(COORDINATESYSTEM_COLOR) {
	//Check feature name
	if (this->_name == "") this->_name = this->_document->GenerateFeatureName(this);
	//Create event handler
	this->_controller = new WCCoordinateSystemController(this);
	//Create tree element
	this->_treeElement = new WCTreeElement(this->_document->TreeView(), this->_name, this->_controller, NULL);
	//Add tree view element
	this->_creator->TreeElement()->AddLastChild(this->_treeElement);
	
	//Create a new point if needed
	if (refPoint == NULL) {
		this->_refPoint = new WCFeaturePoint(this, "", 0.0, 0.0, 0.0);
		//Make sure point was created
		if (this->_refPoint == NULL) {
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCCoordinateSystem::WCCoordinateSystem - Not able to create reference point.");
			//throw error
			return;
		}
		//Make the point not visible
		this->_refPoint->IsVisible(false);
	}
	//Otherwise, add the refPoint as a child in the tree view
	else this->_treeElement->AddLastChild(this->_refPoint->TreeElement());
	//Retain the new point
	this->_refPoint->Retain(*this);
	
	//Close the tree element
	this->_treeElement->IsOpen(false);
	//Add the CS to the document
	this->_document->AddFeature(this, true);
	//Add as visual object into symbols layer
	this->_document->SymbolsLayer()->AddObject(this);
	//Set default visibility
	this->_isVisible = COORDINATESYSTEM_DEFAULT_VISIBILITY;
}


WCCoordinateSystem::~WCCoordinateSystem() {
	//Delete objects
	if (this->_treeElement != NULL) delete this->_treeElement;
	if (this->_controller != NULL) delete this->_controller;
	//Check the reference point
	this->_refPoint->Release(*this);
	if (this->_refPoint->RefCount() == 0) delete this->_refPoint;
}



void WCCoordinateSystem::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
	CLOGGER_WARN(WCLogManager::RootLogger(), "WCCoordinateSystem::ReceiveNotice - Not yet implemented.");
}

	
xercesc::DOMElement* WCCoordinateSystem::Serialize(xercesc::DOMDocument *document) {
	return NULL;
}


WCCoordinateSystem* WCCoordinateSystem::Deserialize(xercesc::DOMElement* obj) {
	return NULL;
}


bool WCCoordinateSystem::Validate(xercesc::DOMElement* obj) {
	return false;
}

/*
WCVisualObject* WCCoordinateSystem::HitTest(const WCRay &ray, const WPFloat tolerance) {
	return NULL;
}


void WCCoordinateSystem::ApplyTransform(const WCMatrix4 &transform) {
}


void WCCoordinateSystem::ApplyTranslation(const WCVector4 &translation) {
}
*/

void WCCoordinateSystem::Render(const GLuint defaultProg, const WCColor color) {
	//Make sure to check if is visible
	if (!this->_isVisible) return;
	WCVector4 xAxis = this->_refPoint->Data() + this->_xAxis;
	WCVector4 yAxis = this->_refPoint->Data() + this->_yAxis;
	WCVector4 zAxis = this->_refPoint->Data() + this->_zAxis;
	//Enable the color
	if (color == WCColor::Default()) this->_color.Enable();
	else color.Enable();
	
	GLfloat vData[18] = { this->_refPoint->X(), this->_refPoint->Y(), this->_refPoint->Z(), xAxis.I(), xAxis.J(), xAxis.K(),
						  this->_refPoint->X(), this->_refPoint->Y(), this->_refPoint->Z(), yAxis.I(), yAxis.J(), yAxis.K(),
						  this->_refPoint->X(), this->_refPoint->Y(), this->_refPoint->Z(), zAxis.I(), zAxis.J(), zAxis.K() };
	//Set up render state
	glLineWidth(COORDINATESYSTEM_WIDTH);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vData);
	//Draw the lines
	glDrawArrays(GL_LINES, 0, 6);
	//Clean up state
	glDisableClientState(GL_VERTEX_ARRAY);	
}


/***********************************************~***************************************************/


WCCoordinateSystemAction* WCCoordinateSystem::ActionCreate(WCFeature *creator, const std::string systemName,
 WCFeaturePoint *refPoint, const WCVector4 &xAxis, const WCVector4 &yAxis, const WCVector4 &zAxis) {
	//Create new action for coordinate system creation
	WCCoordinateSystemAction *action;
	action = new WCCoordinateSystemAction(creator, systemName, refPoint, xAxis, yAxis, zAxis);
	return action;
}


WCCoordinateSystemAction* WCCoordinateSystem::ActionDelete(WCCoordinateSystem *system) {
	return NULL;
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCCoordinateSystem &cs) {
	out << "CoordinateSystem(*: " << &cs << ", RefPoint: " << cs._refPoint << ")\n";
	out << "\tX: " << cs._xAxis << std::endl;
	out << "\tY: " << cs._yAxis << std::endl;
	out << "\tZ: " << cs._zAxis << std::endl;
	return out;
}


/***********************************************~***************************************************/


