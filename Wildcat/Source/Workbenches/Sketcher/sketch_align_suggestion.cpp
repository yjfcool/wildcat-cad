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


/*** Included Header Files ***/
#include "Sketcher/sketch_align_suggestion.h"
#include "Sketcher/sketch_workbench.h"
#include "Sketcher/sketch.h"
#include "Sketcher/grid.h"
#include "Kernel/document.h"
#include "PartDesign/part_plane.h"


/***********************************************~***************************************************/


#define SKETCHSUGGESTION_NONE					0
#define SKETCHSUGGESTION_COINCIDENT_POINT		1
#define SKETCHSUGGESTION_VERTICAL_POINT			2
#define SKETCHSUGGESTION_HORIZONTAL_POINT		3
#define SKETCHSUGGESTION_COINCIDENT_LINE		4
#define SKETCHSUGGESTION_VERTICAL_LINE			5
#define SKETCHSUGGESTION_HORIZONTAL_LINE		6
#define SKETCHSUGGESTION_COINCIDENT_CURVE		7
#define SKETCHSUGGESTION_VERTICAL_CURVE			8
#define SKETCHSUGGESTION_HORIZONTAL_CURVE		9


/***********************************************~***************************************************/


WCSketchAlignmentSuggestion WCSketchAlignmentSuggestion::None(void) { 
	return WCSketchAlignmentSuggestion(SKETCHSUGGESTION_NONE, NULL, NULL, NULL, NULL, NULL); }


/***********************************************~***************************************************/


class WCEvaluateCoincidentToPoint : public WCSketchAlignmentEvaluator {
public:
	WCSketchAlignmentSuggestion Evaluate(WCSketchWorkbench *wb, WCGeometricObject *object, WPFloat &x, WPFloat &y) {
		//Look through all points
		WPFloat distance = 1.0;
		WCVector4 pos;
		WCVector4 inPos(x, y, 0.0, 1.0);
		std::map<WCGeometricPoint*,WCEventController*>::iterator pointIter;
		for (pointIter = wb->Sketch()->PointMap().begin(); pointIter != wb->Sketch()->PointMap().end(); pointIter++) {
			//See if matches object - if not evaluate for match
			if ((*pointIter).first != object) {
				//Project point back to sketch plane
				pos = wb->Sketch()->ReferencePlane()->InverseTransformMatrix() * (*pointIter).first->Data();		
				//Check absolute distance
				distance = inPos.Distance(pos);
				if (distance < (0.6 * wb->Grid()->XSpacing())) {
					x = pos.I();
					y = pos.J();
					return WCSketchAlignmentSuggestion::CoincidentToPoint((*pointIter).first);
				}
			}
		}
		//Otherwie, no suggestion at this time
		return WCSketchAlignmentSuggestion::None();	
	}
};


/***********************************************~***************************************************/


class WCEvaluateVerticalToPoint : public WCSketchAlignmentEvaluator {
public:
	WCSketchAlignmentSuggestion Evaluate(WCSketchWorkbench *wb, WCGeometricObject *object, WPFloat &x, WPFloat &y) {
		//Look through all points
		WPFloat distance = 1.0;
		WCVector4 pos;
		std::map<WCGeometricPoint*,WCEventController*>::iterator pointIter;
		for (pointIter = wb->Sketch()->PointMap().begin(); pointIter != wb->Sketch()->PointMap().end(); pointIter++) {
			//See if matches object - if not evaluate for match
			if ((*pointIter).first != object) {
				//Project point back to sketch plane
				pos = wb->Sketch()->ReferencePlane()->InverseTransformMatrix() * (*pointIter).first->Data();	
				//Check vertical distance
				distance = sqrt( pow(x - pos.I(), 2) );
				if (distance < (0.6 * wb->Grid()->XSpacing())) {
					x = pos.I();
					return WCSketchAlignmentSuggestion::VerticalToPoint((*pointIter).first);
				}
			}
		}
		//Otherwie, no suggestion at this time
		return WCSketchAlignmentSuggestion::None();
	}
};


/***********************************************~***************************************************/


