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
void WCPartPad::GeneratePoints(void) {
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
		this->_topoBottomPoints.push_back(lowerPoints);
		this->_topoTopPoints.push_back(upperPoints);
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
void WCPartPad::GenerateCurves(void) {
	//Check to make sure there is a depth
	if (this->_firstOffset - this->_secondOffset < 0.01) return;

	/*** Create side lines from each curve start-point in each profile ***/
	WCGeometricLine *line, *newLine;
	WCNurbsCurve *nurb, *newNurb;
	WCGeometricCurve *curve, *newCurve;
	WCVector4 pt, lower, upper;
	WSEdgeUse *tmpUse1, *tmpUse2;
	std::list<WCSketchProfile*>::iterator profilesIter;
	std::list<std::list<WCSketchProfile*> >::iterator listIter;
	std::list< std::pair<WCGeometricCurve*,bool> > curveList, tmpBottom, tmpTop;
	std::list< std::list< std::pair<WCGeometricCurve*,bool> > > tmpBottoms, tmpTops;
	std::list< std::pair<WCGeometricCurve*,bool> >::iterator curveIter;
	std::list<WSEdgeUse*> bottomCurves, sideCurves, topCurves;
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
				tmpUse1 = new WSEdgeUse();
				tmpUse1->curve = newCurve;
				tmpUse1->orientation = (*curveIter).second;
				topCurves.push_back(tmpUse1);
				//Create side edge use
				tmpUse2 = new WSEdgeUse();
				tmpUse2->curve = newCurve;
				tmpUse2->orientation = !(*curveIter).second;
				sideCurves.push_back(tmpUse2);
				//Set edgeUse radials
				tmpUse1->radial = tmpUse2;
				tmpUse2->radial = tmpUse1;
				
				/*** Create Extrusion Lines ***/

				//Make sure curveList is > 1
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
					tmpUse1 = new WSEdgeUse();
					tmpUse1->curve = newLine;
					tmpUse1->orientation = false;	// -- Goes from top to bottom
					sideCurves.push_back(tmpUse1);
					tmpUse2 = new WSEdgeUse();
					tmpUse2->curve = newLine;
					tmpUse2->orientation = true;		// -- Goes from bottom to top
					sideCurves.push_back(tmpUse2);
					//Set edgeUse radials
					tmpUse1->radial = tmpUse2;
					tmpUse2->radial = tmpUse1;
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
				tmpUse1 = new WSEdgeUse();
				tmpUse1->curve = newCurve;
				tmpUse1->orientation = (*curveIter).second;
				bottomCurves.push_back(tmpUse1);
				//Create side edge use
				tmpUse2 = new WSEdgeUse();
				tmpUse2->curve = newCurve;
				tmpUse2->orientation = !(*curveIter).second;
				sideCurves.push_back(tmpUse2);
				//Set edgeUse radials
				tmpUse1->radial = tmpUse2;
				tmpUse2->radial = tmpUse1;

				/*** Done with creating ***/
			}
			//Add tmpBottom and tmpTop to tmpBottoms and tmpTops - make sure to clear them afterwards
			tmpBottoms.push_back( tmpBottom );
			tmpBottom.clear();
			tmpTops.push_back( tmpTop );
			tmpTop.clear();
		}
		//Add tmpBottom and tmpTop to bottomTrim and topTrim - make sure to clear them afterwards
		this->_bottomTrims.push_back( tmpBottoms );
		tmpBottoms.clear();
		this->_topTrims.push_back(tmpTops );
		tmpTops.clear();
		//Add topology edge uses to master list
		this->_topoTopCurves.push_back(topCurves);
		this->_topoSideCurves.push_back(sideCurves);
		this->_topoBottomCurves.push_back(bottomCurves);
		//Clear the topo lists
		topCurves.clear();
		sideCurves.clear();
		bottomCurves.clear();
	}
}


/*** GenerateSideSurfaces Algorithm ***
 * This algorithm generates all of the "vertical" surfaces of the extrusion.  They are linear extrusions of the base profile curves.
 *	The algorithm just steps through curve list, in normal list order, and extrudes each curve along the direction for offset distance.
 *	The only complication of the method is that curves must face either inward or outward based on if the profile is exterior or
 *	interior and whether the curve is same oriented or opposite.
 ***/
void WCPartPad::GenerateSideSurfaces(void) {
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
		this->_topoSurfaces.push_back(topoList);
	}
}


/*** GenerateTopBottom ***
 * The GenerateTopBottom algorithm generates just the top and bottom surfaces.  These are trim surfaces, so two components must
 * be created:
 *		1) The underlying NURBS surface control points and knot values must be calculated
 *		2) The set of trim profiles, different for top and bottom, are passed into surface creation
 ***/
