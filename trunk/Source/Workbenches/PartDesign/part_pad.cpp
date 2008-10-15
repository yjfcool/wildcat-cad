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
#include "PartDesign/part_pad.h"
#include "PartDesign/part_body.h"
#include "PartDesign/part.h"
#include "PartDesign/part_plane.h"
#include "Sketcher/sketch_profile.h"
#include "Sketcher/sketch.h"


/***********************************************~***************************************************/


/*** GeneratePoints Algorithm ***
 *	This algorithm generates all of a pad's points based on an ordered set of input profiles.  If there are M curves
 *	 in all of the profiles, then there will be M points on the bottom portion of the extrusion and an additional M
 *	 points on the top portion of the extrusion ( 2*M total points).  The order of the points in render point list is lower, upper.
 *	 The order of the points in the topoList is as follows:
 *		1) Bottom profile in reverse list order, starting with ext profile initial point
 *		2) Top profile in list order, starting with ext profile initial point
 *	 Since the profiles are <ext,int,int,...> the points follow the same convention.  Because it is easier to generate both the
 *	 top and bottom points at the same time, the top points are placed into a temporary list while the bottom points are placed
 *	 directly into the _points list.  At the end the temp list is spliced in to create a correct ordering.
 ***/
void WCPartPad::GeneratePoints(std::list<std::list<WSVertexUse*> > &topoBottomPoints, std::list<std::list<WSVertexUse*> > &topoTopPoints) {
	//Check to make sure there is a depth
	if (this->_firstOffset - this->_secondOffset < 0.01) return;

	//Create a point from the start of each curve in each profile
	WCVector4 pt, lower, upper;
	WSVertexUse *tmpUse;
	WCGeometricPoint *lowerPt, *upperPt;
	std::list<WSVertexUse*> lowerPoints, upperPoints;
	std::list<WCSketchProfile*>::iterator profilesIter;
	std::list<std::list<WCSketchProfile*> >::iterator listIter;
	std::list< std::pair<WCGeometricCurve*,bool> > curveList;
	std::list< std::pair<WCGeometricCurve*,bool> >::iterator curveIter;
	//Loop through all profiles and extrude all surfaces
	for (listIter = this->_profiles.begin(); listIter != this->_profiles.end(); listIter++) {
		//Go through the sub-list
		for (profilesIter = (*listIter).begin(); profilesIter != (*listIter).end(); profilesIter++) {
			//Get the list of curves
			curveList = (*profilesIter)->CurveList();
			//Make sure curveList is > 1 (curveList of size zero is a conic section - circle or ellipse)
			if (curveList.size() > 1) {
				//Iterate through all curves in list
				for (curveIter = curveList.begin(); curveIter != curveList.end(); curveIter++) {
					//See if curve is forward
					if ((*curveIter).second) pt = (*curveIter).first->Evaluate(0.0);
					//...Or backwards
					else pt = (*curveIter).first->Evaluate(1.0);
					//Create lower and upper points
					lower = pt + this->_direction * this->_secondOffset;
					upper = pt + this->_direction * this->_firstOffset;
					lowerPt = new WCGeometricPoint(lower);
					upperPt = new WCGeometricPoint(upper);
					//Set the properties for the points
					lowerPt->Color( WCPartFeature::DefaultPointColor );
					upperPt->Color( WCPartFeature::DefaultPointColor );
					lowerPt->Size( PARTFEATURE_POINT_SIZE );
					upperPt->Size( PARTFEATURE_POINT_SIZE );
					//Add the points to the lists of all points
					this->_points.push_back(lowerPt);
					this->_points.push_back(upperPt);
					//Create bottom vertex uses for topology
					tmpUse = new WSVertexUse();
					tmpUse->point = lowerPt;
					lowerPoints.push_back(tmpUse);
					//Create top vertex uses for topology
					tmpUse = new WSVertexUse();
					tmpUse->point = upperPt;
					upperPoints.push_back(tmpUse);
				}
			}
		}
		//Push the whole think onto the topoPoints lists
		topoBottomPoints.push_back(lowerPoints);
		topoTopPoints.push_back(upperPoints);
		//Clear lower and upper points
		lowerPoints.clear();
		upperPoints.clear();
	}
}


/*** GenerateCurves Algorithm ***
 * This algorithm generates all curves and lines necessary for the pad.  If there are M curves in all of the profiles
 *	there will be M curves on the bottom, M curves for the extrusion lines, and M curves for the top ( 3*M total curves).
 *	The order is as follows:
 *		1) Bottom profiles' curves in list order
 *		2) Extrusion side lines in clockwise order, starting with ext profile initial point
 *		3) Top profiles' curves in list order
 *	 One complicating factor is that for display purposes the curve and lines need to be separated.  For topological reasons
 *	 the ordering of all curves must be preserved.
 *	 The other complicating factor is that the top and bottom trim profile lists must be generated.  The top is straightforward and
 *	 is just a duplicate of the existing profiles list but using the new top curves.  The bottom uses the new bottom curves,
 *	 but both the curve order and the curve orientation must be reveresed.
 ***/
