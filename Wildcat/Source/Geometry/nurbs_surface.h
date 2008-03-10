/*******************************************************************************
* Copyright (c) 2007, 2008, CerroKai Development
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of CerroKai Development nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY CerroKai Development ``AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL CerroKai Development BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************************/


#ifndef __NURBS_SURFACE_H__
#define __NURBS_SURFACE_H__


/*** Included Header Files ***/
#include "Geometry/wgeol.h"
#include "Geometry/geometric_types.h"


/*** Local Defines ***/
//Size constants
#define NURBSSURFACE_MAX_LOD					512
#define NURBSSURFACE_MAX_DEGREE					3
#define NURBSSURFACE_MAX_CONTROLPOINTS			144
#define NURBSSURFACE_MAX_KNOTPOINTS				255
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


/*** Class Predefines ***/
class WCGeometryContext;


/***********************************************~***************************************************/


class WCNurbsSurface : public WCGeometricSurface {
protected:
	//Basic data
	WCGeometryContext							*_context;											//!< Geometry context
	WPUInt										_lodU, _lodV, _numVerts;							//!< Values for LOD calculations
	WPUInt										_degreeU, _degreeV;									//!< Degree in U and V directions
	WCNurbsMode									_modeU, _modeV;										//!< NURBS knot mode for U and V directions
	WPUInt										_cpU, _cpV;											//!< Number of control points in U and V directions
	std::vector<WCVector4>						_controlPoints;										//!< Vector of vectors for control points
	WPUInt										_kpU, _kpV;											//!< Number of knot points in U and V directions
	WPFloat										*_knotPointsU, *_knotPointsV;						//!< Arrays of knot point values
	GLuint										_buffers[4];										//!< Data buffers for vertex, normal, index, and texcoords
	
protected:
	//Private Functions
	void ValidateClosure(void);																		//!< Check the closure of the surface
	void ValidateSelfIntersection(void);															//!< Check the self-intersection of the surface	
	void GenerateKnotPointsVBOs(void);																//!< Generate the knot points VBO
	void GenerateKnotPointsTextures(void);															//!< Generate the knot point textures
	void GenerateControlPointsVBO(void);															//!< Generate the control points VBO
	void GenerateControlPointsTexture(void);														//!< Generate the knot points texture	
	void LoadKnotPoints(const std::vector<WPFloat> &uKP=std::vector<WPFloat>(),						//!< Create all knot point structures
												const std::vector<WPFloat> &vKP=std::vector<WPFloat>());
				
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
	WCNurbsSurface(WCGeometryContext *context, const WPUInt &degreeU, const WPUInt &degreeV,		//!< Primary constructor
					const WPUInt &cpU, const WPUInt &cpV, const std::vector<WCVector4> &controlPoints,
					const WCNurbsMode &modeU, const WCNurbsMode &modeV,
					const std::vector<WPFloat> &knotPointsU=std::vector<WPFloat>(), const std::vector<WPFloat> &knotPointsV=std::vector<WPFloat>());
	WCNurbsSurface(const WCNurbsSurface &surf);														//!< Copy constructor
	WCNurbsSurface(xercesc::DOMElement *element, WCSerialDictionary *dictionary);					//!< Persistance constructor
	~WCNurbsSurface();																				//!< Default destructor
	