void WCPartPad::GenerateTopBottom(void) {
	GLuint prog = this->_part->Scene()->ShaderManager()->ProgramFromName("scn_basiclight_trim");
	std::vector<WCVector4> controlPoints;
	WCVector4 tmpPt1;
	WSFaceUse *tmpUse;
	std::list< std::list<WCTrimProfile> >::iterator topTrim = this->_topTrims.begin();
	std::list< std::list<WCTrimProfile> >::iterator bottomTrim = this->_bottomTrims.begin();
	std::list< std::list<WSFaceUse*> >::iterator topoIter = this->_topoSurfaces.begin();

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


WCTopologyModel* WCPartPad::GenerateTopology(void) {
	//Create one model
	WCTopologyModel *model = new WCTopologyModel();
	//Create one shell per item in main profile list
	WSTopologyShell *shell;
	WPUInt profileCount;
	WSFaceUse *faceUse;
	WSLoopUse *loopUse;
//	WSEdgeUse *edgeUse;
//	WSVertexUse *vertexUse;
	std::list<WCSketchProfile*>::iterator profilesIter;
	std::list<std::list<WCSketchProfile*> >::iterator listIter;
//	std::list<std::list<WCGeometricPoint*> >::iterator	pointsListIter = this->_topoPoints.begin();
//	std::list<std::list<WCGeometricCurve*> >::iterator	curvesListIter = this->_topoCurves.begin();
	std::list<std::list<WSFaceUse*> >::iterator surfacesListIter = this->_topoSurfaces.begin();
	for (listIter = this->_profiles.begin(); listIter != this->_profiles.end(); listIter++) {
		//Get the number of profiles in this shell
		profileCount = (WPUInt)(*listIter).size();
//		std::cout << "Profile Count: " << profileCount << std::endl;
		//Create a shell
		shell = new WSTopologyShell();
		//Set shell values
		shell->model = model;
		//Add shell to model
		model->AddShell(shell);

		/*** Configure all of the FaceUses ***/

		WSFaceUse *prevFU = NULL, *firstFU = NULL, *lastFU = NULL;
		for (std::list<WSFaceUse*>::iterator fuIter = (*surfacesListIter).begin(); fuIter != (*surfacesListIter).end(); fuIter++) {
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
			WSLoopUse *prevLU = NULL, *firstLU = NULL, *lastLU = NULL;
			if ((faceUse == (*surfacesListIter).front()) || (faceUse == (*surfacesListIter).back())) {
				//Add profile count loops
				for (WPUInt i=0; i<profileCount; i++) {
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
			}
		}
		//Clean up dangling faceUse and shell pointers
		firstFU->prev = lastFU;
		lastFU->next = firstFU;
		shell->faceUses = firstFU;
		//Move to the next shell
//		pointsListIter++;
//		curvesListIter++;
		surfacesListIter++;
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
	_firstOffset(firstOffset), _secondOffset(secondOffset), _points(), _lines(), _curves(), _surfaces(), _topTrims(), _bottomTrims(),
	_topoBottomPoints(), _topoTopPoints(), _topoBottomCurves(), _topoSideCurves(), _topoTopCurves(), _topoSurfaces() {
	//Determine direction and offsets
	this->DetermineDirection();

	//Generate the points, curves, and surfaces
	this->GeneratePoints();
	this->GenerateCurves();
	this->GenerateSideSurfaces();
	this->GenerateTopBottom();
	WCTopologyModel* model = this->GenerateTopology();
	std::cout << *model << std::endl;;
	//Now union the pad model (both geometric and topological) with the part model
//	this->_part->Union(model);
	//Clean up
	delete model;

	//Finish initialization
	this->Initialize();
}


WCPartPad::WCPartPad(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCPartFeature( WCSerializeableObject::ElementFromName(element,"PartFeature"), dictionary),
	_profiles(), _isReversed(false), _firstType(WCPartPadType::Dimension()), _secondType(WCPartPadType::Dimension()),
	_firstOffset(0.0), _secondOffset(0.0), _points(), _lines(), _curves(), _surfaces(), _topTrims(), _bottomTrims(),
	_topoBottomPoints(), _topoTopPoints(), _topoBottomCurves(), _topoSideCurves(), _topoTopCurves(), _topoSurfaces() {
	//Restore the pad here
	//...
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


std::ostream& operator<<(std::ostream& out, const WCPartPad &pad) {
	return out;
}


/***********************************************~***************************************************/

