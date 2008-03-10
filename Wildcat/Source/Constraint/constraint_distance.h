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


#ifndef __CONSTRAINT_DISTANCE_H__
#define __CONSTRAINT_DISTANCE_H__


/*** Included Header Files ***/
#include "Constraint/wgcsl.h"
#include "Constraint/sketch_constraint.h"
#include "Constraint/constraint_measure_twopoint.h"
#include "Constraint/constraint_distance_actions.h"
#include "Constraint/constraint_distance_controller.h"
#include "Constraint/constraint_distance_modes.h"


/*** Local Defines ***/
#define CONSTRAINTDISTANCE_CLASSNAME			"Distance"
#define CONSTRAINTDISTANCE_POINTPOINT			0
#define CONSTRAINTDISTANCE_POINTCURVE			1
#define CONSTRAINTDISTANCE_CURVECURVE			2


/*** Class Predefines ***/
class WCSketch;
class WCSketchPoint;
class WCSketchFeature;


/***********************************************~***************************************************/


class WCConstraintDistance : public WCSketchConstraint {
protected:
	WPUInt										_type;												//!< Type of distance constraint
	WCMeasureType								_measureType;										//!< Measure type
	WCSketchFeature								*_featureA, *_featureB;								//!< Reference features
	WCMeasurePosition							_posA, _posB;										//!< Position markers
	WPFloat										_distance;											//!< Distance value
	WCConstraintMeasureTwoPoint					*_measure;											//!< Render measure
private:
	void PointPoint(void);																			//!< Init point-point type
	void PointCurve(void);																			//!< Init point-curve type
	void CurveCurve(void);																			//!< Init curve-curve type
	void Initialize(void);																			//!< Initilization method
	//Deny Access
	WCConstraintDistance();																			//!< Deny access to default constructor
	WCConstraintDistance(const WCConstraintDistance& constraint);									//!< Deny access to copy constructor
	WCConstraintDistance& operator=(const WCConstraintDistance &constraint);						//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCConstraintDistance(WCSketch *sketch, const std::string &name, const WCMeasureType &type,		//!< Primary constructor (point point)
												WCSketchPoint *ptA, WCSketchPoint *ptB);
	WCConstraintDistance(WCSketch *sketch, const std::string &name, const WCMeasureType &type,		//!< Primary constructor (point curve)
												WCSketchPoint *pt,
												WCSketchFeature *curve, const WCMeasurePosition &pos);
	WCConstraintDistance(WCSketch *sketch, const std::string &name, const WCMeasureType &type,		//!< Primary constructor (curve curve)
												WCSketchFeature *curveA, const WCMeasurePosition &posA,
												WCSketchFeature *curveB, const WCMeasurePosition &posB);
	WCConstraintDistance(xercesc::DOMElement *element, WCSerialDictionary *dictionary);				//!< Persistance constructor
	~WCConstraintDistance();																		//!< Default destructor
	
	//Member Access Methods
	inline WPFloat Distance(void) const			{ return this->_distance; }							//!< Get the distance value
	void Distance(const WPFloat value);																//!< Set the distance value
	inline WCConstraintMeasureTwoPoint* Measure(void) { return this->_measure; }					//!< Get the measure
	
	//Inherited Methods
	inline std::string RootName(void) const		{ return CONSTRAINTDISTANCE_CLASSNAME; }			//!< Get the class name
	void InjectConstraints(WCConstraintPlanner *planner);											//!< Inject constraint into system
	bool DependencyCheck(WCSketchFeature *feature);													//!< Perform dependency check
	void ReceiveNotice(WCObjectMsg msg, WCObject *sender);											//!< Receive notice from curve
	bool Regenerate(void);																			//!< Validate and rebuild
	std::list<WCObject*> DeletionDependencies(void);												//!< Downward dependent objects
	void OnSelection(const bool fromManager, std::list<WCVisualObject*> objects);					//!< Called on selection
	void OnDeselection(const bool fromManager);														//!< Called on deselection
	void Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom);				//!< Render the object
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object

	/*** Actions ***/
	static WCDrawingMode* ModeEdit(WCConstraintDistance *constraint);								//!< Return an editing mode controller
	static WCActionConstraintDistanceCreate* ActionCreate(WCSketch *sketch,							//!< Primary creation action
												const std::string &constraintName, const WCMeasureType &type,
												WCSketchFeature *first, WCSketchFeature *second,
												const WCMeasurePosition &posA=WCMeasurePosition::Closest(),
												const WCMeasurePosition &posB=WCMeasurePosition::Closest());
	static WCActionConstraintDistanceMove* ActionMove(WCConstraintDistance *constraint,				//!< Primary modify action
												const WPFloat &offset, const WPFloat &labelOffset);

	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCConstraintDistance &constraint);		//!< Overloaded output operator		
};


/***********************************************~***************************************************/


#endif //__CONSTRAINT_DISTANCE_H__

