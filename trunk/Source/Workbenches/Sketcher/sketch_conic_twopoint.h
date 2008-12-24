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


#ifndef __SKETCH_CONIC_TWOPOINT_H__
#define __SKETCH_CONIC_TWOPOINT_H__


/*** Included Header Files ***/
#include <Kernel/wftrl.h>
#include <Sketcher/sketch_feature.h>
#include <Sketcher/sketch_conic_actions.h>
#include <Sketcher/sketch_conic_controller.h>
#include <Sketcher/sketch_conic_modes.h>


/*** Locally Defined Values ***/
#define SKETCHCONIC_TWOPOINT_CLASSNAME			"Conic"


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCSketch;


/***********************************************~***************************************************/


class WCSketchConicTwoPoint : public WCSketchFeature {
protected:
	WCVector4									_begin, _beginTangent;								//!< Beginning point and tangent
	WCVector4									_end, _endTangent;									//!< Ending point and tangent
	WCVector4									_fifthPoint;										//!< Fifth point of the conic
	WCNurbsCurve								*_curve;											//!< Cooresponding geometric curve
	WCGeometricPoint							*_beginPoint, *_endPoint;							//!< Geometric center point
private:
	void Initialize(void);																			//!< Private initializer
	//Deny Access
	WCSketchConicTwoPoint();																		//!< Deny access to default constructor
	WCSketchConicTwoPoint(const WCSketchConicTwoPoint& conic);										//!< Deny access to copy constructor
	WCSketchConicTwoPoint& operator=(const WCSketchConicTwoPoint &conic);							//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCSketchConicTwoPoint(WCSketch *sketch, const std::string &name,								//!< Primary constructor
												const WCVector4 &begin, const WCVector4 &beginTangent,
												const WCVector4 &end, const WCVector4 &endTangent,
												const WCVector4 &fifthPoint);
	WCSketchConicTwoPoint(xercesc::DOMElement *element, WCSerialDictionary *dictionary);			//!< Persistance constructor
	~WCSketchConicTwoPoint();																		//!< Default destructor
	
	//Member Access Methods
	inline WCNurbsCurve* Base(void)				{ return this->_curve; }							//!< Get base feature curve
	inline WCVector4 Begin(void) const			{ return this->_begin; }							//!< Get the beginning point coordinates
	inline WCVector4 BeginTangent(void) const	{ return this->_beginTangent; }						//!< Get the beginning tangent coordinates
	inline WCVector4 End(void) const			{ return this->_end; }								//!< Get the ending point coordinates
	inline WCVector4 EndTangent(void) const		{ return this->_endTangent; }						//!< Get the ending tangent coordinates

	//Inherited Methods
	inline std::string RootName(void) const		{ return SKETCHCONIC_TWOPOINT_CLASSNAME; }			//!< Get the class name
	void ReceiveNotice(WCObjectMsg msg, WCObject *sender);											//!< Receive notice from point or curve
	bool Regenerate(void);																			//!< Validate and rebuild
	void InjectConstraints(WCConstraintPlanner *planner);											//!< Induce effect onto constraint graph
	std::list<WCObject*> DeletionDependencies(void);												//!< Downward dependent objects
	void IsConstruction(const bool state);															//!< Set the construction state
	void OnSelection(const bool fromManager, std::list<WCVisualObject*> objects);					//!< Called on selection
	void OnDeselection(const bool fromManager);														//!< Called on deselection	
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
	
	/*** Actions & Controllers ***/
	static WCDrawingMode* ModeCreate(WCSketchWorkbench *wb);										//!< Return a two-point conic drawing mode
	static WCDrawingMode* ModeEdit(WCSketchConicTwoPoint *conic);									//!< Return a two-point conic editing mode
	
	static WCAction* ActionCreate(WCSketch *sketch, const std::string &conicName,					//!< Primary creation action
												const WCVector4 &start, const WCVector4 &startTangent,
												const WCVector4 &end, const WCVector4 &endTangent,
												const WCVector4 &fifthPoint);
	
	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCSketchConicTwoPoint &conic);			//!< Overloaded output operator		
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__SKETCH_CONIC_TWOPOINT_H__

