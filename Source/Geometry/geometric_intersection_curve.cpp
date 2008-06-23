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
#include "Geometry/nurbs_curve.h"


/***********************************************~***************************************************/


/*** GeometricIntersection -- Curve and Point ***
 * This algorithm tries to intersect a point with a NURBS curve.  The result can be zero to many hits.  The approach is to
 *	generate the curve into a number of line segments and intersect the point with each line segment.  There are a few things to remember:
 *		1) Point must intersect a given line segment within tolerance
 *		2) Point must be > tol parametric value (u) after the last point - avoids case of point being very close to where segments meet
 *		3) Must check flags for CULL_BOUNDARY case and only add hit as appropriate
 *		4) Make sure to delete generated segement data
***/
std::list<WCIntersectionResult> __WILDCAT_NAMESPACE__::GeometricIntersection(WCNurbsCurve *left,
	WCGeometricPoint *right, const WPFloat &tol, const unsigned int &flags) {
	std::list<WCIntersectionResult> results;
	//See if point is within curve bounding box
	//...

	//Determine estimated length of curve and LOD
	WPUInt lod = 128; //(WPUInt)(left->EstimateLength() / tol);
	GLfloat *data = left->GenerateClientBuffer(lod, true);

	//Initialize the p0 vector and other variables
	WCVector4 p0((WPFloat)data[0], (WPFloat)data[1], (WPFloat)data[2], 1.0);
	WCVector4 p1, direction, tmpVec;
	WPFloat u, dist, lastU=-1.0;
	WPUInt index=4;
	WPFloat paraFactor = 1.0 / (lod - 1);

	//Loop through remaining vertices and test each
	for (WPUInt i=1; i<lod; i++) {
		//Determine p1
		p1.Set( (WPFloat)data[index], (WPFloat)data[index+1], (WPFloat)data[index+2], 1.0 );
		//See if there is an intersection between line and point segment
		direction = p1 - p0;
		dist = p0.Distance(p1);
		//Get point vector
		tmpVec = right->Data() - p0;
		u = direction.DotProduct(tmpVec) / (dist * dist);
		//Bounds check u
		if ((u > -tol) && (u < (1.0 + tol))) {
			//Bound u
			u = STDMAX( STDMIN(u, 1.0), 0.0);
			//Get point on segment
			tmpVec = p0 + direction * u;
			//See distance is within tolerance
			if (tmpVec.Distance(right->Data()) <= tol) {
				//Convert u from segment to curve
				u = (i - 1 + u) * paraFactor;
				//Do consecutive hit check (u is always greater than lastU)
				if (u - lastU > tol) {
					//Create a hit result
					WCIntersectionResult hit;
					hit.type = IntersectPoint;
					hit.leftBoundary = (u < tol) || (1.0 - u < tol);
					hit.rightBoundary = true;
					hit.leftParam.I(u);
					hit.rightParam = right->Data();
					//Do boundary cull check, insert if ok
					if (!(flags * INTERSECT_CULL_BOUNDARY) || !hit.leftBoundary) {
						std::cout << hit;
						//Add the hit to the list
						results.push_back(hit);
					}
					//Set the new last u
					lastU = u;
				}
			}
		}
		//Move p1 to p0
		p0 = p1;
		index += 4;
	}
	//Clean up and return the hit list
	left->ReleaseBuffer(data);
	return results;
}


/*** GeometricIntersection -- Curve and Line ***
 * This algorithm tries to intersect a line with a NURBS curve.  The result can be zero to many hits and may include a linear overlaps.
 * General approach is as follows:
 *		1) Generate curve as multiple line segments
 *		2) Intersect each segment with the line.  Store all hit values.
 *		3) If there is a parallel intersection (overlap), need to 
 ***/
