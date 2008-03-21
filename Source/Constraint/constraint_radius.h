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


#ifndef __CONSTRAINT_RADIUS_H__
#define __CONSTRAINT_RADIUS_H__


/*** Included Header Files ***/
#include "Constraint/wgcsl.h"
#include "Constraint/sketch_constraint.h"
#include "Constraint/constraint_measure_radius.h"
#include "Constraint/constraint_radius_actions.h"
#include "Constraint/constraint_radius_controller.h"
#include "Constraint/constraint_radius_modes.h"


/*** Local Defines ***/
#define CONSTRAINTRADIUS_CLASSNAME				"Radius"


/*** Class Predefines ***/
class WCSketch;
class WCSketchArc;
class WCSketchCircle;


/***********************************************~***************************************************/


class WCConstraintRadius : public WCSketchConstraint {
protected:
	WCSketchFeature								*_feature;											//!< Feature reference
	bool										_isArc;												//!< Is the feature an arc
	WPFloat										_radius;											//!< Radius measure
	WCConstraintMeasureRadius					*_measure;											//!< Render measure
private:
	//Private Methods
	void Initialize(const WPFloat &offset, const WPFloat &labelOffset);								//!< Initialize the measure

	//Deny Access
	WCConstraintRadius();																			//!< Deny access to default constructor
	WCConstraintRadius(const WCConstraintRadius& constraint);										//!< Deny access to copy constructor
	WCConstraintRadius& operator=(const WCConstraintRadius &constraint);							//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCConstraintRadius(WCSketch *sketch, const std::string &name, WCSketchArc *arc);				//!< Primary constructor (arc)
	WCConstraintRadius(WCSketch *sketch, const std::string &name, WCSketchCircle *circle);			//!< Primary constructor (circle)
	WCConstraintRadius(xercesc::DOMElement *element, WCSerialDictionary *dictionary);				//!< Persistance constructor
	~WCConstraintRadius();																			//!< Default destructor
	
	//Member Access Methods
	inline WPFloat Radius(void) const			{ return this->_radius; }							//!< Get the radius value
	void Radius(const WPFloat &value);																//!< Set the radius value
	inline WCConstraintMeasureRadius* Measure(void) { return this->_measure; }						//!< Get the measure
	
	//Inherited Methods
	inline std::string RootName(void) const		{ return CONSTRAINTRADIUS_CLASSNAME; }				//!< Get the class name
	void InjectConstraints(WCConstraintPlanner *planner);											//!< Inject constraint into system
	bool DependencyCheck(WCSketchFeature *feature);													//!< Perform dependency check
	void ReceiveNotice(WCObjectMsg msg, WCObject *sender);											//!< Receive notice from curve
	bool Regenerate(void);																			//!< Validate and rebuild
	std::list<WCObject*> DeletionDependencies(void);												//!< Downward dependent objects
	void OnSelection(const bool fromManager, std::list<WCVisualObject*> objects);					//!< Called on selection
	void OnDeselection(const bool fromManager);														//!< Called on deselection
	void Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom);				//!< Render the object
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object

	/*** Modes and Actions ***/
	static WCDrawingMode* ModeEdit(WCConstraintRadius *constraint);									//!< Return an editing mode controller

	static WCActionConstraintRadiusCreate* ActionCreate(WCSketch *sketch, const						//!< Primary creation action (arc)
												std::string &constraintName, WCSketchArc *arc);
	static WCActionConstraintRadiusCreate* ActionCreate(WCSketch *sketch, const						//!< Primary creation action (circle)
												std::string &constraintName, WCSketchCircle *circle);
	static WCActionConstraintRadiusMove* ActionMove(WCConstraintRadius *constraint,					//!< Primary modify action
												const WPFloat &offset, const WPFloat &labelOffset);

	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCConstraintRadius &constraint);		//!< Overloaded output operator		
};


/***********************************************~***************************************************/


#endif //__CONSTRAINT_RADIUS_H__

