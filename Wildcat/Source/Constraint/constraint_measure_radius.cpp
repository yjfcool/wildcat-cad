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
#include "Constraint/constraint_measure_radius.h"
#include "Kernel/document.h"


//Locall Defined Values
#define MEASURERADIUS_LABELDISPLACEMENT			-0.05


/***********************************************~***************************************************/


void WCConstraintMeasureRadius::CalculateBuffer(void) {
	WCVector4 tmp1;
	this->_uUnit = WCVector4(1.0, 0.0, 0.0, 0.0);
	this->_vUnit = WCVector4(0.0, 1.0, 0.0, 0.0);
	//Setup point on curve
	WCVector4 p0 = this->_toPlane * this->_curve->Evaluate(this->_labelOffset);
	WCVector4 center = this->_toPlane * this->_center;
	WCVector4 tan = p0 - center;
	tan.Normalize(true);
	//Setup end of primary measure line
	WCVector4 p1 = p0 + (tan * this->_offset);

	/*** Primary Measure Line ***/
	tmp1 = this->_fromPlane * p0;
	this->_data[0] = tmp1.I();				
	this->_data[1] = tmp1.J();
	this->_data[2] = tmp1.K();
	tmp1 = this->_fromPlane * p1;
	this->_data[3] = tmp1.I();
	this->_data[4] = tmp1.J();
	this->_data[5] = tmp1.K();
	
	/*** Offset Line ***/
	this->_data[6] = tmp1.I();
	this->_data[7] = tmp1.J();
	this->_data[8] = tmp1.K();
	tmp1 = p1 + (this->_uUnit * MEASURERADIUS_LABELDISPLACEMENT);
	tmp1 = this->_fromPlane * tmp1;
	this->_data[9] = tmp1.I();				
	this->_data[10] = tmp1.J();
	this->_data[11] = tmp1.K();

	/*** Start-end Arrows ***/
	WPFloat arrow = 0.05;
	WCVector4 unit(tan.J(), -tan.I(), 0.0, 0.0);
	//Invert tan if inside arc
	if (this->_offset < 0.0) tan *= -1.0;
	tmp1 = p0 + (tan * arrow) - (unit * arrow);
	tmp1 = this->_fromPlane * tmp1;
	this->_data[12] = this->_data[0];
	this->_data[13] = this->_data[1];
	this->_data[14] = this->_data[2];
	this->_data[15] = tmp1.I();
	this->_data[16] = tmp1.J();
	this->_data[17] = tmp1.K();
	tmp1 = p0 + (tan * arrow) + (unit * arrow);
	tmp1 = this->_fromPlane * tmp1;
	this->_data[18] = this->_data[0];
	this->_data[19] = this->_data[1];
	this->_data[20] = this->_data[2];
	this->_data[21] = tmp1.I();
	this->_data[22] = tmp1.J();
	this->_data[23] = tmp1.K();
	
	/*** Determine position for label ***/
	this->_labelPos = p1 + (this->_uUnit * MEASURERADIUS_LABELDISPLACEMENT);
	//Project points back into 3D
	this->_labelPos = this->_fromPlane * this->_labelPos;
	this->_uUnit = this->_fromPlane * this->_uUnit;
	this->_vUnit = this->_fromPlane * this->_vUnit;
}


/***********************************************~***************************************************/


WCConstraintMeasureRadius::WCConstraintMeasureRadius(WCSketchFeature *parent, const std::string &label, 
	WCGeometricCurve *curve, const WCVector4 &center, const WCMatrix4 &toPlane, const WCMatrix4 &fromPlane, 
	const WPFloat offset, const WPFloat labelOffset) : 
	::WCConstraintMeasure(parent, label, toPlane, fromPlane, offset, labelOffset), _curve(curve), _center(center) {
	//Nothing else for now
}


void WCConstraintMeasureRadius::SlewOffsets(const WPFloat &xSlew, const WPFloat &ySlew) {
	//Setup point on curve
	WCVector4 p0 = this->_toPlane * this->_curve->Evaluate(this->_labelOffset);
	WCVector4 center = this->_toPlane * this->_center;
	WCVector4 tan = p0 - center;
	tan.Normalize(true);
	//Find new p1 and slew it
	WCVector4 p1 = p0 + (tan * this->_offset);
	p1.Set( p1.I() + xSlew, p1.J() + ySlew, 0.0, 1.0);
	//Project p1 from plane
	p1 = this->_fromPlane * p1;
	//Create line from center to far away (along position)
	WCVector4 direction = p1 - this->_center;
	direction.Normalize(true);
	WCVector4 position = p1 + (direction * 1000000.0);
	WCGeometricLine *line = new WCGeometricLine(this->_center, position);
	//Intersect line with curve
	std::list<WPIntersectRec> hitList = this->_curve->Intersect(line, 0.0001);
	//If there is no intersection
	if (hitList.empty()) {
		WCVector4 begin = this->_curve->Evaluate(0.0);
		WCVector4 end = this->_curve->Evaluate(1.0);
		WPFloat beginP1Dist = p1.Distance(begin);
		WPFloat endP1Dist = p1.Distance(end);
		//See if closer to beginning or end
		if (beginP1Dist <= endP1Dist) {
			//Set label offset and position
			this->_labelOffset = 0.0;
			position = begin;
		}
		//End is closer
		else {
			//Set label offset and position
			this->_labelOffset = 1.0;
			position = end;
		}
	}
	//Get the hit results
	else {
		this->_labelOffset = hitList.front().second.first;
		position = hitList.front().first;
	}
	//Distance from position to intersection with curve (sign on if within curve or not)
	this->_offset = p1.Distance(position);
	if (this->_center.Distance(p1) < this->_center.Distance(position))
		this->_offset *= -1.0;
	//Mark as dirty
	this->_isDirty = true;
}


void WCConstraintMeasureRadius::Render(const WCColor &color, const bool &selectMode) {
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
	WCVector4 labelAdj(0.0, MEASURERADIUS_LABELDISPLACEMENT / zoom, 0.0, 0.0);
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