class WCEvaluateHorizontalToPoint : public WCSketchAlignmentEvaluator {
public:
	WCSketchAlignmentSuggestion Evaluate(WCSketchWorkbench *wb, WCGeometricObject *object, WPFloat &x, WPFloat &y) {
		//Look through all points
		WPFloat distance = 1.0;
		WCVector4 pos;
		std::map<WCGeometricPoint*,WCEventController*>::iterator pointIter;
		for (pointIter = wb->Sketch()->PointMap().begin(); pointIter != wb->Sketch()->PointMap().end(); pointIter++) {
			//See if matches object - if not evaluate for match
			if ((*pointIter).first != object) {
				//Project point back to sketch plane
				pos = wb->Sketch()->ReferencePlane()->InverseTransformMatrix() * (*pointIter).first->Data();		
				//Check horizontal distance
				distance = sqrt( pow(y - pos.J(), 2) );
				if (distance < (0.6 * wb->Grid()->XSpacing())) {
					y = pos.J();
					return WCSketchAlignmentSuggestion::HorizontalToPoint((*pointIter).first);
				}
			}
		}
		//Otherwie, no suggestion at this time
		return WCSketchAlignmentSuggestion::None();
	}
};


/***********************************************~***************************************************/


WPFloat PointToLineDistance2(const WPFloat startX, const WPFloat startY, const WPFloat endX, const WPFloat endY, 
	const WPFloat pointX, const WPFloat pointY) {
	//Calculate the distance
	WPFloat num = (endX - startX) * (startY - pointY) - (startX - pointX) * (endY - startY);
	if (num < 0.0) num *= -1.0;
	WPFloat den = sqrt( pow( endX - startX, 2) + pow( endY - startY, 2) );
	if (den < 0.001) return 100000000.0;
	return num / den;
}


void ProjectPointToLine2(const WPFloat startX, const WPFloat startY, const WPFloat endX, const WPFloat endY, 
	WPFloat &pointX, WPFloat &pointY) {
	WPFloat num = (endX - startX) * (startY - pointY) - (startX - pointX) * (endY - startY);
	WPFloat den = sqrt( pow( endX - startX, 2) + pow( endY - startY, 2) );
	WPFloat dist = num / den;
	WPFloat tanX = (endY - startY) / den;
	WPFloat tanY = (startX - endX) / den;
	pointX = pointX - tanX * dist;
	pointY = pointY - tanY * dist;
}


bool IsWithinLine(const WPFloat startX, const WPFloat startY, const WPFloat endX, const WPFloat endY, 
	WPFloat pointX, WPFloat pointY) {
	WPFloat length = sqrt( pow( endX - startX, 2) + pow( endY - startY, 2) );
	WPFloat sDist = sqrt( pow( startX - pointX, 2) + pow( startY - pointY, 2) );
	WPFloat eDist = sqrt( pow( endX - pointX, 2) + pow( endY - pointY, 2) );
	if ((sDist <= length) && (eDist <= length)) return true;
	return false;
}


class WCEvaluateCoincidentToLine : public WCSketchAlignmentEvaluator {
public:
	WCSketchAlignmentSuggestion Evaluate(WCSketchWorkbench *wb, WCGeometricObject *object, WPFloat &x, WPFloat &y) {
		WPFloat dist = 10.0;
		WCVector4 start, end;
		//Look through all lines
		std::map<WCGeometricLine*,WCEventController*>::iterator lineIter;
		for (lineIter = wb->Sketch()->LineMap().begin(); lineIter != wb->Sketch()->LineMap().end(); lineIter++) {		
			//See if matches object - if not evaluate for match
			if ((*lineIter).first != object) {		
				//Inverse project line onto plane
				start = (*lineIter).first->Begin();
				end = (*lineIter).first->End();
				start = wb->Sketch()->ReferencePlane()->InverseTransformMatrix() * start;
				end = wb->Sketch()->ReferencePlane()->InverseTransformMatrix() * end;
				//Determine distance to line
				dist = PointToLineDistance2(start.I(), start.J(), end.I(), end.J(), x, y);
				//If close enough, project to closest point on line
				if (dist < (0.6 * wb->Grid()->XSpacing())) {
					if (IsWithinLine(start.I(), start.J(), end.I(), end.J(), x, y)) {
						//Move x, y onto the line
						ProjectPointToLine2(start.I(), start.J(), end.I(), end.J(), x, y);
						//Return the suggestion
						return WCSketchAlignmentSuggestion::CoincidentToLine((*lineIter).first);
					}
				}
			}
		}
		//Otherwie, no suggestion at this time
		return WCSketchAlignmentSuggestion::None();
	}
};


