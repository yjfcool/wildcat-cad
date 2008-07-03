#ifndef __CONTROL_POINT_H__
#define __CONTROL_POINT_H__


/*** Included Header Files ***/
#include "geometric_point.h"


/*** Local Defines ***/
#define CONTROLPOINT_DEFAULT_COLOR				0.75, 0.5, 0.5, 1.0
#define CONTROLPOINT_SELECTED_COLOR				0.0, 1.0, 0.0, 1.0
#define CONTROLPOINT_MOUSEOVER_COLOR			0.0, 0.0, 1.0, 1.0
#define CONTROLPOINT_POINTSIZE					4.0


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


class WCControlPoint : public WCGeometricPoint {
private:
	//Access Denied
	WCControlPoint();																				//!< Deny access to default constructor
public:
	//Constructors and Destructors
	WCControlPoint(const WPFloat x, const WPFloat y, const WPFloat z, const WPFloat w=1.0);			//!< Primary constructor
	WCControlPoint(const WCVector4 vec);															//!< Vector constructor
	WCControlPoint(const WCControlPoint &point);													//!< Copy constructor
	WCControlPoint(const WPUInt size, const WPFloat *data);											//!< Pointer constructor
	~WCControlPoint()							{ }													//!< Default destructor

	static WCCollection<WCControlPoint*>* CollectionFromArray(const WPUInt count, const WPFloat *data);	//!< Create a collection from an array of points
	
	//Member Functions
	void Render(const GLuint defaultProg=0, const WCColor color=WCColor::Default());				//!< Render the control point
	
	//Serialization and Object Methods
	DOMElement* Serialize(DOMDocument *document);													//!< Serialize the object
	static WCControlPoint* Deserialize(DOMElement* obj);											//!< Deserialize the object
	bool Validate(DOMElement* obj);																	//!< Validate the object
	
//Non-Member Functions
	friend std::ostream& operator<<(std::ostream& out, const WCControlPoint &point);				//!< Overloaded output operator
};


/***********************************************~***************************************************/


#endif //__CONTROL_POINT_H__