std::list<WCIntersectionResult> __WILDCAT_NAMESPACE__::GeometricIntersection(WCNurbsCurve *left,
	WCGeometricLine *right, const WPFloat &tol, const unsigned int &flags) {
	std::list<WCIntersectionResult> results;
	//See if line intersects curve bounding box
	//...

	//Determine estimated length of curve and LOD
	WPUInt lod = 128;// (WPUInt)(left->EstimateLength() / tol);
	GLfloat *data = left->GenerateClientBuffer(lod, true);
	//Evaluate both lines at both ends
	WCVector4 p3 = right->Begin();
	WCVector4 p4 = right->End();

	//Set up lots of variables
	WCVector4 p1((WPFloat)data[0], (WPFloat)data[1], (WPFloat)data[2], 1.0), p2;
	//Get length vectors
	WCVector4 p21, cross, pointOnFirst, pointOnSecond;
	WCVector4 p43(p4 - p3);
	WPFloat dist, d121, d2121, d321, d4321, d143, d343, d4343, denom, numer, mua, mub;
	WPUInt index=4;
	WPFloat paraFactor = 1.0 / (lod - 1);
	WPFloat onePlusTol = 1.0 + tol;
	//Loop through all curve segments
	for (WPUInt i=0; i<lod-1; i++) {
		//Determine secVec
		p2.Set( (WPFloat)data[index], (WPFloat)data[index+1], (WPFloat)data[index+2], 1.0 );
		p21 = p2 - p1;
		p43 = p4 - p3;
		//Check for parallel
		cross = p43.CrossProduct(p21);
		dist = cross.Magnitude();
		if (dist < 0.001) {
			//For now assume parallel is no intersection
//			CLOGGER_WARN(WCLogManager::RootLogger(), "GeometricIntersection::CurveLine - Parallel Intersection.");
		}
		//Non-parallel case
		else {
			//Many dot products
			d121 = p1.DotProduct(p21);				// L
			d2121 = p21.DotProduct(p21);			// M
			d321 = p3.DotProduct(p21);				// N
			d4321 = p43.DotProduct(p21);			// O
			d143 = p1.DotProduct(p43);				// Q
			d343 = p3.DotProduct(p43);				// R
			d4343 = p43.DotProduct(p43);			// S
			denom = (d4343 * d2121) - (d4321 * d4321);
			//Some further calculations...
			numer = d4343 * (d321 - d121) + d4321 * (d143 - d343);
			mua = numer / denom;
			mub = (d143 + d4321 * mua - d343) / d4343;
			//Make sure mua and mub are in (0.0 +- tol, 1.0 +- tol)
			if ( (mua < onePlusTol) && (mua > -tol) && (mub < onePlusTol) && (mub > -tol) ) {
				//Calculate points and distance between them
				pointOnFirst = p1 + p21 * mua;
				pointOnSecond = p3 + p43 * mub;
				//Make sure lines are not > tol apart
				dist = pointOnFirst.Distance(pointOnSecond);
				if (dist < tol) {
					//Determine actual overall parametric value on left
					mua = (i - 1 + mua) * paraFactor;
					//Create intersection result
					WCIntersectionResult hit;
					hit.type = IntersectPoint;
					hit.leftParam.I(mua);
					hit.rightParam.I(mub);
					hit.leftBoundary = (fabs(mua) < tol) || (fabs(mua-1.0) < tol);
					hit.rightBoundary = (fabs(mub) < tol) || (fabs(mub-1.0) < tol);
					
					//Check if culling end-point intersections
					if (!(flags & INTERSECT_CULL_BOUNDARY) ||  (!hit.leftBoundary && !hit.rightBoundary)) {
						//See if genObj
						if (flags & INTERSECT_GEN_POINTS) {
							//Create new point
							WCGeometricPoint *newPoint = new WCGeometricPoint(pointOnFirst);
							//Add to result struct
							hit.object = newPoint;
						}
						else hit.object = NULL;
						//Add the intersection to the list
						std::cout << hit << pointOnFirst << std::endl;
						results.push_back(hit);
					}
				}
			}
		}
		//Move p2 to p1
		p1 = p2;
		index += 4;
	}	
	//Clean up and return the hit list
	left->ReleaseBuffer(data);
	return results;
}


/*** GeometricIntersection -- Curve and Curve ***
 * This algorithm tries to intersect a NURBS curve with a NURBS curve.  The result can be zero to many hits and may include an extended overlap.
 ***/
