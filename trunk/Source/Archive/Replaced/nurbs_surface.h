#ifndef __NURBS_SURFACE_H__
#define __NURBS_SURFACE_H__


/*** Included Header Files ***/
#include "geometric_types.h"
#include "vector.h"
#include "control_point.h"
#include "collection.h"
#include "color.h"
#include "ray.h"


/*** Local Defines ***/
//Size constants
#define NURBSSURFACE_MAX_LOD					512
#define NURBSSURFACE_MAX_DEGREE					7
#define NURBSSURFACE_MAX_CONTROLPOINTS			64
#define NURBSSURFACE_MAX_KNOTPOINTS				64
#define NURBSSURFACE_MAX_VERT_PER_BATCH			1
#define NURBSSURFACE_MAX_TEXSIZE				512
#define NURBSSURFACE_FLOATS_PER_VERTEX			4
#define NURBSSURFACE_FLOATS_PER_NORMAL			4
#define NURBSSURFACE_FLOATS_PER_TEXCOORD		2
//Shader Location Constants
#define NURBSSURFACE_LOC_CP_DEFAULT				0
#define NURBSSURFACE_LOC_CP_DEFAULT23			1
#define NURBSSURFACE_LOC_CP_BEZIER23			2
#define NURBSSURFACE_LOC_KPU_DEFAULT			3
#define NURBSSURFACE_LOC_KPU_DEFAULT23			4
#define NURBSSURFACE_LOC_KPU_BEZIER23			5
#define NURBSSURFACE_LOC_KPV_DEFAULT			6
#define NURBSSURFACE_LOC_KPV_DEFAULT23			7
#define NURBSSURFACE_LOC_KPV_BEZIER23			8
#define NURBSSURFACE_LOC_PARAMSU_DEFAULT		9
#define NURBSSURFACE_LOC_PARAMSU_DEFAULT23		10
#define NURBSSURFACE_LOC_PARAMSU_BEZIER23		11
#define NURBSSURFACE_LOC_PARAMSV_DEFAULT		12
#define NURBSSURFACE_LOC_PARAMSV_DEFAULT23		13
#define NURBSSURFACE_LOC_PARAMSV_BEZIER23		14
//Buffer Constants
#define NURBSSURFACE_VERTEX_BUFFER				0
#define NURBSSURFACE_INDEX_BUFFER				1
#define NURBSSURFACE_NORMAL_BUFFER				2
#define NURBSSURFACE_TEXCOORD_BUFFER			3
//Accuracy Constants
#define NURBSSURFACE_INVERSION_MAX_ITERATIONS	12
#define NURBSSURFACE_EPSILON_ONE				0.0001
#define NURBSSURFACE_EPSILON_TWO				0.0001
#define NURBSSURFACE_EQUALITY_EPSILON			0.001
#define NURBSSURFACE_AREA_ACCURACY				0.001
#define NURBSSURFACE_GENERATE_ACCURACY			0.15
//Performance Levels
#define NURBSSURFACE_PERFLEVEL_HIGH				0
#define NURBSSURFACE_PERFLEVEL_MEDIUM			1
#define NURBSSURFACE_PERFLEVEL_LOW				2
//Other Definitions
#define NURBSSURFACE_SIZE4_CUTOFF				225


/***********************************************~***************************************************/


class WCNurbsSurface : public WCGeometricSurface {
protected:
	//Basic data
	WPUInt										_lodU, _lodV, _numVerts;							//!< Values for LOD calculations
	WPUInt										_degreeU, _degreeV;									//!< Degree in U and V directions
	WCNurbsMode									_modeU, _modeV;										//!< NURBS knot mode for U and V directions
	WPUInt										_cpU, _cpV;											//!< Number of control points in U and V directions
	WCCollection<WCControlPoint*>				_cpCollection;										//!< Collection of pointers to control points
	WPUInt										_kpU, _kpV;											//!< Number of knot points in U and V directions
	WPFloat										*_knotPointsU, *_knotPointsV;						//!< Arrays of knot point values
	GLuint										_buffers[4];										//!< Data buffers for vertex, normal, index, and texcoords
	
protected:
	//Static Initialization Variables
	static WPUInt								_count;												//!< Static class instance count
	static WPUInt								_perfLevel;											//!< Static performance level indicator
	static GLint								*_locations;										//!< Locations for the bindable uniform variables	
	static GLuint								_default, _default23, _bezier23;					//!< Static shader programs for vertex generation	
	//PerfLevel Generation Statics
	static int									_minCPBufferSize, _minKPBufferSize;					//!< Values for the buffer sizes of CP and KP in shaders
	static GLint								_vertsPerBatch;										//!< Number of vertices per batch
	static GLintptr								_kpStride;											//!< Stride of the kp
	static GLuint								_cpBuffer, _kpUBuffer, _kpVBuffer;					//!< High uniform buffers for generation
	static GLuint								_cpTex, _kpUTex, _kpVTex, _inTex, _vertTex, _normTex;//!< Texture objects for use in medium generate
	static GLint								_maxTexSize;										//!< Medium maxium texture size	
	static GLuint								_framebuffer;										//!< Framebuffer for generate use
	
