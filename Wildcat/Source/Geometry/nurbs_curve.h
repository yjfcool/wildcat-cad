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


#ifndef __NURBS_CURVE_H__
#define __NURBS_CURVE_H__


/*** Included Header Files ***/
#include "Geometry/wgeol.h"
#include "Geometry/geometric_types.h"


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
class WCGeometryContext;


/***********************************************~***************************************************/


class WCNurbsCurve : public WCGeometricCurve {
protected:
	//Basic data
	WCGeometryContext							*_context;											//!< Geometry context
	WPUInt										_degree;											//!< Degree of the curve
	WCNurbsMode									_mode;												//!< NURBS knot mode
	WPUInt										_cp, _kp;											//!< Number of control and knot points, N value
	std::vector<WCVector4>						_controlPoints;										//!< Vector of control point vector4s
	WPFloat										*_knotPoints;										//!< Array of knot point values
	GLuint										_buffer;											//!< Vertex buffer
	
private:
	//Private Methods
	void GenerateKnotPointsVBO(void);																//!< Generate the knot points VBO	
	void GenerateKnotPointsTexture(void);															//!< Gerenate the knot points texture
	void GenerateControlPointsVBO(void);															//!< Generate the control points VBO
	void GenerateControlPointsTexture(void);														//!< Generate the knot points texture
	void LoadKnotPoints(const std::vector<WPFloat> &knotPoints=std::vector<WPFloat>());				//!< Create all knot point structures
	
	//Curve Generation Methods
	void GenerateCurveHigh(void);																	//!< Generate GL using High perf level
	void GenerateCurveMedium(void);																	//!< Generate GL using Medium perf level
	void GenerateCurveLow(void);																	//!< Generate GL using Low perf level		
	void GenerateCurveOne(void);																	//!< For 1st degree curves
	
	//Access Denied
	WCNurbsCurve();																					//!< Deny access to default constructor
public:
	//Constructors and Destructors
	WCNurbsCurve(WCGeometryContext *context, const WPUInt &degree,									//!< Primary constructor
												const std::vector<WCVector4> &controlPoints, const WCNurbsMode &mode, 
												const std::vector<WPFloat> &knotPoints=std::vector<WPFloat>());
	WCNurbsCurve(const WCNurbsCurve &curve);														//!< Copy constructor
	WCNurbsCurve(xercesc::DOMElement *element, WCSerialDictionary *dictionary);						//!< Persistance constructor
	~WCNurbsCurve();																				//!< Default destructor
	
	//General Access Functions
	GLuint VertexBuffer(void);																		//!< Return ID for vertex buffer
	inline std::vector<WCVector4> ControlPoints(void)	{ return this->_controlPoints; }			//!< Get the control points vector
	void ControlPoints(const std::vector<WCVector4> &controlPoints);								//!< Set the control points vector
	inline WPUInt NumberControlPoints(void) const{ return this->_cp; }								//!< Get the number of control points
	inline WPFloat* KnotPoints(void)			{ return this->_knotPoints; }						//!< Get the array of knot points
	void KnotPoints(const std::vector<WPFloat> &knotPoints);										//!< Set the knot points vector
	inline WPUInt NumberKnotPoints(void) const	{ return this->_kp; }								//!< Get the number of knot points
	inline WCNurbsMode Mode(void) const			{ return this->_mode; }								//!< Get the knot mode	
	WPUInt Degree(const WPUInt &degree);															//!< Set the degree of the curve
	inline WPUInt Degree(void)	const			{ return this->_degree; }							//!< Get the degree of the curve		
	WPUInt LevelOfDetail(const WPUInt &lod);														//!< Set the Level-Of-Detail value
	inline WPUInt LevelOfDetail(void) const		{ return this->_lod; }								//!< Get the Level-Of-Detail value	
	
	//Inherited Member Functions
	virtual std::list<WPFloat> Intersect(WCGeometricPoint *point, const WPFloat &tol);				//!< Check for intersection with point
	virtual std::list<WPIntersectRec> Intersect(WCGeometricCurve *curve, const WPFloat &tol);		//!< Check for intersection with curve
	WPFloat Length(const WPFloat &tolerance=NURBSCURVE_LENGTH_ACCURACY);							//!< Calculate the length of the curve
	WCVector4 Evaluate(const WPFloat &u);															//!< Evaluate a specific point on the curve
	WCVisualObject* HitTest(const WCRay &ray, const WPFloat &tolerance);							//!< Hit test with a ray
	void ApplyTransform(const WCMatrix4 &transform);												//!< Apply a transform to the curve
	void ApplyTranslation(const WCVector4 &translation);											//!< Apply a linear translation to the object
	void Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom);				//!< Render the object
	void ReceiveNotice(WCObjectMsg msg, WCObject *sender);											//!< Receive messages from other objects

	//Original Member Functions
	WCVector4 Derivative(const WPFloat &u, const WPUInt &der);										//!< Evaluate the derivative at a specific point
	WCRay Tangent(const WPFloat &u);																//!< Get the tangent to the curve at U
	WCVector4 PointInversion(const WCVector4 &point, WPFloat &u=0.0);								//!< Get the closest point on the curve from the given point
	
	//Knot Insertion and Degree Elevation - Does Not Alter Curve Shape
	void InsertKnot(const WPFloat &u, const WPUInt &multiplicity=1);								//!< Insert a knot at parametric value u
	void RefineKnot(void);																			//!< Refine the curve with multiple knot insertions
	void RemoveKnot(void);																			//!< Remove knots from the curve
	void ElevateDegree(void);																		//!< Elevate the degree of the curve
	void ReduceDegree(void);																		//!< Reduce the degree of the curve 	
	
	//Operator Overloads
	WCNurbsCurve& operator=(const WCNurbsCurve &curve);												//!< Equals operator
	bool operator==(const WCNurbsCurve &curve);														//!< Equality operator
	
	//Serialization and Object Methods
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
	
	/*** Static Creation Methods ***/
	static WCNurbsCurve* CircularArc(WCGeometryContext *context, const WCVector4 &center,			//!< Generate a circular arc curve
												const WCVector4 &xUnit, const WCVector4 &yUnit, const WPFloat &radius, 
												const WPFloat &startAngleDeg, const WPFloat &endAngleDeg);
	static WCNurbsCurve* Conic(WCGeometryContext *context, const WCVector4 &first,					//!< Generate a conic section
												const WCVector4 &firstTangent, const WCVector4 &second, const WCVector4 &secondTangent,
												const WCVector4 &third);
	static WCNurbsCurve* Ellipse(WCGeometryContext *context, const WCVector4 &center,				//!< Generate an ellipse with major and minor axis
												const WCVector4 &xUnit, const WCVector4 &yUnit, const WPFloat &major, const WPFloat &minor);
	
	/*** Non-Member Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCNurbsCurve &curve);					//!< Overloaded output operator
};
 

/***********************************************~****************************************************/


#endif //__NURBS_CURVE_H__

