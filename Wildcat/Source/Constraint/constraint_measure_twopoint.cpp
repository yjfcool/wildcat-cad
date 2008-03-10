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
#include "Constraint/constraint_measure_twopoint.h"
#include "Kernel/document.h"


/***********************************************~***************************************************/


void WCConstraintMeasureTwoPoint::CalculateAbsolute(void) {
	WCVector4 p0 = this->_toPlane * this->_p0;
	WCVector4 p1 = this->_toPlane * this->_p1;
	WCVector4 unit(p1 - p0);
	unit.Normalize(true);
	unit.L(0.0);
	WCVector4 tan(unit.J(), -unit.I(), 0.0, 0.0);
	WCVector4 tmp1, tmp2, c0, c1, c0t, c1t;
	WPFloat overshoot, arrowX, arrowY;
	bool arrowsOut;
	
	//Adjust for overshoot
	if (this->_offset > 0) overshoot = CMTMP;
	else overshoot = -CMTMP;
	
	/*** First offset line ***/
	tmp1 = p0 + (tan * overshoot);
	tmp1 = this->_fromPlane * tmp1;
	this->_data[0] = tmp1.I();				
	this->_data[1] = tmp1.J();
	this->_data[2] = tmp1.K();
	tmp1 = p0 + (tan * (this->_offset+overshoot));
	tmp1 = this->_fromPlane * tmp1;
	this->_data[3] = tmp1.I();
	this->_data[4] = tmp1.J();
	this->_data[5] = tmp1.K();

	/*** Second offset line ***/
	tmp1 = p1 + (tan * overshoot);
	tmp1 = this->_fromPlane * tmp1;
	this->_data[6] = tmp1.I();				
	this->_data[7] = tmp1.J();
	this->_data[8] = tmp1.K();
	tmp1 = p1 + (tan * (this->_offset+overshoot));
	tmp1 = this->_fromPlane * tmp1;
	this->_data[9] = tmp1.I();
	this->_data[10] = tmp1.J();
	this->_data[11] = tmp1.K();

	/*** Cross line ***/
	c0 = p0 + (tan * this->_offset);
	c0t = this->_fromPlane * c0;
	this->_data[12] = c0t.I();
	this->_data[13] = c0t.J();
	this->_data[14] = c0t.K();
	c1 = p1 + (tan * this->_offset);
	c1t = this->_fromPlane * c1;
	this->_data[15] = c1t.I();
	this->_data[16] = c1t.J();
	this->_data[17] = c1t.K();	

	//Determine if arrows are inside or outside
	if ((this->_labelOffset < 0.0) || (this->_labelOffset > 1.0)) arrowsOut = true;
	else arrowsOut = false;
	arrowX = 0.05;
	arrowY = 0.05;

	/*** Start-end Arrows ***/
	//Adjust for inside
	if (!arrowsOut) {
		tmp1 = c0 + (unit * arrowX) - (tan * arrowY);
		tmp2 = c0 + (unit * arrowX) + (tan * arrowY);
	}
	else {
		tmp1 = c0 - (unit * arrowX) - (tan * arrowY);
		tmp2 = c0 - (unit * arrowX) + (tan * arrowY);
	}
	tmp1 = this->_fromPlane * tmp1;
	tmp2 = this->_fromPlane * tmp2;
	this->_data[18] = c0t.I();
	this->_data[19] = c0t.J();
	this->_data[20] = c0t.K();
	this->_data[21] = tmp1.I();
	this->_data[22] = tmp1.J();
	this->_data[23] = tmp1.K();
	this->_data[24] = c0t.I();
	this->_data[25] = c0t.J();
	this->_data[26] = c0t.K();
	this->_data[27] = tmp2.I();
	this->_data[28] = tmp2.J();
	this->_data[29] = tmp2.K();
	
	/*** End-end Arrows ***/
	//Adjust for inside
	if (!arrowsOut) {
		tmp1 = c1 - (unit * arrowX) - (tan * arrowY);
		tmp2 = c1 - (unit * arrowX) + (tan * arrowY);
	}
	else {
		tmp1 = c1 + (unit * arrowX) - (tan * arrowY);
		tmp2 = c1 + (unit * arrowX) + (tan * arrowY);
	}
	tmp1 = this->_fromPlane * tmp1;
	tmp2 = this->_fromPlane * tmp2;
	this->_data[30] = c1t.I();
	this->_data[31] = c1t.J();
	this->_data[32] = c1t.K();
	this->_data[33] = tmp1.I();
	this->_data[34] = tmp1.J();
	this->_data[35] = tmp1.K();
	this->_data[36] = c1t.I();
	this->_data[37] = c1t.J();
	this->_data[38] = c1t.K();
	this->_data[39] = tmp2.I();
	this->_data[40] = tmp2.J();
	this->_data[41] = tmp2.K();
	
	/*** Determine position for label ***/
	//Orient unit vectors
	if (unit.I() < 0.0) unit *= -1.0;
	if (tan.J() <= 0.0) tan *= -1.0;
	//Label Pos
	this->_labelPos = (c0 * (1.0 - this->_labelOffset)) + (c1 * this->_labelOffset);
	//Adjust label for size of label
	//...
	//Project points back into 3D
	this->_labelPos = this->_fromPlane * this->_labelPos;
	this->_uUnit = this->_fromPlane * unit;
	this->_vUnit = this->_fromPlane * tan;
}


