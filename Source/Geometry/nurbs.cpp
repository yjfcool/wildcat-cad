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
#include "Geometry/nurbs.h"
#include "Geometry/geometric_algorithms.h"


/***********************************************~***************************************************/


WPFloat __bezier_coef1[] = {-1.0, 1.0,																// Degree 1
							1.0, 0.0};
WPFloat __bezier_coef2[] = {1.0, -2.0, 1.0,															// Degree 2
							-2.0, 2.0, 0.0, 
							1.0, 0.0, 0.0};							
WPFloat __bezier_coef3[] = {-1.0, 3.0, -3.0, 1.0,													// Degree 3
							3.0, -6.0, 3.0, 0.0, 
							-3.0, 3.0, 0.0, 0.0, 
							1.0, 0.0, 0.0, 0.0};
WPFloat __bezier_coef4[] = 	{1.0, -4.0, 6.0, -4.0, 1.0,												// Degree 4
							-4.0, 12.0, -12.0, 4.0, 0.0, 
							6.0, -12.0, 6.0, 0.0, 0.0, 
							-4.0, 4.0, 0.0, 0.0, 0.0,
							1.0, 0.0, 0.0, 0.0, 0.0};
WPFloat __bezier_coef5[] = {-1.0, 5.0, -10.0, 10.0, -5.0, 1.0,										// Degree 5
							5.0, -20.0, 30.0, -20.0, 5.0, 0.0,
							-10.0, 30.0, -30.0, 10.0, 0.0, 0.0, 
							10.0, -20.0, 10.0, 0.0, 0.0, 0.0,	
							-5.0, 5.0, 0.0, 0.0, 0.0, 0.0, 
							1.0, 0.0, 0.0, 0.0, 0.0, 0.0};
