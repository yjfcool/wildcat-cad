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


#ifndef __GEOMETRIC_INTERSECTION_H__
#define __GEOMETRIC_INTERSECTION_H__


/*** Included Header Files ***/
#include "Geometry/wgeol.h"


/*** Locally Defined Values ***/
#define INTERSECT_GEN_NONE					0x0000
#define INTERSECT_GEN_POINTS				0x0001
#define INTERSECT_GEN_LINES					0x0002
#define INTERSECT_GEN_CURVES				0x0004
#define INTERSECT_GEN_SURFACES				0x0008
#define INTERSECT_GEN_TRIMSURFACES			0x0010
#define INTERSECT_GEN_ALL					INTERSECT_GEN_POINTS | INTERSECT_GEN_LINES | INTERSECT_GEN_CURVES | INTERSECT_GEN_SURFACES | INTERSECT_GEN_TRIMSURFACES
#define INTERSECT_CULL_BOUNDARY				0x0020
//Program Location Definitions
#define INTERSECTION_CLI_PARAMS				0
#define INTERSECTION_CLI_LBEGIN				1
#define INTERSECTION_CLI_LEND				2
#define INTERSECTION_CCI_PARAMS				3
#define INTERSECTION_SLI_PARAMS				4
#define INTERSECTION_SLI_LBEGIN				5
#define INTERSECTION_SLI_LEND				6


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {
	
	
/*** Class Predefines ***/
class WCGeometricObject;
class WCGeometricPoint;
class WCGeometricLine;
class WCNurbsCurve;
class WCNurbsSurface;
class WCTrimmedNurbsSurface;
	
	
/***********************************************~***************************************************/


enum WCIntersectionType {
	IntersectPoint,
	IntersectLine,
	IntersectCurve,
	IntersectSurface,
	IntersectTrimSurface
};


struct WCIntersectionResult {
	WCIntersectionType							type;
	bool										leftBoundary, rightBoundary;
	WCVector4									leftParam, rightParam;
	WCGeometricObject							*object;
};


/*** Point-Object Functions ***/
std::list<WCIntersectionResult> GeometricIntersection(WCGeometricPoint *left, WCGeometricPoint *right, const WPFloat &tol, const unsigned int &flags=INTERSECT_GEN_ALL);
std::list<WCIntersectionResult> GeometricIntersection(WCGeometricPoint *left, WCGeometricLine *right,  const WPFloat &tol, const unsigned int &flags=INTERSECT_GEN_ALL);
std::list<WCIntersectionResult> GeometricIntersection(WCGeometricPoint *left, WCNurbsCurve *right, const WPFloat &tol, const unsigned int &flags=INTERSECT_GEN_ALL);
std::list<WCIntersectionResult> GeometricIntersection(WCGeometricPoint *left, WCNurbsSurface *right, const WPFloat &tol, const unsigned int &flags=INTERSECT_GEN_ALL);
std::list<WCIntersectionResult> GeometricIntersection(WCGeometricPoint *left, WCTrimmedNurbsSurface *right, const WPFloat &tol, const unsigned int &flags=INTERSECT_GEN_ALL);


/*** Line-Object Functions ***/
std::list<WCIntersectionResult> GeometricIntersection(WCGeometricLine *left, WCGeometricPoint *right, const WPFloat &tol, const unsigned int &flags=INTERSECT_GEN_ALL);
std::list<WCIntersectionResult> GeometricIntersection(WCGeometricLine *left, WCGeometricLine *right, const WPFloat &tol, const unsigned int &flags=INTERSECT_GEN_ALL);
std::list<WCIntersectionResult> GeometricIntersection(WCGeometricLine *left, WCNurbsCurve *right, const WPFloat &tol, const unsigned int &flags=INTERSECT_GEN_ALL);
std::list<WCIntersectionResult> GeometricIntersection(WCGeometricLine *left, WCNurbsSurface *right, const WPFloat &tol, const unsigned int &flags=INTERSECT_GEN_ALL);
std::list<WCIntersectionResult> GeometricIntersection(WCGeometricLine *left, WCTrimmedNurbsSurface *right, const WPFloat &tol, const unsigned int &flags=INTERSECT_GEN_ALL);


/*** Curve-Object Functions ***/
std::list<WCIntersectionResult> GeometricIntersection(WCNurbsCurve *left, WCGeometricPoint *right, const WPFloat &tol, const unsigned int &flags=INTERSECT_GEN_ALL);
std::list<WCIntersectionResult> GeometricIntersection(WCNurbsCurve *left, WCGeometricLine *right, const WPFloat &tol, const unsigned int &flags=INTERSECT_GEN_ALL);
std::list<WCIntersectionResult> GeometricIntersection(WCNurbsCurve *left, WCNurbsCurve *right, const WPFloat &tol, const unsigned int &flags=INTERSECT_GEN_ALL);
std::list<WCIntersectionResult> GeometricIntersection(WCNurbsCurve *left, WCNurbsSurface *right, const WPFloat &tol, const unsigned int &flags=INTERSECT_GEN_ALL);
std::list<WCIntersectionResult> GeometricIntersection(WCNurbsCurve *left, WCTrimmedNurbsSurface *right, const WPFloat &tol, const unsigned int &flags=INTERSECT_GEN_ALL);


/*** Surface-Object Functions ***/
std::list<WCIntersectionResult> GeometricIntersection(WCNurbsSurface *left, WCGeometricPoint *right, const WPFloat &tol, const unsigned int &flags=INTERSECT_GEN_ALL);
std::list<WCIntersectionResult> GeometricIntersection(WCNurbsSurface *left, WCGeometricLine *right, const WPFloat &tol, const unsigned int &flags=INTERSECT_GEN_ALL);
std::list<WCIntersectionResult> GeometricIntersection(WCNurbsSurface *left, WCNurbsCurve *right, const WPFloat &tol, const unsigned int &flags=INTERSECT_GEN_ALL);
std::list<WCIntersectionResult> GeometricIntersection(WCNurbsSurface *left, WCNurbsSurface *right, const WPFloat &tol, const unsigned int &flags=INTERSECT_GEN_ALL);
std::list<WCIntersectionResult> GeometricIntersection(WCNurbsSurface *left, WCTrimmedNurbsSurface *right, const WPFloat &tol, const unsigned int &flags=INTERSECT_GEN_ALL);


/*** TrimSurface-Object Functions ***/
std::list<WCIntersectionResult> GeometricIntersection(WCTrimmedNurbsSurface *left, WCGeometricPoint *right, const WPFloat &tol, const unsigned int &flags=INTERSECT_GEN_ALL);
std::list<WCIntersectionResult> GeometricIntersection(WCTrimmedNurbsSurface *left, WCGeometricLine *right, const WPFloat &tol, const unsigned int &flags=INTERSECT_GEN_ALL);
std::list<WCIntersectionResult> GeometricIntersection(WCTrimmedNurbsSurface *left, WCNurbsCurve *right, const WPFloat &tol, const unsigned int &flags=INTERSECT_GEN_ALL);
std::list<WCIntersectionResult> GeometricIntersection(WCTrimmedNurbsSurface *left, WCNurbsSurface *right, const WPFloat &tol, const unsigned int &flags=INTERSECT_GEN_ALL);
std::list<WCIntersectionResult> GeometricIntersection(WCTrimmedNurbsSurface *left, WCTrimmedNurbsSurface *right, const WPFloat &tol, const unsigned int &flags=INTERSECT_GEN_ALL);


/*** Function to reverse the left and right result parameters and boundary values ***/
inline std::list<WCIntersectionResult>& IntersectionReverse(std::list<WCIntersectionResult>& results) {
	//Reverse left and right if there are results
	if (!results.empty()) {
		std::list<WCIntersectionResult>::iterator iter(results.begin());
		//Iterate through the list
		for (; iter != results.end(); iter++) {
			//Swap the boundary flag
			std::swap( (*iter).leftBoundary, (*iter).rightBoundary );
			//Swap the param values
			std::swap( (*iter).leftParam, (*iter).rightParam );
		}
	}
	//Return the list
	return results;
}


/*** Function to delete any intersection objects ***/
void IntersectionListDisposal(std::list<WCIntersectionResult> &results);


/*** Non-Member Functions ***/
std::ostream& operator<<(std::ostream& out, const WCIntersectionResult &result);					//!< Overloaded output operator
	

/***********************************************~***************************************************/

	
}	   // End Wildcat Namespace
#endif //__GEOMETRIC_INTERSECTION_H__