void WCPartPad::GenerateCurves(std::list<std::list<WCTrimProfile> > &topTrims, std::list<std::list<WCTrimProfile> > &bottomTrims,
	std::list<std::vector<WSEdgeUse*> > &topoBottomEUs,	std::list<std::vector<WSEdgeUse*> > &topoSideEUs,
	std::list<std::vector<WSEdgeUse*> > &topoTopEUs) {
	//Check to make sure there is a depth
	if (this->_firstOffset - this->_secondOffset < 0.01) return;

	/*** Create side lines from each curve start-point in each profile ***/
	WCGeometricLine *line, *newLine;
	WCNurbsCurve *nurb, *newNurb;
	WCGeometricCurve *curve, *newCurve;
	WCVector4 pt, lower, upper;
	WSEdgeUse *tmpUse, *topUse, *leftUse, *bottomUse, *rightUse;
	std::list<WCSketchProfile*>::iterator profilesIter;
	std::list<std::list<WCSketchProfile*> >::iterator listIter;
	std::list< std::pair<WCGeometricCurve*,bool> > curveList, tmpBottom, tmpTop;
	std::list< std::list< std::pair<WCGeometricCurve*,bool> > > tmpBottoms, tmpTops;
	std::list< std::pair<WCGeometricCurve*,bool> >::iterator curveIter;
	std::vector<WSEdgeUse*> bottomUses, tmpSideUses, sideUses, topUses;
	//Loop through all profiles and extrude all surfaces
	for (listIter = this->_profiles.begin(); listIter != this->_profiles.end(); listIter++) {
		//Go through the sub-list
		for (profilesIter = (*listIter).begin(); profilesIter != (*listIter).end(); profilesIter++) {
			//Get the list of curves
			curveList = (*profilesIter)->CurveList();
			//Iterate through all curves in list
			for (curveIter = curveList.begin(); curveIter != curveList.end(); curveIter++) {
				//Determine type of curve
				curve = (*curveIter).first;
				//Try casting to line, otherwise cast to a nurbs curve
				line = dynamic_cast<WCGeometricLine*>(curve);

				/*** Duplicate curve for top ***/

				if (line) {
					//Duplicate the line
					newLine = new WCGeometricLine( *line );
					//Add the line to the lines list
					this->_lines.push_back( newLine );
					//Set newCurve
					newCurve = newLine;
				}
				//Otherwise, create duplicate of nurbs curve
				else {
					//Cast to a nurb
					nurb = dynamic_cast<WCNurbsCurve*>(curve);
					//Duplicate the curve
					newNurb= new WCNurbsCurve( *nurb );
					//Add the Nurb to the curves list
					this->_curves.push_back( newNurb );
					//Set newCurve
					newCurve = newNurb;
				}
				//Set the properties
				newCurve->Color( WCPartFeature::DefaultCurveColor );
				newCurve->Thickness( PARTFEATURE_LINE_THICKNESS );
				//Transform it to the top
				newCurve->ApplyTranslation( this->_direction * this->_firstOffset );
				//Add it to the list of top curves for trimming
				tmpTop.push_back( std::make_pair(newCurve, (*curveIter).second) );
				//Create top edge use (for topology)
				tmpUse = new WSEdgeUse();
				tmpUse->curve = newCurve;
				tmpUse->orientation = (*curveIter).second;
				topUses.push_back(tmpUse);
				//Create side edge use
				topUse = new WSEdgeUse();
				topUse->curve = newCurve;
				topUse->orientation = !(*curveIter).second;
				//Set edgeUse radials
				tmpUse->radial = topUse;
				topUse->radial = tmpUse;
				
				/*** Create Extrusion Lines ***/

				//Make sure curveList is > 1 (not a circle)
				if (curveList.size() > 1) {
					//See if curve is forward
					if ((*curveIter).second) pt = (*curveIter).first->Evaluate(0.0);
					//...Or backwards
					else pt = (*curveIter).first->Evaluate(1.0);
					//Create a line using the point
					lower = pt + this->_direction * this->_secondOffset;
					upper = pt + this->_direction * this->_firstOffset;
					newLine = new WCGeometricLine(lower, upper);
					//Set the properties for the line
					newLine->Color( WCPartFeature::DefaultCurveColor );
					newLine->Thickness( PARTFEATURE_LINE_THICKNESS );
					//Add the line to the list of all lines
					this->_lines.push_back(newLine);
					//Create side edge uses (for topology)
					rightUse = new WSEdgeUse();
					rightUse->curve = newLine;
					rightUse->orientation = true;		// -- Goes from top to bottom
					leftUse = new WSEdgeUse();
					leftUse->curve = newLine;
					leftUse->orientation = false;		// -- Goes from bottom to top
					//Set edgeUse radials
					rightUse->radial = leftUse;
					leftUse->radial = rightUse;
				}
				//If it is a circle (or ellipse)
				else {
					//Set both left and right uses to NULL
					leftUse = NULL;
					rightUse = NULL;
				}

				/*** Duplicate curve for bottom ***/

				if (line) {
					//Duplicate the line
					newLine = new WCGeometricLine( *line );
					//Add the line to the lines list
					this->_lines.push_back( newLine );
					//Set newCurve
					newCurve = newLine;
				}
				//Otherwise, create duplicate of nurbs curve
				else {
					//Cast to a nurb
					nurb = dynamic_cast<WCNurbsCurve*>(curve);
					//Duplicate the curve
					newNurb= new WCNurbsCurve( *nurb );
					//Add the Nurb to the curves list
					this->_curves.push_back( newNurb );
					//Set newCurve
					newCurve = newNurb;
				}
				//Set the properties
				newCurve->Color( WCPartFeature::DefaultCurveColor );
				newCurve->Thickness( PARTFEATURE_LINE_THICKNESS );
				//Transform it to the bottom
				newCurve->ApplyTranslation( this->_direction * this->_secondOffset );
				//Add it to the list of bottom curves for trimming
				tmpBottom.push_front( std::make_pair(newCurve, !(*curveIter).second) );
				//Create bottom edge use (for topology)
				tmpUse = new WSEdgeUse();
				tmpUse->curve = newCurve;
				tmpUse->orientation = !(*curveIter).second;
				bottomUses.push_back(tmpUse);
				//Create side edge use
				bottomUse = new WSEdgeUse();
				bottomUse->curve = newCurve;
				bottomUse->orientation = (*curveIter).second;
				//Set edgeUse radials
				tmpUse->radial = bottomUse;
				bottomUse->radial = tmpUse;

				/*** Done with creating ***/
				
				//Add side Uses in proper order
				tmpSideUses.push_back(rightUse);
				tmpSideUses.push_back(topUse);
				tmpSideUses.push_back(leftUse);
				tmpSideUses.push_back(bottomUse);
			}
			//Add tmpBottom and tmpTop to tmpBottoms and tmpTops - make sure to clear them afterwards
			tmpBottoms.push_back( tmpBottom );
			tmpBottom.clear();
			tmpTops.push_back( tmpTop );
			tmpTop.clear();
			//Copy first tmpSideUse to the back
			tmpUse = tmpSideUses.front();
			tmpSideUses.push_back(tmpUse);
			//Copy [1,N] tmpSide uses into sideUses
			for (int i=1; i<(int)tmpSideUses.size(); i++)
				sideUses.push_back(tmpSideUses.at(i));
			//Make sure to clear tmpSideUses
			tmpSideUses.clear();
		}
		//Add tmpBottom and tmpTop to bottomTrim and topTrim - make sure to clear them afterwards
		bottomTrims.push_back( tmpBottoms );
		tmpBottoms.clear();
		topTrims.push_back(tmpTops );
		tmpTops.clear();
		//Add topology edge uses to master list
		topoTopEUs.push_back(topUses);
		topoSideEUs.push_back(sideUses);
		topoBottomEUs.push_back(bottomUses);
		//Clear the topo lists
		topUses.clear();
		sideUses.clear();
		bottomUses.clear();
	}
}


