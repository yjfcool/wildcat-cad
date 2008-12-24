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


#ifndef __SKETCH_CONSTRAINT_H__
#define __SKETCH_CONSTRAINT_H__


/*** Included Header Files ***/
#include <Constraint/wgcsl.h>
#include <Kernel/wftrl.h>
#include <Sketcher/sketch_feature.h>


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCSketch;
class WCSketchWorkbench;
class WCDrawingMode;


/***********************************************~***************************************************/


class WCSketchConstraint : public WCSketchFeature, virtual public WCVisualObject {
protected:
	bool										_isEvaluated;										//!< Is evaluated or symbolic
	bool										_isExplicit;										//!< Did the user specify this constraint
private:
	//Deny Access
	WCSketchConstraint();																			//!< Deny access to default constructor
	WCSketchConstraint(const WCSketchConstraint& pt);												//!< Deny access to copy constructor
	WCSketchConstraint& operator=(const WCSketchConstraint &pt);									//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCSketchConstraint(WCSketch *sketch, const std::string &name);									//!< Primary constructor
	WCSketchConstraint(xercesc::DOMElement *element, WCSerialDictionary *dictionary);				//!< Persistance constructor
	virtual ~WCSketchConstraint()				{ }													//!< Default destructor
	
	//Member Access Methods
	inline WCGeometricObject* Base(void)		{ return NULL; }									//!< Get base geometric object
	virtual bool IsEvaluated(void) const		{ return this->_isEvaluated; }						//!< Get evaluation status
	virtual void IsConstruction(const bool state) { }												//!< Set construction flag
	virtual bool DependencyCheck(WCSketchFeature *feature)=0;										//!< Does this constraint depend on a feature
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object	
	
	/*** Actions & Controllers ***/
	static WCDrawingMode* ModeCreateGeometric(WCSketchWorkbench *wb);								//!< Return a geo-constraint drawing mode
	static WCDrawingMode* ModeCreateDimensional(WCSketchWorkbench *wb);								//!< Return a dim-constraint drawing mode
	
	/*** Static Rendering Methods ***/
	static void RenderTwoPointMeasure(const WCColor &color);										//!< Render a two-point measure
	static void RenderCalloutMeasure(const WCColor &color);											//!< Render a callout measure

	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCSketchConstraint &constraint);		//!< Overloaded output operator		
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__SKETCH_CONSTRAINT_H__

