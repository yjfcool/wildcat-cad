#ifndef __NURBS_CURVE_H__
#define __NURBS_CURVE_H__


/*** Included Header Files ***/
#include "geometric_types.h"
#include "vector.h"
#include "control_point.h"
#include "collection.h"
#include "ray.h"
#include "color.h"


/*** Local Defines ***/
//Size constants
#define NURBSCURVE_MAX_LOD						1024
#define NURBSCURVE_MAX_DEGREE					7			//Should be power of 2
#define NURBSCURVE_MAX_CONTROLPOINTS			64			//Should be power of 2
#define NURBSCURVE_MAX_KNOTPOINTS				64			//Should be power of 2
#define NURBSCURVE_MAX_VERT_PER_BATCH			1
#define NURBSCURVE_FLOATS_PER_VERTEX			4
//Shader Location Constants
#define NURBSCURVE_LOC_CP_DEFAULT				0
#define NURBSCURVE_LOC_CP_DEFAULT23				1
#define NURBSCURVE_LOC_CP_BEZIER23				2
#define NURBSCURVE_LOC_KP_DEFAULT				3
#define NURBSCURVE_LOC_KP_DEFAULT23				4
#define NURBSCURVE_LOC_KP_BEZIER23				5
#define NURBSCURVE_LOC_PARAMS_DEFAULT			6
#define NURBSCURVE_LOC_PARAMS_DEFAULT23			7
#define NURBSCURVE_LOC_PARAMS_BEZIER23			8
//Accuracy Constants
#define NURBSCURVE_INVERSION_MAX_ITERATIONS		12
#define NURBSCURVE_EPSILON_ONE					0.0001
#define NURBSCURVE_EPSILON_TWO					0.0001
#define NURBSCURVE_EQUALITY_EPSILON				0.001
#define NURBSCURVE_LENGTH_ACCURACY				0.001
#define NURBSCURVE_GENERATE_ACCURACY			0.0625
//Performance Levels
#define NURBSCURVE_PERFLEVEL_HIGH				0
#define NURBSCURVE_PERFLEVEL_MEDIUM				1
#define NURBSCURVE_PERFLEVEL_LOW				2


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


class WCNurbsCurve : public WCGeometricCurve {
protected:
	//Basic data
	WPUInt										_degree;											//!< Degree of the curve
	WCNurbsMode									_mode;												//!< NURBS knot mode
	bool										_drawCP;											//!< Are control points visible
	WPUInt										_cp, _kp;											//!< Number of control and knot points, N value
	WCCollection<WCControlPoint*>				_cpCollection;										//!< Collection of pointers to control points
	WPFloat										*_knotPoints;										//!< Array of knot point values
	GLuint										_buffer;											//!< Vertex buffer
	
private:
	//General Statics	
	static WPUInt								_count;												//!< Static class instance count
	static WPUInt								_perfLevel;											//!< Static performance level indicator
	static GLint								*_locations;										//!< Values for locations of uniforms
	static GLuint								_default, _default23, _bezier23;					//!< Static shader programs for vertex generation
	//PerfLevel Generation Statics
	static int									_minCPBufferSize, _minKPBufferSize;					//!< Values for the buffer sizes of CP and KP in shaders
	static GLint								_vertsPerBatch;										//!< Number of vertices per batch
	static GLintptr								_kpStride;											//!< Stride of the kp
	static GLuint								_cpBuffer, _kpBuffer;								//!< High uniform buffers for generation
	static GLuint								_cpTex, _kpTex, _inTex, _outTex;					//!< Medium textures for generation
	static GLint								_maxTexSize;										//!< Medium maxium texture size
	static GLuint								_framebuffer;										//!< Framebuffer for medium generate use
	
	//Private Static Functions
	static WPUInt Start(void);																		//!< Static class initialization
	static WPUInt Stop(void);																		//!< Static class deinitialization
	
	//Private Methods
//	WPFloat EstimateLength(void);																	//!< Calculate an estimated length based on CP
	void GenerateKnotPointsVBO(void);																//!< Generate the knot points VBO	
	void GenerateKnotPointsTexture(void);															//!< Gerenate the knot points texture
	void GenerateControlPointsVBO(void);															//!< Generate the control points VBO
	void GenerateControlPointsTexture(void);														//!< Generate the knot points texture
	void LoadKnotPoints(const WCCollection<WPFloat> *collection=NULL);								//!< Create all knot point structures
	void LoadControlPoints(const WCCollection<WCControlPoint*> *controlPoints);						//!< Alternate to using data arrays	
	