void WCConstraintMeasureTwoPoint::CalculateHorizontal(void) {
	WCVector4 p0 = this->_toPlane * this->_p0;
	WCVector4 p1 = this->_toPlane * this->_p1;
	WCVector4 uUnit(1.0, 0.0, 0.0, 0.0);
	WCVector4 vUnit(0.0, 1.0, 0.0, 0.0);
	WCVector4 tmp1, tmp2;
	WPFloat overshoot, arrowX, arrowY;
	bool arrowsOut;

	//Location of the main offset
	WPFloat yVal = p0.J() + this->_offset;
	
	/*** First offset line ***/
	//Adjust for overshoot
	if (yVal < p0.J()) overshoot = -CMTMP;
	else overshoot = CMTMP;
	tmp1.Set(p0.I(), p0.J() + overshoot, 0.0, 1.0);
	tmp2.Set(p0.I(), yVal + overshoot, 0.0, 1.0);
	//Transform to the plane
	tmp1 = this->_fromPlane * tmp1;
	tmp2 = this->_fromPlane * tmp2;
	this->_data[0] = tmp1.I();				
	this->_data[1] = tmp1.J();
	this->_data[2] = tmp1.K();
	this->_data[3] = tmp2.I();
	this->_data[4] = tmp2.J();
	this->_data[5] = tmp2.K();

	/*** Second offset line ***/
	//Adjust for overshoot
	if (yVal < p1.J()) overshoot = -CMTMP;
	else overshoot = CMTMP;
	tmp1.Set(p1.I(), p1.J() + overshoot, 0.0, 1.0);
	tmp2.Set(p1.I(), yVal + overshoot, 0.0, 1.0);
	//Transform to the plane
	tmp1 = this->_fromPlane * tmp1;
	tmp2 = this->_fromPlane * tmp2;
	this->_data[6] = tmp1.I();				
	this->_data[7] = tmp1.J();
	this->_data[8] = tmp1.K();
	this->_data[9] = tmp2.I();
	this->_data[10] = tmp2.J();
	this->_data[11] = tmp2.K();

	/*** Cross line ***/
	WCVector4 c0(p0.I(), yVal, 0.0, 1.0);
	WCVector4 c0t = this->_fromPlane * c0;
	this->_data[12] = c0t.I();
	this->_data[13] = c0t.J();
	this->_data[14] = c0t.K();
	WCVector4 c1(p1.I(), yVal, 0.0, 1.0);
	WCVector4 c1t = this->_fromPlane * c1;
	this->_data[15] = c1t.I();
	this->_data[16] = c1t.J();
	this->_data[17] = c1t.K();

	//Determine if arrows are inside or outside
	if ((this->_labelOffset < 0.0) || (this->_labelOffset > 1.0)) arrowsOut = true;
	else arrowsOut = false;
	arrowX = 0.05;
	arrowY = 0.05;

	/*** Start-end Arrows ***/
	//Adjust for inside and top-bottom
	if ((!arrowsOut) && (c0.I() < c1.I())) {
		tmp1.Set(c0.I() + arrowX, c0.J() - arrowY, 0.0, 1.0);
		tmp2.Set(c0.I() + arrowX, c0.J() + arrowY, 0.0, 1.0);
	}
	else {
		tmp1.Set(c0.I() - arrowX, c0.J() - arrowY, 0.0, 1.0);
		tmp2.Set(c0.I() - arrowX, c0.J() + arrowY, 0.0, 1.0);
	}
	tmp1 = this->_fromPlane * tmp1;
	tmp2 = this->_fromPlane * tmp2;
	this->_data[18] = c0t.I();
	this->_data[19] = c0t.J();
	this->_data[20] = c0t.K();
	this->_data[21] = tmp1.I();
	this->_data[22] = tmp1.J();
	this->_data[23] = tmp1.K();
	this->_data[24] = c0t.I();
	this->_data[25] = c0t.J();
	this->_data[26] = c0t.K();
	this->_data[27] = tmp2.I();
	this->_data[28] = tmp2.J();
	this->_data[29] = tmp2.K();

	/*** End-end Arrows ***/
	//Adjust for inside and top-bottom
	if ((!arrowsOut) && (c0.I() < c1.I())) {
		tmp1.Set(c1.I() - arrowX, c1.J() - arrowY, 0.0, 1.0);
		tmp2.Set(c1.I() - arrowX, c1.J() + arrowY, 0.0, 1.0);
	}
	else {
		tmp1.Set(c1.I() + arrowX, c1.J() - arrowY, 0.0, 1.0);
		tmp2.Set(c1.I() + arrowX, c1.J() + arrowY, 0.0, 1.0);
	}
	tmp1 = this->_fromPlane * tmp1;
	tmp2 = this->_fromPlane * tmp2;
	this->_data[30] = c1t.I();
	this->_data[31] = c1t.J();
	this->_data[32] = c1t.K();
	this->_data[33] = tmp1.I();
	this->_data[34] = tmp1.J();
	this->_data[35] = tmp1.K();
	this->_data[36] = c1t.I();
	this->_data[37] = c1t.J();
	this->_data[38] = c1t.K();
	this->_data[39] = tmp2.I();
	this->_data[40] = tmp2.J();
	this->_data[41] = tmp2.K();

	/*** Determine position for label ***/
	//Label Pos
	this->_labelPos = (c0 * (1.0 - this->_labelOffset)) + (c1 * this->_labelOffset);
	this->_labelPos = this->_fromPlane * this->_labelPos;
	//Orient unit vectors
	if (uUnit.I() < 0.0) uUnit *= -1.0;
	if (vUnit.J() <= 0.0) vUnit *= -1.0;
	//Label unit vectors
	this->_uUnit = this->_fromPlane * uUnit;
	this->_vUnit = this->_fromPlane * vUnit;
}


