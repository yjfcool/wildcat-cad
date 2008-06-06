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
	WCGeometricPoint *right, const WPFloat &tol, const int &flags) {
	//Get line direction vector
	WCVector4 direction = left->End() - left->Begin();
	direction.Normalize(true);
	//Get line-being to point vector
	WCVector4 pointVector = right->Data() - left->Begin();
	WPFloat u = direction.DotProduct(pointVector);

	//Bounds check u for [0, 1] - return empty if out of bounds
	if ((u < -tol) || (u > (1.0 + tol))) return std::list<WCIntersectionResult>();
	//Do cull boundary check - return if true
	if ((flags & INTERSECT_CULL_BOUNDARY) && ((u < tol) || (1.0 - u < tol) )) return std::list<WCIntersectionResult>();

	//Bound u [0.0 to 1.0]
	u = STDMAX( STDMIN(u, 1.0), 0.0);
	//Get the point on the line
	WCVector4 pt = left->Begin() + direction * u;
	//Get distance from point on line to point
	if (pt.Distance(right->Data()) <= tol) {
		//Create intesect result
		WCIntersectionResult result;
		result.type = IntersectPoint;
		result.leftBoundary = (u < tol) || (1.0 - u < tol);
		result.rightBoundary = true;
		result.leftParam.I(u);
		result.rightParam = right->Data();
		//See if genObj
		if (flags & INTERSECT_GEN_POINTS) {
			//Create new point
			WCGeometricPoint *newPoint = new WCGeometricPoint(pt);
			//Add to result struct
			result.object = newPoint;
		}
		else result.object = NULL;
		return std::list<WCIntersectionResult>(1, result);
	}
	//Otherwise, return nothing
	return std::list<WCIntersectionResult>();
}


std::list<WCIntersectionResult> __WILDCAT_NAMESPACE__::GeometricIntersection(WCGeometricLine *left,
	WCGeometricLine *right, const WPFloat &tol, const int &flags) {
	//Evaluate both lines at both ends
	WCVector4 p1 = left->Begin();
	WCVector4 p2 = left->End();
	WCVector4 p3 = right->Begin();
	WCVector4 p4 = right->End();
	
	//Get length vectors
	WCVector4 p21(p2 - p1);
	WCVector4 p43(p4 - p3);
	
	//Make sure both lines have some minimal length
	if ((p21.Magnitude() < tol) || (p43.Magnitude() < tol)) return std::list<WCIntersectionResult>();
	
	//Check for parallel
	WCVector4 cross = p43.CrossProduct(p21);
	WPFloat denom = cross.Magnitude();
	if (denom < 0.01) {
		//Determine if overlaping or just parallel
		//...
		return std::list<WCIntersectionResult>();
	}
	
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
		return std::list<WCIntersectionResult>();
	}
	
	WPFloat numer = d4343 * (d321 - d121) + d4321 * (d143 - d343);
	WPFloat mua = numer / denom;
	WPFloat mub = (d143 + d4321 * mua - d343) / d4343;
	//Make sure mua and mub are (0.0 +- tol, 1.0 +- tol)
	if ( (mua >= 1.0+tol) || (mua <= 0.0-tol) || (mub >= 1.0+tol) || (mub <= 0.0-tol) )
		return std::list<WCIntersectionResult>();
	
	//Calculate points and distance between them
	WCVector4 pointOnFirst = p1 + p21 * mua;
	WCVector4 pointOnSecond = p3 + p43 * mub;
	//Make sure lines are not > tol apart
	WPFloat dist = pointOnFirst.Distance(pointOnSecond);
	if (dist > tol) return std::list<WCIntersectionResult>();
	
	//Create intersection result
	WCIntersectionResult result;
	result.type = IntersectPoint;
	result.leftParam.I(mua);
	result.rightParam.I(mub);
	result.leftBoundary = (fabs(mua) < tol) || (fabs(mua-1.0) < tol);
	result.rightBoundary = (fabs(mub) < tol) || (fabs(mub-1.0) < tol);
	//See if genObj
	if (flags & INTERSECT_GEN_POINTS) {
		//Create new point
		WCGeometricPoint *newPoint = new WCGeometricPoint(mua);
		//Add to result struct
		result.object = newPoint;
	}
	else result.object = NULL;
	//Return result
	return std::list<WCIntersectionResult>(1, result);
}


std::list<WCIntersectionResult> __WILDCAT_NAMESPACE__::GeometricIntersection(WCGeometricLine *left,
	WCNurbsCurve *right, const WPFloat &tol, const int &flags) {
	//Get intersection list
	std::list<WCIntersectionResult> results = GeometricIntersection(right, left, tol, flags);
	//Return the reversed the list
	return IntersectionReverse( results );
}


std::list<WCIntersectionResult> __WILDCAT_NAMESPACE__::GeometricIntersection(WCGeometricLine *left,
	WCNurbsSurface *right, const WPFloat &tol, const int &flags) {
	//Get intersection list
	std::list<WCIntersectionResult> results = GeometricIntersection(right, left, tol, flags);
	//Return the reversed the list
	return IntersectionReverse( results );
}


std::list<WCIntersectionResult> __WILDCAT_NAMESPACE__::GeometricIntersection(WCGeometricLine *left,
	WCTrimmedNurbsSurface *right, const WPFloat &tol, const int &flags) {
	//Get intersection list
	std::list<WCIntersectionResult> results = GeometricIntersection(right, left, tol, flags);
	//Return the reversed the list
	return IntersectionReverse( results );
}


/***********************************************~***************************************************/