WPFloat __bezier_coef6[] = {1.0, -6.0, 15.0, -20.0, 15.0, -6.0, 1.0,								// Degree 6
							-6.0, 30.0, -60.0, 60.0, -30.0, 6.0, 0.0,
							15.0, -60.0, 90.0, -60.0, 15.0, 0.0, 0.0,
							-20.0, 60.0, -60.0, 20.0, 0.0, 0.0, 0.0,
							15.0, -30.0, 15.0, 0.0, 0.0, 0.0, 0.0,
							-6.0, 6.0, 0.0, 0.0, 0.0, 0.0, 0.0,
							1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
WPFloat __bezier_coef7[] = {-1.0, 7.0, -21.0, 35.0, -35.0, 21.0, -7.0, 1.0,							// Degree 7
							7.0, -42.0, 105.0, -140.0, 105.0, -42.0, 7.0, 0.0,
							-21.0, 105.0, -210.0, 210.0, -105.0, 21.0, 0.0, 0.0,
							35.0, -140.0, 210.0, -140.0, 35.0, 0.0, 0.0, 0.0,
							-35.0, 105.0, -105.0, 35.0, 0.0, 0.0, 0.0, 0.0,
							21.0, -42.0, 21.0, 0.0, 0.0, 0.0, 0.0, 0.0,
							-7.0, 7.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
							1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};										
WPFloat* __bezier_coef[8] = { NULL, __bezier_coef1, __bezier_coef2, __bezier_coef3, __bezier_coef4, __bezier_coef5, __bezier_coef6, __bezier_coef7};


/***********************************************~***************************************************/


WPUInt WCNurbs::FindSpan(const WPUInt &numCP, const WPUInt &degree, const WPFloat &u, const WPFloat *knotPoints) {
	//Check special case
	if (u >= knotPoints[numCP]) return numCP-1;
	//Setup search
	WPUInt low = degree;
	WPUInt high = numCP;
	WPUInt mid = (low + high) / 2;
	//Start binary search
	while ((u < knotPoints[mid]) || (u >= knotPoints[mid+1])) {
		if (u < knotPoints[mid]) high = mid;
		else low = mid;
		mid = (low + high) / 2;
	}
	//Return the span value
	return mid;
}


WPUInt WCNurbs::FindUniformSpan(const WPUInt &numCP, const WPUInt &degree, const WPFloat &u, const WPFloat *knotPoints) {
	//Check special case
	if (u >= knotPoints[numCP+degree]) return numCP+degree;
	//Setup search
	WPUInt low = 0;
	WPUInt high = numCP + degree;
	WPUInt mid = (low + high) / 2;
	//Start binary search
	while ((u < knotPoints[mid]) || (u >= knotPoints[mid+1])) {
		if (u < knotPoints[mid]) high = mid;
		else low = mid;
		mid = (low + high) / 2;
	}
	//Return the span value
	return mid;
}


void WCNurbs::FindSpanMultiplicity(const WPUInt &numCP, const WPUInt &degree, const WPFloat &u, const WPFloat *knotPoints, 
	WPUInt &span, WPUInt &multiplicity) {
	//First get the span
	span = WCNurbs::FindSpan(numCP, degree, u, knotPoints);
	//The see how many times u occurs
	multiplicity = 0;
	for (WPUInt i=0; i<=numCP+degree; i++) if (knotPoints[i] == u) multiplicity++;
}


WPFloat* WCNurbs::BasisValues(const WPUInt &span, const WPFloat &u, const WPUInt &degree, const WPFloat *knotPoints, const WPUInt &der) {
	//Allocate arrays for ndu and a
	WPFloat **ndu = new WPFloat*[degree+1];
	for (WPUInt i=0; i<=degree; i++) ndu[i] = new WPFloat[degree+1];
	WPFloat **a = new WPFloat*[2];
	a[0] = new WPFloat[degree+1];
	a[1] = new WPFloat[degree+1];

	WPFloat *left = new WPFloat[degree+1];
	WPFloat *right = new WPFloat[degree+1];
	int j, k, r, j1, j2, s1, s2, rk, pk;
	WPFloat saved, temp, d;
	WPUInt useDer = STDMIN(der, degree-1);
	
	//Allocate space for the output
	WPFloat *ders = new WPFloat[(der+1) * (degree+1)];
	//Check to make sure memory is allocated
	if (ders == NULL) 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::BasisValues - Not able to allocate memory for results array.");
	//Set all values to zero
	memset(ders, 0, sizeof(WPFloat)*(der+1)*(degree+1));
		
	//Calculate basis values
	ndu[0][0] = 1.0;
	for (j=1; j<=(int)degree; j++) {
		left[j] = u - knotPoints[span+1-j];
		right[j] = knotPoints[span+j] - u;
		saved = 0.0;
		for (r=0; r<j; r++) {
			ndu[j][r] = right[r+1] + left[j-r];			// Lower triangle
			temp = ndu[r][j-1] / ndu[j][r];
			ndu[r][j] = saved + right[r+1] * temp;		// Upper triangle
			saved = left[j-r] * temp;
		}
		ndu[j][j] = saved;
	}
	//Copy the basis values into the results
	for (j=0; j<=(int)degree; j++) ders[j] = ndu[j][degree];
	//Delete left and right arrays
	delete left;
	delete right;
	//Check for der==0 optimization
	if (der == 0) {
			//Delete arrays
			for (WPUInt i=0; i<degree; i++) delete ndu[i];
			delete ndu;
			delete a[0];
			delete a[1];
			delete a;
			//Return the values
			return ders;
	}
	
	//Loop over function index
	for (r=0; r<=(int)degree; r++) {
		s1 = 0;
		s2 = 1;
		a[0][0] = 1.0;
		//Loop to compute kth derivative
		for (k=1; k<=(int)useDer; k++) {
			d = 0.0;
			rk = r - k;
			pk = degree - k;
			if (r >= k) {
				a[s2][0] = a[s1][0] / ndu[pk+1][rk];
				d = a[s2][0] * ndu[rk][pk];
			}
			if (rk >= -1) j1 = 1;
			else j1 = -rk;
			if (r-1 <= pk) j2 = k - 1;
			else j2 = degree - r;
			for (j=j1; j<=j2; j++) {
				a[s2][j] = (a[s1][j] - a[s1][j-1]) / ndu[pk+1][rk+j];
				d += a[s2][j] * ndu[rk+j][pk];
			}
			if (r <= pk) {
				a[s2][k] = -a[s1][k-1] / ndu[pk+1][r];
				d += a[s2][k] * ndu[r][pk];
			}
			ders[k*(degree+1)+r] = d;
			//swap rows
			j = s1; s1 = s2; s2 = j;
		}
	}
	//Delete ndu and A arrays
	for (WPUInt i=0; i<=degree; i++) delete ndu[i];
	delete ndu;
	delete a[0];
	delete a[1];
	delete a;
	
	//Multiply each der value by the factorial
	r = degree;
	for (k=1; k<=(int)useDer; k++) {
		for (j=0; j<=(int)degree; j++) 
			ders[k*(degree+1)+j] *= r;
		r *= (degree - k);
	}

	/*** Debug ***
	for (k=0; k<(der+1)*(degree+1); k++) 
		std::cout << "ders[" << k << "]: " << ders[k] << std::endl;	
/*** Debug ***/
	//Return the array
	return ders;
}

			
WPFloat* WCNurbs::LoadDefaultKnotPoints(const WPUInt &kp, const WPUInt &degree) {
	//Allocate space for the knot points array
	WPFloat *knotPoints = new WPFloat[kp];	
	//Make sure memory was allocated
	if (knotPoints == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbs::LoadDefaultKnotPoints - Out of memory for array.");
		//throw error	
		return NULL;
	}

	WPFloat x=0.0, dx=1.0/(kp-2*degree-1);
	//Set the value for each knot point
	for (WPUInt i=0; i<kp; i++) {
		//Set the bottom ones to zero
		if (i < degree) knotPoints[i] = 0.0;
		//Have regular steps in the middle
		else if (i < kp-degree) {
			knotPoints[i] = x;
			x += dx;
		}
		//Set the top ones to one
		else knotPoints[i] = 1.0;
	}
	return knotPoints;
}


WPFloat* WCNurbs::LoadBezierKnotPoints(const WPUInt &kp) {
	//Allocate space for the knot points array
	WPFloat *knotPoints = new WPFloat[kp];	
	//Make sure memory was allocated
	if (knotPoints == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbs::LoadBezierKnotPoints - Out of memory for array.");
		//throw error	
		return NULL;
	}
	
	//Loop through each kp and set it
	for (WPUInt i=0; i<kp; i++) {
		if (i < kp/2) knotPoints[i] = 0.0;
		else knotPoints[i] = 1.0;
	}
	return knotPoints;
}


WPFloat* WCNurbs::LoadUniformKnotPoints(const WPUInt &kp) {
	//Allocate space for the knot points array
	WPFloat *knotPoints = new WPFloat[kp];	
	//Make sure memory was allocated
	if (knotPoints == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbs::LoadUniformKnotPoints - Out of memory for array.");
		//throw error	
		return NULL;
	}
	
	WPFloat dx=1.0/(kp-1.0);
	//Set the value for each knot point
	for (WPUInt i=0; i<kp; i++)
		knotPoints[i] = dx * i;
	return knotPoints;
}


WPFloat* WCNurbs::LoadCustomKnotPoints(const std::vector<WPFloat> &inPoints) {
	//Make sure knotpoints is not null
	if (inPoints.size() == 0) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbs::LoadCustomKnotPoints - knotPoints passed as NULL.");
		//throw error
		//Exit the function
		return NULL;
	}
	//Allocate space for the knot points array
	WPFloat *knotPoints = new WPFloat[inPoints.size()];	
	//Make sure memory was allocated
	if (knotPoints == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbs::LoadCustomKnotPoints - Out of memory for array.");
		//throw error	
		return NULL;
	}
	
	WPFloat min = knotPoints[0];
	//Copy the data into the array
	for (WPUInt i=0; i<inPoints.size(); i++) {
		knotPoints[i] = inPoints.at(i);
		//Make sure they are in ascending order
		if (knotPoints[i] >= min) min = knotPoints[i];
		knotPoints[i] = min;
	}
	return knotPoints;
}


