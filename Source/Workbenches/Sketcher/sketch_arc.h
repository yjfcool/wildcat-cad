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


#ifndef __SKETCH_ARC_H__
#define __SKETCH_ARC_H__


/*** Included Header Files ***/
#include "Kernel/wftrl.h"
#include "Sketcher/sketch_feature.h"
#include "Sketcher/sketch_arc_type.h"
#include "Sketcher/sketch_arc_actions.h"
#include "Sketcher/sketch_arc_controller.h"
#include "Sketcher/sketch_arc_modes.h"


/*** Locally Defined Values ***/
#define SKETCHARC_CLASSNAME						"Arc"


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCSketch;
class WCControlPoint;
class WCNurbsCurve;
class WCGeometricPoint;


/***********************************************~***************************************************/


class WCSketchArc : public WCSketchFeature {
protected:
	WCSketchArcType								_type;												//!< Type of arc
	WCVector4									_center;											//!< Center position
	WPFloat										_radius;											//!< Arc radius
	WPFloat										_startAngle, _endAngle;								//!< Initial and final angles
	WCNurbsCurve								*_curve;											//!< Cooresponding geometric curve
	WCGeometricPoint							*_centerPoint;										//!< Three-point arc point
private:
	void GenerateBase(void);																		//!< Generate base objects
	void Initialize(void);																			//!< Initialization routine
	//Deny Access
	WCSketchArc();																					//!< Deny access to default constructor
	WCSketchArc(const WCSketchArc& pt);																//!< Deny access to copy constructor
	WCSketchArc& operator=(const WCSketchArc &pt);													//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCSketchArc(WCSketch *sketch, const std::string &name, const WCVector4 &center,					//!< Standard arc constructor
												const WPFloat &radius, const WPFloat &startAngle, const WPFloat &endAngle);
	WCSketchArc(WCSketch *sketch, const std::string &name, const WCVector4 &center,					//!< Three-point arc constructor
												const WPFloat &radius, const WPFloat &startAngle, const WPFloat &endAngle,
												const WCVector4 &threePoint);
	WCSketchArc(xercesc::DOMElement *element, WCSerialDictionary *dictionary);						//!< Persistance constructor
	~WCSketchArc();																					//!< Default destructor
	
	//Member Access Methods
	inline WCSketchArcType Type(void) const		{ return this->_type; }								//!< Get the arc type
	inline WCNurbsCurve* Base(void)				{ return this->_curve; }							//!< Get base feature curve
	inline WCVector4 Center(void) const			{ return this->_center; }							//!< Get the center's position
	void Center(const WCVector4 &center);															//!< Set the center's position
	inline WPFloat Radius(void)	const			{ return this->_radius; }							//!< Get the circle radius
	void Radius(const WPFloat &radius);																//!< Set the circle radius

	//Inherited Methods
	inline std::string RootName(void) const		{ return SKETCHARC_CLASSNAME; }						//!< Get the class name
	void ReceiveNotice(WCObjectMsg msg, WCObject *sender);											//!< Receive notice from point or curve
	bool Regenerate(void);																			//!< Validate and rebuild
	void InjectConstraints(WCConstraintPlanner *planner);											//!< Induce effect onto constraint graph
	std::list<WCObject*> DeletionDependencies(void);												//!< Downward dependent objects
	void IsConstruction(const bool state);															//!< Set the construction state
	void OnSelection(const bool fromManager, std::list<WCVisualObject*> objects);					//!< Called on selection
	void OnDeselection(const bool fromManager);														//!< Called on deselection	
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
	
	/*** Actions & Controllers ***/
	static WCDrawingMode* ModeCreate(WCSketchWorkbench *wb, const WCSketchArcType &type);			//!< Return drawing mode controller
	static WCDrawingMode* ModeEdit(WCSketchArc* arc);												//!< Return editing mode controller
	static WCActionSketchArcCreate* ActionCreate(WCSketch *sketch, const std::string &arcName,		//!< Standard arc creation action
												const WCVector4 &center, const WPFloat &radius,
												const WPFloat &startAngle, const WPFloat &endAngle);
	static WCActionSketchArcCreate* ActionCreate(WCSketch *sketch, const std::string &arcName,		//!< Three-point arc creation action
												const WCVector4 &center, const WPFloat &radius,
												const WPFloat &startAngle, const WPFloat &endAngle, const WCVector4 &threePoint);
	static WCActionSketchArcMove* ActionMove(WCSketchArc *arc, const WCVector4 &centerPos);			//!< Move the arc
	static WCActionSketchArcRadius* ActionRadius(WCSketchArc *arc, const WPFloat &radius);			//!< Set the arc radius
//	static WCAction* ActionAngles(void)			{ return NULL; }									//!< Adjust the arc start/stop angles
	
	/*** Static Methods ***/
	static WPFloat PointsToDegrees(const WCVector4 rotAxis, const WCVector4 point);					//!< Convert two points into degrees
	static WPFloat CalculateCenterAndRadius(const WCVector4 &p1, const WCVector4 &p2,				//!< Calculate center and radius
												 const WCVector4 &p3, WCVector4 &center);
	
	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCSketchArc &circle);				//!< Overloaded output operator		
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__SKETCH_ARC_H__