	//General Access Functions
	GLuint VertexBuffer(void);																		//!< Return ID for vertex buffer
	inline WPUInt NumberControlPointsU(void) const{ return this->_cpU; }							//!< Get the number of control points
	inline WPUInt NumberControlPointsV(void) const{ return this->_cpV; }							//!< Get the number of control points	
	inline std::vector<WCVector4> ControlPoints(void) { return this->_controlPoints; }				//!< Return a copy of the cp collection
	inline WPUInt NumberKnotPointsU(void) const	{ return this->_kpU; }								//!< Get the number of knot points for U
	inline WPUInt NumberKnotPointsV(void) const	{ return this->_kpV; }								//!< Get the number of knot points for V
	inline WCNurbsMode ModeU(void) const		{ return this->_modeU; }							//!< Get the knot mode for U
	inline WCNurbsMode ModeV(void) const		{ return this->_modeV; }							//!< Get the knot mode for V
	void Degree(const WPUInt &degreeU, const WPUInt &degreeV);										//!< Set the U and V degrees
	WPUInt DegreeU(void) const					{ return this->_degreeU; }							//!< Get the U degree of the curve	
	WPUInt DegreeV(void) const 					{ return this->_degreeV; }							//!< Get the V degree of the curve	
	void LevelOfDetail(const WPUInt &lodU, const WPUInt &lodV);										//!< Set the Level-Of-Detail value
	WPUInt LevelOfDetailU(void) const			{ return this->_lodU; }								//!< Get the Level-Of-Detail value	
	WPUInt LevelOfDetailV(void) const			{ return this->_lodV; }								//!< Get the Level-Of-Detail value		
	
	//Inherited Member Functions
	bool Intersect(const WCGeometricPoint &point, const WPFloat &tolerance=NURBSSURFACE_EQUALITY_EPSILON);	//!< Check for intersection with point
	bool Intersect(const WCGeometricCurve &curve, const WPFloat &tolerance=NURBSSURFACE_EQUALITY_EPSILON);	//!< Check for intersection with curve	
	bool Intersect(const WCGeometricSurface &surface, const WPFloat &tolerance=NURBSSURFACE_EQUALITY_EPSILON);//!< Check for intersection with surface		
	WPFloat Area(void);																				//!< Return the area of the surface
	WCVector4 Evaluate(const WPFloat &u, const WPFloat &v);											//!< Evaluate a specific point on the surface
	WCVisualObject* HitTest(const WCRay &ray, const WPFloat &tolerance);							//!< Hit test with a ray	
	void ApplyTransform(const WCMatrix4 &transform);												//!< Apply a transform to the surface
	void ApplyTranslation(const WCVector4 &translation);											//!< Apply a linear translation to the object
	void Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom);				//!< Render the object
	void ReceiveNotice(WCObjectMsg msg, WCObject *sender);											//!< Receive messages from other objects

	//Original Member Functions
	WCVector4 Derivative(const WPFloat &u, const WPUInt &uDer, const WPFloat &v, const WPUInt &vDer);//!< Get the ith, jth derivative of the surface at a u,v
	WCRay Tangent(const WPFloat &u, const WPFloat &v);												//!< Get a tangential ray from the surface at u,v
	WCVector4 PointInversion(const WCVector4 &point);												//!< Project from point to closest location on surface
	
	//Knot Insertion and Degree Elevation - Does not Alter Surface Shape
	void InsertKnotU(const WPFloat &u, const WPUInt &multiplicity=1);								//!< Insert a knot a parametric value u
	void InsertKnotV(const WPFloat &v, const WPUInt &multiplicity=1);								//!< Insert a knot a parametric value v	
	void RefineKnot(void);																			//!< Refine the curve with multiple knot insertions
	void RemoveKnot(void);																			//!< Remove knots from the surface
	void ElevateDegree(void);																		//!< Elevate the degree of the surface
	void ReduceDegree(void);																		//!< Reduce the degree of the surface
	
	//Operator Overloads
	WCNurbsSurface& operator=(const WCNurbsSurface &surface);										//!< Equals operator
	bool operator==(const WCNurbsSurface &surface);													//!< Equality operator
	
	//Serialization and Object Methods
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object

	/*** Static Creation Methods ***/
	static WCNurbsSurface* ExtrudeCurve(WCGeometryContext *context, WCGeometricCurve* curve,		//!< Extrude a geometric curve into a surface
												const WCVector4 &direction,	const WPFloat &posDepth, const WPFloat &negDepth, const bool &aligned);
	static WCNurbsSurface* RevolveCurve(WCGeometryContext *context, WCGeometricCurve *curve, 		//!< Revolve a geometric curve into a surface
												const WCRay *axis, const WPFloat &angle, const bool &aligned);

	/*** Non-Member Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCNurbsSurface &surface);				//!< Overloaded output operator
};
 

/***********************************************~***************************************************/


#endif //__NURBS_SURFACE_H__

