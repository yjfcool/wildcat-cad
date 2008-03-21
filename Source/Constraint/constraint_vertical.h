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


#ifndef __CONSTRAINT_VERTICAL_H__
#define __CONSTRAINT_VERTICAL_H__


/*** Included Header Files ***/
#include "Constraint/wgcsl.h"
#include "Constraint/sketch_constraint.h"
#include "Constraint/constraint_vertical_actions.h"
#include "Constraint/constraint_vertical_controller.h"


/*** Local Defines ***/
#define CONSTRAINTVERTICAL_CLASSNAME			"Vertical"


/*** Class Predefines ***/
class WCSketch;
class WCSketchLine;


/***********************************************~***************************************************/


class WCConstraintVertical : public WCSketchConstraint {
protected:
	WCSketchLine								*_line;												//!< Line reference
	GLfloat										_vertData[12];										//!< Vertex data
	GLfloat										_texData[8];										//!< TexCoord data
	WSTexture									*_texture;											//!< Icon texture
private:
	void GenerateBuffer(void);																		//!< Generate the render data
	void Initialize(void);																			//!< Initialization method
	//Deny Access
	WCConstraintVertical();																			//!< Deny access to default constructor
	WCConstraintVertical(const WCConstraintVertical& constraint);									//!< Deny access to copy constructor
	WCConstraintVertical& operator=(const WCConstraintVertical &constraint);						//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCConstraintVertical(WCSketch *sketch, const std::string &name, WCSketchLine *line);			//!< Primary constructor
	WCConstraintVertical(xercesc::DOMElement *element, WCSerialDictionary *dictionary);				//!< Persistance constructor
	~WCConstraintVertical();																		//!< Default destructor
	
	//Member Access Methods
	inline WCSketchLine* Line(void)				{ return this->_line; }								//!< Get the associated object
	
	//Inherited Methods
	inline std::string RootName(void) const		{ return CONSTRAINTVERTICAL_CLASSNAME; }			//!< Get the class name
	inline bool DependencyCheck(WCSketchFeature *feature) {											//!< Perform dependency check
												return ((WCSketchFeature*)this->_line == feature); }
	inline std::list<WCObject*> DeletionDependencies(void) { return std::list<WCObject*>(); }		//!< Downward dependent objects
	void InjectConstraints(WCConstraintPlanner *planner);											//!< Inject constraint into system
	void ReceiveNotice(WCObjectMsg msg, WCObject *sender);											//!< Receive notice from curve
	bool Regenerate(void);																			//!< Validate and rebuild
	void OnSelection(const bool fromManager, std::list<WCVisualObject*> objects);					//!< Called on selection
	void OnDeselection(const bool fromManager);														//!< Called on deselection
	void Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom);				//!< Render the object
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object

	/*** Actions ***/
	static WCActionConstraintVerticalCreate* ActionCreate(WCSketch *sketch,							//!< Primary creation action
												const std::string &constraintName, WCSketchLine *line);

	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCConstraintVertical &constraint);		//!< Overloaded output operator		
};


/***********************************************~***************************************************/


#endif //__CONSTRAINT_VERTICAL_H__

