#ifndef __FEATURE_LINE_ACTION_H__
#define __FEATURE_LINE_ACTION_H__


/*** Included Header Files ***/
#include "wftrl.h"
#include "action.h"


/*** Local Defines ***/
//None


/*** Class Predefines ***/
class WCFeatureLine;
class WCFeaturePoint;


/***********************************************~***************************************************/


class WCFeatureLineAction : public WCAction {
private:
	WCFeatureLine								*_line;												//!< Associated line
	std::string									_lineName;											//!< Name of new line
	WCFeaturePoint								*_p0, *_p1;											//!< Base points

	//Deny Access
	WCFeatureLineAction(WCFeature *creator, const std::string &lineName, WCFeaturePoint *p0, WCFeaturePoint *p1); 	//!< Primary constructor
	//Friend Declarations
	friend class WCFeatureLine;																		//!< Make FeatureLine a friend
public:
	~WCFeatureLineAction();																			//!< Default destructor
	
	//Inherited Methods
	WCFeature* Execute(void);																		//!< Execute the action
	bool Merge(WCAction *action);																	//!< Try to merge two actions
	bool Rollback(void);																			//!< Try to rollback the action
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document);																	//!< Serialize the object
	static WCFeatureLineAction* Deserialize(xercesc::DOMElement* obj);										//!< Deserialize the object
	bool Validate(xercesc::DOMElement* obj);																//!< Validate an object
};


/***********************************************~***************************************************/


#endif //__FEATURE_LINE_ACTION_H__

