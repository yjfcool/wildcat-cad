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


/*** Included Header Files ***/
#include <Geometry/geometric_intersection.h>
#include <Geometry/geometry_context.h>
#include <Geometry/geometric_point.h>
#include <Geometry/geometric_line.h>
#include <Geometry/nurbs_curve.h>
#include <Geometry/nurbs_surface.h>


/***********************************************~***************************************************/


std::list<WCIntersectionResult> __WILDCAT_NAMESPACE__::GeometricIntersection(WCNurbsSurface *left,
	WCGeometricPoint *right, const WPFloat &tol, const unsigned int &flags) {
	std::list<WCIntersectionResult> results;
	//See if point is within surface bounding box
	if (left->BoundingBox().Intersection(right->BoundingBox())) return results;

	//Determine estimated area of surface and LODs
	WPUInt lodU = STDMIN((WPUInt)(WCNurbs::EstimateLengthU(left->ControlPoints(), left->NumberControlPointsU()) / tol), (WPUInt)WCAdapter::GetMax2DTextureSize());
	WPUInt lodV = STDMIN((WPUInt)(WCNurbs::EstimateLengthV(left->ControlPoints(), left->NumberControlPointsV()) / tol), (WPUInt)WCAdapter::GetMax2DTextureSize());
	//Generate surface to precision of tol and place into local buffer
	std::vector<GLfloat*> buffers = left->GenerateClientBuffers(0.0, 1.0, lodU, 0.0, 1.0, lodV, true);
	GLfloat* vertexBuffer = buffers.at(NURBSSURFACE_VERTEX_BUFFER);
	GLuint* indexBuffer = (GLuint*)buffers.at(NURBSSURFACE_INDEX_BUFFER);

	//Determine number of triangles in the surface
	WCVector4 p = right->Data();
	WPUInt indexA, indexB, indexC;
	WCVector4 v0, v1, v2, v1v0, v2v0, d, dNorm, pvec, tvec, qvec;
	WPFloat det, u, v, t;
	WPUInt numTris = (lodU - 1) * (lodV - 1) * 2;
	//Compare point against each triangle within buffer
	for (WPUInt triIndex=0; triIndex < numTris; triIndex++) {
		//Get the indices for the three vertices
		indexA = indexBuffer[triIndex];
		indexB = indexBuffer[triIndex+1];
		indexC = indexBuffer[triIndex+2];
		//Get the vertex values for the triangle
		v0 = WCVector4(vertexBuffer[indexA]);
		v1 = WCVector4(vertexBuffer[indexB]);
		v2 = WCVector4(vertexBuffer[indexC]);

		//Get vXv0 vectors and normal
		v1v0 = v1 - v0;
		v2v0 = v2 - v0;
		d = v1v0.CrossProduct(v2v0);
		dNorm = d;
		dNorm.Normalize();

		//Calculate determinant
		pvec = d.CrossProduct(v2v0);
		det = pvec.DotProduct(v1v0);
		//Get tvec and U
		tvec = p - v0;
		u = tvec.DotProduct(pvec);
		//Make sure u is within bounds
		if (u >= 0.0 && u <= det) {
			qvec = tvec.CrossProduct(v1v0);
			//Calculate and bounds test v
			v = d.DotProduct(qvec);
			if (v >= 0.0 && u + v <= det) {
				//Calculate t and scale u and v
				t = v2v0.DotProduct(qvec);
				det = 1.0 / det;
				t *= det;
				//Make sure t is very small
				//...
				u *= det;
				v *= det;
				//Convert u and v into surface u and v values
				//...

				//Create intersection result for the point
				WCIntersectionResult hit;
				hit.type = IntersectPoint;
//				hit.leftBoundary = (u < tol) || (1.0 - u < tol);
//				hit.rightBoundary = true;
//				hit.leftParam.I(u);
//				hit.rightParam = right->Data();
				//Do boundary cull check, insert if ok
				if (!(flags * INTERSECT_CULL_BOUNDARY) || !hit.leftBoundary) {
					//Add the hit to the list
					results.push_back(hit);
				}
			}
		}
	}

	//Release the surface buffers
	left->ReleaseBuffers(buffers);
	//Return the results
	return results;
}


