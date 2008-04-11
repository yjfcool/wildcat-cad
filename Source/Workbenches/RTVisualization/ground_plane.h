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


#ifndef __GROUND_PLANE_H__
#define __GROUND_PLANE_H__


/*** Included Header Files ***/
#include "RTVisualization/wrtv.h"
#include "RTVisualization/visualization.h"


/*** Locally Defined Values ***/
#define GROUNDPLANE_DEFAULT_COLOR				0.2f, 0.8f, 0.2f, 1.0f
#define GROUNDPLANE_LINEWIDTH					1.0
//#define GRID_DEFAULT_GRADUATIONS				5
//#define GRID_SUBLINE_LIMIT						24
//#define GRID_LINESTIPPLE_FACTOR					2
//#define GRID_LINESTIPPLE_PATTERN				0x5555


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


class WCGroundPlane : virtual public WCVisualObject {
protected:
	WCVisualization								*_visualization;									//!< Parent visualization
	WPFloat										_xSpacing, _ySpacing;								//!< X and Y line spacing
//	WPUInt										_xGraduations, _yGraduations;						//!< X and Y marker graduations
	GLuint										_vertBuffer, _normBuffer;							//!< Display buffer object
	GLint										_numLines;											//!< Number of lines in the buffer
//	GLuint										_subBuffer;											//!< Subline buffer object
//	GLint										_numSubLines;										//!< Number of sublines
//	WPFloat										_xMin, _yMin, _xMax, _yMax;							//!< Corners of the grid
private:
	void GenerateBuffers(void);																		//!< Generate the display buffer
	//Hidden Constructors
	WCGroundPlane();																				//!< Deny access to default constructor
	WCGroundPlane(const WCGroundPlane&);															//!< Deny access to copy constructor
	WCGroundPlane& operator=(const WCGroundPlane&);													//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCGroundPlane(WCVisualization &vis, const WPFloat &xSpacing, const WPFloat &ySpacing);			//!< Primary constructor
	~WCGroundPlane();																				//!< Default destructor
	
	//Member Access Methods
	inline WPFloat XSpacing(void) const			{ return this->_xSpacing; }							//!< Get the x spacing
	inline void XSpacing(const WPFloat &spacing){ this->_xSpacing = spacing; }						//!< Set the x spacing
	inline WPFloat YSpacing(void) const			{ return this->_ySpacing; }							//!< Get the y spacing
	inline void YSpacing(const WPFloat &spacing){ this->_ySpacing = spacing; }						//!< Set the y spacing	
//	inline WPUInt XGraduations(void) const		{ return this->_xGraduations; }						//!< Get the x graduations
//	inline void XGraduations(const WPUInt &grads){ this->_xGraduations = grads; }					//!< Set the x graduations	
//	inline WPUInt YGraduations(void) const		{ return this->_yGraduations; }						//!< Get the y graduations
//	inline void YGraduations(const WPUInt &grads){ this->_yGraduations = grads; }					//!< Set the y graduations
//	inline WPFloat XMin(void) const				{ return this->_xMin; }								//!< Get the xMin value
//	inline WPFloat XMax(void) const				{ return this->_xMax; }								//!< Get the xMax value
//	inline WPFloat YMin(void) const				{ return this->_yMin; }								//!< Get the yMin value
//	inline WPFloat YMax(void) const				{ return this->_yMax; }								//!< Get the yMax value

	//Inherited Methods
	virtual void ReceiveNotice(WCObjectMsg msg, WCObject *sender);									//!< Receive notice
	virtual void Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom);		//!< Render the object
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object

	/*** Non-Member Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCGroundPlane &ground);				//!< Overloaded output operator	
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__GROUND_PLANE_H__

