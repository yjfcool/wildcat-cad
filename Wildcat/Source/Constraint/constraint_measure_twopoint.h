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


#ifndef __CONSTRAINT_MEASURE_TWOPOINT_H__
#define __CONSTRAINT_MEASURE_TWOPOINT_H__


/*** Included Header Files ***/
#include "Constraint/wgcsl.h"
#include "Constraint/constraint_measure.h"


/*** Local Defines ***/
//None


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


class WCConstraintMeasureTwoPoint : public WCConstraintMeasure {
private:
	WCMeasureType								_type;												//!< Measure type indicator
	WCVector4									_p0, _p1;											//!< Points for the measure
	GLfloat										_data[7*2*3];										//!< Buffer data for rendering

	//Private Methods
	void CalculateAbsolute(void);																	//!< Calculate absolute measure
	void CalculateHorizontal(void);																	//!< Calculate horizontal measure
	void CalculateVertical(void);																	//!< Calculate vertical measure

	//Deny Access
	WCConstraintMeasureTwoPoint();																	//!< Deny access to default constructor
	WCConstraintMeasureTwoPoint(const WCConstraintMeasureTwoPoint &measure);						//!< Deny access to copy constructor
public:
	//Constructors and Destructors
	WCConstraintMeasureTwoPoint(WCSketchFeature *parent, const std::string &label,					//!< Primary constructor
												const WCMeasureType &type, const WCVector4 &p0, const WCVector4 &p1,
												const WCMatrix4 &toPlane, const WCMatrix4 &fromPlane,
												const WPFloat offset,	const WPFloat labelOffset);
	~WCConstraintMeasureTwoPoint()				{ }													//!< Default destructor

	//Member Access Methods
	void SlewOffsets(const WPFloat &xSlew, const WPFloat &ySlew);									//!< Move offsets according to an x,y slew
	inline WCVector4 FirstPoint(void)			{ return this->_p0; }								//!< Get the first point
	inline WCVector4 SecondPoint(void)			{ return this->_p1; }								//!< Get the second point
	void Points(const WCVector4 &p0, const WCVector4 &p1);											//!< Set the points of the measure
	inline WCMeasureType Type(void) const		{ return this->_type; }								//!< Get the measure type
	void Type(const WCMeasureType &type);															//!< Set the measure type

	//Render Methods
	void Render(const WCColor &color, const bool &selectMode=false);								//!< Render the object
};


/***********************************************~***************************************************/


#endif //__CONSTRAINT_MEASURE_TWOPOINT_H__