/***********************************************~***************************************************/


class WCEvaluateVerticalToLine : public WCSketchAlignmentEvaluator {
public:
	WCSketchAlignmentSuggestion Evaluate(WCSketchWorkbench *wb, WCGeometricObject *object, WPFloat &x, WPFloat &y) {
		WPFloat dist = 10.0;
		WCVector4 begin, end;
		//Look through all lines
		std::map<WCGeometricLine*,WCEventController*>::iterator lineIter;
		for (lineIter = wb->Sketch()->LineMap().begin(); lineIter != wb->Sketch()->LineMap().end(); lineIter++) {		
			//See if matches object - if not evaluate for match
			if ((*lineIter).first != object) {
				//Project begin and end points back to sketch plane
				begin = wb->Sketch()->ReferencePlane()->InverseTransformMatrix() * (*lineIter).first->Begin();
				end = wb->Sketch()->ReferencePlane()->InverseTransformMatrix() * (*lineIter).first->End();
				//Check vertical distance to begin point
				dist = sqrt( pow(x - begin.I(), 2) );
				if (dist < (0.6 * wb->Grid()->XSpacing())) {
					x = begin.I();
					return WCSketchAlignmentSuggestion::VerticalToLine((*lineIter).first);
				}
				//Check vertical distance to end point
				dist = sqrt( pow(x - end.I(), 2) );
				if (dist < (0.6 * wb->Grid()->XSpacing())) {
					x = end.I();
					return WCSketchAlignmentSuggestion::VerticalToLine((*lineIter).first);
				}
			}
		}
		//Otherwie, no suggestion at this time
		return WCSketchAlignmentSuggestion::None();
	}
};


/***********************************************~***************************************************/


class WCEvaluateHorizontalToLine : public WCSketchAlignmentEvaluator {
public:
	WCSketchAlignmentSuggestion Evaluate(WCSketchWorkbench *wb, WCGeometricObject *object, WPFloat &x, WPFloat &y) {
		WPFloat dist = 10.0;
		WCVector4 begin, end;
		//Look through all lines
		std::map<WCGeometricLine*,WCEventController*>::iterator lineIter;
		for (lineIter = wb->Sketch()->LineMap().begin(); lineIter != wb->Sketch()->LineMap().end(); lineIter++) {		
			//See if matches object - if not evaluate for match
			if ((*lineIter).first != object) {
				//Project begin and end points back to sketch plane
				begin = wb->Sketch()->ReferencePlane()->InverseTransformMatrix() * (*lineIter).first->Begin();
				end = wb->Sketch()->ReferencePlane()->InverseTransformMatrix() * (*lineIter).first->End();
				//Check horizontal distance to begin point
				dist = sqrt( pow(y - begin.J(), 2) );
				if (dist < (0.6 * wb->Grid()->XSpacing())) {
					y = begin.J();
					return WCSketchAlignmentSuggestion::HorizontalToLine((*lineIter).first);
				}
				//Check horizontal distance to end point
				dist = sqrt( pow(y - end.J(), 2) );
				if (dist < (0.6 * wb->Grid()->XSpacing())) {
					y = end.J();
					return WCSketchAlignmentSuggestion::HorizontalToLine((*lineIter).first);
				}
			}
		}
		//Otherwie, no suggestion at this time
		return WCSketchAlignmentSuggestion::None();
	}
};


/***********************************************~***************************************************/


