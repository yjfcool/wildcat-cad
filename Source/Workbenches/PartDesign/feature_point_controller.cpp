#include "feature_point.h"
#include "feature_point_controller.h"


/***********************************************~***************************************************/


WCFeaturePointController::WCFeaturePointController(WCFeaturePoint *point) : ::WCEventController(point->TreeElement()),
	_point(point) { 
	//Nothing else for now
}


inline WCFeaturePoint* WCFeaturePointController::Associate(void) {
	return this->_point;
}


void WCFeaturePointController::OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) {
}


void WCFeaturePointController::OnDeselection(const bool fromManager) {
}


void WCFeaturePointController::OnContextClick(void) {
}


/***********************************************~***************************************************/
