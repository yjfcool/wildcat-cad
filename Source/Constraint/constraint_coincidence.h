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


#ifndef __CONSTRAINT_COINCIDENCE_H__
#define __CONSTRAINT_COINCIDENCE_H__


/*** Included Header Files ***/
#include <Constraint/wgcsl.h>
#include <Constraint/sketch_constraint.h>
#include <Constraint/constraint_coincidence_actions.h>
#include <Constraint/constraint_coincidence_controller.h>


/*** Locally Defined Values ***/
#define CONSTRAINTCOINCIDENCE_CLASSNAME			"Coincidence"
#define CONSTRAINTCOINCIDENCE_POINTPOINT		0
#define CONSTRAINTCOINCIDENCE_POINTCURVE		1
#define CONSTRAINTCOINCIDENCE_CURVECURVE		2


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCSketch;


/***********************************************~***************************************************/


class WCConstraintCoincidence : public WCSketchConstraint {
protected:
	WPUInt										_type;												//!< Type of coincidence constraint
	WCSketchFeature								*_featureA, *_featureB;								//!< Reference features
	WCMeasurePosition							_posA, _posB;										//!< Position markers
	GLfloat										_data[3];											//!< Render data
private:
	void PointPoint(void);																			//!< Init point-point type
	void PointCurve(void);																			//!< Init point-curve type
	void CurveCurve(void);																			//!< Init curve-curve type
	void Initialize(void);																			//!< Private initilizer
	//Deny Access
	WCConstraintCoincidence();																		//!< Deny access to default constructor
	WCConstraintCoincidence(const WCConstraintCoincidence& constraint);								//!< Deny access to copy constructor
	WCConstraintCoincidence& operator=(const WCConstraintCoincidence &constraint);					//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCConstraintCoincidence(WCSketch *sketch, const std::string &name,								//!< Primary constructor
												WCSketchFeature *featureA, const WCMeasurePosition &posA,
												WCSketchFeature *featureB, const WCMeasurePosition &posB);
	WCConstraintCoincidence(xercesc::DOMElement *element, WCSerialDictionary *dictionary);			//!< Persistance constructor
	~WCConstraintCoincidence();																		//!< Default destructor
	
	//Member Access Methods
	//...
	
	//Inherited Methods
	inline std::string RootName(void) const		{ return CONSTRAINTCOINCIDENCE_CLASSNAME; }			//!< Get the class name
	inline bool DependencyCheck(WCSketchFeature *feature){ return (this->_featureA == feature) ||	//!< Perform dependency check
												(this->_featureB == feature); }
	void InjectConstraints(WCConstraintPlanner *planner);											//!< Inject constraint into system
	void ReceiveNotice(WCObjectMsg msg, WCObject *sender);											//!< Receive notice from curve
	bool Regenerate(void);																			//!< Validate and rebuild
	std::list<WCObject*> DeletionDependencies(void);												//!< Downward dependent objects
	void OnSelection(const bool fromManager, std::list<WCVisualObject*> objects);					//!< Called on selection
	void OnDeselection(const bool fromManager);														//!< Called on deselection
	void Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom);				//!< Render the object
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object

	/*** Actions ***/
	static WCActionConstraintCoincidenceCreate* ActionCreate(WCSketch *sketch, const std::string &constraintName, //!< Primary creation action
												WCSketchFeature *featureA, const WCMeasurePosition &posA,
												WCSketchFeature *featureB, const WCMeasurePosition &posB);

	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCConstraintCoincidence &constraint);	//!< Overloaded output operator		
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__CONSTRAINT_COINCIDENCE_H__

