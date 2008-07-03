/*** Included Header Files ***/
#include "feature_line_action.h"
#include "feature_line.h"
#include "document.h"


/***********************************************~***************************************************/


WCFeatureLineAction::WCFeatureLineAction(WCFeature *creator, const std::string &lineName, WCFeaturePoint *p0, WCFeaturePoint *p1) : 
	WCAction("Create Line", creator), _line(NULL), _lineName(lineName), _p0(p0), _p1(p1) {
	//Nothing else to do for now
}


WCFeatureLineAction::~WCFeatureLineAction() {
}


WCFeature* WCFeatureLineAction::Execute(void) {
	//Create the line
	WCFeatureLine *line = new WCFeatureLine(this->_creator, this->_lineName, this->_p0, this->_p1);
	//Make sure line is not null
	if (line == NULL)
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCFeatureLineAction::Execute Error - Feature Line could not be created.");
	//Set the pointer and return
	this->_line = line;
	return this->_line;
}


bool WCFeatureLineAction::Merge(WCAction *action) { 
	return false; 
}


bool WCFeatureLineAction::Rollback(void) {
	return false;
}


xercesc::DOMElement* WCFeatureLineAction::Serialize(xercesc::DOMDocument *document) {
	return NULL;
}


WCFeatureLineAction* WCFeatureLineAction::Deserialize(xercesc::DOMElement* obj) {
	return NULL;
}


bool WCFeatureLineAction::Validate(xercesc::DOMElement* obj) {
	return false;
}


/***********************************************~***************************************************/

