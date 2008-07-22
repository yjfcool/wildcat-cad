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
#include "Geometry/geometric_intersection.h"
#include "Geometry/geometric_point.h"
#include "Geometry/geometric_line.h"


/***********************************************~***************************************************/


std::list<WCIntersectionResult> __WILDCAT_NAMESPACE__::GeometricIntersection(WCGeometricLine *left,
	WCGeometricPoint *right, const WPFloat &tol, const unsigned int &flags) {
	std::list<WCIntersectionResult> results;
	//Get line direction vector
	WCVector4 direction = left->End() - left->Begin();
	//Get the length of the direction vector
	WPFloat dirLen = direction.Magnitude();
	//Get line-being to point vector
	WCVector4 pointVector = right->Data() - left->Begin();
	WPFloat u = direction.DotProduct(pointVector) / (dirLen * dirLen);

	//Bounds check u for [0, 1] - return empty if out of bounds
	if ((u < -tol) || (u > (1.0 + tol))) return results;

	//Do cull boundary check - return if true
	if ((flags & INTERSECT_CULL_BOUNDARY) && ((u < tol) || (1.0 - u < tol) )) return results;

	//Bound u [0.0 to 1.0]
	u = STDMAX( STDMIN(u, 1.0), 0.0);
	//Get the point on the line
	WCVector4 pt = left->Begin() + direction * u;
	//Get distance from point on line to point
	if (pt.Distance(right->Data()) <= tol) {
		//Create intesect result
		WCIntersectionResult hit;
		hit.type = IntersectPoint;
		hit.leftBoundary = (u < tol) || (1.0 - u < tol);
		hit.rightBoundary = true;
		hit.leftParam.I(u);
		hit.rightParam = right->Data();
		//See if genObj
		if (flags & INTERSECT_GEN_POINTS) {
			//Create new point
			WCGeometricPoint *newPoint = new WCGeometricPoint(pt);
			//Add to result struct
			hit.object = newPoint;
		}
		else hit.object = NULL;
		//Add result to the output list
		std::cout << hit << right->Data() << std::endl;
		results.push_back(hit);
	}
	//Return the results
	return results;
}


