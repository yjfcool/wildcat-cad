#ifndef __FEATURE_LINE_CONTROLLER_H__
#define __FEATURE_LINE_CONTROLLER_H__


/*** Included Header Files ***/
#include "wftrl.h"
#include "event_controller.h"


/*** Local Defines ***/
//None


/*** Class Predefines ***/
class WCFeatureLine;


/***********************************************~***************************************************/



class WCFeatureLineController : public WCEventController {
private:
	WCFeatureLine								*_line;												//!< Associated line
	WCFeatureLineController();																		//!< Deny access to default constructor
	WCFeatureLineController(const WCFeatureLineController& contoller);								//!< Deny access to copy constructor
	WCFeatureLineController& operator=(const WCFeatureLineController& controller);					//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCFeatureLineController(WCFeatureLine *line);													//!< Primary constructor
	~WCFeatureLineController()					{ }													//!< Default destructor
	
	//Inherited Methods
	WCFeatureLine* Associate(void);																	//!< Return associated object
	void OnSelection(void);																			//!< On select handler
	void OnDeselection(void);																		//!< On reorder handler
	void OnContextClick(void);																		//!< On context click handler
};


/***********************************************~***************************************************/


#endif //__FEATURE_LINE_CONTROLLER_H__

