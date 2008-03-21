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


#ifndef __NURBS_H__
#define __NURBS_H__


/*** Included Header Files ***/
#include "Geometry/wgeol.h"


/*** Local Defines ***/
extern WPFloat* __bezier_coef[8];


/***********************************************~***************************************************/


class WCNurbs {																			
public:
	static WPUInt FindSpan(const WPUInt &numCP, const WPUInt &degree,								//!< Find the span of the index value
												const WPFloat &u, const WPFloat *knotPoints);
	static WPUInt FindUniformSpan(const WPUInt &numCP, const WPUInt &degree,						//!< Find the span of the index value for a uniform array
												const WPFloat &u, const WPFloat *knotPoints);
	static void FindSpanMultiplicity(const WPUInt &numCP, const WPUInt &degree,						//!< Find the span and multiplicity
												const WPFloat &u, const WPFloat *knotPoints, 
												WPUInt &span, WPUInt &multiplicity);
	static WPFloat *BasisValues(const WPUInt &span, const WPFloat &u, const WPUInt &degree,			//!< Calculate the basis functions on U
												const WPFloat *knotPoints, const WPUInt &der=0);	
	static WPFloat* LoadDefaultKnotPoints(const WPUInt &kp, const WPUInt &degree);					//!< Initialize an array to default knot values
	static WPFloat* LoadBezierKnotPoints(const WPUInt &kp);											//!< Initialize an array to bezier knot values
	static WPFloat* LoadUniformKnotPoints(const WPUInt &kp);										//!< Initialize an array to uniform knot values
	static WPFloat* LoadCustomKnotPoints(const std::vector<WPFloat> &inPoints);						//!< Initialize an array to custom knot values
	static WPFloat EstimateLength(const std::vector<WCVector4> &controlPoints);						//!< Estimate the length of the curve
	static WPFloat EstimateLengthU(const std::vector<WCVector4> &controlPoints, const WPUInt &numCPU);	//!< Estimate the U direction length of the surface
	static WPFloat EstimateLengthV(const std::vector<WCVector4> &controlPoints, const WPUInt &numCPV);	//!< Estimate the V direction length of the surface

	static void CircularPoints(const WCVector4 &center, const WCVector4 &xUnit, const WCVector4 &yUnit,	//!< Generate circular arc curve points
												const WPFloat &radius, const WPFloat &startAngleDeg, const WPFloat &endAngleDeg,
												std::vector<WCVector4> &controlPoints, std::vector<WPFloat> &knotPoints);
	static void EllipsePoints(const WCVector4 &center, const WCVector4 &xUnit, const WCVector4 &yUnit,	//!< Generate elliptical curve points
												const WPFloat &major, const WPFloat &minor,
												std::vector<WCVector4> &controlPoints, std::vector<WPFloat> &knotPoints);
};
 

/***********************************************~****************************************************/


#endif //__NURBS_H__

