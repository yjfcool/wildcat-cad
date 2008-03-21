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


#ifndef __SKETCH_AXIS_H__
#define __SKETCH_AXIS_H__


/*** Included Header Files ***/
#include "Kernel/wftrl.h"
#include "Sketcher/sketch_feature.h"
#include "Sketcher/sketch_axis_actions.h"
#include "Sketcher/sketch_axis_controller.h"
#include "Sketcher/sketch_axis_modes.h"


/*** Local Defines ***/
#define SKETCHAXIS_CLASSNAME					"Axis"
#define SKETCHAXIS_LINE_THICKNESS				3.0
#define SKETCHAXIS_LINESTIPPLE_FACTOR			1
#define SKETCHAXIS_LINESTIPPLE_PATTERN			0x0F0F


/*** Class Predefines ***/
class WCSketch;


/***********************************************~***************************************************/


class WCSketchAxis : virtual public WCSketchFeature, virtual public WCVisualObject {
protected:
	WCVector4									_begin, _end;										//!< Start and end coordinates
	WCGeometricLine								*_line;												//!< Cooresponding geometric line
private:
	void Initialize(void);																			//!< Initialization routine
	//Deny Access
	WCSketchAxis();																					//!< Deny access to default constructor
	WCSketchAxis(const WCSketchAxis& pt);															//!< Deny access to copy constructor
	WCSketchAxis& operator=(const WCSketchAxis &pt);												//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCSketchAxis(WCSketch *sketch, const std::string &name, const WCVector4 &begin, const WCVector4 &end);//!< Primary constructor
	WCSketchAxis(xercesc::DOMElement *element, WCSerialDictionary *dictionary);						//!< Persistance constructor
	~WCSketchAxis();																				//!< Default destructor
	
	//Member Access Methods
	inline WCGeometricLine* Base(void)			{ return this->_line; }								//!< Get the feature line
	inline WCVector4 Begin(void) const			{ return this->_begin; }							//!< Get the begin value
	inline WCVector4 End(void) const			{ return this->_end; }								//!< Get the end value
	void Set(const WCVector4 &begin, const WCVector4 &end);											//!< Set the line position
	
	//Inherited Methods
	inline std::string RootName(void) const		{ return SKETCHAXIS_CLASSNAME; }					//!< Get the class name
	void ReceiveNotice(WCObjectMsg msg, WCObject *sender);											//!< Receive notice from point or curve
	bool Regenerate(void);																			//!< Validate and rebuild
	void InjectConstraints(WCConstraintPlanner *planner);											//!< Induce effect onto constraint graph
	std::list<WCObject*> DeletionDependencies(void);												//!< Downward dependent objects
	void IsConstruction(const bool state)		{ }													//!< Set the construction state
	void OnSelection(const bool fromManager, std::list<WCVisualObject*> objects);					//!< Called on selection
	void OnDeselection(const bool fromManager);														//!< Called on deselection	
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
	void Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom);				//!< Render the object
	
	/*** Actions & Controllers ***/
	static WCDrawingMode* ModeCreate(WCSketchWorkbench *wb);										//!< Return a line drawing mode controller
	static WCDrawingMode* ModeEdit(WCSketchAxis *axis);												//!< Return a line editing mode controller
	static WCActionSketchAxisCreate* ActionCreate(WCSketch *sketch, const std::string &axisName,	//!< Primary creation action
												const WCVector4 &begin, const WCVector4 &end);
	static WCActionSketchAxisMove* ActionMove(WCSketchAxis *axis, const WCVector4 &begin,			//!< Primary modify action
												const WCVector4 &end);

	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCSketchAxis &line);					//!< Overloaded output operator		
};


/***********************************************~***************************************************/


#endif //__SKETCH_AXIS_H__

