#ifndef __COORDINATE_SYSTEM_ACTION_H__
#define __COORDINATE_SYSTEM_ACTION_H__


/*** Included Header Files ***/
#include <wftrl.h>
#include <action.h>


/*** Locally Defined Values ***/
//None


/*** Class Predefines ***/
class WCCoordinateSystem;
class WCFeaturePoint;


/***********************************************~***************************************************/


class WCCoordinateSystemAction : public WCAction {
private:
	WCCoordinateSystem							*_cs;												//!< Associated cs
	std::string									_systemName;										//!< Name of new CS
	WCFeaturePoint								*_refPoint;											//!< Reference base point
	WCVector4									_xAxis, _yAxis, _zAxis;								//!< Three axis'

	//Constructors
	WCCoordinateSystemAction();																		//!< Deny access to default constructor
	WCCoordinateSystemAction(const WCCoordinateSystemAction& action);								//!< Deny access to copy constructor
	WCCoordinateSystemAction(WCFeature *creator, const std::string &systemName,	WCFeaturePoint *point,//!< Primary constructor
												const WCVector4 &xAxis, const WCVector4 &yAxis, const WCVector4 &zAxis);
	//Make CoordinateSystem a friend
	friend class WCCoordinateSystem;																//!< Friend declaration
public:
	~WCCoordinateSystemAction()					{ }													//!< Default destructor
	//Inherited Methods
	WCFeature* Execute(void);																				//!< Execute the action
	bool Merge(WCAction *action)				{ return false; }									//!< Try to merge two actions
	bool Rollback(void)							{ return false; }									//!< Try to rollback the action
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document);																	//!< Serialize the object
	static WCCoordinateSystemAction* Deserialize(xercesc::DOMElement* obj);									//!< Deserialize the object
	bool Validate(xercesc::DOMElement* obj);																//!< Validate an object
};


/***********************************************~***************************************************/


#endif //__COORDINATE_SYSTEM_ACTION_H__