/*** GenerateSideSurfaces Algorithm ***
 * This algorithm generates all of the "vertical" surfaces of the extrusion.  They are linear extrusions of the base profile curves.
 *	The algorithm just steps through curve list, in normal list order, and extrudes each curve along the direction for offset distance.
 *	The only complication of the method is that curves must face either inward or outward based on if the profile is exterior or
 *	interior and whether the curve is same oriented or opposite.
 ***/
void WCPartPad::GenerateSideSurfaces(std::list<std::list<WSFaceUse*> > &topoFaceUses) {
	//Check to make sure there is a depth
	if (this->_firstOffset - this->_secondOffset < 0.01) return;
	
	//Create a surface from each curve in each profile
	bool exterior;
	WCGeometricCurve* curve;
	WCNurbsSurface* surface;
	WSFaceUse *tmpUse;
	std::list<WCSketchProfile*>::iterator profilesIter;
	std::list<std::list<WCSketchProfile*> >::iterator listIter;
	std::list< std::pair<WCGeometricCurve*,bool> > curveList;
	std::list< std::pair<WCGeometricCurve*,bool> >::iterator curveIter;
	//Loop through all profiles and extrude all surfaces
	for (listIter = this->_profiles.begin(); listIter != this->_profiles.end(); listIter++) {
		//Mark as exterior
		exterior = true;
		//Create list of surfaces in topological order
		std::list<WSFaceUse*> topoList;
		//Go through the sub-list
		for (profilesIter = (*listIter).begin(); profilesIter != (*listIter).end(); profilesIter++) {
			//Get the list of curves
			curveList = (*profilesIter)->CurveList();
			//Iterate through all curves in list
			for (curveIter = curveList.begin(); curveIter != curveList.end(); curveIter++) {
				curve = (*curveIter).first;
				//First profile is exterior, extrude
				if (exterior) {
					surface = WCNurbsSurface::ExtrudeCurve(this->_document->Scene()->GeometryContext(), curve, this->_direction,
						this->_firstOffset, this->_secondOffset, !(*curveIter).second);
				}
				//Interior profiles...
				else {
					//Extrude each curve into a surface
					surface = WCNurbsSurface::ExtrudeCurve(this->_document->Scene()->GeometryContext(), curve, this->_direction,
						this->_firstOffset, this->_secondOffset, (*curveIter).second);
				}
				//Set color and render program
				surface->Color( WCPartFeature::DefaultSurfaceColor );
				surface->RenderProgram(WCPartFeature::DefaultSurfaceRenderer);
				//Add surface into surfaces list
				this->_surfaces.push_back(surface);
				//Create surface use (for topology)
				tmpUse = new WSFaceUse();
				tmpUse->surface = surface;
				tmpUse->orientation = true;
				topoList.push_back(tmpUse);
			}
			//Mark as interior
			exterior = false;
		}
		//Add the topoList to the master
		topoFaceUses.push_back(topoList);
	}
}


/*** GenerateTopBottom ***
 * The GenerateTopBottom algorithm generates just the top and bottom surfaces.  These are trim surfaces, so two components must
 * be created:
 *		1) The underlying NURBS surface control points and knot values must be calculated
 *		2) The set of trim profiles, different for top and bottom, are passed into surface creation
 ***/
void WCPartPad::GenerateTopBottom(std::list<std::list<WCTrimProfile> > &topTrims, std::list<std::list<WCTrimProfile> > &bottomTrims,
	std::list<std::list<WSFaceUse*> > &topoFaceUses) {
	GLuint prog = this->_part->Scene()->ProgramFromName("scn_basiclight_trim");
	std::vector<WCVector4> controlPoints;
	WCVector4 tmpPt1;
	WSFaceUse *tmpUse;
	std::list< std::list<WCTrimProfile> >::iterator topTrim = topTrims.begin();
	std::list< std::list<WCTrimProfile> >::iterator bottomTrim = bottomTrims.begin();
	std::list< std::list<WSFaceUse*> >::iterator topoIter = topoFaceUses.begin();

	//Loop through list of profile lists
	std::list<std::list<WCSketchProfile*> >::iterator listIter;
	std::list<WCVector4> boundaryList;
	for (listIter = this->_profiles.begin(); listIter != this->_profiles.end(); listIter++) {
		//Build list of boundary points from original exterior profile
		(*listIter).front()->BoundaryList(false, boundaryList);
		//Find minimum bounding rectangle for bounding points
		WCPartPlane *refPlane = this->_profiles.front().front()->Sketch()->ReferencePlane();
		std::list<WCVector4> baseCorners = MinimumBoundingRectangle(boundaryList, refPlane->InverseTransformMatrix(), refPlane->TransformMatrix());
		WCVector4 corners[4];
		corners[0] = baseCorners.front(); baseCorners.pop_front();		// Lower-left
		corners[1] = baseCorners.front(); baseCorners.pop_front();		// Upper-left
		corners[2] = baseCorners.front(); baseCorners.pop_front();		// Upper-right
		corners[3] = baseCorners.front(); baseCorners.pop_front();		// Lower-right
		//Clear the boundary list
		boundaryList.clear();

		/*** Bottom Trim Surface ***/

		//Now use the base corners to define the bottom trim surface
		controlPoints.clear();
		//First point (uses upper-left in base)
		tmpPt1 = corners[1] + this->_direction * this->_secondOffset;
		controlPoints.push_back(tmpPt1);
		//Second point (uses upper-right in base)
		tmpPt1 = corners[2] + this->_direction * this->_secondOffset;
		controlPoints.push_back(tmpPt1);
		//Third point (uses lower-left in base)
		tmpPt1 = corners[0] + this->_direction * this->_secondOffset;
		controlPoints.push_back(tmpPt1);
		//Fourth point (uses lower-right in base)
		tmpPt1 = corners[3] + this->_direction * this->_secondOffset;
		controlPoints.push_back(tmpPt1);
		//Create the bottom surface
		WCTrimmedNurbsSurface *bottom = new WCTrimmedNurbsSurface(this->_document->Scene()->GeometryContext(), 
			*bottomTrim, 1, 1, 2, 2, controlPoints, WCNurbsMode::Default(), WCNurbsMode::Default());
		bottom->Color( WCPartFeature::DefaultSurfaceColor );
		bottom->RenderProgram(prog);
		//Add to master list
		this->_surfaces.push_back(bottom);
		//Create Face Use and add to the topoList (for Topology)
		tmpUse = new WSFaceUse();
		tmpUse->surface = bottom;
		tmpUse->orientation = true;
		(*topoIter).push_back(tmpUse);
			
		/*** Top Trim Surface ***/
	
		 //Now use the base corners to define the top trim surface
		controlPoints.clear();
		//First point (uses lower-left in base)
		tmpPt1 = corners[0] + this->_direction * this->_firstOffset;
		controlPoints.push_back(tmpPt1);
		//Second point (uses lower-right in base)
		tmpPt1 = corners[3] + this->_direction * this->_firstOffset;
		controlPoints.push_back(tmpPt1);
		//Third point (uses upper-left in base)
		tmpPt1 = corners[1] + this->_direction * this->_firstOffset;
		controlPoints.push_back(tmpPt1);
		//Fourth point (uses upper-right in base)
		tmpPt1 = corners[2] + this->_direction * this->_firstOffset;
		controlPoints.push_back(tmpPt1);
		//Create the top surface
		WCTrimmedNurbsSurface *top = new WCTrimmedNurbsSurface(this->_document->Scene()->GeometryContext(), 
			*topTrim, 1, 1, 2, 2, controlPoints, WCNurbsMode::Default(), WCNurbsMode::Default());
		top->Color( WCPartFeature::DefaultSurfaceColor );
		top->RenderProgram(prog);
		//Add to the master list
		this->_surfaces.push_back(top);
		//Create face use and add to the topo list (for Topology)
		tmpUse = new WSFaceUse();
		tmpUse->surface = top;
		tmpUse->orientation = true;
		(*topoIter).push_front(tmpUse);

		/*** End Top-Bottom Surface Generation ***/
		
		//Make sure to increment topTrim and bottomTrim iterators
		bottomTrim++;
		topTrim++;
		//Increment topoIter
		topoIter++;
	}
}


