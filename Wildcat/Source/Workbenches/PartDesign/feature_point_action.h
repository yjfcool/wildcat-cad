#ifndef __FEATURE_POINT_ACTION_H__
#define __FEATURE_POINT_ACTION_H__


/*** Included Header Files ***/
#include "wftrl.h"
#include "action.h"


/*** Local Defines ***/
//None


/*** Class Predefines ***/
class WCFeaturePoint;


/***********************************************~***************************************************/


class WCFeaturePointAction : public WCAction {
private:
	WCFeaturePoint								*_point;											//!< Associated point
	std::string									_pointName;											//!< Name of new point
	WPFloat										_xVal, _yVal, _zVal;
	//Constructors
	WCFeaturePointAction(WCFeature *creator, const std::string &pointName, const WPFloat &xVal, 	//!< Primary constructor
												const WPFloat &yVal, const WPFloat &zVal);
	//Make CoordinateSystem a friend
	friend class WCFeaturePoint;
public:
	~WCFeaturePointAction();																		//!< Default destructor
	
	//Inherited Methods
	WCFeature* Execute(void);																		//!< Execute the action
	bool Merge(WCAction *action);																	//!< Try to merge two actions
	bool Rollback(void);																			//!< Try to rollback the action
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document);																	//!< Serialize the object
	static WCFeaturePointAction* Deserialize(xercesc::DOMElement* obj);										//!< Deserialize the object
	bool Validate(xercesc::DOMElement* obj);																//!< Validate an object
};


/***********************************************~***************************************************/


#endif //__FEATURE_POINT_ACTION_H__

