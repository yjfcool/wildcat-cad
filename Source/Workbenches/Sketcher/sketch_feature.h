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


#ifndef __SKETCH_FEATURE_H__
#define __SKETCH_FEATURE_H__


/*** Included Header Files ***/
#include <Kernel/wftrl.h>
#include <Kernel/feature.h>
#include <Constraint/constraint_planner.h>


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCSketch;


/***********************************************~***************************************************/


class WCSketchFeature : public WCFeature {
public:
	//Static Members
	static WCColor								DefaultColor;										//!< Default feature color
	static WCColor								DefaultTextColor;									//!< Default feature text color
	static WCColor								ConstraintColor;									//!< Default constraint color
	static WCColor								SelectedColor;										//!< Selected feature color
	static WCColor								ConstructionColor;									//!< Construction feature color
	static WCColor								InprocessColor;										//!< Inprocess feature color
	static GLfloat								PointSize;											//!< Default point size
	static GLfloat								LineThickness;										//!< Default line thickness
	static GLint								LineStippleFactor;									//!< Default line stipple factor
	static GLushort								LineStipplePattern;									//!< Default line stipple patter
	
protected:
	WCSketch									*_sketch;											//!< Parent sketch
	std::list<WCConstraintNode*>				_constraintNodes;									//!< List of constraint nodes

private:
	//Deny Access
	WCSketchFeature();																				//!< Deny access to default constructor
	WCSketchFeature(const WCSketchFeature& feature);												//!< Deny access to copy constructor
	WCSketchFeature& operator=(const WCSketchFeature& feature);										//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCSketchFeature(WCSketch *sketch, const std::string &name="");									//!< Primary constructor
	WCSketchFeature(xercesc::DOMElement *element, WCSerialDictionary *dictionary);					//!< Persistance constructor
	virtual ~WCSketchFeature();																		//!< Default destructor
	
	//Member Access Methods
	inline WCSketch* Sketch(void)				{ return this->_sketch; }							//!< Get the parent sketch
	virtual bool Name(const std::string &name);														//!< Modify name
	virtual void MarkDirty(void);																	//!< Mark as dirty
	virtual std::list<WCConstraintNode*>& ConstraintNodes(void)	{ return this->_constraintNodes; }	//!< Return the list of constraint nodes

	//Required Methods
	virtual WCGeometricObject* Base(void)=0;														//!< Get underlying geometric object
	virtual void InjectConstraints(WCConstraintPlanner *planner)=0;									//!< Induce effect onto constraint graph
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict)=0;		//!< Serialize the object

	/*** Static Deserialization Method ***/
	static bool Deserialize(xercesc::DOMElement* featureElement, WCSerialDictionary *dictionary);	//!< Deserialization method

	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCSketchFeature &feature);				//!< Overloaded output operator	
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__SKETCH_FEATURE_H__