	//Private Static Functions
	static WPUInt Start(void);																		//!< Static class initializer
	static WPUInt Stop(void);																		//!< Static class deinitializer

	//Private Functions
	void ValidateClosure(void);																		//!< Check the closure of the surface
	void ValidateSelfIntersection(void);															//!< Check the self-intersection of the surface	
	void GenerateKnotPointsVBOs(void);																//!< Generate the knot points VBO
	void GenerateKnotPointsTextures(void);															//!< Generate the knot point textures
	void GenerateControlPointsVBO(void);															//!< Generate the control points VBO
	void GenerateControlPointsTexture(void);														//!< Generate the knot points texture	
	void LoadKnotPoints(const WCCollection<WPFloat> *uColl=NULL, const WCCollection<WPFloat> *vColl=NULL);//!< Create all knot point structures
	void LoadControlPoints(const WCCollection<WCControlPoint*> *controlPoints);						//!< Alternate to using data arrays	
				
	//Visualization Functions
	void GenerateSurfaceHigh(void);																	//!< Generate GL using High perf level
	void GenerateSurfaceMedium(void);																//!< Generate GL using Medium perf level
	void GenerateSurfaceLow(void);																	//!< Generate GL using Low perf level		
	void GenerateSurfaceOne(void);																	//!< Generate GL for 1st degree surfaces
	void GenerateSurfaceSize4(void);																//!< Generate GL for surf w/ 4 cp
	void GenerateIndex(void);																		//!< Generate GL array index data
	
private:	
	//Access Denied
	WCNurbsSurface();																				//!< Deny access to default constructor
	WPUInt LevelOfDetail(const WPUInt lod) { return 0; }											//!< Deny access - not well inherited
	WPUInt LevelOfDetail(void) { return 0; }														//!< Deny access - not well inherited
public:
	//Constructors and Destructors
	WCNurbsSurface(const WPUInt degreeU, const WPUInt degreeV,										//!< Primary constructor
					const WPUInt cpU, const WPUInt cpV, const WCCollection<WCControlPoint*> *cpCollection,
					const WCNurbsMode &modeU, const WCNurbsMode &modeV,
					const WCCollection<WPFloat> *knotPointsU=NULL, const WCCollection<WPFloat> *knotPointsV=NULL); 				
	WCNurbsSurface(const WCNurbsSurface &surf);														//!< Copy constructor				
	~WCNurbsSurface();																				//!< Default destructor
	
	//General Access Functions
	inline GLuint VertexBuffer(void) const		{ return this->_buffers[NURBSSURFACE_VERTEX_BUFFER]; }//!< Return ID for vertex buffer
	inline WPUInt NumberControlPointsU(void) const{ return this->_cpU; }							//!< Get the number of control points
	inline WPUInt NumberControlPointsV(void) const{ return this->_cpV; }							//!< Get the number of control points	
	inline WCCollection<WCControlPoint*> *ControlPoints(void)										//!< Return a pointer to the cp collection
												{ return &(this->_cpCollection); }
	inline WPUInt NumberKnotPointsU(void) const	{ return this->_kpU; }								//!< Get the number of knot points for U
	inline WPUInt NumberKnotPointsV(void) const	{ return this->_kpV; }								//!< Get the number of knot points for V
	inline WCNurbsMode ModeU(void) const		{ return this->_modeU; }							//!< Get the knot mode for U
	inline WCNurbsMode ModeV(void) const		{ return this->_modeV; }							//!< Get the knot mode for V
	void Degree(const WPUInt degreeU, const WPUInt degreeV);										//!< Set the U and V degrees
	WPUInt DegreeU(void) const					{ return this->_degreeU; }							//!< Get the U degree of the curve	
	WPUInt DegreeV(void) const 					{ return this->_degreeV; }							//!< Get the V degree of the curve	
	void LevelOfDetail(const WPUInt lodU, const WPUInt lodV);										//!< Set the Level-Of-Detail value
	WPUInt LevelOfDetailU(void) const			{ return this->_lodU; }								//!< Get the Level-Of-Detail value	
	WPUInt LevelOfDetailV(void) const			{ return this->_lodV; }								//!< Get the Level-Of-Detail value		
	