class WCEvaluateCoincidentToCurve : public WCSketchAlignmentEvaluator {
public:
	WCSketchAlignmentSuggestion Evaluate(WCSketchWorkbench *wb, WCGeometricObject *object, WPFloat &x, WPFloat &y) {
		WPFloat dist = 10.0;
		//Project x,y into 3D
		WCVector4 pos(x, y, 0.0, 1.0), loc;
		WPFloat u;
		pos = wb->Sketch()->ReferencePlane()->TransformMatrix() * pos;
		//Look through all lines
		std::map<WCNurbsCurve*,WCEventController*>::iterator curveIter;
		for (curveIter = wb->Sketch()->CurveMap().begin(); curveIter != wb->Sketch()->CurveMap().end(); curveIter++) {		
			//See if matches object - if not evaluate for match
			if ((*curveIter).first != object) {
				//Inverse project pos onto curve
				loc = (*curveIter).first->PointInversion(pos, u);
				//Determine distance to point
				dist = loc.Distance(pos);
				//If close enough, project to closest point on line
				if (dist < (0.6 * wb->Grid()->XSpacing())) {
					//Project loc back to plane
					loc = wb->Sketch()->ReferencePlane()->InverseTransformMatrix() * loc;
					//Update x and y
					x = loc.I();
					y = loc.J();
					//Return the suggestion
					return WCSketchAlignmentSuggestion::CoincidentToCurve( (*curveIter).first );
				}
			}
		}
		//Otherwie, no suggestion at this time
		return WCSketchAlignmentSuggestion::None();
	}
};


/***********************************************~***************************************************/


class WCEvaluateVerticalToCurve : public WCSketchAlignmentEvaluator {
public:
	WCSketchAlignmentSuggestion Evaluate(WCSketchWorkbench *wb, WCGeometricObject *object, WPFloat &x, WPFloat &y) {
		//Otherwie, no suggestion at this time
		return WCSketchAlignmentSuggestion::None();
	}
};


/***********************************************~***************************************************/


class WCEvaluateHorizontalToCurve : public WCSketchAlignmentEvaluator {
public:
	WCSketchAlignmentSuggestion Evaluate(WCSketchWorkbench *wb, WCGeometricObject *object, WPFloat &x, WPFloat &y) {
		//Otherwie, no suggestion at this time
		return WCSketchAlignmentSuggestion::None();
	}
};


/***********************************************~***************************************************/


class WCRenderCoincidentTo : public WCSketchAlignmentRenderer {
public:
	void Render(WCSketchWorkbench *wb, const WPFloat &x, const WPFloat &y) {
		WPFloat iconSize = 16 * SCREEN_PIXEL_WIDTH;
		WPFloat localX = x + 0.01;
		WPFloat localY = y + 0.01;
		//Calculate box slighly above and right of location
		WCVector4 ll(localX, localY, 0.0, 1.0);
		WCVector4 xUnit(1.0, 0.0, 0.0, 1.0);
		WCVector4 yUnit(0.0, 1.0, 0.0, 1.0);

		//Project from the plane (adjust for zoom factor to maintain icon size)
		ll = wb->Sketch()->ReferencePlane()->TransformMatrix() * ll;
		xUnit = wb->Sketch()->ReferencePlane()->TransformMatrix() * xUnit;
		yUnit = wb->Sketch()->ReferencePlane()->TransformMatrix() * yUnit;
		xUnit = xUnit * (iconSize / wb->Feature()->Document()->Scene()->ActiveCamera()->Zoom());
		yUnit = yUnit * (iconSize / wb->Feature()->Document()->Scene()->ActiveCamera()->Zoom());
		WCVector4 ul = ll + yUnit;
		WCVector4 ur = ll + xUnit + yUnit;
		WCVector4 lr = ll + xUnit;

		//Set up data arrays
		GLfloat data[12] = {(GLfloat)ll.I(), (GLfloat)ll.J(), (GLfloat)ll.K(),
							(GLfloat)ul.I(), (GLfloat)ul.J(), (GLfloat)ul.K(),
							(GLfloat)ur.I(), (GLfloat)ur.J(), (GLfloat)ur.K(), 
							(GLfloat)lr.I(), (GLfloat)lr.J(), (GLfloat)lr.K()};
		WSTexture *tex = wb->Feature()->Document()->Scene()->TextureManager()->TextureFromName("concentric32");
		GLfloat texCoords[8] = { 0.0, 0.0, 
								 0.0, tex->_height, 
								 tex->_width, tex->_height, 
								 tex->_width, 0.0 };
		//Enable state
		glDisable(GL_DEPTH_TEST);
		glEnable(tex->_target);
		glEnableClientState(GL_VERTEX_ARRAY);
		glBindTexture(tex->_target, tex->_id);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glColor4f(1.0, 1.0, 1.0, 1.0);
		
		//Draw textured quad
		glVertexPointer(3, GL_FLOAT, 0, data);
		glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
		glDrawArrays(GL_QUADS, 0, 4);
		
		//Clean up state
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindTexture(tex->_target, 0);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisable(tex->_target);
		glEnable(GL_DEPTH_TEST);
	}
};


