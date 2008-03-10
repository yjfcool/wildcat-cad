#ifndef __COORDINATE_SYSTEM_CONTROLLER_H__
#define __COORDINATE_SYSTEM_CONTROLLER_H__


/*** Included Header Files ***/
#include "wftrl.h"
#include "event_controller.h"


/*** Local Defines ***/
//None


/*** Class Predefines ***/
class WCCoordinateSystem;


/***********************************************~***************************************************/



class WCCoordinateSystemController : public WCEventController {
private:
	WCCoordinateSystem							*_cs;
	WCCoordinateSystemController();																	//!< Deny access to default constructor
	WCCoordinateSystemController(const WCCoordinateSystemController& contoller);					//!< Deny access to copy constructor
	WCCoordinateSystemController& operator=(const WCCoordinateSystemController& controller);		//!< Deny access to equals operator
public:
	WCCoordinateSystemController(WCCoordinateSystem *cs);											//!< Primary constructor
	~WCCoordinateSystemController()				{ }													//!< Default destructor
	
	//Inherited Methods
	WCCoordinateSystem* Associate(void);															//!< Return associated object
	void OnSelection(const bool fromManager, std::list<WCVisualObject*> objects);					//!< On select handler
	void OnDeselection(const bool fromManager);														//!< On reorder handler
	void OnContextClick(void);																		//!< On context click handler
};


/***********************************************~***************************************************/


#endif //__COORDINATE_SYSTEM_CONTROLLER_H__