	//Inherited Member Functions
	void IsSelected(const bool status);																//!< Set the selection flag
	void IsMouseOver(const bool status);															//!< Set the mouse over flag	
	bool Intersect(const WCGeometricPoint &point, const WPFloat tolerance=NURBSSURFACE_EQUALITY_EPSILON);	//!< Check for intersection with point
	bool Intersect(const WCGeometricCurve &curve, const WPFloat tolerance=NURBSSURFACE_EQUALITY_EPSILON);	//!< Check for intersection with curve	
	bool Intersect(const WCGeometricSurface &surface, const WPFloat tolerance=NURBSSURFACE_EQUALITY_EPSILON);//!< Check for intersection with surface		
	WPFloat Area(void);																				//!< Return the area of the surface
	WCVector4 Evaluate(const WPFloat u, const WPFloat v);											//!< Evaluate a specific point on the surface
	WCVisualObject* HitTest(const WCRay &ray, const WPFloat tolerance);								//!< Hit test with a ray	
	void ApplyTransform(const WCMatrix4 &transform);												//!< Apply a transform to the surface
	void ApplyTranslation(const WCVector4 &translation);											//!< Apply a linear translation to the object
	void Render(const GLuint defaultProg=0, const WCColor color=WCColor::Default());				//!< Render the surface
	void ReceiveNotice(WCObjectMsg msg, WCObject *sender);											//!< Receive messages from other objects

	//Original Member Functions
	WCVector4 Derivative(const WPFloat u, const WPUInt uDer, const WPFloat v, const WPUInt vDer);	//!< Get the ith, jth derivative of the surface at a u,v
	WCRay Tangent(const WPFloat u, const WPFloat v);												//!< Get a tangential ray from the surface at u,v
	WCVector4 PointInversion(const WCGeometricPoint &point);										//!< Project from point to closest location on surface
	
	//Knot Insertion and Degree Elevation - Does not Alter Curve Shape
	void InsertKnotU(const WPFloat u, const WPUInt multiplicity=1);									//!< Insert a knot a parametric value u
	void InsertKnotV(const WPFloat v, const WPUInt multiplicity=1);									//!< Insert a knot a parametric value v	
	void RefineKnot(void);																			//!< Refine the curve with multiple knot insertions
	void RemoveKnot(void);																			//!< Remove knots from the curve
	void ElevateDegree(void);																		//!< Elevate the degree of the curve
	void ReduceDegree(void);																		//!< 
	
	//Operator Overloads
	WCNurbsSurface& operator=(const WCNurbsSurface &surface);										//!< Equals operator
	bool operator==(const WCNurbsSurface &surface);													//!< Equality operator
	
	//Serialization and Object Methods
	DOMElement* Serialize(DOMDocument *document);													//!< Serialize the object
	static WCNurbsSurface* Deserialize(DOMElement* obj);											//!< Deserialize the object
	bool Validate(DOMElement* obj);																	//!< Validate the object

	/*** Static Creation Methods ***/
	static WCNurbsSurface* ExtrudeCurve(WCGeometricCurve* curve, const WCVector4 &direction,		//!< Extrude a geometric curve into a surface
												const WPFloat &posDepth, const WPFloat &negDepth, const bool &aligned);

	/*** Non-Member Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCNurbsSurface &surface);				//!< Overloaded output operator
	friend std::ostream& operator<<(std::ostream& out, const WCNurbsSurface *surface) 				//!< Overloaded output operator		
												{ return out << *surface; }
};
 

/***********************************************~***************************************************/


#endif //__NURBS_SURFACE_H__