class WCRenderVerticalTo : public WCSketchAlignmentRenderer {
public:
	void Render(WCSketchWorkbench *wb, const WPFloat &x, const WPFloat &y) {
		//Calculate vertical line start and stop
		WCVector4 start(x, wb->Grid()->YMin(), 0.0, 1.0);
		WCVector4 end(x, wb->Grid()->YMax(), 0.0, 1.0);
		start = wb->Sketch()->ReferencePlane()->TransformMatrix() * start;
		end = wb->Sketch()->ReferencePlane()->TransformMatrix() * end;
		//Set up data
		GLfloat data[6] = { (GLfloat)start.I(), (GLfloat)start.J(), (GLfloat)start.K(),
							(GLfloat)end.I(), (GLfloat)end.J(), (GLfloat)end.K() };
		//Set up draw environment
		glLineWidth(1.0);
		glLineStipple(SKETCHWORKBENCH_SUGGEST_LINE_FACTOR, SKETCHWORKBENCH_SUGGEST_LINE_PATTERN);
		glEnable(GL_LINE_STIPPLE);
		glDisable(GL_DEPTH_TEST);
		glColor4f(SKETCHWORKBENCH_SUGGEST_COLOR);
		glEnableClientState(GL_VERTEX_ARRAY);
		
		//Draw the line
		glVertexPointer(3, GL_FLOAT, 0, data);
		glDrawArrays(GL_LINES, 0, 2);
		
		//Clean up state
		glDisableClientState(GL_VERTEX_ARRAY);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_LINE_STIPPLE);
	}
};


class WCRenderHorizontalTo : public WCSketchAlignmentRenderer {
public:
	void Render(WCSketchWorkbench *wb, const WPFloat &x, const WPFloat &y) {
		//Calculate horizontal line start and stop
		WCVector4 start(wb->Grid()->XMin(), y, 0.0, 1.0);
		WCVector4 end(wb->Grid()->XMax(), y, 0.0, 1.0);
		start = wb->Sketch()->ReferencePlane()->TransformMatrix() * start;
		end = wb->Sketch()->ReferencePlane()->TransformMatrix() * end;
		//Set up data
		GLfloat data[6] = { (GLfloat)start.I(), (GLfloat)start.J(), (GLfloat)start.K(),
							(GLfloat)end.I(), (GLfloat)end.J(), (GLfloat)end.K() };
//		GLfloat data[6] = { start.I(), start.J(), start.K(),
//							end.I(), end.J(), end.K() };
		//Set up draw environment
		glLineWidth(1.0);
		glLineStipple(SKETCHWORKBENCH_SUGGEST_LINE_FACTOR, SKETCHWORKBENCH_SUGGEST_LINE_PATTERN);
		glEnable(GL_LINE_STIPPLE);
		glDisable(GL_DEPTH_TEST);
		glColor4f(SKETCHWORKBENCH_SUGGEST_COLOR);
		glEnableClientState(GL_VERTEX_ARRAY);
		
		//Draw the line
		glVertexPointer(3, GL_FLOAT, 0, data);
		glDrawArrays(GL_LINES, 0, 2);
		
		//Clean up state
		glDisableClientState(GL_VERTEX_ARRAY);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_LINE_STIPPLE);
	}
};


/***********************************************~***************************************************/