std::list<WCIntersectionResult> __WILDCAT_NAMESPACE__::GeometricIntersection(WCNurbsCurve *left,
	WCNurbsCurve *right, const WPFloat &tol, const unsigned int &flags) {
	std::list<WCIntersectionResult> results;
	//Check if self intersection
	if (left == right) return results;

	//See if bounding boxes intersect
	//...
	
	//Determine optimum LOD for each curve
	WPUInt lodLeft = 64;
	WPUInt lodRight = 64;
	//Generate buffers for both curves
	GLfloat *leftBuffer = left->GenerateClientBuffer(lodLeft, true);
	GLfloat *rightBuffer = right->GenerateClientBuffer(lodRight, true);
	//Setup some variables
	WPFloat paraLeft = 1.0 / (lodLeft - 1);
	WPFloat paraRight = 1.0 / (lodRight - 1);
	WPFloat onePlusTol=1.0+tol, denom, d121, d2121, d321, d4321, d143, d343, d4343, numer, mua, mub, dist;
	WCVector4 p1, p2, p3, p4, p21, p43, cross, pointOnFirst, pointOnSecond;
	WPUInt indexL=4, indexR;
	p1.Set((WPFloat)leftBuffer[0], (WPFloat)leftBuffer[1], (WPFloat)leftBuffer[2], 1.0);
	//Loop through first curve segments
	for (WPUInt i=1; i<lodLeft; i++) {
		//Set p2 vector
		p2.Set((WPFloat)leftBuffer[indexL], (WPFloat)leftBuffer[indexL+1], (WPFloat)leftBuffer[indexL+2], 1.0);
		p21 = p2 - p1;
		//Set p3 vector
		p3.Set((WPFloat)rightBuffer[0], (WPFloat)rightBuffer[1], (WPFloat)rightBuffer[2], 1.0);
		indexR = 4;
		//Loop through second curve segments
		for (WPUInt j=1; j<lodRight; j++) {
			//Set p4 vector
			p4.Set((WPFloat)rightBuffer[indexR], (WPFloat)rightBuffer[indexR+1], (WPFloat)rightBuffer[indexR+2], 1.0);
			p43 = p4 - p3;
			//Check for parallel
			cross = p43.CrossProduct(p21);
			denom = cross.Magnitude();
			if (denom < 0.0001) {
				//Parallel case, do nothing for now...
			}
			//Not parallel
			else {
				//Many dot products
				d121 = p1.DotProduct(p21);			// L
				d2121 = p21.DotProduct(p21);		// M
				d321 = p3.DotProduct(p21);			// N
				d4321 = p43.DotProduct(p21);		// O
				d143 = p1.DotProduct(p43);			// Q
				d343 = p3.DotProduct(p43);			// R
				d4343 = p43.DotProduct(p43);		// S
				denom = (d4343 * d2121) - (d4321 * d4321);
				//What does this correspond to?
				if (denom == 0.0) {
					CLOGGER_WARN(WCLogManager::RootLogger(), "GeometricIntersection::CurveCurve -  Denominator == 0.0.");
				}
				//Otherwise, continue...
				else {
					//Calculate parametric intersection values
					numer = d4343 * (d321 - d121) + d4321 * (d143 - d343);
					mua = numer / denom;
					mub = (d143 + d4321 * mua - d343) / d4343;
					//Make sure mua and mub are (0.0 +- tol, 1.0 +- tol)
					if ( (mua < onePlusTol) && (mua > -tol) && (mub < onePlusTol) && (mub > -tol) ) {
						//Calculate points and distance between them
						pointOnFirst = p1 + p21 * mua;
						pointOnSecond = p3 + p43 * mub;
						//Make sure lines are not > tol apart
						dist = pointOnFirst.Distance(pointOnSecond);
						if (dist < tol) {
							//Determine actual overall parametric value on left
							mua = (i - 1 + mua) * paraLeft;
							//Determine actual overall parametric value on right
							mub = (j - 1 + mub) * paraRight;
							//Create intersection result
							WCIntersectionResult hit;
							hit.type = IntersectPoint;
							hit.leftParam.I(mua);
							hit.rightParam.I(mub);
							hit.leftBoundary = (fabs(mua) < tol) || (fabs(mua-1.0) < tol);
							hit.rightBoundary = (fabs(mub) < tol) || (fabs(mub-1.0) < tol);
							//Check if culling end-point intersections
							if (!(flags & INTERSECT_CULL_BOUNDARY) ||  (!hit.leftBoundary && !hit.rightBoundary)) {
								//See if genObj
								if (flags & INTERSECT_GEN_POINTS) {
									//Create new point
									WCGeometricPoint *newPoint = new WCGeometricPoint(pointOnFirst);
									//Add to result struct
									hit.object = newPoint;
								}
								else hit.object = NULL;
								//Add the intersection to the list
								std::cout << hit << pointOnFirst << std::endl;
								results.push_back(hit);
							}
						}
					}
				}
			}
			//Move right points from p4 to p3
			p3 = p4;
			indexR += 4;
		}
		//Move left points from p2 to p1
		p1 = p2;
		indexL += 4;
	}
	//Release the buffers
	left->ReleaseBuffer(leftBuffer);
	right->ReleaseBuffer(rightBuffer);
	//Return the hit list
	return results;
}


std::list<WCIntersectionResult> __WILDCAT_NAMESPACE__::GeometricIntersection(WCNurbsCurve *left,
	WCNurbsSurface *right, const WPFloat &tol, const unsigned int &flags) {
	//Get intersection list
	std::list<WCIntersectionResult> results = GeometricIntersection(right, left, tol, flags);
	//Return the reversed the list
	return IntersectionReverse( results );
}


std::list<WCIntersectionResult> __WILDCAT_NAMESPACE__::GeometricIntersection(WCNurbsCurve *left,
	WCTrimmedNurbsSurface *right, const WPFloat &tol, const unsigned int &flags) {
	//Get intersection list
	std::list<WCIntersectionResult> results = GeometricIntersection(right, left, tol, flags);
	//Return the reversed the list
	return IntersectionReverse( results );
}


/***********************************************~***************************************************/