void _ConfigureVertexUses(void) {
	//Do nothing for now...
}


void _ConfigureTopEdgeUses(WSLoopUse *loopUse, WPUInt &index, const WPUInt &count, std::vector<WSEdgeUse*> &edgeUseList, 
	std::list<WSVertexUse*> &vuList) {
	WSEdgeUse *edgeUse, *prevEU = NULL, *firstEU = NULL, *lastEU = NULL;
	//Loop through edgeUses in top list
	for (int i=(int)index; i<(int)(index+count); i++) {
		//Get the edgeUse and set the loop
		edgeUse = edgeUseList.at(i);
		edgeUse->loop = loopUse;
		//Set the firstEU value if necessary
		if (!firstEU) firstEU = edgeUse;
		//Set the lastEU value
		lastEU = edgeUse;
		//Set the ccw and cw values
		edgeUse->ccw = prevEU;
		if (prevEU) prevEU->cw = edgeUse;
		//Set the value of prevEU
		prevEU = edgeUse;
	}
	//Clean up dangling edgeUse and loopUse pointers
	firstEU->ccw = lastEU;
	lastEU->cw = firstEU;
	loopUse->edgeUses = firstEU;
	//Increment index
	index += count;
}


void _ConfigureSideEdgeUses(WSLoopUse *loopUse, WPUInt &index, const WPUInt &outerFaces, std::vector<WSEdgeUse*> &edgeUseList, 
	std::list<WSVertexUse*> &vuTopList, std::list<WSVertexUse*> &vuBottomList) {
	WSEdgeUse *top, *left, *bottom, *right;
	//Get the values for all 4 edgeUses
	top = edgeUseList.at(index++);
	right = edgeUseList.at(index++);
	bottom = edgeUseList.at(index++);
	left = edgeUseList.at(index++);
	//Set the loop value for top and bottom
	top->loop = loopUse;
	bottom->loop = loopUse;
	
	//Check to see if this is part of a circle or ellipse
	if (!left || !right) {
		//Just have top point to bottom and bottom to top
		top->cw = bottom;
		top->ccw = bottom;
		bottom->cw = top;
		bottom->ccw =top;
	}
	//Setup all of the pointers
	else {
		//Check to see if this is the inside set of edgeUses
		if (index > outerFaces * 4) {
			//Make the list pointers correct (winding backwards and invert orientation
			top->cw = left;
			top->ccw = right;
			left->cw = bottom;
			left->ccw = top;
			bottom->cw = right;
			bottom->ccw = left;
			right->cw = top;
			right->ccw = bottom;
			//Invert the orientation flags
			top->orientation = !top->orientation;
			left->orientation = !left->orientation;
			bottom->orientation = !bottom->orientation;
			right->orientation = !right->orientation;
		}
		//Must be outside faces
		else {
			//Make the list pointers correct
			top->cw = right;
			top->ccw = left;
			right->cw = bottom;
			right->ccw = top;
			bottom->cw = left;
			bottom->ccw = right;
			left->cw = top;
			left->ccw = bottom;
		}
		//Set the loop value for left and right
		left->loop = loopUse;
		right->loop = loopUse;
	}
	//Always use top as the link
	loopUse->edgeUses = top;
}


void _ConfigureBottomEdgeUses(WSLoopUse *loopUse, WPUInt &index, const WPUInt &count, std::vector<WSEdgeUse*> &edgeUseList, 
	std::list<WSVertexUse*> &vuList) {
	WSEdgeUse *edgeUse, *prevEU = NULL, *firstEU = NULL, *lastEU = NULL;
	//Loop through edgeUses in top list
	for (int i=(int)index; i<(int)(index+count); i++) {
		//Get the edgeUse and set the loop
		edgeUse = edgeUseList.at(i);
		edgeUse->loop = loopUse;
		//Set the firstEU value if necessary
		if (!firstEU) firstEU = edgeUse;
		//Set the lastEU value
		lastEU = edgeUse;
		//Set the ccw and cw values
		edgeUse->cw = prevEU;
		if (prevEU) prevEU->ccw = edgeUse;
		//Set the value of prevEU
		prevEU = edgeUse;
	}
	//Clean up dangling edgeUse and loopUse pointers
	firstEU->cw = lastEU;
	lastEU->ccw = firstEU;
	loopUse->edgeUses = firstEU;
	//Increment index
	index += count;
}


