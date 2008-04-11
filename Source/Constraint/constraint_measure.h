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


#ifndef __CONSTRAINT_MEASURE_H__
#define __CONSTRAINT_MEASURE_H__


/*** Included Header Files ***/
#include "Constraint/wgcsl.h"
#include "Constraint/sketch_constraint.h"


/*** Locally Defined Values ***/
#define CONSTRAINTMEASURE_ARROW_LENGTH			0.05
#define CONSTRAINTMEASURE_CROSSLINE_OFFSET		0.8
#define CMTMP									0.05


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


class WCConstraintMeasure : virtual public WCSerializeableObject {
protected:
	WCSketchFeature								*_parent;											//!< Parent feature
	WCMatrix4									_toPlane, _fromPlane;								//!< Planar drawing units
	WPFloat										_offset, _labelOffset;								//!< Offsets from the points
	std::string									_label;												//!< Value of the measure
	WCText										*_text;												//!< Text for the label
	WCVector4									_labelPos, _uUnit, _vUnit;							//!< Position of the label
	bool										_isDirty;											//!< Dirty flag
private:
	//Hidden Constructors
	WCConstraintMeasure();																			//!< Deny access to default constructor
	WCConstraintMeasure(const WCConstraintMeasure&);												//!< Deny access to copy constructor
public:
	//Constructors and Destructors
	WCConstraintMeasure(WCSketchFeature *parent, const std::string &label, const WCMatrix4 &toPlane,//!< Primary constructor
												const WCMatrix4 &fromPlane,	const WPFloat offset,	const WPFloat labelOffset);
	WCConstraintMeasure(xercesc::DOMElement *element, WCSerialDictionary *dictionary);				//!< Persistance constructor
	virtual ~WCConstraintMeasure();																	//!< Default destructor

	//Member Access Methods
	virtual inline void MarkDirty(void)			{ this->_isDirty = true; }							//!< Mark the measure as dirty
	virtual inline WPFloat Offset(void) const	{ return this->_offset; }							//!< Get uOffset value
	virtual inline WPFloat LabelOffset(void) const	{ return this->_labelOffset; }					//!< Get vOffset value
	virtual inline WCText* Text(void)			{ return this->_text; }								//!< Get the text pointer
	virtual void Offsets(const WPFloat &offset,	const WPFloat &labelOffset);						//!< Set the offsets of the measure
	virtual void Matrices(const WCMatrix4 &toPlane, const WCMatrix4 &fromPlane);					//!< Set the units of the measure

	//Inherited Methods
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object

	//Required Methods
	virtual void SlewOffsets(const WPFloat &xSlew, const WPFloat &ySlew)=0;							//!< Required offsets slew method
	virtual void Render(const WCColor &color, const bool &selectMode=false)=0;						//!< Required render method
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__CONSTRAINT_MEASURE_H__

