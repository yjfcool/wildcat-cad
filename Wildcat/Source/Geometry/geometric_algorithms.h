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


#ifndef __GEOMETRIC_ALGORITMS_H__
#define __GEOMETRIC_ALGORITMS_H__


/*** Included Header Files ***/
#include "Geometry/wgeol.h"


/*** Local Defines ***/
//None


/***********************************************~***************************************************/


inline bool IsOnRight2D(const WPFloat &startX, const WPFloat &startY, const WPFloat &endX, const WPFloat &endY, 
	const WPFloat &pointX, const WPFloat &pointY) {
	WPFloat tmp = (endX - startX) * (startY - pointY) - (startX - pointX) * (endY - startY);
	if (tmp >= 0.0) return true;
	else return false;
}


inline WPFloat PointToLineDistance2D(const WPFloat &startX, const WPFloat &startY, const WPFloat &endX, const WPFloat &endY, 
	const WPFloat &pointX, const WPFloat &pointY, const bool &abs=true) {
	//Calculate the distance
	WPFloat num = (endX - startX) * (startY - pointY) - (startX - pointX) * (endY - startY);
	if ((num < 0.0) && abs) num *= -1.0;
	WPFloat den = sqrt( pow( endX - startX, 2) + pow( endY - startY, 2) );
	if (den < 0.001) return 100000000.0;
	return num / den;
}


inline void ProjectPointToLine2D(const WPFloat &startX, const WPFloat &startY, const WPFloat &endX, const WPFloat &endY, 
	WPFloat &pointX, WPFloat &pointY) {
	//Do some calculations
	WPFloat num = (endX - startX) * (startY - pointY) - (startX - pointX) * (endY - startY);
	WPFloat den = sqrt( pow( endX - startX, 2) - pow( endY - startY, 2) );
	WPFloat dist = num / den;
	WPFloat tanX = (endY - startY) / den;
	WPFloat tanY = (startX - endX) / den;
	pointX = pointX - tanX * dist;
	pointY = pointY - tanY * dist;
}


inline WCVector4 ProjectPointToLine3D(const WCVector4 &base, const WCVector4 &direction, const WCVector4 &point) {
	//Get unit vector to point
	WCVector4 pointVec = point - base;
	//Get dot product
	WPFloat u = direction.DotProduct(pointVec);
	//Otherwise evaluate at a given value
	WCVector4 pt = base + direction * u;
	return pt;
}


/***********************************************~***************************************************/

//Return values
#define INTERSECTION_NONE						0
#define INTERSECTION_INTERSECTION				1
#define INTERSECTION_PARALLEL					2


WPUInt LineLineIntersection(const WCVector4 &p1, const WCVector4 &p2, const WCVector4 &p3, const WCVector4 &p4, const bool &includeEnds,
											const WPFloat &tol, WCVector4 &value, WPFloat &uValue, WPFloat &vValue);


WPUInt RayRayIntersection(const WCVector4 &p0, const WCVector4 &t0, const WCVector4 &p2,
												 const WCVector4 &t2, WPFloat &alf0, WPFloat &alf2, WCVector4 &p);

												 
bool OneArcConic(const WCVector4 &p0, const WCVector4 &t0, const WCVector4 &p2, const WCVector4 &t2, const WCVector4 &p4, WCVector4 &p);


WPFloat Angle(const WCVector4 &p0, const WCVector4 &p1, const WCVector4 &p2);


void SplitArc(const WCVector4 &p0, const WCVector4 &p1, const WCVector4 &p2, WCVector4 &q1, WCVector4 &s, WCVector4 &r1);


std::list<WCVector4> ConvexHull2D(std::list<WCVector4> pointList, const WPFloat &tol);


std::list<WCVector4> MinimumBoundingRectangle(const std::list<WCVector4> &pointList, const WCMatrix4 &toPlane, const WCMatrix4 &fromPlane);


GLint* TriangulatePolygon(const std::list<WCVector4> &pointList);


/***********************************************~***************************************************/


#endif //__GEOMETRIC_ALGORITMS_H__