void _ConfigureFaceUses(std::list<WCSketchProfile*> &profileList, WSTopologyShell *shell, std::list<WSFaceUse*> &faceList,
	std::vector<WSEdgeUse*> &bottomEUs, std::vector<WSEdgeUse*> &sideEUs, std::vector<WSEdgeUse*> &topEUs,
	std::list<WSVertexUse*> &bottomVUs, std::list<WSVertexUse*> &topVUs) {
	WSFaceUse *faceUse, *prevFU = NULL, *firstFU = NULL, *lastFU = NULL;
	WPUInt sideIndex = 0;
	//Loop through all of the faces for the shell
	for (std::list<WSFaceUse*>::iterator fuIter = faceList.begin(); fuIter != faceList.end(); fuIter++) {
		faceUse = *fuIter;
		//Set the shell value for the faceUse
		faceUse->shell = shell;
		//Set the firstFU value if necessary
		if (!firstFU) firstFU = faceUse;
		//Set the lastFU value
		lastFU = faceUse;
		//Set the previous and next values
		faceUse->prev = prevFU;
		if (prevFU) prevFU->next = faceUse;
		//Set the value of prevFU
		prevFU = faceUse;
		
		/*** Create all of the LoopUses ***/

		// Top and bottom faces may have multiple loopUses
		WSLoopUse *loopUse, *prevLU = NULL, *firstLU = NULL, *lastLU = NULL;
		if ((faceUse == faceList.front()) || (faceUse == faceList.back())) {
			WPUInt index = 0;
			//Add profile count loops
			for (std::list<WCSketchProfile*>::iterator profileIter = profileList.begin(); profileIter != profileList.end(); profileIter++) {
				//Create a new loopUse and attach it to the faceUse
				loopUse = new WSLoopUse();
				loopUse->face = faceUse;
				//Set the firstLU value if necessary
				if (!firstLU) firstLU = loopUse;
				//Set the last LU value
				lastLU = loopUse;
				//Set the previous and next values
				loopUse->prev = prevLU;
				if (prevLU) prevLU->next = loopUse;
				//Set the value of prevLU
				prevLU = loopUse;
				
				/*** Configure dependent edgeUses ***/
				
				//If top face, pass in top edgeUses and top vertexUses
				if (faceUse == faceList.front())
					_ConfigureTopEdgeUses(loopUse, index, (*profileIter)->CurveCount(), topEUs, topVUs);
				//Must be bottom edgeUses
				else
					_ConfigureBottomEdgeUses(loopUse, index, (*profileIter)->CurveCount(), bottomEUs, bottomVUs);

			}
			//Clean up dangling loopUse and faceUse pointers
			firstLU->prev = lastLU;
			lastLU->next = firstLU;
			faceUse->loopUses = firstLU;
		}
		//Otherwise side faces only have one loop
		else {
			//Create a new loopUse and attach it to the faceUse
			loopUse = new WSLoopUse();
			loopUse->face = faceUse;
			loopUse->next = loopUse;
			loopUse->prev = loopUse;
			faceUse->loopUses = loopUse;

			/*** Configure dependent edgeUses ***/
			_ConfigureSideEdgeUses(loopUse, sideIndex, profileList.front()->CurveCount(),  sideEUs, topVUs, bottomVUs);
		}
	}
	//Clean up dangling faceUse and shell pointers
	firstFU->prev = lastFU;
	lastFU->next = firstFU;
	shell->faceUses = firstFU;
}


WCTopologyModel* WCPartPad::GenerateTopology(std::list<std::list<WSVertexUse*> > &topoBottomPoints, 
	std::list<std::list<WSVertexUse*> > &topoTopPoints, std::list<std::vector<WSEdgeUse*> > &topoBottomEUs,
	std::list<std::vector<WSEdgeUse*> > &topoSideEUs, std::list<std::vector<WSEdgeUse*> > &topoTopEUs,
	std::list<std::list<WSFaceUse*> > &topoFaceUses) {
	//Create one model
	WCTopologyModel *model = new WCTopologyModel();
	//Create one shell per item in main profile list
	WSTopologyShell *shell;

	//Ready all of the Use lists
	std::list<std::list<WSFaceUse*> >::iterator faceListIter = topoFaceUses.begin();
	std::list<std::vector<WSEdgeUse*> >::iterator	euBottomListIter = topoBottomEUs.begin();
	std::list<std::vector<WSEdgeUse*> >::iterator	euSideListIter = topoSideEUs.begin();
	std::list<std::vector<WSEdgeUse*> >::iterator	euTopListIter = topoTopEUs.begin();
	std::list<std::list<WSVertexUse*> >::iterator vuBottomListIter = topoBottomPoints.begin();
	std::list<std::list<WSVertexUse*> >::iterator vuTopListIter = topoTopPoints.begin();

	std::list<std::list<WCSketchProfile*> >::iterator profileListIter;
	//Loop through each separate shell
	for (profileListIter = this->_profiles.begin(); profileListIter != this->_profiles.end(); profileListIter++) {
		//Create a shell
		shell = new WSTopologyShell();
		//Set shell values
		shell->model = model;
		//Add shell to model
		model->AddShell(shell);
		//Configure all of the FaceUses (and below) for this shell
		_ConfigureFaceUses(*profileListIter, shell, *faceListIter, *euBottomListIter, *euSideListIter,
						   *euTopListIter, *vuBottomListIter, *vuTopListIter);

		//Make sure to increment to the next lists of Uses
		faceListIter++;
		euBottomListIter++;
		euSideListIter++;
		euTopListIter++;
		vuBottomListIter++;
		vuTopListIter++;
	}
	//Return the model
	return model;
}


