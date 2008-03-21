#ifndef __CURVE_PROFILE_H__
#define __CURVE_PROFILE_H__


/*** Included Header Files ***/
#include "geometric_types.h"


/*** Local Defines ***/
#define CURVEPROFILE_CONNECTIVITY_TOLERANCE		0.001


/*** Class Predefines ***/
class WCVector4;


/***********************************************~***************************************************/


class WCCurveProfile : virtual public WCGeometricCurve {
protected:
	WCCollection<WCGeometricCurve*>				_curves;											//!< List of retained sub-curves
	WCCollection<bool>							_parametricDirection;								//!< List of directions of sub-curves
	bool										_isLinear;											//!< Linearity flag
	
private:
	//Private validation methods
	void ValidateLinearity(void);																	//!< Check to see if profile is linear
	void ValidateClosure(void);																		//!< Check to see if profile is closed
	
public:	
	//Constructors and Destructors
	WCCurveProfile();																				//!< Default constructor
	WCCurveProfile(const WCCurveProfile &profile);													//!< Copy constructor
	~WCCurveProfile();																				//!< Default destructor

	//General Access Functions
	inline WPUInt Size(void) const				{ return this->_curves.Size(); }					//!< Get the number of curves in the profile
	inline WCGeometricCurve* Curve(WPUInt index){ if (index < this->_curves.Size())					//!< Return the indexed curve
												     return this->_curves.At(index); 
												  else return NULL; }		
	inline bool IsLinear(void) const			{ return this->_isLinear; }							//!< Get the linearity of the profile

	//Inherited Member Functions
	WCAlignedBoundingBox BoundingBox(void);															//!< Create a bounding box for the profile
	bool Intersect(const WCGeometricPoint &point, const WPFloat tolerance=GEOMETRICOBJECT_DEFAULT_EPSILON);	//!< Check for intersection with point
	bool Intersect(const WCGeometricCurve &curve, const WPFloat tolerance=GEOMETRICOBJECT_DEFAULT_EPSILON);	//!< Check for intersection with curve
	WPFloat Length(WPFloat tolerance=GEOMETRICOBJECT_DEFAULT_EPSILON);								//!< Calculate the length of the curve
	WCVector4 Evaluate(const WPFloat u);															//!< Evaluate is not a valid method
	WCVector4 Derivative(const WPFloat u, const WPUInt der);										//!< Evaluate the derivative at a specific point
	WCRay Tangent(const WPFloat u);																	//!< Get the tangent to the curve at U
	WCVector4 PointInversion(const WCGeometricPoint &point);										//!< Get the closest point on the curve from the given point		
	WCVisualObject* HitTest(const WCRay &ray, const WPFloat tolerance);								//!< Hit test on the object
	void ApplyTransform(const WCMatrix4 &transform);												//!< Apply a general transform to the object
	void ApplyTranslation(const WCVector4 &translation);											//!< Apply a linear translation to the object
	void Render(const GLuint defaultProg=0, const WCColor color=WCColor::Default());				//!< Render the object
	void ReceiveNotice(WCObjectMsg msg, WCObject *sender);											//!< Receive messages from other objects
#ifdef __WILDCAT_SCENE_INCLUDED__
	void Layer(WCLayer *layer);																		//!< Set the layers
#endif
	
	//Original Member Functions
	bool AddCurve(WCGeometricCurve *curve);															//!< Add a curve to the profile
	bool RemoveCurve(WCGeometricCurve *curve);														//!< Remove a curve from the profile
	
	//Operator Overloads
	WCCurveProfile& operator=(const WCCurveProfile &profile);										//!< Equals operator
	bool operator==(const WCCurveProfile &profile);													//!< Equality operator
	
	//Serialization and Object Methods
	DOMElement* Serialize(DOMDocument *document);													//!< Serialize the object
	static WCCurveProfile* Deserialize(DOMElement* obj);											//!< Deserialize the object
	bool Validate(DOMElement* obj);																	//!< Validate the object

//Non-Member Functions
	friend std::ostream& operator<<(std::ostream& out, const WCCurveProfile &profile);				//!< Overloaded output operator
};


/***********************************************~***************************************************/


#endif //__CURVE_PROFILE_H__

