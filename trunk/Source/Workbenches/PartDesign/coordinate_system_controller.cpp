#include "coordinate_system.h"
#include "coordinate_system_controller.h"


/***********************************************~***************************************************/


WCCoordinateSystemController::WCCoordinateSystemController(WCCoordinateSystem *cs) : ::WCEventController(cs->TreeElement()),
	_cs(cs) { 
	//Nothing else for now
}


inline WCCoordinateSystem* WCCoordinateSystemController::Associate(void) {
	return this->_cs;
}


void WCCoordinateSystemController::OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) {
}


void WCCoordinateSystemController::OnDeselection(const bool fromManager) {
}


void WCCoordinateSystemController::OnContextClick(void) {
}


/***********************************************~***************************************************/