void WCPartPad::Initialize(void) {
	//Check feature name
	if (this->_name == "") this->_name = this->_part->GenerateFeatureName(this);
	//Create event handler
	this->_controller = new WCPartPadController(this);
	//Create tree element
	WSTexture* padIcon = this->_document->Scene()->TextureManager()->TextureFromName("pad32");
	this->_treeElement = new WCTreeElement(this->_document->TreeView(), this->_name, this->_controller, padIcon);

	//Add profiles as children
	std::list<WCSketchProfile*>::iterator listIter;
	std::list<std::list<WCSketchProfile*> >::iterator profListIter;
	for (profListIter = this->_profiles.begin(); profListIter != this->_profiles.end(); profListIter++) {
		//Go through the sub-list
		for (listIter = (*profListIter).begin(); listIter != (*profListIter).end(); listIter++) {
			this->_treeElement->AddLastChild( (*listIter)->TreeElement() );
		}
	}
	this->_treeElement->IsOpen(false);
	//Add tree view element
	this->_creator->TreeElement()->AddLastChild(this->_treeElement);
	
	//Add the pad to the part (true for visualize and select)
	this->_part->AddFeature(this, false);

	//Add all points into the part
	std::list<WCGeometricPoint*>::iterator pointIter;
	for (pointIter = this->_points.begin(); pointIter != this->_points.end(); pointIter++)
		this->_part->AddPoint( *pointIter, this->_controller);
	//Add all lines into the part
	std::list<WCGeometricLine*>::iterator lineIter;
	for (lineIter = this->_lines.begin(); lineIter != this->_lines.end(); lineIter++)
		this->_part->AddLine( *lineIter, this->_controller);
	//Add all curves into the part	
	std::list<WCNurbsCurve*>::iterator curveIter;
	for (curveIter = this->_curves.begin(); curveIter != this->_curves.end(); curveIter++)
		this->_part->AddCurve( *curveIter, this->_controller);
	//Add all surfaces into the part
	std::list<WCNurbsSurface*>::iterator surfIter;
	for (surfIter = this->_surfaces.begin(); surfIter != this->_surfaces.end(); surfIter++)
		this->_part->AddSurface( *surfIter, this->_controller);
}


void WCPartPad::DetermineDirection(void) {
	WCPartPlane *refPlane = this->_profiles.front().front()->Sketch()->ReferencePlane();
	//Determine direction vector
	WCVector4 dir(0.0, 0.0, 1.0, 0.0);
	dir = refPlane->TransformMatrix() * dir;
	dir.Normalize(true);
	this->_direction = dir;
	//Check offsets
	//Check to make sure there is a depth
	if (this->_firstOffset - this->_secondOffset < 0.01) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCPartPad::DetermineDirection - Offset differences too small.");
	}
}


/***********************************************~***************************************************/


WCPartPad::WCPartPad(WCPartBody *body, const std::string &name, std::list<std::list<WCSketchProfile*> > &profiles, 
	const bool &reverseDir, const WCPartPadType &firstType, const WCPartPadType &secondType, const WPFloat &firstOffset, const WPFloat &secondOffset) : 
	::WCPartFeature(body, name), _profiles(profiles), _isReversed(reverseDir), _firstType(firstType), _secondType(secondType),
	_firstOffset(firstOffset), _secondOffset(secondOffset), _points(), _lines(), _curves(), _surfaces(), _topologyModel(NULL) {
	//Determine direction and offsets
	this->DetermineDirection();

	//Lots of lists to hold things
	std::list<std::list<WCTrimProfile> > topTrims, bottomTrims;		
	std::list<std::list<WSVertexUse*> >	topoBottomPoints, topoTopPoints;
	std::list<std::vector<WSEdgeUse*> >	topoBottomEUs, topoSideEUs, topoTopEUs;
	std::list<std::list<WSFaceUse*> > topoFaceUses;
	//Generate the points, curves, and surfaces
	this->GeneratePoints(topoBottomPoints, topoTopPoints);
	this->GenerateCurves(topTrims, bottomTrims, topoBottomEUs, topoSideEUs, topoTopEUs);
	this->GenerateSideSurfaces(topoFaceUses);
	this->GenerateTopBottom(topTrims, bottomTrims, topoFaceUses);
	this->_topologyModel = this->GenerateTopology(topoBottomPoints, topoTopPoints, topoBottomEUs, topoSideEUs, topoTopEUs, topoFaceUses);
	//Now union the pad model (both geometric and topological) with the part model
	this->_part->TopologyModel()->Union(this->_topologyModel);
	//Finish initialization
	this->Initialize();
}


