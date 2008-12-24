#ifndef __FEATURE_LINE_H__
#define __FEATURE_LINE_H__


/*** Included Header Files ***/
#include <wftrl.h>
#include <feature.h>
#include <geometric_line.h>
#include <feature_line_action.h>


/*** Locally Defined Values ***/
#define FEATURELINE_CLASSNAME					"Line"


/*** Class Predefines ***/
class WCFeaturePoint;


/***********************************************~***************************************************/


class WCFeatureLine : public WCFeature {
protected:
	WCGeometricLine								*_base;												//!< Cooresponding geometric line
	WCFeaturePoint								*_p0, *_p1;											//!< Start and end points of the line
private:
	//Deny Access
	WCFeatureLine();																				//!< Deny access to default constructor
	WCFeatureLine(const WCFeatureLine &line);														//!< Deny access to copy constructor
	WCFeatureLine& operator=(const WCFeatureLine &line);											//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCFeatureLine(WCFeature *creator, const std::string &name, WCFeaturePoint* p0, WCFeaturePoint *p1);//!< Primary constructor
	~WCFeatureLine();																				//!< Default destructor
	
	//Member Access Methods
	inline WCGeometricLine* Base(void)			{ return this->_base; }								//!< Get the base geometric line
	inline WCFeaturePoint* Begin(void)			{ return this->_p0; }								//!< Get the beginning point
	inline WCFeaturePoint* End(void)			{ return this->_p1; }								//!< Get the ending point
	
	//Inherited Methods
	std::string RootName(void) const			{ return FEATURELINE_CLASSNAME; }					//!< Get the class name
	void ReceiveNotice(WCObjectMsg msg, WCObject *sender);											//!< Receive notice from point or curve
	bool Regenerate(void);																			//!< Validate and rebuild
	
	//Persistence Methods
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document);																	//!< Serialize the object
	static WCFeatureLine* Deserialize(xercesc::DOMElement* obj);											//!< Deserialize the object
	bool Validate(xercesc::DOMElement* obj);																//!< Validate the object
	
	//Visual Object Methods
	inline WCAlignedBoundingBox BoundingBox(void){ return this->_base->BoundingBox(); }				//!< Base's box	
//	WCVisualObject* HitTest(const WCRay &ray, const WPFloat tolerance);								//!< Hit test on the object
//	void ApplyTransform(const WCMatrix4 &transform);												//!< Apply a general transform to the object
//	void ApplyTranslation(const WCVector4 &translation);											//!< Apply a linear translation to the object
	void Render(const GLuint defaultProg=0, const WCColor color=WCColor::Default());				//!< Render the object		
	
	/*** Actions ***/
	static WCFeatureLineAction* ActionCreate(WCFeature *creator, const std::string lineName,		//!< Primary creation action
												WCFeaturePoint *p0, WCFeaturePoint *p1);
	//static WCFeatureLineAction* ActionModify();													//!< Primary modify action
	static WCFeatureLineAction* ActionDelete(WCFeatureLine *line);									//!< Primary delete action
	
	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCFeatureLine &line);					//!< Overloaded output operator		
};


/***********************************************~***************************************************/


#endif //__FEATURE_LINE_H__

