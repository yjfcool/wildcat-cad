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


#ifndef __CONSTRAINT_MEASURE_RADIUS_H__
#define __CONSTRAINT_MEASURE_RADIUS_H__


/*** Included Header Files ***/
#include <Constraint/wgcsl.h>
#include <Constraint/constraint_measure.h>


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


class WCConstraintMeasureRadius : public WCConstraintMeasure {
private:
	WCGeometricCurve							*_curve;											//!< Curve for the measure
	WCVector4									_center;											//!< Center of the curve
	GLfloat										_data[4*2*3];										//!< Buffer data for rendering

	//Private Methods
	void CalculateBuffer(void);																		//!< Calculate position of measure

	//Deny Access
	WCConstraintMeasureRadius();																	//!< Deny access to default constructor
	WCConstraintMeasureRadius(const WCConstraintMeasureRadius &measure);							//!< Deny access to copy constructor
public:
	//Constructors and Destructors
	WCConstraintMeasureRadius(WCSketchFeature *parent, const std::string &label, WCGeometricCurve *curve,//!< Primary constructor
												const WCVector4 &center, const WCMatrix4 &toPlane, const WCMatrix4 &fromPlane,
												const WPFloat offset, const WPFloat labelOffset);
	~WCConstraintMeasureRadius()				{ }													//!< Default destructor

	//Member Access Methods
	void SlewOffsets(const WPFloat &xSlew, const WPFloat &ySlew);									//!< Move offsets according to an x,y slew

	//Render Methods
	void Render(const WCColor &color, const bool &selectMode=false);								//!< Render the object
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__CONSTRAINT_MEASURE_RADIUS_H__

