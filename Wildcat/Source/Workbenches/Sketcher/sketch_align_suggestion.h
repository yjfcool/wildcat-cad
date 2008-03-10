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


#ifndef __SKETCH_ALIGN_SUGGESTION_H__
#define __SKETCH_ALIGN_SUGGESTION_H__


/*** Included Header Files ***/
#include "Kernel/wftrl.h"
#include "Kernel/workbench.h"


/*** Local Defines ***/
#define SKETCHWORKBENCH_SUGGEST_TOLERANCE		0.05
#define SKETCHWORKBENCH_SUGGEST_COLOR			1.0, 0.0, 1.0, 1.0
#define SKETCHWORKBENCH_SUGGEST_LINE_FACTOR		2.0
#define SKETCHWORKBENCH_SUGGEST_LINE_PATTERN	0x5555


/*** Class Predefines ***/
class WCSketchWorkbench;
class WCSketchAlignmentSuggestion;
class WCSketchFeature;


/***********************************************~***************************************************/


class WCSketchAlignmentEvaluator {
public:
	WCSketchAlignmentEvaluator()				{ }
	virtual ~WCSketchAlignmentEvaluator()		{ }
	//Manditory Virtual Method	
	virtual WCSketchAlignmentSuggestion Evaluate(WCSketchWorkbench *wb, WCGeometricObject *object , WPFloat &x, WPFloat &y)=0;
};


class WCSketchAlignmentRenderer {
public:
	WCSketchAlignmentRenderer()					{ }
	virtual ~WCSketchAlignmentRenderer()		{ }
	//Manditory Virtual Method
	virtual void Render(WCSketchWorkbench *wb, const WPFloat x, const WPFloat y)=0;
};


/***********************************************~***************************************************/


class WCSketchAlignmentSuggestion {
private:
	WPUInt										_type;												//!< Type of suggestion
	WCSketchAlignmentEvaluator					*_evaluator;										//!< Suggestion evaluator
	WCSketchAlignmentRenderer					*_renderer;											//!< Suggestion renderer
	WCGeometricPoint							*_point;											//!< Suggested point
	WCGeometricLine								*_line;												//!< Suggested line
	WCGeometricCurve							*_curve;											//!< Suggested curve
	
	//Deny Access
	WCSketchAlignmentSuggestion();																	//!< Deny access to default constructor
	WCSketchAlignmentSuggestion(const WPUInt type, WCSketchAlignmentEvaluator *evaluator,			//!< Primary constructor
								WCSketchAlignmentRenderer *renderer,
								WCGeometricPoint *point, WCGeometricLine *line, WCGeometricCurve *curve) :
								_type(type), _evaluator(evaluator), _renderer(renderer), _point(point), _line(line), _curve(curve) { }
public:
	//Constructors and Destructors
	WCSketchAlignmentSuggestion(const WCSketchAlignmentSuggestion &type) : _type(type._type),		//!< Copy constructor
												_evaluator(type._evaluator), _renderer(type._renderer),
												_point(type._point), _line(type._line), _curve(type._curve) { }
	~WCSketchAlignmentSuggestion()				{ }													//!< Default destructor

	//Member Access Methods
	inline WCGeometricPoint* Point(void)		{ return this->_point; }							//!< Get suggested point
	inline WCGeometricLine* Line(void)			{ return this->_line; }								//!< Get suggested line
	inline WCGeometricCurve* Curve(void)		{ return this->_curve; }							//!< Get suggested curve
	
	//Evaluator and Render Methods
	inline WCSketchAlignmentSuggestion Evaluate(WCSketchWorkbench *wb, WCGeometricObject *obj,		//! Evaluate a suggestion
												WPFloat &x, WPFloat &y) const { 
													if (this->_evaluator != NULL) 
														return this->_evaluator->Evaluate(wb, obj, x, y);
													else return WCSketchAlignmentSuggestion::None(); }
	inline void Render(WCSketchWorkbench *wb, const WPFloat x, const WPFloat y) const {					//!< Render a suggestion
												if (this->_renderer != NULL) this->_renderer->Render(wb, x, y); }

	//Static Creation Methods
	static WCSketchAlignmentSuggestion None(void);
	//Point Suggestions
	static WCSketchAlignmentSuggestion CoincidentToPoint(WCGeometricPoint *point=NULL);
	static WCSketchAlignmentSuggestion VerticalToPoint(WCGeometricPoint *point=NULL);
	static WCSketchAlignmentSuggestion HorizontalToPoint(WCGeometricPoint *point=NULL);
	//Line Suggestions
	static WCSketchAlignmentSuggestion CoincidentToLine(WCGeometricLine *line=NULL);
	static WCSketchAlignmentSuggestion VerticalToLine(WCGeometricLine *line=NULL);
	static WCSketchAlignmentSuggestion HorizontalToLine(WCGeometricLine *line=NULL);
	//Curve Suggestions
	static WCSketchAlignmentSuggestion CoincidentToCurve(WCGeometricCurve *curve=NULL);
	static WCSketchAlignmentSuggestion VerticalToCurve(WCGeometricCurve *curve=NULL);
	static WCSketchAlignmentSuggestion HorizontalToCurve(WCGeometricCurve *curve=NULL);

	//Operator Overlaods
	WCSketchAlignmentSuggestion& operator=(const WCSketchAlignmentSuggestion &type) { this->_type = type._type;		//!< Equals operator
												this->_evaluator = type._evaluator; this->_renderer = type._renderer;
												this->_point = type._point; this->_line = type._line;
												this->_curve = type._curve; return *this; }
	bool operator==(const WCSketchAlignmentSuggestion &type) const { return (this->_type == type._type); }	//!< Equality operator
	bool operator!=(const WCSketchAlignmentSuggestion &type) const { return (this->_type != type._type); }	//!< Inequality operator
	
	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCSketchAlignmentSuggestion &suggestion) {		//!< Overloaded output operator	
												out << "Suggestion: " << suggestion._type << std::endl;
												out << "\tEvaluator: " << suggestion._evaluator << std::endl;
												out << "\tRenderer: " << suggestion._renderer << std::endl;
												out << "\tPoint: " << suggestion._point << std::endl;
												out << "\tLine: " << suggestion._line << std::endl;
												out << "\tCurve: " << suggestion._curve << std::endl;
												return out; }
};


/***********************************************~***************************************************/


#endif //__SKETCH_ALIGN_SUGGESTION_H__

