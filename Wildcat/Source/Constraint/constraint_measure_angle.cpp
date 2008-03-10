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
#include "Constraint/constraint_measure_angle.h"
#include "Kernel/document.h"


//Locall Defined Values
#define MEASUREANGLE_LABELDISPLACEMENT			-0.05


/***********************************************~***************************************************/


void WCConstraintMeasureAngle::CalculateBuffer(void) {
	//Setup unit vectors
	this->_uUnit = this->_fromPlane * WCVector4(1.0, 0.0, 0.0, 0.0);
	this->_vUnit = this->_fromPlane * WCVector4(0.0, 1.0, 0.0, 0.0);	

	//Get line directions and norms
	WCVector4 line0Dir = this->_first - this->_center;
	line0Dir.Normalize();
	WCVector4 line0Norm = this->_toPlane * line0Dir;
	WCVector4 line1Dir = this->_second - this->_center;
	line1Dir.Normalize();
	WCVector4 line1Norm = this->_toPlane * line1Dir;

	//Determine start and end angles
	WPFloat angle = Angle(this->_first, this->_center, this->_second);
	WPFloat startAngleDeg = atan2(line0Norm.J(), line0Norm.I()) * 180.0 / M_PI;
//	std::cout << "StartAngle: " << startAngleDeg << std::endl;
	WPFloat endAngleDeg = atan2(line1Norm.J(), line1Norm.I()) * 180.0 / M_PI;
//	std::cout << "EndAngle: " << endAngleDeg << std::endl;
	//See if angles need to be swapped
	if ((fabs(endAngleDeg - startAngleDeg - angle) > 0.01) && (this->_shortSide)) {
//		std::cout << "Reversed\n";
		this->_isReversed = true;
		std::swap(startAngleDeg, endAngleDeg);
	}
	else this->_isReversed = false;

	//Generate the curve
	if (this->_curve != NULL) delete this->_curve;
	this->_curve =  WCNurbsCurve::CircularArc(this->_parent->Document()->Scene()->GeometryContext(), 
											  this->_center, this->_uUnit, this->_vUnit, this->_offset, 
											  startAngleDeg,  endAngleDeg);

	/*** Arrow stuff ***/

	WPFloat arrow = 0.05;
	WCVector4 tmp1;
	line0Norm.Set(line0Norm.J(), -line0Norm.I(), 0.0, 0.0);
	line0Norm = this->_fromPlane * line0Norm;
	//Invert tan if inside arc
	if (this->_labelOffset < 0.0) line0Norm *= -1.0;
	line1Norm.Set(line1Norm.J(), -line1Norm.I(), 0.0, 0.0);
	line1Norm = this->_fromPlane * line1Norm;
	//Invert tan if inside arc
	if (this->_labelOffset < 0.0) line1Norm *= -1.0;
	WCVector4 p0 = this->_curve->Evaluate(0.0);
	WCVector4 p1 = this->_curve->Evaluate(1.0);
	//See about reversed
	if (this->_isReversed) {
		std::swap(line0Dir, line1Dir);
		std::swap(line0Norm, line1Norm);
	}

	/*** Determine starting arrow values ***/

	//Set staring arrow values
	this->_data[0] = p0.I();
	this->_data[1] = p0.J();
	this->_data[2] = p0.K();
	tmp1 = p0 - (line0Norm * arrow) - (line0Dir * arrow);
	this->_data[3] = tmp1.I();
	this->_data[4] = tmp1.J();
	this->_data[5] = tmp1.K();
	this->_data[6] = p0.I();
	this->_data[7] = p0.J();
	this->_data[8] = p0.K();
	tmp1 = p0 - (line0Norm * arrow) + (line0Dir * arrow);
	this->_data[9] = tmp1.I();
	this->_data[10] = tmp1.J();
	this->_data[11] = tmp1.K();

	/*** Determine ending arrow values ***/

	//Set ending arrow values
	this->_data[12] = p1.I();
	this->_data[13] = p1.J();
	this->_data[14] = p1.K();
	tmp1 = p1 + (line1Norm * arrow) - (line1Dir * arrow);
	this->_data[15] = tmp1.I();
	this->_data[16] = tmp1.J();
	this->_data[17] = tmp1.K();
	this->_data[18] = p1.I();
	this->_data[19] = p1.J();
	this->_data[20] = p1.K();
	tmp1 = p1 + (line1Norm * arrow) + (line1Dir * arrow);
	this->_data[21] = tmp1.I();
	this->_data[22] = tmp1.J();
	this->_data[23] = tmp1.K();
		
	/*** Determine position for label ***/
	if (this->_isReversed) this->_labelPos = this->_curve->Evaluate(1.0 - this->_labelOffset);
	else this->_labelPos = this->_curve->Evaluate(this->_labelOffset);
}


