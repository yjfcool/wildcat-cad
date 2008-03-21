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


#ifndef __CONSTRAINT_FIX_H__
#define __CONSTRAINT_FIX_H__


/*** Included Header Files ***/
#include "Constraint/wgcsl.h"
#include "Constraint/sketch_constraint.h"
#include "Constraint/constraint_fix_actions.h"
#include "Constraint/constraint_fix_controller.h"


/*** Local Defines ***/
#define CONSTRAINTFIX_CLASSNAME					"Fix"


/*** Class Predefines ***/
class WCSketch;


/***********************************************~***************************************************/


class WCConstraintFix : public WCSketchConstraint {
protected:
	WCSketchFeature								*_feature;											//!< Object reference
	GLfloat										_vertData[12];										//!< Vertex data
	GLfloat										_texData[8];										//!< TexCoord data
	WSTexture									*_texture;											//!< Icon texture
private:
	void GenerateBuffer(void);																		//!< Generate the render data
	//Deny Access
	WCConstraintFix();																				//!< Deny access to default constructor
	WCConstraintFix(const WCConstraintFix& constraint);												//!< Deny access to copy constructor
	WCConstraintFix& operator=(const WCConstraintFix &constraint);									//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCConstraintFix(WCSketch *sketch, const std::string &name, WCSketchFeature *feature);			//!< Primary constructor
	WCConstraintFix(xercesc::DOMElement *element, WCSerialDictionary *dictionary);					//!< Persistance constructor
	~WCConstraintFix();																				//!< Default destructor

	//Member Access Methods
	inline WCSketchFeature* Reference(void)		{ return this->_feature; }							//!< Get the reference object

	//Inherited Methods
	std::string RootName(void) const			{ return CONSTRAINTFIX_CLASSNAME; }					//!< Get the class name
	void InjectConstraints(WCConstraintPlanner *planner);											//!< Inject constraint into system
	inline bool DependencyCheck(WCSketchFeature *feature) { return (this->_feature == feature); }	//!< Perform dependency check
	void ReceiveNotice(WCObjectMsg msg, WCObject *sender);											//!< Receive notice from curve
	bool Regenerate(void);																			//!< Validate and rebuild
	std::list<WCObject*> DeletionDependencies(void);												//!< Downward dependent objects
	void OnSelection(const bool fromManager, std::list<WCVisualObject*> objects);					//!< Called on selection
	void OnDeselection(const bool fromManager);														//!< Called on deselection
	void Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom);				//!< Render the object
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object

	/*** Actions ***/
	static WCAction* ActionCreate(WCSketch *sketch, const std::string &constraintName,				//!< Primary creation action
												WCSketchFeature *feature);

	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCConstraintFix &constraint);			//!< Overloaded output operator		
};


/***********************************************~***************************************************/


#endif //__CONSTRAINT_FIX_H__

