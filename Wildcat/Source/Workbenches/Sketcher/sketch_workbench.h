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


#ifndef __SKETCH_WORKBENCH_H__
#define __SKETCH_WORKBENCH_H__


/*** Included Header Files ***/
#include "Kernel/wftrl.h"
#include "Kernel/workbench.h"
#include "Sketcher/sketch_align_suggestion.h"


/*** Local Defines ***/
#define SKETCHWORKBENCH_CROSSHAIR_SIZE			0.125
#define SKETCHWORKBENCH_POINTER_DEFAULT_COLOR		1.0, 0.0, 0.0, 1.0
#define SKETCHWORKBENCH_POINTER_CONSTRUCTION_COLOR	0.4, 0.4, 1.0, 1.0

/*** Class Predefines ***/
class WCSketch;
class WCGrid;


/***********************************************~***************************************************/


class WCSketchWorkbench : public WCWorkbench {
protected:
	WCSketch									*_sketch;											//!< Sketch parent
	WCGrid										*_grid;												//!< Grid object
	WPFloat										_mouseX, _mouseY;									//!< Mouse tracking
	WPFloat										_snapMouseX, _snapMouseY;							//!< Snapped mouse tracking
	bool										_isConstruction;									//!< Is workbench in construction mode
	bool										_isSuggest;											//!< Is snap suggestion on
	bool										_geoConstraints;									//!< Auto-create geometric constraints
	bool										_dimConstraints;									//!< Auto-create dimensional constraints

private:
	//Private Methods
	void CalculateMousePosition();																	//!< Update mouse tracking
	void RenderCoordinates(const WCColor &color);													//!< Write the current coordinates
	
	//Deny Access
	WCSketchWorkbench(const WCSketchWorkbench& bench);												//!< Deny access to copy constructor
	WCSketchWorkbench& operator=(const WCSketchWorkbench& bench);									//!< Deny access to operator equals
	
public:
	//Constructors and Destructors
	WCSketchWorkbench(WCSketch *sketch);															//!< Primary constructor
	~WCSketchWorkbench();																			//!< Default destructor
	
	//Member Access Methods
	inline WCSketch* Sketch(void)				{ return this->_sketch; }							//!< Get the sketch
	inline WCGrid* Grid(void)					{ return this->_grid; }								//!< Get the sketch grid
	inline WPFloat MouseX(void) const			{ return this->_mouseX; }							//!< Get mouse X position
	inline WPFloat MouseY(void) const			{ return this->_mouseY; }							//!< Get mouse Y position
	inline WPFloat SnapMouseX(void) const		{ return this->_snapMouseX; }						//!< Get the snapped mouse X position
	inline WPFloat SnapMouseY(void) const		{ return this->_snapMouseY; }						//!< Get the snapped mouse Y position
	inline bool IsConstruction(void) const		{ return this->_isConstruction; }					//!< Get construction mode state
	inline void IsConstruction(const bool state){ this->_isConstruction = state; }					//!< Set construction mode state
	inline bool IsSuggestionSnap(void) const		{ return this->_isSuggest; }					//!< Get suggestion mode state
	inline void IsSuggestionSnap(const bool state)	{ this->_isSuggest = state; }					//!< Set suggestion mode state
	inline bool IsGeometricConstraint(void) const	{ return this->_geoConstraints; }				//!< Get auto-geo state
	inline void IsGeometricConstraint(const bool state) { this->_geoConstraints = state; }			//!< Set auto-geo state
	inline bool IsDimensionalConstraint(void) const		{ return this->_dimConstraints; }			//!< Get auto-dim state
	inline void IsDimensionalConstraint(const bool state){ this->_dimConstraints = state; }			//!< Set auto-dim state

	//Methods
	void CreateInitialObjects(void);																//!< Crappy way to do this

	//Inherited Member Methods
	bool OnEnter(void);																				//!< Entrance method
	bool OnExit(void);																				//!< Forced exit method
	bool OnUserMessage(const WCUserMessage &message);												//!< React to user message
	void OnRotatePress(void)					{ }													//!< React to rotate press event
	void OnRotateRelease(void)					{ }													//!< React to rotate release event
	void OnMouseMove(const int x, const int y);														//!< React to mouse movement even
	
	//Special Options
	WCSketchAlignmentSuggestion SuggestAlignment(const std::list<WCSketchAlignmentSuggestion> &rules,//!< Suggest an alignment location
												WCGeometricObject *object, WPFloat &x, WPFloat &y);
	void RenderAlignmentSuggestion(const WCSketchAlignmentSuggestion suggest,						//!< Render appropriate type of suggestion
												const WPFloat x, const WPFloat y);
	void RenderPoint(const WPFloat x, const WPFloat y, const WCColor &color=WCColor::Default());	//!< Render point at current location
	void RenderCrosshairs(const WPFloat x, const WPFloat y, const WCColor &color=WCColor::Default());	//!< Render crosshairs at current location
	void RenderCrossTarget(const WPFloat x, const WPFloat y, const WCColor &color=WCColor::Default());	//!< Render crosshairs with target box
	
	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCSketchWorkbench &wb);				//!< Overloaded output operator	
};


/***********************************************~***************************************************/


#endif //__SKETCH_WORKBENCH_H__