WCPartPad::WCPartPad(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCPartFeature( WCSerializeableObject::ElementFromName(element,"PartFeature"), dictionary),
	_profiles(), _isReversed(false), _firstType(WCPartPadType::Dimension()), _secondType(WCPartPadType::Dimension()),
	_firstOffset(0.0), _secondOffset(0.0), _points(), _lines(), _curves(), _surfaces(), _topologyModel(NULL) {
	//Make sure element if not null
	if (element == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCPartPad::WCPartPad - NULL Element passed.");
		//throw error
		return;
	}
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

	//Restore isReversed
	this->_isReversed = WCSerializeableObject::GetBoolAttrib(element, "reversed");
	//Add FirstType, SecondType
	this->_firstType.FromElement(element, "firstType");
	this->_secondType.FromElement(element, "secondType");
	//Add FirstOffset, SecondOffset
	this->_firstOffset = WCSerializeableObject::GetFloatAttrib(element, "firstOffset");
	this->_secondOffset = WCSerializeableObject::GetFloatAttrib(element, "secondOffset");
	//Add Direction
	this->_direction.FromElement( WCSerializeableObject::ElementFromName(element,"Direction") );

	//Restore sketch profiles
	XMLCh* xmlString = xercesc::XMLString::transcode("Profiles");
	xercesc::DOMNodeList *profileList = element->getElementsByTagName(xmlString)->item(0)->getChildNodes();
	xercesc::XMLString::release(&xmlString);
	xercesc::DOMNode *tmpNode;
	xercesc::DOMElement *tmpElement;
	std::list<WCSketchProfile*> tmpList;
	//Loop through all features
	for (WPUInt index=0; index < profileList->getLength(); index++) {
		//Get the indexed node
		tmpNode = profileList->item(index);
		//Make sure node is element
		if (tmpNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
			//Cast node to element
			tmpElement = (xercesc::DOMElement*)tmpNode;
			WCSketchProfile* profile = (WCSketchProfile*)WCSerializeableObject::GetGUIDAttrib(tmpElement, "address", dictionary);
			bool outerProfile = WCSerializeableObject::GetBoolAttrib(tmpElement, "external");
			//If is an outer profile, add list of profiles to master list (make sure to skip first outer
			if (outerProfile && !tmpList.empty()) {
				this->_profiles.push_back(tmpList);
				//Make sure to clear the temp list
				tmpList.clear();
			}
			//Add profile to temp list
			tmpList.push_back(profile);
		}
	}
	//Add last list if not empty
	if (!tmpList.empty()) this->_profiles.push_back(tmpList);

	//Restore Points
	xmlString = xercesc::XMLString::transcode("Points");
	xercesc::DOMNodeList *nodeList = element->getElementsByTagName(xmlString)->item(0)->getChildNodes();
	xercesc::XMLString::release(&xmlString);
	//Loop through all nodes
	for (WPUInt index=0; index < nodeList->getLength(); index++) {
		//Get the indexed node
		tmpNode = nodeList->item(index);
		//Make sure node is element
		if (tmpNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
			//Cast node to element
			tmpElement = (xercesc::DOMElement*)tmpNode;
			WCGeometricPoint* point = new WCGeometricPoint(tmpElement, dictionary);
			//Add profile to temp list
			this->_points.push_back(point);
		}
	}

	//Restore Lines
	xmlString = xercesc::XMLString::transcode("Lines");
	nodeList = element->getElementsByTagName(xmlString)->item(0)->getChildNodes();
	xercesc::XMLString::release(&xmlString);
	//Loop through all nodes
	for (WPUInt index=0; index < nodeList->getLength(); index++) {
		//Get the indexed node
		tmpNode = nodeList->item(index);
		//Make sure node is element
		if (tmpNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
			//Cast node to element
			tmpElement = (xercesc::DOMElement*)tmpNode;
			WCGeometricLine* line = new WCGeometricLine(tmpElement, dictionary);
			//Add line to temp list
			this->_lines.push_back(line);
		}
	}

	//Restore Curves
	xmlString = xercesc::XMLString::transcode("Curves");
	nodeList = element->getElementsByTagName(xmlString)->item(0)->getChildNodes();
	xercesc::XMLString::release(&xmlString);
	//Loop through all nodes
	for (WPUInt index=0; index < nodeList->getLength(); index++) {
		//Get the indexed node
		tmpNode = nodeList->item(index);
		//Make sure node is element
		if (tmpNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
			//Cast node to element
			tmpElement = (xercesc::DOMElement*)tmpNode;
			WCNurbsCurve* curve = new WCNurbsCurve(tmpElement, dictionary);
			//Add curve to temp list
			this->_curves.push_back(curve);
		}
	}

	//Restore Surfaces
	xmlString = xercesc::XMLString::transcode("Surfaces");
	nodeList = element->getElementsByTagName(xmlString)->item(0)->getChildNodes();
	xercesc::XMLString::release(&xmlString);
	WCNurbsSurface *surface;
	//Loop through all nodes
	for (WPUInt index=0; index < nodeList->getLength(); index++) {
		//Get the indexed node
		tmpNode = nodeList->item(index);
		//Make sure node is element
		if (tmpNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
			//Cast node to element
			tmpElement = (xercesc::DOMElement*)tmpNode;
			const XMLCh* elementName = tmpElement->getTagName();
			char *str = xercesc::XMLString::transcode(elementName);
			std::string value(str);
			delete str;
			if (value == "TrimmedNURBSSurface") surface = new WCTrimmedNurbsSurface(tmpElement, dictionary);
			else if (value == "NurbsSurface") surface = new WCNurbsSurface(tmpElement, dictionary);
			else surface = NULL;
			//Add surface to temp list (as appropriate)
			if (surface) this->_surfaces.push_back(surface);
		}
	}

	//Restore topology model
	this->_topologyModel = new WCTopologyModel( WCSerializeableObject::ElementFromName(element,"TopologyModel"), dictionary );
	this->_part->TopologyModel()->Union(this->_topologyModel);
	//Finish initialization
	this->Initialize();
}


WCPartPad::~WCPartPad() {
	//Remove from the part
	if (!this->_part->RemoveFeature(this, true)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCPartShaft::~WCPartShaft - Problem removing feature from part.");	
	}
	//Remove all points into the part
	std::list<WCGeometricPoint*>::iterator pointIter;
	for (pointIter = this->_points.begin(); pointIter != this->_points.end(); pointIter++)
		this->_part->RemovePoint( *pointIter);
	//Remove all lines from part
	std::list<WCGeometricLine*>::iterator lineIter;
	for (lineIter = this->_lines.begin(); lineIter != this->_lines.end(); lineIter++)
		this->_part->RemoveLine( *lineIter);
	//Remove all curves from part	
	std::list<WCNurbsCurve*>::iterator curveIter;
	for (curveIter = this->_curves.begin(); curveIter != this->_curves.end(); curveIter++)
		this->_part->RemoveCurve( *curveIter);
	//Remove all surfaces from part
	std::list<WCNurbsSurface*>::iterator surfIter;
	for (surfIter = this->_surfaces.begin(); surfIter != this->_surfaces.end(); surfIter++)
		this->_part->RemoveSurface( *surfIter);
}


void WCPartPad::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
	//Not yet implemented
	//...
}


bool WCPartPad::Regenerate(void) {
	//Not yet implemented
	//...
	return false;
}


void WCPartPad::OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) {
	//Change the color of all points
	std::list<WCGeometricPoint*>::iterator pointIter;
	for (pointIter = this->_points.begin(); pointIter != this->_points.end(); pointIter++)
		(*pointIter)->Color( WCPartFeature::SelectedColor );
	//Change the color of all lines
	std::list<WCGeometricLine*>::iterator lineIter;
	for (lineIter = this->_lines.begin(); lineIter != this->_lines.end(); lineIter++)
		(*lineIter)->Color( WCPartFeature::SelectedColor );
	//Change the color of all curves
	std::list<WCNurbsCurve*>::iterator curveIter;
	for (curveIter = this->_curves.begin(); curveIter != this->_curves.end(); curveIter++)
		(*curveIter)->Color( WCPartFeature::SelectedColor );
	//Change the color of all surfaces
	std::list<WCNurbsSurface*>::iterator surfIter;
	for (surfIter = this->_surfaces.begin(); surfIter != this->_surfaces.end(); surfIter++)
		(*surfIter)->Color( WCPartFeature::SelectedColor );
	//Mark as selected
	this->_isSelected = true;
}


