#include <coordinate_system_action.h>
#include <document.h>
#include <coordinate_system.h>


/***********************************************~***************************************************/


WCCoordinateSystemAction::WCCoordinateSystemAction(WCFeature *creator, const std::string &systemName, 
	WCFeaturePoint *refPoint, const WCVector4 &xAxis, const WCVector4 &yAxis, const WCVector4 &zAxis) : ::WCAction("", creator),
	_cs(NULL), _systemName(systemName), _refPoint(refPoint), _xAxis(xAxis), _yAxis(yAxis), _zAxis(zAxis) {
	//Set to the appropriate action name
	this->_name = "Create Coordinate System";
}


WCFeature* WCCoordinateSystemAction::Execute(void) {
	//Create the coordinate system
	WCCoordinateSystem *cs = new WCCoordinateSystem(this->_creator, this->_systemName, this->_refPoint,
		this->_xAxis, this->_yAxis, this->_zAxis);
	//Make sure cs is not null
	if (cs == NULL) 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCCoordinateSystemAction::Execute - Coordinate System could not be created.");
	//Set the cs pointer and return
	this->_cs = cs;
	return this->_cs;
}


xercesc::DOMElement* WCCoordinateSystemAction::Serialize(xercesc::DOMDocument *document) {
	return NULL;
}


WCCoordinateSystemAction* WCCoordinateSystemAction::Deserialize(xercesc::DOMElement* obj) {
	return NULL;
}


bool WCCoordinateSystemAction::Validate(xercesc::DOMElement* obj) {
	return false;
}


/***********************************************~***************************************************/

