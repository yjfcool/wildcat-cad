#ifndef __BEZIER_CURVE_H__
#define __BEZIER_CURVE_H__


/*** Included Header Files ***/
#include "geometric_types.h"
#include "ray.h"


/*** Local Defines ***/
//Size constants
#define BEZIERCURVE_MAX_LOD						1024
#define BEZIERCURVE_MAX_DEGREE					7			//Should be power of 2
#define BEZIERCURVE_FLOATS_PER_VERTEX			4
//Shader Location Constants
//#define NURBSCURVE_LOC_CP_DEFAULT				0
//#define NURBSCURVE_LOC_CP_DEFAULT23				1
//#define NURBSCURVE_LOC_CP_BEZIER23				2
//#define NURBSCURVE_LOC_KP_DEFAULT				3
//#define NURBSCURVE_LOC_KP_DEFAULT23				4
//#define NURBSCURVE_LOC_KP_BEZIER23				5
//#define NURBSCURVE_LOC_PARAMS_DEFAULT			6
//#define NURBSCURVE_LOC_PARAMS_DEFAULT23			7
//#define NURBSCURVE_LOC_PARAMS_BEZIER23			8
//Accuracy Constants
//#define NURBSCURVE_INVERSION_MAX_ITERATIONS		12
//#define NURBSCURVE_EPSILON_ONE					0.0001
//#define NURBSCURVE_EPSILON_TWO					0.0001
//#define NURBSCURVE_EQUALITY_EPSILON				0.001
//#define NURBSCURVE_LENGTH_ACCURACY				0.001
//#define NURBSCURVE_GENERATE_ACCURACY			0.0625
//Performance Levels
#define BEZIERCURVE_PERFLEVEL_HIGH				0
#define BEZIERCURVE_PERFLEVEL_MEDIUM			1
#define BEZIERCURVE_PERFLEVEL_LOW				2


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


class WCBezierCurve : public WCGeometricCurve {
protected:
	//Basic data
	WPUInt										_degree;											//!< Degree of the curve
	std::vector<WCVector4>						_controlPoints;										//!< Vector of control points
	std::vector<WPFloat>						_knotPoints;										//!< Array of knot point values
	GLuint										_buffer;											//!< Vertex buffer
	
private:
	//General Statics	
//	static WPUInt								_count;												//!< Static class instance count
//	static WPUInt								_perfLevel;											//!< Static performance level indicator
//	static GLint								*_locations;										//!< Values for locations of uniforms
//	static GLuint								_default, _default23, _bezier23;					//!< Static shader programs for vertex generation
	//PerfLevel Generation Statics
//	static int									_minCPBufferSize, _minKPBufferSize;					//!< Values for the buffer sizes of CP and KP in shaders
//	static GLint								_vertsPerBatch;										//!< Number of vertices per batch
//	static GLintptr								_kpStride;											//!< Stride of the kp
//	static GLuint								_cpBuffer, _kpBuffer;								//!< High uniform buffers for generation
//	static GLuint								_cpTex, _kpTex, _inTex, _outTex;					//!< Medium textures for generation
//	static GLint								_maxTexSize;										//!< Medium maxium texture size
//	static GLuint								_framebuffer;										//!< Framebuffer for medium generate use
	
	//Private Static Functions
//	static WPUInt Start(void);																		//!< Static class initialization
//	static WPUInt Stop(void);																		//!< Static class deinitialization
	
	//Private Methods
//	WPFloat EstimateLength(void);																	//!< Calculate an estimated length based on CP
//	void GenerateKnotPointsVBO(void);																//!< Generate the knot points VBO	
//	void GenerateKnotPointsTexture(void);															//!< Gerenate the knot points texture
//	void GenerateControlPointsVBO(void);															//!< Generate the control points VBO
//	void GenerateControlPointsTexture(void);														//!< Generate the knot points texture
//	void LoadKnotPoints(const WCCollection<WPFloat> *collection=NULL);								//!< Create all knot point structures
//	void LoadControlPoints(const WCCollection<WCControlPoint*> *controlPoints);						//!< Alternate to using data arrays	
	
