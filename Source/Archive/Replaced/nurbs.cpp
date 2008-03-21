#include "nurbs.h"


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


WPUInt WCNurbs::FindSpan(const WPUInt numCP, const WPUInt degree, const WPFloat u, const WPFloat *knotPoints) {
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


WPUInt WCNurbs::FindUniformSpan(const WPUInt numCP, const WPUInt degree,	const WPFloat u, const WPFloat *knotPoints) {
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


void WCNurbs::FindSpanMultiplicity(const WPUInt numCP, const WPUInt degree, const WPFloat u, const WPFloat *knotPoints, WPUInt &span, WPUInt &multiplicity) {
	//First get the span
	span = WCNurbs::FindSpan(numCP, degree, u, knotPoints);
	//The see how many times u occurs
	multiplicity = 0;
	for (int i=0; i<=numCP+degree; i++) if (knotPoints[i] == u) multiplicity++;
}


WPFloat* WCNurbs::BasisValues(const WPUInt span, const WPFloat u, const WPUInt degree, const WPFloat *knotPoints, const WPUInt der) {
	WPFloat ndu[degree+1][degree+1];
	WPFloat a[2][degree+1];
	WPFloat left[degree+1];
	WPFloat right[degree+1];
	int j, k, r, s1, s2, j1, j2, rk, pk;
	WPFloat saved, temp, d;
	WPUInt useDer = std::min(der, degree-1);
	
	//Allocate space for the output
	WPFloat *ders = new WPFloat[(der+1) * (degree+1)];
	//Check to make sure memory is allocated
	if (ders == NULL) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::BasisValues - Not able to allocate memory for results array.");
	//Set all values to zero
	memset(ders, 0.0, sizeof(WPFloat)*(der+1)*(degree+1));
		
	//Calculate basis values
	ndu[0][0] = 1.0;
	for (j=1; j<=degree; j++) {
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
	for (j=0; j<=degree; j++) ders[j] = ndu[j][degree];
	//Check for der==0 optimization
	if (der == 0) return ders;
	
	//Loop over function index
	for (r=0; r<=degree; r++) {
		s1 = 0;
		s2 = 1;
		a[0][0] = 1.0;
		//Loop to compute kth derivative
		for (k=1; k<=useDer; k++) {
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
	//Multiply each der value by the factorial
	r = degree;
	for (k=1; k<=useDer; k++) {
		for (j=0; j<=degree; j++) 
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



			
WPFloat* WCNurbs::LoadDefaultKnotPoints(WPUInt kp, WPUInt degree) {
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
	for (int i=0; i<kp; i++) {
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


WPFloat* WCNurbs::LoadBezierKnotPoints(WPUInt kp) {
	//Allocate space for the knot points array
	WPFloat *knotPoints = new WPFloat[kp];	
	//Make sure memory was allocated
	if (knotPoints == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbs::LoadBezierKnotPoints - Out of memory for array.");
		//throw error	
		return NULL;
	}
	
	//Loop through each kp and set it
	for (int i=0; i<kp; i++) {
		if (i < kp/2) knotPoints[i] = 0.0;
		else knotPoints[i] = 1.0;
	}
	return knotPoints;
}


WPFloat* WCNurbs::LoadUniformKnotPoints(WPUInt kp) {
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
	for (int i=0; i<kp; i++)
		knotPoints[i] = dx * i;
	return knotPoints;
}


WPFloat* WCNurbs::LoadCustomKnotPoints(const WCCollection<WPFloat> *inPoints) {
	//Make sure knotpoints is not null
	if (inPoints == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbs::LoadCustomKnotPoints - knotPoints passed as NULL.");
		//throw error
		//Exit the function
		return NULL;
	}
	//Allocate space for the knot points array
	WPFloat *knotPoints = new WPFloat[inPoints->Size()];	
	//Make sure memory was allocated
	if (knotPoints == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbs::LoadCustomKnotPoints - Out of memory for array.");
		//throw error	
		return NULL;
	}
	
	WPFloat min = knotPoints[0];
	//Copy the data into the array
	for (int i=0; i<inPoints->Size(); i++) {
		knotPoints[i] = inPoints->At(i);
		//Make sure they are in ascending order
		if (knotPoints[i] >= min) min = knotPoints[i];
		knotPoints[i] = min;
	}
	return knotPoints;
}


WPFloat WCNurbs::EstimateLength(WCCollection<WCControlPoint*> &cpCollection) {
	//Initialize length variable
	WPFloat length = 0.0;
	//Approximate length using control points
	for (int i=0; i<cpCollection.Size()-1; i++) {
		//Get vector from control point to the next
		length += (cpCollection.At(i+1)->Data() - cpCollection.At(i)->Data()).Magnitude();
	}
	return length;
}


WPFloat WCNurbs::EstimateLengthU(WCCollection<WCControlPoint*> &cpCollection, WPUInt numCPU) {
	//Initialize length variable
	WPFloat length = 0.0;
	//Approximate length using control points along u edge with v index = 0
	for (int i=0; i<numCPU-1; i++) {
		//Get vector from one control point to the next
		length += (cpCollection.At(i+1)->Data() - cpCollection.At(i)->Data()).Magnitude();
	}
	return length;
}


WPFloat WCNurbs::EstimateLengthV(WCCollection<WCControlPoint*> &cpCollection, WPUInt numCPV) {
	//Initialize length variable
	WPFloat length = 0.0;
	WPUInt numCPU = cpCollection.Size() / numCPV;
	//Approximate length using control points along v edge with u index = 0
	for (int i=0; i<numCPV-1; i++) {
		//Get vector from one control point to the next
		length += (cpCollection.At((i+1)*numCPU)->Data() - cpCollection.At(i*numCPU)->Data()).Magnitude();
	}
	return length;
}


/***********************************************~***************************************************/
	