/***********************************************~***************************************************/


WCConstraintMeasureAngle::WCConstraintMeasureAngle(WCSketchFeature *parent, const std::string &label,
	const WCVector4 &center, const WCVector4 &first, const WCVector4 &second, const bool &shortSide,
	const WCMatrix4 &toPlane, const WCMatrix4 &fromPlane, const WPFloat offset,	const WPFloat labelOffset) :
	::WCConstraintMeasure(parent, label, toPlane, fromPlane, offset, labelOffset),
	_center(center), _first(first), _second(second), _curve(NULL), _shortSide(shortSide), _isReversed(false) {
	//Nothing else for now
}


WCConstraintMeasureAngle::~WCConstraintMeasureAngle() {
	//Delete the curve if appropriate
	if (this->_curve != NULL) delete this->_curve;
}


void WCConstraintMeasureAngle::SlewOffsets(const WPFloat &xSlew, const WPFloat &ySlew) {
	//Get angle in radians
	WPFloat angle = Angle(this->_first, this->_center, this->_second) * M_PI / 180.0;
	WPFloat rotAngle = angle * this->_labelOffset;
	//Take reversal into account
	if (this->_isReversed) rotAngle *= -1.0;

	//Determine 1st dir (on plane)
	WCVector4 firstDir = this->_first - this->_center;
	firstDir.Normalize(true);
	firstDir = this->_toPlane * firstDir;
	//Rotate right dir by rotAngle
	WCVector4 dir(0.0, 0.0, 0.0, 0.0);
	dir.I( firstDir.I() * cos(rotAngle) - firstDir.J() * sin(rotAngle) );
	dir.J( firstDir.I() * sin(rotAngle) + firstDir.J() * cos(rotAngle) );
	//Get pt
	WCVector4 center = this->_toPlane * this->_center;
	WCVector4 first = this->_toPlane * this->_first;
	WCVector4 second = this->_toPlane * this->_second;
	WCVector4 pt = center + dir * this->_offset;
	pt.Set(pt.I() + xSlew, pt.J() + ySlew, 0.0, 1.0);
	WCVector4 pt3D = this->_fromPlane * pt;

	//Offset is distance from center to pt
	this->_offset = std::max(0.0, this->_center.Distance(pt3D));
	WPFloat newAngle = Angle(this->_first, this->_center, pt3D) * M_PI / 180.0;
	//See if pt is to the right of first
	bool ptOnRight = IsOnRight2D(center.I(), center.J(), first.I(), first.J(), pt.I(), pt.J());
	bool secOnRight = IsOnRight2D(center.I(), center.J(), first.I(), first.J(), second.I(), second.J());
	//If pt and sec are on opposite sides of center->first, angle is negative
	if (ptOnRight != secOnRight) newAngle *= -1.0;
	//Label offset is percent of newAngle to angle
	this->_labelOffset = newAngle / angle;
	//Limit label offset to [0,1]
	this->_labelOffset = std::max(0.0, std::min(1.0, this->_labelOffset));
	//Mark as dirty
	this->_isDirty = true;
}


void WCConstraintMeasureAngle::Render(const WCColor &color, const bool &selectMode) {
	//See if dirty
	if (this->_isDirty) {
		//Regenerate the buffer
		this->CalculateBuffer();
		//Mark as clean
		this->_isDirty = false;
	}
	//Set some parameters
	color.Enable();
	//Determine measure text unit vectors
	WPFloat zoom = this->_parent->Document()->Scene()->ActiveCamera()->Zoom();
	WCVector4 labelAdj(0.0, MEASUREANGLE_LABELDISPLACEMENT / zoom, 0.0, 0.0);
	labelAdj = this->_fromPlane * labelAdj;
	WCVector4 uUnit = this->_uUnit / zoom;
	WCVector4 vUnit = this->_vUnit / zoom;
	//Check the color
	if (!selectMode) {
		//Set some parameters
		glLineWidth(1.0);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, this->_data);
		//Draw the lines
		glDrawArrays(GL_LINES, 0, 8);
		//Clean up a bit
		glDisableClientState(GL_VERTEX_ARRAY);
		//Draw the curve
		if (this->_curve != NULL) this->_curve->Render(0, color, zoom);
		this->_text->DrawAtPoint(this->_labelPos + labelAdj, uUnit, vUnit);
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

