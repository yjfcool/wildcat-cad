#ifndef __NURBS_H__
#define __NURBS_H__


/*** Included Header Files ***/
#include "visual_object.h"
#include "collection.h"
#include "control_point.h"


/*** Local Defines ***/
extern WPFloat* __bezier_coef[8];


/***********************************************~***************************************************/


class WCNurbs {																			
public:
	static WPUInt FindSpan(const WPUInt numCP, const WPUInt degree,									//!< Find the span of the index value
					const WPFloat u, const WPFloat *knotPoints);
	static WPUInt FindUniformSpan(const WPUInt numCP, const WPUInt degree,							//!< Find the span of the index value for a uniform array
					const WPFloat u, const WPFloat *knotPoints);
	static void FindSpanMultiplicity(const WPUInt numCP, const WPUInt degree,						//!< Find the span and multiplicity
					const WPFloat u, const WPFloat *knotPoints, WPUInt &span, WPUInt &multiplicity);
	static WPFloat *BasisValues(const WPUInt span, const WPFloat u, const WPUInt degree,			//!< Calculate the basis functions on U
					const WPFloat *knotPoints, const WPUInt der=0);	
	static WPFloat* LoadDefaultKnotPoints(const WPUInt kp, const WPUInt degree);					//!< Initialize an array to default knot values
	static WPFloat* LoadBezierKnotPoints(const WPUInt kp);											//!< Initialize an array to bezier knot values
	static WPFloat* LoadUniformKnotPoints(const WPUInt kp);											//!< Initialize an array to uniform knot values
	static WPFloat* LoadCustomKnotPoints(const WCCollection<WPFloat> *inPoints);					//!< Initialize an array to custom knot values
	static WPFloat EstimateLength(WCCollection<WCControlPoint*> &cpCollection);						//!< Estimate the length of the curve
	static WPFloat EstimateLengthU(WCCollection<WCControlPoint*> &cpCollection, WPUInt numCPU);		//!< Estimate the U direction length of the surface
	static WPFloat EstimateLengthV(WCCollection<WCControlPoint*> &cpCollection, WPUInt numCPU);		//!< Estimate the V direction length of the surface
};
 

/***********************************************~****************************************************/


#endif //__NURBS_H__