//Static Evaluators
static WCSketchAlignmentEvaluator *evaluateCoincidentToPoint	= new WCEvaluateCoincidentToPoint();
static WCSketchAlignmentEvaluator *evaluateVerticalToPoint		= new WCEvaluateVerticalToPoint();
static WCSketchAlignmentEvaluator *evaluateHorizontalToPoint	= new WCEvaluateHorizontalToPoint();
static WCSketchAlignmentEvaluator *evaluateCoincidentToLine		= new WCEvaluateCoincidentToLine();
static WCSketchAlignmentEvaluator *evaluateVerticalToLine		= new WCEvaluateVerticalToLine();
static WCSketchAlignmentEvaluator *evaluateHorizontalToLine		= new WCEvaluateHorizontalToLine();
static WCSketchAlignmentEvaluator *evaluateCoincidentToCurve	= new WCEvaluateCoincidentToCurve();
static WCSketchAlignmentEvaluator *evaluateVerticalToCurve		= new WCEvaluateVerticalToCurve();
static WCSketchAlignmentEvaluator *evaluateHorizontalToCurve	= new WCEvaluateHorizontalToCurve();


//Static Renderers
static WCSketchAlignmentRenderer *renderCoincidentTo	= new WCRenderCoincidentTo();
static WCSketchAlignmentRenderer *renderVerticalTo		= new WCRenderVerticalTo();
static WCSketchAlignmentRenderer *renderHorizontalTo	= new WCRenderHorizontalTo();


/***********************************************~***************************************************/


WCSketchAlignmentSuggestion WCSketchAlignmentSuggestion::CoincidentToPoint(WCGeometricPoint *point) {
	//Create the suggestion
	return WCSketchAlignmentSuggestion(SKETCHSUGGESTION_COINCIDENT_POINT, evaluateCoincidentToPoint, 
												renderCoincidentTo, point, NULL, NULL); 
}


WCSketchAlignmentSuggestion WCSketchAlignmentSuggestion::VerticalToPoint(WCGeometricPoint *point) { 
	return WCSketchAlignmentSuggestion(SKETCHSUGGESTION_VERTICAL_POINT, evaluateVerticalToPoint, 
												renderVerticalTo, point, NULL, NULL); }


WCSketchAlignmentSuggestion WCSketchAlignmentSuggestion::HorizontalToPoint(WCGeometricPoint *point) { 
	return WCSketchAlignmentSuggestion(SKETCHSUGGESTION_HORIZONTAL_POINT, evaluateHorizontalToPoint, 
												renderHorizontalTo, point, NULL, NULL); }


WCSketchAlignmentSuggestion WCSketchAlignmentSuggestion::CoincidentToLine(WCGeometricLine *line) { 
	return WCSketchAlignmentSuggestion(SKETCHSUGGESTION_COINCIDENT_LINE, evaluateCoincidentToLine,
												renderCoincidentTo, NULL, line, NULL); }


WCSketchAlignmentSuggestion WCSketchAlignmentSuggestion::VerticalToLine(WCGeometricLine *line) { 
	return WCSketchAlignmentSuggestion(SKETCHSUGGESTION_VERTICAL_LINE, evaluateVerticalToLine,
												renderVerticalTo, NULL, line, NULL); }


WCSketchAlignmentSuggestion WCSketchAlignmentSuggestion::HorizontalToLine(WCGeometricLine *line) { 
	return WCSketchAlignmentSuggestion(SKETCHSUGGESTION_HORIZONTAL_LINE, evaluateHorizontalToLine,
												renderHorizontalTo, NULL, line, NULL); }


WCSketchAlignmentSuggestion WCSketchAlignmentSuggestion::CoincidentToCurve(WCGeometricCurve *curve) { 
	return WCSketchAlignmentSuggestion(SKETCHSUGGESTION_COINCIDENT_CURVE, evaluateCoincidentToCurve,
												renderCoincidentTo, NULL, NULL, curve); }


WCSketchAlignmentSuggestion WCSketchAlignmentSuggestion::VerticalToCurve(WCGeometricCurve *curve) { 
	return WCSketchAlignmentSuggestion(SKETCHSUGGESTION_VERTICAL_CURVE, evaluateVerticalToCurve,
												renderVerticalTo, NULL, NULL, curve); }


WCSketchAlignmentSuggestion WCSketchAlignmentSuggestion::HorizontalToCurve(WCGeometricCurve *curve) { 
	return WCSketchAlignmentSuggestion(SKETCHSUGGESTION_HORIZONTAL_CURVE, evaluateHorizontalToCurve,
												renderHorizontalTo, NULL, NULL, curve); }


/***********************************************~***************************************************/