void WCConstraintMeasureTwoPoint::CalculateVertical(void) {
	WCVector4 p0 = this->_toPlane * this->_p0;
	WCVector4 p1 = this->_toPlane * this->_p1;
	WCVector4 uUnit(1.0, 0.0, 0.0, 0.0);
	WCVector4 vUnit(0.0, 1.0, 0.0, 0.0);
	WCVector4 tmp1, tmp2;
	WPFloat overshoot, arrowX, arrowY;
	bool arrowsOut;

	//Location of the main offset
	WPFloat xVal = p0.I() + this->_offset;
	
	/*** First offset line ***/
	//Adjust for overshoot
	if (xVal < p0.I()) overshoot = -CMTMP;
	else overshoot = CMTMP;
	tmp1.Set(p0.I() + overshoot, p0.J(), 0.0, 1.0);
	tmp2.Set(xVal + overshoot, p0.J(), 0.0, 1.0);
	//Transform to the plane
	tmp1 = this->_fromPlane * tmp1;
	tmp2 = this->_fromPlane * tmp2;
	this->_data[0] = tmp1.I();				
	this->_data[1] = tmp1.J();
	this->_data[2] = tmp1.K();
	this->_data[3] = tmp2.I();
	this->_data[4] = tmp2.J();
	this->_data[5] = tmp2.K();

	/*** Second offset line ***/
	//Adjust for overshoot
	if (xVal < p1.I()) overshoot = -CMTMP;
	else overshoot = CMTMP;
	tmp1.Set(p1.I() + overshoot, p1.J(), 0.0, 1.0);
	tmp2.Set(xVal + overshoot, p1.J(), 0.0, 1.0);
	//Transform to the plane
	tmp1 = this->_fromPlane * tmp1;
	tmp2 = this->_fromPlane * tmp2;
	this->_data[6] = tmp1.I();				
	this->_data[7] = tmp1.J();
	this->_data[8] = tmp1.K();
	this->_data[9] = tmp2.I();
	this->_data[10] = tmp2.J();
	this->_data[11] = tmp2.K();

	/*** Cross line ***/
	WCVector4 c0(xVal, p0.J(), 0.0, 1.0);
	WCVector4 c0t = this->_fromPlane * c0;
	this->_data[12] = c0t.I();
	this->_data[13] = c0t.J();
	this->_data[14] = c0t.K();
	WCVector4 c1(xVal, p1.J(), 0.0, 1.0);
	WCVector4 c1t = this->_fromPlane * c1;
	this->_data[15] = c1t.I();
	this->_data[16] = c1t.J();
	this->_data[17] = c1t.K();

	//Determine if arrows are inside or outside
	if ((this->_labelOffset < 0.0) || (this->_labelOffset > 1.0)) arrowsOut = true;
	else arrowsOut = false;
	arrowX = 0.05;
	arrowY = 0.05;

	/*** Start-end Arrows ***/
	//Adjust for inside and top-bottom
	if ((!arrowsOut) && (c0.J() < c1.J())) {
		tmp1.Set(c0.I() - arrowX, c0.J() + arrowY, 0.0, 1.0);
		tmp2.Set(c0.I() + arrowX, c0.J() + arrowY, 0.0, 1.0);
	}
	else {
		tmp1.Set(c0.I() - arrowX, c0.J() - arrowY, 0.0, 1.0);
		tmp2.Set(c0.I() + arrowX, c0.J() - arrowY, 0.0, 1.0);
	}
	tmp1 = this->_fromPlane * tmp1;
	tmp2 = this->_fromPlane * tmp2;
	this->_data[18] = c0t.I();
	this->_data[19] = c0t.J();
	this->_data[20] = c0t.K();
	this->_data[21] = tmp1.I();
	this->_data[22] = tmp1.J();
	this->_data[23] = tmp1.K();
	this->_data[24] = c0t.I();
	this->_data[25] = c0t.J();
	this->_data[26] = c0t.K();
	this->_data[27] = tmp2.I();
	this->_data[28] = tmp2.J();
	this->_data[29] = tmp2.K();

	/*** End-end Arrows ***/
	//Adjust for inside and top-bottom
	if ((!arrowsOut) && (c0.J() < c1.J())) {
		tmp1.Set(c1.I() - arrowX, c1.J() - arrowY, 0.0, 1.0);
		tmp2.Set(c1.I() + arrowX, c1.J() - arrowY, 0.0, 1.0);
	}
	else {
		tmp1.Set(c1.I() - arrowX, c1.J() + arrowY, 0.0, 1.0);
		tmp2.Set(c1.I() + arrowX, c1.J() + arrowY, 0.0, 1.0);
	}
	tmp1 = this->_fromPlane * tmp1;
	tmp2 = this->_fromPlane * tmp2;
	this->_data[30] = c1t.I();
	this->_data[31] = c1t.J();
	this->_data[32] = c1t.K();
	this->_data[33] = tmp1.I();
	this->_data[34] = tmp1.J();
	this->_data[35] = tmp1.K();
	this->_data[36] = c1t.I();
	this->_data[37] = c1t.J();
	this->_data[38] = c1t.K();
	this->_data[39] = tmp2.I();
	this->_data[40] = tmp2.J();
	this->_data[41] = tmp2.K();

	/*** Determine position for label ***/
	//Label Pos
	this->_labelPos = (c0 * (1.0 - this->_labelOffset)) + (c1 * this->_labelOffset);
	this->_labelPos = this->_fromPlane * this->_labelPos;
	//Orient unit vectors
	if (uUnit.I() < 0.0) uUnit *= -1.0;
	if (vUnit.J() <= 0.0) vUnit *= -1.0;
	//Label unit vectors
	this->_uUnit = this->_fromPlane * uUnit;
	this->_vUnit = this->_fromPlane * vUnit;
}


