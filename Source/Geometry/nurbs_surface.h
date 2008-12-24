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
#include <Geometry/wgeol.h>
#include <Geometry/geometric_types.h>


/*** Locally Defined Values ***/
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
#define NURBSSURFACE_LOC_PARAMS_DEFAULT23		9
#define NURBSSURFACE_LOC_PARAMSU_DEFAULT		10
#define NURBSSURFACE_LOC_PARAMSU_DEFAULT23		11
#define NURBSSURFACE_LOC_PARAMSU_BEZIER23		12
#define NURBSSURFACE_LOC_PARAMSV_DEFAULT		13
#define NURBSSURFACE_LOC_PARAMSV_DEFAULT23		14
#define NURBSSURFACE_LOC_PARAMSV_BEZIER23		15
//Buffer Constants
#define NURBSSURFACE_VERTEX_BUFFER				0
#define NURBSSURFACE_NORMAL_BUFFER				1
#define NURBSSURFACE_TEXCOORD_BUFFER			2
#define NURBSSURFACE_INDEX_BUFFER				3
//Accuracy Constants
#define NURBSSURFACE_AREA_ACCURACY				0.001
//Performance Levels
#define NURBSSURFACE_PERFLEVEL_HIGH				0
#define NURBSSURFACE_PERFLEVEL_MEDIUM			1
#define NURBSSURFACE_PERFLEVEL_LOW				2
//Other Definitions
#define NURBSSURFACE_SIZE4_CUTOFF				225


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCGeometryContext;


/***********************************************~***************************************************/


class WCNurbsSurface : public WCGeometricSurface {
protected:
	WPUInt										_degreeU, _degreeV;									//!< Degree in U and V directions
	WCNurbsMode									_modeU, _modeV;										//!< NURBS knot mode for U and V directions
	WPUInt										_cpU, _cpV;											//!< Number of control points in U and V directions
	std::vector<WCVector4>						_controlPoints;										//!< Vector of vectors for control points
	WPUInt										_kpU, _kpV;											//!< Number of knot points in U and V directions
	WPFloat										*_knotPointsU, *_knotPointsV;						//!< Arrays of knot point values
	WPFloat										_lengthU, _lengthV;									//!< Estimated length along u and v axis
	WPUInt										_lodU, _lodV;										//!< Values for LOD calculations
	std::vector<GLuint>							_buffers;											//!< Data buffers - GPU for vertex, normal, index, and texcoords
	std::vector<GLfloat*>						_altBuffers;										//!< Data buffers - CPU for vertex, normal, index, and texcoords
private:
	//Private Methods
	void ValidateClosure(void);																		//!< Check the closure of the surface
	void ValidateSelfIntersection(void);															//!< Check the self-intersection of the surface
	void ValidatePlanar(void);																		//!< Check the planar property of the surface
	void GenerateKnotPointsVBOs(void);																//!< Generate the knot points VBO
	void GenerateKnotPointsTextures(void);															//!< Generate the knot point textures
	void GenerateControlPointsVBO(void);															//!< Generate the control points VBO
	void GenerateControlPointsTexture(void);														//!< Generate the knot points texture	
	void LoadKnotPoints(const std::vector<WPFloat> &uKP=std::vector<WPFloat>(),						//!< Create all knot point structures
												const std::vector<WPFloat> &vKP=std::vector<WPFloat>());
	//Surface Generation Methods
	std::vector<GLfloat*> GenerateSurfaceHigh(const WPFloat &uStart, const WPFloat &uStop, const WPUInt &lodU,		//!< Generate GL using High perf level
												const WPFloat &vStart, const WPFloat &vStop, const WPUInt &lodV,
												const bool &server, std::vector<GLuint> &buffers);
	std::vector<GLfloat*> GenerateSurfaceMedium(const WPFloat &uStart, const WPFloat &uStop, const WPUInt &lodU,	//!< Generate GL using Medium perf level
												const WPFloat &vStart, const WPFloat &vStop, const WPUInt &lodV,
												const bool &server, std::vector<GLuint> &buffers);
	std::vector<GLfloat*> GenerateSurfaceLow(const WPFloat &uStart, const WPFloat &uStop, const WPUInt &lodU,		//!< Generate GL using Low perf level
											 const WPFloat &vStart, const WPFloat &vStop, const WPUInt &lodV,
												const bool &server, std::vector<GLuint> &buffers);
	std::vector<GLfloat*> GenerateSurfaceSize4(const WPFloat &uStart, const WPFloat &uStop, const WPUInt &lodU,		//!< Generate GL for surf w/ 4 cp
											   const WPFloat &vStart, const WPFloat &vStop, const WPUInt &lodV,
												const bool &server, std::vector<GLuint> &buffers);
	GLuint* GenerateIndex(const WPFloat &uStart, const WPFloat &uStop, const WPUInt &lodU,			//!< Generate GL array index data
												const WPFloat &vStart, const WPFloat &vStop, const WPUInt &lodV, 
												const bool &server, GLuint &buffer);
	//Hidden Constructors
	WCNurbsSurface();																				//!< Deny access to default constructor
public:
	//Constructors and Destructors
	WCNurbsSurface(WCGeometryContext *context, const WPUInt &degreeU, const WPUInt &degreeV,		//!< Primary constructor
					const WPUInt &cpU, const WPUInt &cpV, const std::vector<WCVector4> &controlPoints,
					const WCNurbsMode &modeU, const WCNurbsMode &modeV,
					const std::vector<WPFloat> &knotPointsU=std::vector<WPFloat>(), const std::vector<WPFloat> &knotPointsV=std::vector<WPFloat>());
	WCNurbsSurface(const WCNurbsSurface &surface);													//!< Copy constructor
	WCNurbsSurface(xercesc::DOMElement *element, WCSerialDictionary *dictionary);					//!< Persistance constructor
	virtual ~WCNurbsSurface();																		//!< Default destructor
	
