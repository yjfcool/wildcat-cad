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


#ifndef __PART_PLANE_H__
#define __PART_PLANE_H__


/*** Included Header Files ***/
#include "Kernel/wftrl.h"
#include "PartDesign/part_feature.h"
#include "PartDesign/part_plane_actions.h"
#include "PartDesign/part_plane_controller.h"


/*** Local Defines ***/
#define PARTPLANE_CLASSNAME							"Plane"
#define PARTPLANE_DEFAULT_VISIBILITY				true
#define PARTPLANE_SCALE								0.25
#define PARTPLANE_LINE_THICKNESS					1.5


/*** Class Predefines ***/
class WCPart;


/***********************************************~***************************************************/


class WCPartPlane : public WCPartFeature, virtual public WCVisualObject {
protected:
	GLuint										_buffer;											//!< Corners buffer
	WCVector4									_base, _uAxis, _vAxis;								//!< Orienting vectors
	WCMatrix4									_matrix, _inverseMatrix;							//!< Plane matrices
private:
	void GenerateVBO(void);																			//!< Genreate the display VBO
	void GenerateMatrices(void);																	//!< Generate the matrices
	void Initialize(void);																			//!< Initialization method
	//Deny Access
	WCPartPlane();																					//!< Deny access to default constructor
	WCPartPlane(const WCPartPlane &plane);															//!< Deny access to copy constructor
	WCPartPlane& operator=(const WCPartPlane &plane);												//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCPartPlane(WCFeature *creator, const std::string &name, const WCVector4 &p0, const WCVector4 &p1,//!< Primary constructor
												const WCVector4 &p2);
	WCPartPlane(xercesc::DOMElement *element, WCSerialDictionary *dictionary);						//!< Persistance constructor
	~WCPartPlane();																					//!< Default destructor

	//Member Access Methods
	WCMatrix4 TransformMatrix(void) const		{ return this->_matrix; }							//!< Get transform matrix
	WCMatrix4 InverseTransformMatrix(void) const{ return this->_inverseMatrix; }					//!< Get inverse transform matrix
	inline WCVector4 Base(void) const			{ return this->_base; }								//!< Get base vector
	inline WCVector4 UAxis(void) const			{ return this->_uAxis; }							//!< Get u-axis vector
	inline WCVector4 VAxis(void) const			{ return this->_vAxis; }							//!< Get v-axis vector

	//Inherited Required Methods
	virtual inline std::string RootName(void) const	{ return PARTPLANE_CLASSNAME; }					//!< Get the class name
	virtual void ReceiveNotice(WCObjectMsg msg, WCObject *sender);									//!< Receive notice from point or curve
	virtual bool Regenerate(void);																	//!< Validate and rebuild
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
	virtual void Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom);		//!< Render the object
	virtual void OnSelection(const bool fromManager, std::list<WCVisualObject*> objects);			//!< Called on selection
	virtual void OnDeselection(const bool fromManager);												//!< Called on deselection
	
	/*** Actions and Drawing Modes ***/
	static WCActionPartPlaneCreate* ActionCreate(WCFeature *creator, const std::string &name,			//!< Primary create action
												const WCVector4 &p0, const WCVector4 &p1, const WCVector4 &p2);

	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCPartPlane &plane);					//!< Overloaded output operator	
};


/***********************************************~***************************************************/


#endif //__PART_PLANE_H__

