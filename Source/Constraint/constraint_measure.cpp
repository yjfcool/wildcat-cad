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
#include "Constraint/constraint_measure.h"


/***********************************************~***************************************************/


WCConstraintMeasure::WCConstraintMeasure(WCSketchFeature *parent, const std::string &label, const WCMatrix4 &toPlane,
	const WCMatrix4 &fromPlane,	const WPFloat offset, const WPFloat labelOffset) :
	_parent(parent), _toPlane(toPlane), _fromPlane(fromPlane), _offset(offset), _labelOffset(labelOffset),
	_label(label), _text(NULL), _labelPos(), _uUnit(), _vUnit(), _isDirty(true) {
	//Create text for the label
	this->_text = new WCText(this->_label, WCColor(WCSketchFeature::DefaultTextColor), WCTextFont::Times(), WCTextStyle::Roman(), 18.0);
}


WCConstraintMeasure::~WCConstraintMeasure() {
	//See if text needs to be deleted
	if (this->_text != NULL) delete this->_text;
}


void WCConstraintMeasure::Offsets(const WPFloat &offset, const WPFloat &labelOffset) {
	//Set the new values
	this->_offset = offset;
	this->_labelOffset = labelOffset;
	//Mark as dirty
	this->_isDirty = true;
}


void WCConstraintMeasure::Matrices(const WCMatrix4 &toPlane, const WCMatrix4 &fromPlane) {
	//Set the new values
	this->_toPlane = toPlane;
	this->_fromPlane = fromPlane;
	//Mark as dirty
	this->_isDirty = true;
}


/***********************************************~***************************************************/

