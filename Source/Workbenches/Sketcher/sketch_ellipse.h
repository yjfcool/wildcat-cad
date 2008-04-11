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


#ifndef __SKETCH_ELLIPSE_H__
#define __SKETCH_ELLIPSE_H__


/*** Included Header Files ***/
#include "Kernel/wftrl.h"
#include "Sketcher/sketch_feature.h"
#include "Sketcher/sketch_ellipse_actions.h"
#include "Sketcher/sketch_ellipse_controller.h"
#include "Sketcher/sketch_ellipse_modes.h"


/*** Locally Defined Values ***/
#define SKETCHELLIPSE_CLASSNAME					"Ellipse"


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCSketch;


/***********************************************~***************************************************/


class WCSketchEllipse : public WCSketchFeature {
protected:
	WCVector4									_center, _majorAxis;								//!< Center point coordinates, major axis
	WPFloat										_semiMinor, _semiMajor;								//!< Semi-major and minor radii
	WCNurbsCurve								*_curve;											//!< Cooresponding geometric curve
	WCGeometricPoint							*_centerPoint, *_majorPoint, *_minorPoint;			//!< Visual geometric points
private:
	void GenerateBase(void);																		//!< Generate base objects
	void Initialize(void);																			//!< Private intiailizer
	//Deny Access
	WCSketchEllipse();																				//!< Deny access to default constructor
	WCSketchEllipse(const WCSketchEllipse& ellipse);												//!< Deny access to copy constructor
	WCSketchEllipse& operator=(const WCSketchEllipse &ellipse);										//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCSketchEllipse(WCSketch *sketch, const std::string &name, const WCVector4 &center,				//!< Primary constructor
												const WCVector4 &majorAxis, const WPFloat &semiMajor, const WPFloat &semiMinor);
	WCSketchEllipse(xercesc::DOMElement *element, WCSerialDictionary *dictionary);					//!< Persistance constructor
	~WCSketchEllipse();																				//!< Default destructor
	
	//Member Access Methods
	inline WCGeometricCurve* Base(void)			{ return this->_curve; }							//!< Get the geometric curve
	inline WCVector4 Center(void) const			{ return this->_center; }							//!< Get the center coordinates
	void Center(const WCVector4 &center);															//!< Set center position
	inline WCVector4 MajorAxis(void) const		{ return this->_majorAxis; }						//!< Get the semimajor axis
	inline WPFloat MajorRadius(void) const		{ return this->_semiMajor; }						//!< Get the semiMajor radius
	void SemiMajor(const WCVector4 &axis, const WPFloat &radius);									//!< Set semiMajor axis and radius
	inline WPFloat SemiMinor(void) const		{ return this->_semiMinor; }						//!< Get the semiminor radius
	void SemiMinor(const WPFloat &minor);															//!< Set semiminor radius
	
	//Inherited Methods
	std::string RootName(void) const			{ return SKETCHELLIPSE_CLASSNAME; }					//!< Get the class name
	void ReceiveNotice(WCObjectMsg msg, WCObject *sender);											//!< Receive notice from point or curve
	bool Regenerate(void);																			//!< Validate and rebuild
	void InjectConstraints(WCConstraintPlanner *planner);											//!< Induce effect onto constraint graph
	std::list<WCObject*> DeletionDependencies(void);												//!< Downward dependent objects
	void IsConstruction(const bool state);															//!< Set the construction state
	void OnSelection(const bool fromManager, std::list<WCVisualObject*> objects);					//!< Called on selection
	void OnDeselection(const bool fromManager);														//!< Called on deselection	
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
	
	/*** Actions & Controllers ***/
	static WCDrawingMode* ModeCreate(WCSketchWorkbench *wb);										//!< Return ellipse drawing mode controller
	static WCDrawingMode* ModeEdit(WCSketchEllipse *line);											//!< Return ellipse editing mode controller
	static WCActionSketchEllipseCreate* ActionCreate(WCSketch *sketch, const std::string &ellipseName,	//!< Primary creation action
												const WCVector4 &center, const WCVector4 &MajorAxis, 
												const WPFloat &semiMajor, const WPFloat &semiMinor);
	static WCActionSketchEllipseMove* ActionMove(WCSketchEllipse *ellipse, const WCVector4 &centerPos);	//!< Primary move action
	static WCActionSketchEllipseMajor* ActionMajor(WCSketchEllipse *ellipse, const WCVector4 &axis,	//!< Reposition major point
												 const WPFloat &major);
	static WCActionSketchEllipseMinor* ActionMinor(WCSketchEllipse *ellipse, const WPFloat &minor);	//!< Reposition minor point
	
	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCSketchEllipse &ellipse);				//!< Overloaded output operator		
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__SKETCH_ELLIPSE_H__