	//General Access Methods
	inline std::vector<WCVector4> ControlPoints(void){ return this->_controlPoints; }				//!< Get the control points
	void ControlPoints(const std::vector<WCVector4> &controlPoints);								//!< Set the control points
	inline WPUInt NumberControlPointsU(void) const	{ return this->_cpU; }							//!< Get the number of control points
	inline WPUInt NumberControlPointsV(void) const	{ return this->_cpV; }							//!< Get the number of control points	
	inline WPFloat* KnotPointsU(void)				{ return this->_knotPointsU; }					//!< Get the U knot points
	inline WPFloat* KnotPointsV(void)				{ return this->_knotPointsV; }					//!< Get the V knot points
	void KnotPointsU(const std::vector<WPFloat> &knotPointsU);										//!< Set the U knot points
	void KnotPointsV(const std::vector<WPFloat> &knotPointsV);										//!< Set the V knot points
	inline WPUInt NumberKnotPointsU(void) const		{ return this->_kpU; }							//!< Get the number of knot points for U
	inline WPUInt NumberKnotPointsV(void) const		{ return this->_kpV; }							//!< Get the number of knot points for V
	inline WCNurbsMode ModeU(void) const			{ return this->_modeU; }						//!< Get the knot mode for U
	inline WCNurbsMode ModeV(void) const			{ return this->_modeV; }						//!< Get the knot mode for V
	void Degree(const WPUInt &degreeU, const WPUInt &degreeV);										//!< Set the U and V degrees
	inline WPUInt DegreeU(void) const				{ return this->_degreeU; }						//!< Get the U degree of the curve	
	inline WPUInt DegreeV(void) const				{ return this->_degreeV; }						//!< Get the V degree of the curve	
	
	//Inherited Member Methods
	virtual WPFloat Area(const WPFloat &tolerance=GEOMETRICOBJECT_DEFAULT_EPSILON);					//!< Return the area of the surface
	virtual WCVector4 Evaluate(const WPFloat &u, const WPFloat &v);									//!< Evaluate a specific point on the surface
	virtual WCVector4 Derivative(const WPFloat &u, const WPUInt &uDer,								//!< Get the surface derivative
												const WPFloat &v, const WPUInt &vDer);
	virtual WCRay Tangent(const WPFloat &u, const WPFloat &v);										//!< Get a tangential ray from the surface at u,v
	virtual std::pair<WCVector4,WCVector4> PointInversion(const WCVector4 &point);					//!< Project from point to closest location on surface
	virtual WCVisualObject* HitTest(const WCRay &ray, const WPFloat &tolerance);					//!< Hit test with a ray	
	virtual void ApplyTransform(const WCMatrix4 &transform);										//!< Apply a transform to the surface
	virtual void ApplyTranslation(const WCVector4 &translation);									//!< Apply a linear translation to the object
	virtual void Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom);		//!< Render the object
	virtual void ReceiveNotice(WCObjectMsg msg, WCObject *sender);									//!< Receive messages from other objects

	//Buffer Generation Methods
	std::vector<GLfloat*> GenerateClientBuffers(const WPFloat &uStart, const WPFloat &uStop, WPUInt &lodU,	//!< Generate uo to LOD (vert, tex, norm, index) - put in RAM
												const WPFloat &vStart, const WPFloat &vStop, WPUInt &lodV, const bool &managed);
	void ReleaseBuffers(std::vector<GLfloat*> &buffers);											//!< Manage the release of buffer resources
	void GenerateServerBuffers(const WPFloat &uStart, const WPFloat &uStop, WPUInt &lodU,			//!< Generate uo to LOD (vert, tex, norm, index) - put in VRAM
													const WPFloat &vStart, const WPFloat &vStop, WPUInt &lodV,
													std::vector<GLuint> &buffers, const bool &managed);
	void ReleaseBuffers(std::vector<GLuint> &buffers);												//!< Manage the release of buffer resources
	void GenerateTextureBuffers(const WPFloat &uStart, const WPFloat &uStop, WPUInt &lodU,			//!< Generate a texture buffer of the surface
													const WPFloat &vStart, const WPFloat &vStop, WPUInt &lodV,
													std::vector<GLuint> &textures, const bool &managed);
	void ReleaseTextures(std::vector<GLuint> &buffers);												//!< Manage the release of texture buffer resource

	//Original Member Methods
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
	virtual xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);//!< Serialize the object

	/*** Static Creation Methods ***/
	static WCNurbsSurface* ExtrudeCurve(WCGeometryContext *context, WCGeometricCurve* curve,		//!< Extrude a geometric curve into a surface
												const WCVector4 &direction,	const WPFloat &posDepth, const WPFloat &negDepth, const bool &aligned);
	static WCNurbsSurface* RevolveCurve(WCGeometryContext *context, WCGeometricCurve *curve, 		//!< Revolve a geometric curve into a surface
												const WCRay *axis, const WPFloat &angle, const bool &aligned);

	/*** Non-Member Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCNurbsSurface &surface);				//!< Overloaded output operator
};
 

/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__NURBS_SURFACE_H__