WPFloat WCNurbs::EstimateLength(const std::vector<WCVector4> &controlPoints) {
	//Initialize length variable
	WPFloat length = 0.0;
	//Approximate length using control points
	for (WPUInt i=0; i<controlPoints.size()-1; i++) {
		//Get vector from control point to the next
		length += controlPoints.at(i+1).Distance( controlPoints.at(i) );
	}
	return length;
}


WPFloat WCNurbs::EstimateLengthU(const std::vector<WCVector4> &controlPoints, const WPUInt &numCPU) {
	//Initialize length variable
	WPFloat length = 0.0;
	//Approximate length using control points along u edge with v index = 0
	for (WPUInt i=0; i<numCPU-1; i++) {
		//Get vector from one control point to the next
		length += controlPoints.at(i+1).Distance( controlPoints.at(i) );
	}
	return length;
}


WPFloat WCNurbs::EstimateLengthV(const std::vector<WCVector4> &controlPoints, const WPUInt &numCPV) {
	//Initialize length variable
	WPFloat length = 0.0;
	WPUInt numCPU = controlPoints.size() / numCPV;
	//Approximate length using control points along v edge with u index = 0
	for (WPUInt i=0; i<numCPV-1; i++) {
		//Get vector from one control point to the next
		length += controlPoints.at((i+1)*numCPU).Distance( controlPoints.at(i*numCPU) );
	}
	return length;
}


/***********************************************~***************************************************/


