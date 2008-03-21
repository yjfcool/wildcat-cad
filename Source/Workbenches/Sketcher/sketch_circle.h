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


#ifndef __SKETCH_CIRCLE_H__
#define __SKETCH_CIRCLE_H__


/*** Included Header Files ***/
#include "Kernel/wftrl.h"
#include "Sketcher/sketch_feature.h"
#include "Sketcher/sketch_circle_actions.h"
#include "Sketcher/sketch_circle_controller.h"
#include "Sketcher/sketch_circle_modes.h"


/*** Local Defines ***/
#define SKETCHCIRCLE_CLASSNAME					"Circle"


/*** Class Predefines ***/
class WCSketch;


/***********************************************~***************************************************/


class WCSketchCircle : public WCSketchFeature {
protected:
	WCVector4									_center;											//!< Center point coordinates
	WPFloat										_radius;											//!< Circle radius
	WCNurbsCurve								*_curve;											//!< Cooresponding geometric curve
	WCGeometricPoint							*_centerPoint;										//!< Geometric center point
private:
	void GenerateBase(void);																		//!< Generate base objects
	void Initialize(void);																			//!< Initialization method
	//Deny Access
	WCSketchCircle();																				//!< Deny access to default constructor
	WCSketchCircle(const WCSketchCircle &circle);													//!< Deny access to copy constructor
	WCSketchCircle& operator=(const WCSketchCircle &circle);										//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCSketchCircle(WCSketch *sketch, const std::string &name, const WCVector4 &center,				//!< Primary constructor
												const WPFloat &radius);
	WCSketchCircle(xercesc::DOMElement *element, WCSerialDictionary *dictionary);					//!< Persistance constructor
	~WCSketchCircle();																				//!< Default destructor
	
	//Member Access Methods
	inline WCNurbsCurve* Base(void)				{ return this->_curve; }							//!< Get base feature curve
	inline WCGeometricPoint* CenterPoint(void)	{ return this->_centerPoint; }						//!< Get the geometric center
	inline WCVector4 Center(void) const			{ return this->_center; }							//!< Get the center's position
	void Center(const WCVector4 &center);															//!< Set the center's position
	inline WPFloat Radius(void)	const			{ return this->_radius; }							//!< Get the circle radius
	void Radius(const WPFloat &radius);																//!< Set the circle radius

	//Inherited Methods
	inline std::string RootName(void) const		{ return SKETCHCIRCLE_CLASSNAME; }					//!< Get the class name
	void ReceiveNotice(WCObjectMsg msg, WCObject *sender);											//!< Receive notice from point or curve
	bool Regenerate(void);																			//!< Validate and rebuild
	void InjectConstraints(WCConstraintPlanner *planner);											//!< Induce effect onto constraint graph
	std::list<WCObject*> DeletionDependencies(void);												//!< Downward dependent objects
	void IsConstruction(const bool state);															//!< Set the construction state
	void OnSelection(const bool fromManager, std::list<WCVisualObject*> objects);					//!< Called on selection
	void OnDeselection(const bool fromManager);														//!< Called on deselection	
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
	
	/*** Actions & Controllers ***/
	static WCDrawingMode* DrawingMode(WCSketchWorkbench *wb);										//!< Return a line drawing mode controller
	static WCDrawingMode* ModeEdit(WCSketchCircle *circle);											//!< Return a line editing mode controller
	static WCActionSketchCircleCreate* ActionCreate(WCSketch *sketch, const std::string &circleName,//!< Primary creation action
												const WCVector4 &center, const WPFloat &radius);
	static WCActionSketchCircleMove* ActionMove(WCSketchCircle *circle, const WCVector4 &centerPos);//!< Primary move action
	static WCActionSketchCircleRadius* ActionRadius(WCSketchCircle *circle, const WPFloat &radius);	//!< Primary action to set radius
	
	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCSketchCircle &circle);				//!< Overloaded output operator		
};


/***********************************************~***************************************************/


#endif //__SKETCH_CIRCLE_H__

