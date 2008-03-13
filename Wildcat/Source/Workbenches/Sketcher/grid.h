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


#ifndef __GRID_H__
#define __GRID_H__


/*** Included Header Files ***/
#include "Kernel/wftrl.h"
#include "Sketcher/sketch.h"


/*** Local Defines ***/
#define GRID_DEFAULT_COLOR						0.2f, 0.8f, 0.2f, 0.25f
#define GRID_DEFAULT_GRADUATIONS				5
#define GRID_SUBLINE_LIMIT						24
#define GRID_LINEWIDTH							1.0
#define GRID_LINESTIPPLE_FACTOR					2
#define GRID_LINESTIPPLE_PATTERN				0x5555


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


class WCGrid : virtual public WCVisualObject {
protected:
	WCSketch									*_sketch;											//!< Parent sketch
	bool										_isSnap, _isDistortable;							//!< Enable snapping and distortions
	WPFloat										_xSpacing, _ySpacing;								//!< X and Y line spacing
	WPUInt										_xGraduations, _yGraduations;						//!< X and Y marker graduations
	GLuint										_buffer;											//!< Display buffer object
	GLint										_numLines;											//!< Number of lines in the buffer
	GLuint										_subBuffer;											//!< Subline buffer object
	GLint										_numSubLines;										//!< Number of sublines
	WPFloat										_xMin, _yMin, _xMax, _yMax;							//!< Corners of the grid

private:
	void GenerateBuffers(void);																		//!< Generate the display buffer

	//Deny Access
	WCGrid();																						//!< Deny access to default constructor
	WCGrid(const WCGrid& grid);																		//!< Deny access to copy constructor
	WCGrid& operator=(const WCGrid &grid);															//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCGrid(WCSketch &sketch, const WPFloat &xSpacing, const WPFloat &ySpacing,						//!< Primary constructor
		const WPUInt &xGrads=GRID_DEFAULT_GRADUATIONS, const WPUInt &yGrads=GRID_DEFAULT_GRADUATIONS);
	~WCGrid();																						//!< Default destructor
	
	//Member Access Methods
	inline bool IsSnap(void) const				{ return this->_isSnap; }							//!< Get the snap state
	inline void IsSnap(const bool state)		{ this->_isSnap = state; }							//!< Set the snap state
	inline bool IsDistortable(void) const		{ return this->_isDistortable; }					//!< Get the distortion state
	inline void IsDistortable(const bool state)	{ this->_isDistortable = state; }					//!< Set the distortion state
	inline WPFloat XSpacing(void) const			{ return this->_xSpacing; }							//!< Get the x spacing
	inline void XSpacing(const WPFloat &spacing){ this->_xSpacing = spacing; }						//!< Set the x spacing
	inline WPFloat YSpacing(void) const			{ return this->_ySpacing; }							//!< Get the y spacing
	inline void YSpacing(const WPFloat &spacing){ this->_ySpacing = spacing; }						//!< Set the y spacing	
	inline WPUInt XGraduations(void) const		{ return this->_xGraduations; }						//!< Get the x graduations
	inline void XGraduations(const WPUInt &grads){ this->_xGraduations = grads; }					//!< Set the x graduations	
	inline WPUInt YGraduations(void) const		{ return this->_yGraduations; }						//!< Get the y graduations
	inline void YGraduations(const WPUInt &grads){ this->_yGraduations = grads; }					//!< Set the y graduations
	inline WPFloat XMin(void) const				{ return this->_xMin; }								//!< Get the xMin value
	inline WPFloat XMax(void) const				{ return this->_xMax; }								//!< Get the xMax value
	inline WPFloat YMin(void) const				{ return this->_yMin; }								//!< Get the yMin value
	inline WPFloat YMax(void) const				{ return this->_yMax; }								//!< Get the yMax value

	//Primary Methods
	bool Snap(WPFloat &x, WPFloat &y);																//!< Snap coords to the grid

	//Inherited Methods
	void ReceiveNotice(WCObjectMsg msg, WCObject *sender);											//!< Receive notice
	void Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom);				//!< Render the object
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object

	/*** Non-Member Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCGrid &grid);							//!< Overloaded output operator	
};


/***********************************************~***************************************************/


#endif //__GRID_H__

