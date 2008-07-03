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
#include "Geometry/nurbs_surface.h"
#include "Geometry/trimmed_nurbs_surface.h"


/***********************************************~***************************************************/


std::list<WCIntersectionResult> __WILDCAT_NAMESPACE__::GeometricIntersection(WCTrimmedNurbsSurface *left,
	WCGeometricPoint *right, const WPFloat &tol, const unsigned int &flags) {
	//See if point is within surface bounding box
	//...
	
	//Otherwise, generate surface to precision of tol and place into local buffer
	//...
	
	//Compare point against each triangle within buffer
	//...
	
	return std::list<WCIntersectionResult>();
}


std::list<WCIntersectionResult> __WILDCAT_NAMESPACE__::GeometricIntersection(WCTrimmedNurbsSurface *left,
	WCGeometricLine *right, const WPFloat &tol, const unsigned int &flags) {
	//See if line intersects surface bounding box
	//...
	
	//Otherwise, generate surface to precision of tol and place into local buffer
	//...
	
	//Try to intersect line with each triangle within buffer
	//...
	
	return std::list<WCIntersectionResult>();
}


std::list<WCIntersectionResult> __WILDCAT_NAMESPACE__::GeometricIntersection(WCTrimmedNurbsSurface *left,
	WCNurbsCurve *right, const WPFloat &tol, const unsigned int &flags) {
	//See if curve bounding box intersects surface bounding box
	//...
	
	//Otherwise, generate surface to precision of tol and place into local buffer
	//...
	
	//Generate curve to precision of tol and place into local buffer
	//...
	
	//Try to intersect all sections of curve in buffer with each triangle in buffer
	//...
	
	return std::list<WCIntersectionResult>();
}


std::list<WCIntersectionResult> __WILDCAT_NAMESPACE__::GeometricIntersection(WCTrimmedNurbsSurface *left,
	WCNurbsSurface *right, const WPFloat &tol, const unsigned int &flags) {
	//See if left bounding box intersects right bounding box
	//...
	
	//Otherwise, generate left and right to precision of tol and place into local buffers
	//...
	
	//Try to intersect all triangles of left with each triangle in right
	//...
	
	return std::list<WCIntersectionResult>();
}


std::list<WCIntersectionResult> __WILDCAT_NAMESPACE__::GeometricIntersection(WCTrimmedNurbsSurface *left,
	WCTrimmedNurbsSurface *right, const WPFloat &tol, const unsigned int &flags) {
	//See if left bounding box intersects right bounding box
	//...
	
	//Otherwise, generate left and right to precision of tol and place into local buffers
	//...
	
	//Try to intersect all triangles of left with each triangle in right
	//...
	
	return std::list<WCIntersectionResult>();
}


/***********************************************~***************************************************/