/***********************************************~***************************************************/


WCConstraintMeasureTwoPoint::WCConstraintMeasureTwoPoint(WCSketchFeature *parent, const std::string &label, 
	const WCMeasureType &type, const WCVector4 &p0, const WCVector4 &p1, const WCMatrix4 &toPlane, 
	const WCMatrix4 &fromPlane, const WPFloat offset, const WPFloat labelOffset) : 
	::WCConstraintMeasure(parent, label, toPlane, fromPlane, offset, labelOffset), _type(type), _p0(p0), _p1(p1) {
	//Nothing else for now
}


void WCConstraintMeasureTwoPoint::SlewOffsets(const WPFloat &xSlew, const WPFloat &ySlew) {
	//Get in plane units
	WCVector4 uUnit = this->_toPlane * this->_uUnit;
	WCVector4 vUnit = this->_toPlane * this->_vUnit;
	WCVector4 p0 = this->_toPlane * this->_p0;
	WCVector4 p1 = this->_toPlane * this->_p1;
	//Dot slew with units
	WCVector4 slew(xSlew, ySlew, 0.0);
	WPFloat uSlew = uUnit.DotProduct(slew);
	WPFloat vSlew = vUnit.DotProduct(slew);

	//Switch on measure type (try absolute first)
	if (this->_type == WCMeasureType::Absolute()) {
		//Adjust slew if p1 left of p0
		if (p1.I() < p0.I()) {
			uSlew *= -1.0;
			vSlew *= -1.0;
		}
		//Update offset values
		this->_offset -= vSlew;
		this->_labelOffset += (uSlew / this->_p0.Distance(this->_p1));
	}
	//Now try horizontal
	else if (this->_type == WCMeasureType::Horizontal()) {
		//Adjust slew if p1 above p0
		if (p1.J() > p0.I()) uSlew *= -1.0;
		//Update offset values
		this->_offset += vSlew;
		this->_labelOffset -= (uSlew / std::fabs(p0.I() - p1.I()));
	}
	//Must be vertical
	else {
		//Adjust slew if p1 above p0
		if (p1.J() > p0.J()) vSlew *= -1.0;
		//Update offset values
		this->_offset += uSlew;
		this->_labelOffset -= (vSlew / std::fabs(p0.J() - p1.J()));
	}
	//Mark as dirty
	this->_isDirty = true;
}


