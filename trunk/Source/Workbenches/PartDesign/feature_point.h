#ifndef __FEATURE_POINT_H__
#define __FEATURE_POINT_H__


/*** Included Header Files ***/
#include <wftrl.h>
#include <feature.h>
#include <geometric_point.h>
#include <feature_point_action.h>


/*** Locally Defined Values ***/
#define FEATUREPOINT_CLASSNAME					"Point"


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


class WCFeaturePoint : public WCFeature {
protected:
	WCGeometricPoint							*_base;												//!< Cooresponding geometric point
private:
	//Deny Access
	WCFeaturePoint();																				//!< Deny access to default constructor
	WCFeaturePoint(const WCFeaturePoint& pt);														//!< Deny access to copy constructor
	WCFeaturePoint& operator=(const WCFeaturePoint &pt);											//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCFeaturePoint(WCFeature *creator, const std::string &name, const WPFloat x, const WPFloat y, const WPFloat z);//!< Primary constructor
	~WCFeaturePoint();																				//!< Default destructor
	
	//Member Access Methods
	inline WCGeometricPoint* Base(void)			{ return this->_base; }								//!< Get the base geometric point
	inline WCVector4 Data(void) const			{ return this->_base->Data(); }						//!< Get the whole vector
	inline WPFloat X(void) const				{ return this->_base->X(); }						//!< Get the x value
	inline WPFloat Y(void) const				{ return this->_base->Y(); }						//!< Get the y value
	inline WPFloat Z(void) const				{ return this->_base->Z(); }						//!< Get the z value
	void Set(const WCVector4 &position);															//!< Set the position
	
	//Inherited Methods
	std::string RootName(void) const			{ return FEATUREPOINT_CLASSNAME; }					//!< Get the class name
	void ReceiveNotice(WCObjectMsg msg, WCObject *sender);											//!< Receive notice from point or curve
	bool Regenerate(void);																			//!< Validate and rebuild
	
	//Persistence Methods
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document);																	//!< Serialize the object
	static WCFeaturePoint* Deserialize(xercesc::DOMElement* obj);											//!< Deserialize the object
	bool Validate(xercesc::DOMElement* obj);																//!< Validate the object
	
	//Visual Object Methods
	inline WCAlignedBoundingBox BoundingBox(void){ return this->_base->BoundingBox(); }				//!< Base's box	
//	WCVisualObject* HitTest(const WCRay &ray, const WPFloat tolerance);								//!< Hit test on the object
//	void ApplyTransform(const WCMatrix4 &transform);												//!< Apply a general transform to the object
//	void ApplyTranslation(const WCVector4 &translation);											//!< Apply a linear translation to the object
	void Render(const GLuint defaultProg=0, const WCColor color=WCColor::Default());				//!< Render the object		
	
	/*** Actions ***/
	static WCFeaturePointAction* ActionCreate(WCFeature *creator, const std::string pointName,		//!< Primary creation action
												const WPFloat x, const WPFloat y, const WPFloat z);
	//static WCFeaturePointAction* ActionModify();													//!< Primary modify action
	static WCFeaturePointAction* ActionDelete(WCFeaturePoint *point);								//!< Primary delete action
	
	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCFeaturePoint &point);				//!< Overloaded output operator		
};


/***********************************************~***************************************************/


#endif //__FEATURE_POINT_H__

