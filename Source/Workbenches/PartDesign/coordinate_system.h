#ifndef __COORDINATE_SYSTEM_H__
#define __COORDINATE_SYSTEM_H__


/*** Included Header Files ***/
#include <wftrl.h>
#include <feature.h>
#include <vector.h>
#include <color.h>
#include <coordinate_system_action.h>


/*** Locally Defined Values ***/
#define COORDINATESYSTEM_CLASSNAME				"Coordinate System"
#define COORDINATESYSTEM_COLOR					0.6, 0.6, 0.6, 1.0
#define COORDINATESYSTEM_WIDTH					1.0
#define COORDINATESYSTEM_DEFAULT_VISIBILITY		false


/*** Class Predefines ***/
class WCPart;
class WCFeaturePoint;


/***********************************************~***************************************************/


class WCCoordinateSystem : virtual public WCFeature {
protected:
	WCFeaturePoint								*_refPoint;											//!< Reference to origin point
	WCVector4									_xAxis, _yAxis, _zAxis;								//!< Axis vectors
	bool										_rightHanded;										//!< Handedness of the system
	WCColor										_color;
private:
	//Deny Access
	WCCoordinateSystem();																			//!< Deny access to default constructor
	WCCoordinateSystem(const WCCoordinateSystem& cs);												//!< Deny access to copy constructor
	WCCoordinateSystem& operator=(const WCCoordinateSystem &cs);									//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCCoordinateSystem(WCFeature *creator, const std::string name, WCFeaturePoint *refPoint, 		//!< Primary constructor
												const WCVector4 &xAxis, const WCVector4 &yAxis, const WCVector4 &zAxis);
	virtual ~WCCoordinateSystem();																	//!< Default destructor
	
	//Member Access Methods
	inline WCFeaturePoint* ReferencePoint(void)	{ return this->_refPoint; }							//!< Return reference point
	inline WCVector4 XAxis(void) const			{ return this->_xAxis; }							//!< Get the x-axis direction
	inline WCVector4 YAxis(void) const			{ return this->_yAxis; }							//!< Get the y-axis direction
	inline WCVector4 ZAxis(void) const			{ return this->_zAxis; }							//!< Get the z-axis direction
	inline bool RightHanded(void) const			{ return this->_rightHanded; }						//!< Get the righthandedness of the CS
	
	//Inherited Methods
	std::string RootName(void) const			{ return COORDINATESYSTEM_CLASSNAME; }				//!< Get the class name
	void ReceiveNotice(WCObjectMsg msg, WCObject *sender);											//!< Receive notice from point or curve
	bool Regenerate(void)						{ return true; }									//!< Validate and rebuild
	
	//Persistence Methods
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document);																	//!< Serialize the object
	static WCCoordinateSystem* Deserialize(xercesc::DOMElement* obj);										//!< Deserialize the object
	bool Validate(xercesc::DOMElement* obj);																//!< Validate the object
	
	//Visual Object Methods
//	WCVisualObject* HitTest(const WCRay &ray, const WPFloat tolerance);								//!< Hit test on the object
//	void ApplyTransform(const WCMatrix4 &transform);												//!< Apply a general transform to the object
//	void ApplyTranslation(const WCVector4 &translation);											//!< Apply a linear translation to the object
	void Render(const GLuint defaultProg=0, const WCColor color=WCColor::Default());				//!< Render the object		
	
	/*** Actions ***/
	static WCCoordinateSystemAction* ActionCreate(WCFeature *creator, const std::string systemName, //!< Primary creation action
												WCFeaturePoint *refPoint, const WCVector4 &xAxis, 
												const WCVector4 &yAxis, const WCVector4 &zAxis);
	//static WCCoordiateSystemAction* ActionModify();
	static WCCoordinateSystemAction* ActionDelete(WCCoordinateSystem *system);
	
	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCCoordinateSystem &cs);				//!< Overloaded output operator		
};


/***********************************************~***************************************************/


#endif //__COORDINATE_SYSTEM_H__