void WCConstraintMeasureTwoPoint::Points(const WCVector4 &p0, const WCVector4 &p1) {
	//Set the new values
	this->_p0 = p0;
	this->_p1 = p1;
	//Mark as dirty
	this->_isDirty = true;
}


void WCConstraintMeasureTwoPoint::Type(const WCMeasureType &type) {
	//Set the type
	this->_type = type;
	//Mark as dirty
	this->_isDirty = true;
}


void WCConstraintMeasureTwoPoint::Render(const WCColor &color, const bool &selectMode) {
	//See if dirty
	if (this->_isDirty) {
		//Switch on type
		if (this->_type == WCMeasureType::Absolute()) this->CalculateAbsolute();
		else if (this->_type == WCMeasureType::Horizontal()) this->CalculateHorizontal();
		else this->CalculateVertical();
		//Mark as clean
		this->_isDirty = false;
	}

	//Set some parameters
	color.Enable();
	//Determine measure text unit vectors
	WCVector4 uUnit = this->_uUnit / this->_parent->Document()->Scene()->ActiveCamera()->Zoom();
	WCVector4 vUnit = this->_vUnit / this->_parent->Document()->Scene()->ActiveCamera()->Zoom();
/*** DEBUG ***/
	//Orient unit vectors
//	if (uUnit.I() < 0.0) uUnit *= -1.0;
//	if (vUnit.J() <= 0.0) vUnit *= -1.0;
/*** DEBUG ***/
	//Check the color
	if (!selectMode) {
		//Set some parameters
		glLineWidth(1.0);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, this->_data);
		//Draw the lines
		glDrawArrays(GL_LINES, 0, 14);
		//Clean up a bit
		glDisableClientState(GL_VERTEX_ARRAY);
		this->_text->DrawAtPoint(this->_labelPos, uUnit, vUnit);
	}
	//Otherwise, draw text with the selected color
	else {
		//Determine size of texture
		GLfloat width = this->_text->Width();
		GLfloat height = this->_text->Height();
		//Find rect corners
		WCVector4 ul = this->_labelPos + (vUnit * height);
		WCVector4 ur = this->_labelPos + (vUnit * height) + (uUnit * width);
		WCVector4 lr = this->_labelPos + (uUnit * width);
		//Load data
		GLfloat vertData[12] = {this->_labelPos.I(), this->_labelPos.J(), this->_labelPos.K(), 
								ul.I(), ul.J(), ul.K(),
								ur.I(), ur.J(), ur.K(),
								lr.I(), lr.J(), lr.K() };		
		//Set drawing state
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, vertData);
		//Draw the quad
		glDrawArrays(GL_QUADS, 0, 4);
		//Clean up a bit
		glDisableClientState(GL_VERTEX_ARRAY);
	}
}


/***********************************************~***************************************************/

