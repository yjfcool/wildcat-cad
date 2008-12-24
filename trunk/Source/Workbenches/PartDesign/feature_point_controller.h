#ifndef __FEATURE_POINT_CONTROLLER_H__
#define __FEATURE_POINT_CONTROLLER_H__


/*** Included Header Files ***/
#include <wftrl.h>
#include <event_controller.h>


/*** Locally Defined Values ***/
//None


/*** Class Predefines ***/
class WCFeaturePoint;


/***********************************************~***************************************************/



class WCFeaturePointController : public WCEventController {
private:
	WCFeaturePoint								*_point;
	WCFeaturePointController();																		//!< Deny access to default constructor
	WCFeaturePointController(const WCFeaturePointController& contoller);							//!< Deny access to copy constructor
	WCFeaturePointController& operator=(const WCFeaturePointController& controller);				//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCFeaturePointController(WCFeaturePoint *point);												//!< Primary constructor
	~WCFeaturePointController()					{ }													//!< Default destructor
	
	//Inherited Methods
	WCFeaturePoint* Associate(void);																//!< Return associated object
	void OnSelection(const bool fromManager, std::list<WCVisualObject*> objects);					//!< On select handler
	void OnDeselection(const bool fromManager);														//!< On deselect handler
	void OnContextClick(void);																		//!< On context click handler
};


/***********************************************~***************************************************/


#endif //__FEATURE_POINT_CONTROLLER_H__