std::list<WCIntersectionResult> __WILDCAT_NAMESPACE__::GeometricIntersection(WCNurbsSurface *left,
	WCGeometricLine *right, const WPFloat &tol, const unsigned int &flags) {
	std::list<WCIntersectionResult> results;
	//See if line intersects surface bounding box
	if (left->BoundingBox().Intersection(right->BoundingBox())) return results;
	
	//Determine estimated area of surface and LODs
	WPUInt lodU = STDMIN((WPUInt)(WCNurbs::EstimateLengthU(left->ControlPoints(), left->NumberControlPointsU()) / tol), (WPUInt)WCAdapter::GetMax2DTextureSize());
	WPUInt lodV = STDMIN((WPUInt)(WCNurbs::EstimateLengthV(left->ControlPoints(), left->NumberControlPointsV()) / tol), (WPUInt)WCAdapter::GetMax2DTextureSize());
	//Generate surface to precision of tol and place into local buffer
	std::vector<GLuint> textures;
	left->GenerateTextureBuffers(0.0, 1.0, lodU, 0.0, 1.0, lodV, textures, true);
	GLuint vertexTex = textures.at(NURBSSURFACE_VERTEX_BUFFER);
	GLuint indexTex = textures.at(NURBSSURFACE_INDEX_BUFFER);
	if (vertexTex == indexTex) { }
	//Set program values
	glUseProgram(left->Context()->SurfaceLineProgram());
	glUniform3f(left->Context()->IntersectionLocations()[INTERSECTION_SLI_PARAMS], (GLfloat)lodU, (GLfloat)lodV, (GLfloat)tol);
	glUniform4f(left->Context()->IntersectionLocations()[INTERSECTION_SLI_LBEGIN], 
				(GLfloat)right->Begin().I(), (GLfloat)right->Begin().J(), (GLfloat)right->Begin().K(), 1.0);
	glUniform4f(left->Context()->IntersectionLocations()[INTERSECTION_SLI_LEND],
				(GLfloat)right->End().I(), (GLfloat)right->End().J(), (GLfloat)right->End().K(), 1.0);

	//Try to intersect line with each triangle within buffer
	//...
	left->ReleaseTextures(textures);
	return results;
}


std::list<WCIntersectionResult> __WILDCAT_NAMESPACE__::GeometricIntersection(WCNurbsSurface *left,
	WCNurbsCurve *right, const WPFloat &tol, const unsigned int &flags) {
	std::list<WCIntersectionResult> results;
	//See if curve bounding box intersects surface bounding box
	if (left->BoundingBox().Intersection(right->BoundingBox())) return results;
	
	//Otherwise, generate surface to precision of tol and place into local buffer
	//...

	//Generate curve to precision of tol and place into local buffer
	//...
	
	//Try to intersect all sections of curve in buffer with each triangle in buffer
	//...
	
	return results;
}


std::list<WCIntersectionResult> __WILDCAT_NAMESPACE__::GeometricIntersection(WCNurbsSurface *left,
	WCNurbsSurface *right, const WPFloat &tol, const unsigned int &flags) {
	std::list<WCIntersectionResult> results;
	//See if left bounding box intersects right bounding box
	if (left->BoundingBox().Intersection(right->BoundingBox())) return results;
	
	//Otherwise, generate left and right to precision of tol and place into local buffers
	//...
	
	//Try to intersect all triangles of left with each triangle in right
	//...
	
	return results;
}


std::list<WCIntersectionResult> __WILDCAT_NAMESPACE__::GeometricIntersection(WCNurbsSurface *left,
	WCTrimmedNurbsSurface *right, const WPFloat &tol, const unsigned int &flags) {
	//Get intersection list
	std::list<WCIntersectionResult> results = GeometricIntersection(right, left, tol, flags);
	//Return the reversed the list
	return IntersectionReverse( results );
}


/***********************************************~***************************************************/

