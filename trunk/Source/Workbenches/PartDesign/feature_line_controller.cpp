#include <feature_line.h>
#include <feature_line_controller.h>


/***********************************************~***************************************************/


WCFeatureLineController::WCFeatureLineController(WCFeatureLine *line) : ::WCEventController(line->TreeElement()),
	_line(line) { 
	//Nothing else for now
}


inline WCFeatureLine* WCFeatureLineController::Associate(void) {
	return this->_line;
}


void WCFeatureLineController::OnSelection(void) {
}


void WCFeatureLineController::OnDeselection(void) {
}


void WCFeatureLineController::OnContextClick(void) {
}


/***********************************************~***************************************************/