void WCNurbs::CircularPoints(const WCVector4 &center, const WCVector4 &xUnit, const WCVector4 &yUnit,
	const WPFloat &radius, const WPFloat &startAngleDeg, const WPFloat &endAngleDeg,
	std::vector<WCVector4> &controlPoints, std::vector<WPFloat> &knotPoints) {
	//Setup angle parameters
	WPFloat startAngle = (2.0 * M_PI *startAngleDeg) / 360.0;
	WPFloat endAngle = (2.0 * M_PI * endAngleDeg) / 360.0;

	//Determine some angle stuff
	if (endAngle < startAngle) endAngle = (2.0 * M_PI) + endAngle;
	WPFloat theta = endAngle - startAngle;
	WPUInt numArcs;
	if (theta <= M_PI_2) numArcs = 1;
	else if (theta <= M_PI) numArcs = 2;
	else if (theta <= (M_PI_2 + M_PI)) numArcs = 3;
	else numArcs = 4;
	WPFloat deltaTheta = theta / numArcs;
	WPUInt numControlPoints = 2 * numArcs + 1;
	WPUInt numKnotPoints = 2 * numArcs + 4;

//Try to isolate some win32 specifics
#ifdef __WIN32__
	WCVector4 points[64];
	WPFloat knots[64];
#else
	WCVector4 points[numControlPoints+1];
	WPFloat *knots = new WPFloat[numKnotPoints];
#endif
	
	
	WPFloat w1 = cos(deltaTheta / 2.0);
	WCVector4 p0 = center + (xUnit * radius * cos(startAngle)) + (yUnit * radius * sin(startAngle));
	WCVector4 t0 = (yUnit * cos(startAngle)) - (xUnit * sin(startAngle));
	points[0] = p0;
	WPUInt index = 0;
	WPFloat angle = startAngle;
	
	//Create all the control points
	WCVector4 p1, p2, t2;
	WPFloat dummy;
	for (WPUInt i=0; i<=numArcs; i++) {
		angle = angle + deltaTheta;
		p2 = center + (xUnit * radius * cos(angle)) + (yUnit * radius * sin(angle));
		points[index+2] = p2;
		t2 = (yUnit * cos(angle)) - (xUnit * sin(angle));
		RayRayIntersection(p0, t0, p2, t2, dummy, dummy, p1);
		p1.L(w1);
		points[index+1] = p1;
		index += 2;
		if (i < numArcs) {
			p0 = p2;
			t0 = t2;
		}
	}
	
	//Load knot vector
	WPUInt tmpInt = 2 * numArcs + 1;
	for (int i=0; i<3; i++) {
		knots[i] = 0.0;
		knots[i + tmpInt] = 1.0;		
	}
	switch(numArcs) {
		case 1: break;
		case 2: knots[3] = knots[4] = 0.5; break;
		case 3: knots[3] = knots[4] = 1.0 / 3.0;
				knots[5] = knots[6] = 2.0 / 3.0; break;
		case 4: knots[3] = knots[4] = 0.25;
				knots[5] = knots[6] = 0.5;
				knots[7] = knots[8] = 0.75; break;
	}

	//Make sure cp and kp are clear
	controlPoints.clear();
	knotPoints.clear();

	//Create vector of control points
	for (WPUInt i=0; i<numControlPoints; i++)
		controlPoints.push_back(points[i]);
	//Create vector of knot points
	for (WPUInt i=0; i<numKnotPoints; i++)
		knotPoints.push_back(knots[i]);

	//Delete arrays
#ifndef __WIN32__
//	delete points;
	delete knots;
#endif
}


/***********************************************~***************************************************/


void WCNurbs::EllipsePoints(const WCVector4 &center, const WCVector4 &xUnit, const WCVector4 &yUnit, 
	const WPFloat &major, const WPFloat &minor, std::vector<WCVector4> &controlPoints, std::vector<WPFloat> &knotPoints) {
	//Calculate the control point values
	WCVector4 x = xUnit * major;
	WCVector4 y = yUnit * minor;
	controlPoints.push_back( center + x );
	controlPoints.push_back( center + x + y ); 
	controlPoints.push_back( center + y );
	controlPoints.push_back( center - x + y );
	controlPoints.push_back( center - x );
	controlPoints.push_back( center - x - y );
	controlPoints.push_back( center - y );
	controlPoints.push_back( center + x - y );
	controlPoints.push_back( controlPoints[0] );
	//Set the control point weights
	WPFloat w = M_SQRT2 / 2.0;
	controlPoints[0].L( 1.0 );
	controlPoints[1].L( w );
	controlPoints[2].L( 1.0 );
	controlPoints[3].L( w );
	controlPoints[4].L( 1.0 );
	controlPoints[5].L( w );
	controlPoints[6].L( 1.0 );
	controlPoints[7].L( w );
	controlPoints[8].L( 1.0 );

	//Set up arrays for the control points and knots
	knotPoints.push_back(0.0);
	knotPoints.push_back(0.0);
	knotPoints.push_back(0.0);
	knotPoints.push_back(0.25);
	knotPoints.push_back(0.25);
	knotPoints.push_back(0.5);
	knotPoints.push_back(0.5);
	knotPoints.push_back(0.75);
	knotPoints.push_back(0.75);
	knotPoints.push_back(1.0);
	knotPoints.push_back(1.0);
	knotPoints.push_back(1.0);
}


/***********************************************~***************************************************/