	//Curve Generation Methods
	void GenerateCurveHigh(void);																	//!< Generate GL using High perf level
	void GenerateCurveMedium(void);																	//!< Generate GL using Medium perf level
	void GenerateCurveLow(void);																	//!< Generate GL using Low perf level		
	void GenerateCurveOne(void);																	//!< For 1st degree curves
	
	//Access Denied
	WCNurbsCurve();																					//!< Deny access to default constructor
public:
	//Constructors and Destructors
	WCNurbsCurve(const WPUInt degree, const WCCollection<WCControlPoint*> *cpCollection,			//!< Collection constructor
					const WCNurbsMode &mode, const WCCollection<WPFloat> *kpCollection=NULL);								
	WCNurbsCurve(const WCNurbsCurve &curve);														//!< Copy constructor
	~WCNurbsCurve();																				//!< Default destructor
	
	//General Access Functions
	inline GLuint VertexBuffer(void) const		{ return this->_buffer;	}							//!< Return ID for vertex buffer
	inline void DrawControlPoints(const bool vis){ this->_drawCP = vis; }							//!< Flag for cp visibility
	inline bool DrawControlPoints(void) const	{ return this->_drawCP; }							//!< Return cp visibility flag	
	WCCollection<WCControlPoint*> ControlPoints(void) { return this->_cpCollection; }				//!< Return the control points collection
	inline WPUInt NumberControlPoints(void) const{ return this->_cp; }								//!< Get the number of control points
	inline WPFloat* KnotPoints(void)			{ return this->_knotPoints; }						//!< Get the array of knot points
	inline WPUInt NumberKnotPoints(void) const	{ return this->_kp; }								//!< Get the number of knot points
	inline WCNurbsMode Mode(void) const			{ return this->_mode; }								//!< Get the knot mode	
	WPUInt Degree(const WPUInt degree);																//!< Set the degree of the curve
	inline WPUInt Degree(void)	const			{ return this->_degree; }							//!< Get the degree of the curve		
	WPUInt LevelOfDetail(const WPUInt lod);															//!< Set the Level-Of-Detail value
	inline WPUInt LevelOfDetail(void) const		{ return this->_lod; }								//!< Get the Level-Of-Detail value	
	
	//Inherited Member Functions
	virtual std::list<WPFloat> Intersect(WCGeometricPoint *point, const WPFloat &tol);				//!< Check for intersection with point
	virtual std::list<WPIntersectRec> Intersect(WCGeometricCurve *curve, const WPFloat &tol);		//!< Check for intersection with curve
	WPFloat Length(WPFloat tolerance=NURBSCURVE_LENGTH_ACCURACY);									//!< Calculate the length of the curve
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
	
	//Knot Insertion and Degree Elevation - Does Not Alter Curve Shape
	void InsertKnot(const WPFloat u, const WPUInt multiplicity=1);									//!< Insert a knot at parametric value u
	void RefineKnot(void);																			//!< Refine the curve with multiple knot insertions
	void RemoveKnot(void);																			//!< Remove knots from the curve
	void ElevateDegree(void);																		//!< Elevate the degree of the curve
	void ReduceDegree(void);																		//!< Reduce the degree of the curve 	
	
	//Operator Overloads
	WCNurbsCurve& operator=(const WCNurbsCurve &curve);												//!< Equals operator
	bool operator==(const WCNurbsCurve &curve);														//!< Equality operator
	
	//Serialization and Object Methods
	DOMElement* Serialize(DOMDocument *document);													//!< Serialize the object
	static WCNurbsCurve* Deserialize(DOMElement* obj);												//!< Deserialize the object
	bool Validate(DOMElement* obj);																	//!< Validate the object
	
	/*** Static Creation Methods ***/
	static WCNurbsCurve* CircularArc(const WCVector4 &center, const WCVector4 &xUnit,				//!< Generate a circular arc curve
												const WCVector4 &yUnit, const WPFloat radius, 
												const WPFloat startAngleDeg, const WPFloat endAngleDeg);
	static WCNurbsCurve* Conic(const WCVector4 &first, const WCVector4 &firstTangent,				//!< Generate a conic section
												const WCVector4 &second, const WCVector4 &secondTangent,
												const WCVector4 &third);
	static WCNurbsCurve* Ellipse(const WCVector4 &center, const WCVector4 &xUnit,					//!< Generate an ellipse with major and minor axis
												const WCVector4 &yUnit, const WPFloat major, const WPFloat minor);
	
	/*** Non-Member Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCNurbsCurve &curve);					//!< Overloaded output operator
};
 

/***********************************************~****************************************************/


#endif //__NURBS_CURVE_H__

