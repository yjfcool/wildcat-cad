/*** Included Header Files ***/
#include "feature_point_action.h"
#include "feature_point.h"
#include "document.h"


/***********************************************~***************************************************/


WCFeaturePointAction::WCFeaturePointAction(WCFeature *creator, const std::string &pointName, 
	const WPFloat &xVal, const WPFloat &yVal, const WPFloat &zVal) : WCAction("", creator), 
	_point(NULL), _pointName(pointName), _xVal(xVal), _yVal(yVal), _zVal(zVal) {
	//Set the action name appropriately
	this->_name = "Create Point";
}


WCFeaturePointAction::~WCFeaturePointAction() {
}


WCFeature* WCFeaturePointAction::Execute(void) {
	//Create the point
	WCFeaturePoint *point = new WCFeaturePoint(this->_creator, this->_pointName, this->_xVal, this->_yVal, this->_zVal);
	//Make sure point is not null
	if (point == NULL)
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCFeaturePointAction::Execute Error - Point Feature could not be created.");
	//Set the pointer and return
	this->_point = point;
	return this->_point;
}


bool WCFeaturePointAction::Merge(WCAction *action) { 
	return false; 
}


bool WCFeaturePointAction::Rollback(void) {
	return false;
}


xercesc::DOMElement* WCFeaturePointAction::Serialize(xercesc::DOMDocument *document) {
	return NULL;
}


WCFeaturePointAction* WCFeaturePointAction::Deserialize(xercesc::DOMElement* obj) {
	return NULL;
}


bool WCFeaturePointAction::Validate(xercesc::DOMElement* obj) {
	return false;
}


/***********************************************~***************************************************/

