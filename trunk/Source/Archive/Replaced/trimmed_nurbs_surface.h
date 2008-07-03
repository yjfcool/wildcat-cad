#ifndef __TRIMMED_NURBS_SURFACE_H__
#define __TRIMMED_NURBS_SURFACE_H__


/*** Included Header Files ***/
#include "wgeol.h"
#include "nurbs_surface.h"
#include "collection.h"


/*** Class Predefines ***/
class WCGeometricObject;
class WCGeometricPoint;
class WCRay;
class WCCurveProfile;
class WCNurbsCurve;
class WCGeometricSurface;
class WCControlPoint;


/*** Local Defines ***/
#define TRIMSURFACE_TEX_LOCATION				0
#define TRIMSURFACE_DEFAULT_TEX_WIDTH			256
#define TRIMSURFACE_DEFAULT_TEX_HEIGHT			256
//Performance Levels
#define TRIMSURFACE_PERFLEVEL_HIGH				0
#define TRIMSURFACE_PERFLEVEL_MEDIUM			1
#define TRIMSURFACE_PERFLEVEL_LOW				2


/***********************************************~***************************************************/


class WCTrimmedNurbsSurface : public WCNurbsSurface {
protected:
	WCCollection<WCGeometricCurve*>				_interiorCurves;									//!< Vector of interior curves
	WCCollection<WCGeometricCurve*>				_exteriorCurves;									//!< Vector of exterior curves
	GLuint										_trimTexture;										//!< Trim texture
	WPUInt										_texWidth, _texHeight;								//!< Trim texture width and height

private:
	//Static Initialization Variables
	static WPUInt								_count;												//!< Static class instance count
	static GLuint								_framebuffer;										//!< Framebuffer object for trim tex rendering
	static GLuint								_visProgram;										//!< Static shader programs
	static GLint								_texLocation;										//!< Locations for the uniform variables	
	static WPUInt								_perfLevel;											//!< Static performance level indicator
	
	//Private Static Functions
	static WPUInt Start(void);																		//!< Static class initializer
	static WPUInt Stop(void);																		//!< Static class deinitializer
	
	//Private Methods
	void ValidateClosure(void);																		//!< Check the closure of the surface
	void ValidateSelfIntersection(void);															//!< Check the self-intersection of the surface		
	void GenerateTextureHigh(void);																	//!< Generate texture for display - High-end card
	void GenerateTextureMedium(void);																//!< Generate texture for display - Medium card
	void GenerateTextureLow(void);																	//!< Generate texture for display - CPU based		
	
	//Access Denied
	WCTrimmedNurbsSurface();																		//!< Deny access to default constructor
	WCTrimmedNurbsSurface(const WCTrimmedNurbsSurface &surf);										//!< For now deny access to copy constructor
	WCTrimmedNurbsSurface& operator=(const WCTrimmedNurbsSurface &surf);							//!< For now deny access to equals operator
public:
	//Constructors and Destructors
	WCTrimmedNurbsSurface(const WPUInt degreeU, const WPUInt degreeV,								//!< Primary constructor
					const WPUInt cpU, const WPUInt cpV, const WCCollection<WCControlPoint*> *controlPoints,
					const WCNurbsMode &modeU, const WCNurbsMode &modeV,
					const WCCollection<WPFloat> *kpU=NULL, const WCCollection<WPFloat> *kpV=NULL);
	~WCTrimmedNurbsSurface();																		//!< Default destructor
	
	//Member Access Functions
	//...
	
	//Inherited Member Functions
	bool Intersect(WCGeometricPoint &point)		{ return false; }									//!< Check for intersection with point
	bool Intersect(WCGeometricCurve &curve)		{ return false; }									//!< Check for intersection with curve	
	bool Intersect(WCGeometricSurface &surface)			{ return false; }									//!< Check for intersection with surface		
	WPFloat Area(void)							{ return 1.0; }										//!< Return the area of the surface
	void Render(GLuint defaultProg);																//!< Render the surface
	void ReceiveNotice(WCObjectMsg msg, WCObject *sender);											//!< Receive messages from other objects
	
	//Trimming Methods
	void AddTrimCurve(WCGeometricCurve *curve, const bool inside);									//!< Add a trimming curve
	void RemoveTrimCurve(const WCGeometricCurve *curve);											//!< Remove a trimming curve
	
	//Serialization and Object Methods
	DOMElement* Serialize(DOMDocument *document);													//!< Serialize the object
	static WCTrimmedNurbsSurface* Deserialize(DOMElement* obj);										//!< Deserialize the object
	bool Validate(DOMElement* obj);																	//!< Validate the object
	
	/*** Non-Member Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCTrimmedNurbsSurface &surface);		//!< Overloaded output operator
	friend std::ostream& operator<<(std::ostream& out, const WCTrimmedNurbsSurface *surface) 		//!< Overloaded output operator		
												{ return out << *surface; }
};
 

/***********************************************~***************************************************/


#endif //__TRIMMED_NURBS_SURFACE_H__

