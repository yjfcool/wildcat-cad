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


#ifndef __CONSTRAINT_LENGTH_H__
#define __CONSTRAINT_LENGTH_H__


/*** Included Header Files ***/
#include <Constraint/wgcsl.h>
#include <Constraint/sketch_constraint.h>
#include <Constraint/constraint_measure_twopoint.h>
#include <Constraint/constraint_length_actions.h>
#include <Constraint/constraint_length_controller.h>
#include <Constraint/constraint_length_modes.h>


/*** Locally Defined Values ***/
#define CONSTRAINTLENGTH_CLASSNAME				"Length"


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCSketch;
class WCSketchLine;


/***********************************************~***************************************************/


class WCConstraintLength : public WCSketchConstraint {
protected:
	WCSketchLine								*_line;												//!< Line reference
	WPFloat										_length;											//!< Length measure
	WCConstraintMeasureTwoPoint					*_measure;											//!< Render measure
private:
	void GenerateMeasure(const WPFloat &offset, const WPFloat &labelOffset);						//!< (Re)Create the measure
	void Initialize(void);																			//!< Initialize the constraint
	//Hidden Constructors
	WCConstraintLength();																			//!< Deny access to default constructor
	WCConstraintLength(const WCConstraintLength& constraint);										//!< Deny access to copy constructor
	WCConstraintLength& operator=(const WCConstraintLength &constraint);							//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCConstraintLength(WCSketch *sketch, const std::string &name, WCSketchLine *line);				//!< Primary constructor
	WCConstraintLength(xercesc::DOMElement *element, WCSerialDictionary *dictionary);				//!< Persistance constructor
	~WCConstraintLength();																			//!< Default destructor
	
	//Member Access Methods
	inline WPFloat Length(void) const			{ return this->_length; }							//!< Get the length value
	void Length(const WPFloat &value);																//!< Set the length value
	inline WCConstraintMeasureTwoPoint* Measure(void) { return this->_measure; }					//!< Get the measure
	
	//Inherited Methods
	inline std::string RootName(void) const		{ return CONSTRAINTLENGTH_CLASSNAME; }				//!< Get the class name
	inline bool DependencyCheck(WCSketchFeature *feature){ return ((WCSketchFeature*)this->_line == feature); }		//!< Perform dependency check
	void InjectConstraints(WCConstraintPlanner *planner);											//!< Inject constraint into system
	void ReceiveNotice(WCObjectMsg msg, WCObject *sender);											//!< Receive notice from curve
	bool Regenerate(void);																			//!< Validate and rebuild
	std::list<WCObject*> DeletionDependencies(void);												//!< Downward dependent objects
	void OnSelection(const bool fromManager, std::list<WCVisualObject*> objects);					//!< Called on selection
	void OnDeselection(const bool fromManager);														//!< Called on deselection
	void Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom);				//!< Render the object
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object

	/*** Modes and Actions ***/
	static WCDrawingMode* ModeEdit(WCConstraintLength *constraint);									//!< Return an editing mode controller

	static WCActionConstraintLengthCreate* ActionCreate(WCSketch *sketch, const						//!< Primary creation action
												std::string &constraintName, WCSketchLine *line);
	static WCActionConstraintLengthMove* ActionMove(WCConstraintLength *constraint,					//!< Primary modify action
												const WPFloat &offset, const WPFloat &labelOffset);

	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCConstraintLength &constraint);		//!< Overloaded output operator		
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__CONSTRAINT_LENGTH_H__