	//Curve Generation Methods
//	void GenerateCurveHigh(void);																	//!< Generate GL using High perf level
//	void GenerateCurveMedium(void);																	//!< Generate GL using Medium perf level
	void GenerateCurveLow(void);																	//!< Generate GL using Low perf level		
//	void GenerateCurveOne(void);																	//!< For 1st degree curves
	
	//Access Denied
	WCBezierCurve();																				//!< Deny access to default constructor
public:
	//Constructors and Destructors
	WCBezierCurve(const WPUInt degree, const std::vector<WCVector4> &controlPoints);				//!< Collection constructor						
	WCBezierCurve(const WCBezierCurve &curve);														//!< Copy constructor
	~WCBezierCurve();																				//!< Default destructor
	
	//General Access Functions
	inline GLuint VertexBuffer(void) const				{ return this->_buffer;	}					//!< Return ID for vertex buffer
	inline std::vector<WCVector4> ControlPoints(void)	{ return this->_controlPoints; }			//!< Return the control points vector
	inline WPUInt NumberControlPoints(void) const{ return this->_controlPoints.size(); }			//!< Get the number of control points
	inline std::vector<WPFloat> KnotPoints(void){ return this->_knotPoints; }						//!< Get the array of knot points
	inline WPUInt NumberKnotPoints(void) const	{ return this->_knotPoints.size(); }				//!< Get the number of knot points
	WPUInt Degree(const WPUInt degree);																//!< Set the degree of the curve
	inline WPUInt Degree(void)	const			{ return this->_degree; }							//!< Get the degree of the curve		
	WPUInt LevelOfDetail(const WPUInt lod);															//!< Set the Level-Of-Detail value
	inline WPUInt LevelOfDetail(void) const		{ return this->_lod; }								//!< Get the Level-Of-Detail value	
	
	//Inherited Member Functions
	bool Intersect(const WCGeometricPoint &point, const WPFloat tolerance);							//!< Check for intersection with point
	bool Intersect(const WCGeometricCurve &curve, const WPFloat toleranc);							//!< Check for intersection with curve
	WPFloat Length(WPFloat toleranc);																//!< Calculate the length of the curve
	WCVector4 Evaluate(const WPFloat u);															//!< Evaluate a specific point on the curve
	WCVisualObject* HitTest(const WCRay &ray, const WPFloat tolerance);								//!< Hit test with a ray
	void ApplyTransform(const WCMatrix4 &transform);												//!< Apply a transform to the curve
	void ApplyTranslation(const WCVector4 &translation);											//!< Apply a linear translation to the object
	void Render(const GLuint defaultProg=0, const WCColor color=WCColor::Default());				//!< Render the surface
	void ReceiveNotice(WCObjectMsg msg, WCObject *sender);											//!< Receive messages from other objects

	//Original Member Functions
	WCVector4 Derivative(const WPFloat u, const WPUInt der);										//!< Evaluate the derivative at a specific point
	WCRay Tangent(const WPFloat u);																	//!< Get the tangent to the curve at U
	WCVector4 PointInversion(const WCGeometricPoint &point);										//!< Get the closest point on the curve from the given point
	
	//Operator Overloads
	WCBezierCurve& operator=(const WCBezierCurve &curve);											//!< Equals operator
	bool operator==(const WCBezierCurve &curve);													//!< Equality operator
	
	//Serialization and Object Methods
	DOMElement* Serialize(DOMDocument *document);													//!< Serialize the object
	static WCBezierCurve* Deserialize(DOMElement* obj);												//!< Deserialize the object
	bool Validate(DOMElement* obj);																	//!< Validate the object
	
	/*** Static Creation Methods ***/
	//
	
	/*** Non-Member Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCBezierCurve &curve);					//!< Overloaded output operator
};
 

/***********************************************~****************************************************/


#endif //__NURBS_CURVE_H__