std::list<WCIntersectionResult> __WILDCAT_NAMESPACE__::GeometricIntersection(WCGeometricLine *left,
	WCGeometricLine *right, const WPFloat &tol, unsigned const int &flags) {
	std::list<WCIntersectionResult> results;
	//Check if self intersecting
	if (left == right) return results;

	//Evaluate both lines at both ends
	WCVector4 p1 = left->Begin();
	WCVector4 p2 = left->End();
	WCVector4 p3 = right->Begin();
	WCVector4 p4 = right->End();
	
	//Get length vectors
	WCVector4 p21(p2 - p1);
	WCVector4 p43(p4 - p3);
	WPFloat dist;
	
	//Make sure both lines have some minimal length
//	if ((p21mag < tol) || (p43Mag < tol)) return results;
	
	//Check for parallel
	WCVector4 cross = p43.CrossProduct(p21);
	WPFloat denom = cross.Magnitude();
	WPFloat onePlusTol = 1.0 + tol;
	if (denom < 0.001) {
		//Determine perpendicular distance
		WPFloat p21mag = p21.Magnitude();
		WCVector4 p13(p1 - p3);
		WCVector4 num = p21.CrossProduct(p13);
		dist = num.Magnitude() / p21mag;
		//If outside of tolerance, no intersection
		if (dist <= tol) {
			//Project all four points onto opposite lines
			p21 /= p21mag;
			WPFloat p43mag = p43.Magnitude();
			p43 /= p43mag;
			WCVector4 p31(p3 - p1);
			WCVector4 p41(p4 - p1);
			WCVector4 p23(p2 - p3);
			WPFloat p1Proj = p43.DotProduct(p13) / p43mag;
			WPFloat p2Proj = p43.DotProduct(p23) / p43mag;
			WPFloat p3Proj = p21.DotProduct(p31) / p21mag;
			WPFloat p4Proj = p21.DotProduct(p41) / p21mag;

			//See if each point is in 0.0 to 1.0 +- tolerance
			bool p1In = (p1Proj > -tol) && (p1Proj < onePlusTol);
			bool p2In = (p2Proj > -tol) && (p2Proj < onePlusTol);
			bool p3In = (p3Proj > -tol) && (p3Proj < onePlusTol);
			bool p4In = (p4Proj > -tol) && (p4Proj < onePlusTol);
			//If none are in then no overlap, return
			if ( p1In || p2In || p3In || p4In) {
				//Bounds check all of the projections
				p1Proj = STDMAX(0.0, STDMIN(1.0, p1Proj));
				p2Proj = STDMAX(0.0, STDMIN(1.0, p2Proj));
				p3Proj = STDMAX(0.0, STDMIN(1.0, p3Proj));
				p4Proj = STDMAX(0.0, STDMIN(1.0, p4Proj));
				//Create hit result
				WCIntersectionResult hit;
				hit.type = IntersectLine;
				WCVector4 pStart, pEnd;
				//Case 1 - Left is completely within Right
				if (p1In && p2In) {
					//Set the start and end points
					pStart = p1;
					pEnd = p2;
					//Set the param values
					hit.leftParam.I( 0.0 );
					hit.leftParam.J( 1.0 );
					hit.rightParam.I( STDMIN(p1Proj,p2Proj) );
					hit.rightParam.J( STDMAX(p1Proj,p2Proj) );
					//Set boundary values
					hit.leftBoundary = true;
					hit.rightBoundary = false;
				}
				//Case 2 - Right is completely within Left
				else if (p3In && p4In) {
					//Set the start and end points
					pStart = p3;
					pEnd = p4;
					//Set the param values
					hit.leftParam.I( STDMIN(p3Proj,p4Proj) );
					hit.leftParam.J( STDMAX(p3Proj,p4Proj) );
					hit.rightParam.I( 0.0 );
					hit.rightParam.J( 1.0 );
					//Set boundary values
					hit.leftBoundary = false;
					hit.rightBoundary = true;
				}
				//Remaining Cases
				else {
					//Simple sets
					if (p1In) { hit.leftParam.I(0.0); pStart = p1; }
					if (p2In) { hit.leftParam.J(1.0); pStart = p2; }
					if (p3In) { hit.rightParam.I(0.0); pEnd = p3; }
					if (p4In) { hit.rightParam.J(1.0); pEnd = p4; }
					//Double sets
					if (p1In && p3In) { hit.leftParam.J(p3Proj); hit.rightParam.J(p1Proj); }
					if (p1In && p4In) { hit.leftParam.J(p4Proj); hit.rightParam.I(p1Proj); }
					if (p2In && p3In) { hit.leftParam.I(p3Proj); hit.rightParam.J(p2Proj); }
					if (p2In && p4In) { hit.leftParam.I(p4Proj); hit.rightParam.I(p2Proj); }
					
					//Check to see if intersection is a point (think of two lines end to end)
					if (pStart.Distance(pEnd) < tol) hit.type = IntersectPoint;

					//This is not the best way to check for CULL BOUNDARY, but if pStart and pEnd distance < tol, must be end-to-end
					if ((flags & INTERSECT_CULL_BOUNDARY) && (hit.type == IntersectPoint))
						return results;

					//Set boundary values
					hit.leftBoundary = true;
					hit.rightBoundary = true;
				}
				//Overlap forces boundarys to false

				//See if genObj
				if ((hit.type == IntersectPoint) && (flags & INTERSECT_GEN_POINTS)) {
					//Create new point
					WCGeometricPoint *newPoint = new WCGeometricPoint(pStart);
					//Add to result struct
					hit.object = newPoint;					
				}
				else if ((hit.type == IntersectLine) && (flags & INTERSECT_GEN_LINES)) {
					//Create new line
					WCGeometricLine *newLine = new WCGeometricLine(pStart, pEnd);
					//Add to result struct
					hit.object = newLine;
				}
				else hit.object = NULL;
				//Add the hit to the list
				results.push_back(hit);
			}
		}
	}
	//Non-Parallel Case
	else {
		//Many dot products
		WPFloat d121 = p1.DotProduct(p21);			// L
		WPFloat d2121 = p21.DotProduct(p21);		// M
		WPFloat d321 = p3.DotProduct(p21);			// N
		WPFloat d4321 = p43.DotProduct(p21);		// O
		WPFloat d143 = p1.DotProduct(p43);			// Q
		WPFloat d343 = p3.DotProduct(p43);			// R
		WPFloat d4343 = p43.DotProduct(p43);		// S
		denom = (d4343 * d2121) - (d4321 * d4321);
		//What does this correspond to?
		if (denom == 0.0) {
			CLOGGER_WARN(WCLogManager::RootLogger(), "GeometricIntersection::LineLine -  Denominator == 0.0.");
		}
		//Otherwise
		else {
			//Calculate parametric intersection values
			WPFloat numer = d4343 * (d321 - d121) + d4321 * (d143 - d343);
			WPFloat mua = numer / denom;
			WPFloat mub = (d143 + d4321 * mua - d343) / d4343;
			//Make sure mua and mub are (0.0 +- tol, 1.0 +- tol)
			if ( (mua < onePlusTol) && (mua > -tol) && (mub < onePlusTol) && (mub > -tol) ) {
				//Bound mua and mub [0, 1]
				mua = STDMAX(0.0, STDMIN(1.0, mua));
				mub = STDMAX(0.0, STDMIN(1.0, mub));
				//Calculate points and distance between them
				WCVector4 pointOnFirst = p1 + p21 * mua;
				WCVector4 pointOnSecond = p3 + p43 * mub;
				//Make sure lines are not > tol apart
				dist = pointOnFirst.Distance(pointOnSecond);
				if (dist < tol) {
					//Create intersection result
					WCIntersectionResult hit;
					hit.type = IntersectPoint;
					hit.leftParam.I(mua);
					hit.rightParam.I(mub);
					hit.leftBoundary = (fabs(mua) < tol) || (fabs(mua-1.0) < tol);
					hit.rightBoundary = (fabs(mub) < tol) || (fabs(mub-1.0) < tol);

					//Check for culling end-point intersections
					if (!(flags & INTERSECT_CULL_BOUNDARY) || (!hit.leftBoundary && !hit.rightBoundary)) {
						//See if genObj
						if (flags & INTERSECT_GEN_POINTS) {
							//Create new point
							WCGeometricPoint *newPoint = new WCGeometricPoint(pointOnFirst);
							//Add to result struct
							hit.object = newPoint;
						}
						else hit.object = NULL;
						//Add the intersection to the list
//						std::cout << hit << pointOnFirst << std::endl;
						results.push_back(hit);
					}
				}
			}
		}
	}
	//Return result
	return results;
}


std::list<WCIntersectionResult> __WILDCAT_NAMESPACE__::GeometricIntersection(WCGeometricLine *left,
	WCNurbsCurve *right, const WPFloat &tol, const unsigned int &flags) {
	//Get intersection list
	std::list<WCIntersectionResult> results = GeometricIntersection(right, left, tol, flags);
	//Return the reversed the list
	return IntersectionReverse( results );
}


std::list<WCIntersectionResult> __WILDCAT_NAMESPACE__::GeometricIntersection(WCGeometricLine *left,
	WCNurbsSurface *right, const WPFloat &tol, const unsigned int &flags) {
	//Get intersection list
	std::list<WCIntersectionResult> results = GeometricIntersection(right, left, tol, flags);
	//Return the reversed the list
	return IntersectionReverse( results );
}


std::list<WCIntersectionResult> __WILDCAT_NAMESPACE__::GeometricIntersection(WCGeometricLine *left,
	WCTrimmedNurbsSurface *right, const WPFloat &tol, const unsigned int &flags) {
	//Get intersection list
	std::list<WCIntersectionResult> results = GeometricIntersection(right, left, tol, flags);
	//Return the reversed the list
	return IntersectionReverse( results );
}


/***********************************************~***************************************************/