void WCPartPad::OnDeselection(const bool fromManager) {
	//Change the color of all points
	std::list<WCGeometricPoint*>::iterator pointIter;
	for (pointIter = this->_points.begin(); pointIter != this->_points.end(); pointIter++)
		(*pointIter)->Color( WCPartFeature::DefaultPointColor );
	//Change the color of all lines
	std::list<WCGeometricLine*>::iterator lineIter;
	for (lineIter = this->_lines.begin(); lineIter != this->_lines.end(); lineIter++)
		(*lineIter)->Color( WCPartFeature::DefaultCurveColor );
	//Change the color of all curves
	std::list<WCNurbsCurve*>::iterator curveIter;
	for (curveIter = this->_curves.begin(); curveIter != this->_curves.end(); curveIter++)
		(*curveIter)->Color( WCPartFeature::DefaultCurveColor );
	//Change the color of all surfaces
	std::list<WCNurbsSurface*>::iterator surfIter;
	for (surfIter = this->_surfaces.begin(); surfIter != this->_surfaces.end(); surfIter++)
		(*surfIter)->Color( WCPartFeature::DefaultSurfaceColor );
	//Mark as not selected
	this->_isSelected = false;
}


xercesc::DOMElement* WCPartPad::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create the base element for the object
	XMLCh* xmlString = xercesc::XMLString::transcode("PartPad");
	xercesc::DOMElement* element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Include the part feature element
	xercesc::DOMElement* featureElement = this->WCPartFeature::Serialize(document, dictionary);
	element->appendChild(featureElement);
	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);
	//Add IsReversed attribute
	WCSerializeableObject::AddBoolAttrib(element, "reversed", this->_isReversed);
	//Add FirstType, SecondType
	this->_firstType.ToElement(element, "firstType");
	this->_secondType.ToElement(element, "secondType");
	//Add FirstOffset, SecondOffset
	WCSerializeableObject::AddFloatAttrib(element, "firstOffset", this->_firstOffset);
	WCSerializeableObject::AddFloatAttrib(element, "secondOffset", this->_secondOffset);
	//Add Direction
	this->_direction.ToElement(element, "Direction");

	//Loop through all sketch profiles and add them
	xmlString = xercesc::XMLString::transcode("Profiles");
	XMLCh* profileStr = xercesc::XMLString::transcode("Profile");
	xercesc::DOMElement *profiles = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	element->appendChild(profiles);
	xercesc::DOMElement *profileElem;
	std::list<std::list<WCSketchProfile*> >::iterator profilesIter;
	std::list<WCSketchProfile*>::iterator profileIter;
	for (profilesIter = this->_profiles.begin(); profilesIter != this->_profiles.end(); profilesIter++) {
		for (profileIter = (*profilesIter).begin(); profileIter != (*profilesIter).end(); profileIter++) {
			//Create an entry for each profile
			profileElem = document->createElement(profileStr);
			//Add profile address
			WCSerializeableObject::AddGUIDAttrib(profileElem, "address", *profileIter, dictionary);
			//Add profile int/ext flag
			WCSerializeableObject::AddBoolAttrib(profileElem, "external", (*profileIter) == (*profilesIter).front());
			//Add profile to list
			profiles->appendChild(profileElem);
		}
	}
	xercesc::XMLString::release(&profileStr);
	
	//Add TopologyModel
	xercesc::DOMElement* topologyElement = this->_topologyModel->Serialize(document, dictionary);
	element->appendChild(topologyElement);

	//Add Geometric Points
	xmlString = xercesc::XMLString::transcode("Points");
	xercesc::DOMElement *points = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	element->appendChild(points);
	std::list<WCGeometricPoint*>::iterator pointIter;
	for (pointIter = this->_points.begin(); pointIter != this->_points.end(); pointIter++) {
		xercesc::DOMElement *pointElement = (*pointIter)->Serialize(document, dictionary);
		points->appendChild(pointElement);
	}

	//Add Geometric Lines
	xmlString = xercesc::XMLString::transcode("Lines");
	xercesc::DOMElement *lines = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	element->appendChild(lines);
	std::list<WCGeometricLine*>::iterator lineIter;
	for (lineIter = this->_lines.begin(); lineIter != this->_lines.end(); lineIter++) {
		xercesc::DOMElement *lineElement = (*lineIter)->Serialize(document, dictionary);
		lines->appendChild(lineElement);
	}

	//Add Geometric Curves
	xmlString = xercesc::XMLString::transcode("Curves");
	xercesc::DOMElement *curves = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	element->appendChild(curves);
	std::list<WCNurbsCurve*>::iterator curveIter;
	for (curveIter = this->_curves.begin(); curveIter != this->_curves.end(); curveIter++) {
		xercesc::DOMElement *curveElement = (*curveIter)->Serialize(document, dictionary);
		curves->appendChild(curveElement);
	}

	//Add Geometric Surfaces
	xmlString = xercesc::XMLString::transcode("Surfaces");
	xercesc::DOMElement *surfaces = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	element->appendChild(surfaces);
	std::list<WCNurbsSurface*>::iterator surfaceIter;
	for (surfaceIter = this->_surfaces.begin(); surfaceIter != this->_surfaces.end(); surfaceIter++) {
		xercesc::DOMElement *surfaceElement = (*surfaceIter)->Serialize(document, dictionary);
		surfaces->appendChild(surfaceElement);
	}

	//TrimSurfaces...

	//Return the primary element
	return element;
}


/***********************************************~***************************************************/


WCDrawingMode* WCPartPad::ModeCreate(WCPartWorkbench *wb) {
	//Create a new drawing mode
	return new WCModePartPadCreate(wb);
}


WCActionPartPadCreate* WCPartPad::ActionCreate(WCPartBody *body, const std::string &padName,
	std::list<std::list<WCSketchProfile*> > &profiles, const bool &reversed, const WCPartPadType &firstType, 
	const WCPartPadType &secondType, const WPFloat &firstOffset, const WPFloat &secondOffset) {
	//Create a new pad create action
	return new WCActionPartPadCreate(body, padName, profiles, reversed, firstType, secondType, firstOffset, secondOffset);
}


/***********************************************~***************************************************/


std::ostream& __WILDCAT_NAMESPACE__::operator<<(std::ostream& out, const WCPartPad &pad) {
	out << "Part Pad(" << &pad << ")\n";
	//Print out geometry
	//...
	//Print the topology model
	out << *(pad._topologyModel);
	return out;
}


/***********************************************~***************************************************/

