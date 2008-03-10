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
#include "PartDesign/part_shaft.h"
#include "PartDesign/part_body.h"
#include "PartDesign/part.h"
#include "PartDesign/part_plane.h"
#include "Sketcher/sketch_profile.h"
#include "Sketcher/sketch.h"
#include "Sketcher/sketch_axis.h"


/***********************************************~***************************************************/


std::list<WCTrimProfile> WCShaft::GenerateFrontProfile(const WCRay &ray) {
	//Make sure there is some angle
	if (this->_cwAngle - this->_ccwAngle < 0.01) return std::list<WCTrimProfile>();
	
	std::list<WCTrimProfile> trimProfiles;
	std::list<WCTrimProfile>::iterator trimProfileIter;
	WCTrimProfile trimProfile;
	std::list<std::pair<WCSketchProfile*,bool> >::iterator profileIter;
	std::list<std::pair<WCGeometricCurve*,bool> > curveList;
	std::list<std::pair<WCGeometricCurve*,bool> >::iterator curveIter;	
	//Set up transform matrix
	WCMatrix4 transform(true);
	WCVector4 forMove(ray.Base().I(), ray.Base().J(), ray.Base().K(), 0.0);
	WCVector4 invMove(-ray.Base().I(), -ray.Base().J(), -ray.Base().K(), 0.0);

	//(Full circle case) or (ccwAngle = 0 case)
	if ((this->_cwAngle - this->_ccwAngle >= 2.0*M_PI) || (this->_ccwAngle == 0.0)){
		//Loop through all profiles
		for (profileIter = this->_profiles.begin(); profileIter != this->_profiles.end(); profileIter++) {
			//Get the profile's list of curves
			curveList = (*profileIter).first->CurveList();
			//Loop through all curves and copy into trimProfiles
			for (curveIter = curveList.begin(); curveIter != curveList.end(); curveIter++) {
				//Based on side either maintain or reverse profiles
				if (this->_profilesOnRight)
					//Add curve to trim profile in reverse order
					trimProfile.first.push_front( std::make_pair( (*curveIter).first, !(*curveIter).second ) );
				else 
					//Add curve to trim profile and maintain order
					trimProfile.first.push_back( std::make_pair( (*curveIter).first, (*curveIter).second ) );
			}
			//Set the trimProfile outside state
			trimProfile.second = (*profileIter).second;
			//Add the trim profile into the list
			trimProfiles.push_back(trimProfile);
			//Clear the trimProfile list
			trimProfile.first.clear();
		}
		//Return the list if full circle case
		if (this->_cwAngle - this->_ccwAngle >= 2.0*M_PI) return trimProfiles;
	}
	//Otherwise, project profiles onto front face and copy all curves
	else {
		//Determine transform for front face projection
		WCQuaternion rotQuat;
		//Convert angle to degrees
		WPFloat angle = this->_ccwAngle * R2D;
		rotQuat.FromEulerAxis(ray.Direction().I(), ray.Direction().J(), ray.Direction().K(), -angle);
		transform = rotQuat.ToDCM();
				
		WCNurbsCurve *nurbs, *newNurb;
		WCGeometricLine *line, *newLine;
		WCGeometricCurve *newCurve;
		//Loop through all profiles
		for (profileIter = this->_profiles.begin(); profileIter != this->_profiles.end(); profileIter++) {
			//Get the profile's list of curves
			curveList = (*profileIter).first->CurveList();
			//Loop through all curves in the profile
			for (curveIter = curveList.begin(); curveIter != curveList.end(); curveIter++) {
				//Try casting to a line
				line = dynamic_cast<WCGeometricLine*>( (*curveIter).first );
				//If it is a line
				if (line != NULL) {
					//Duplicate the line
					newLine = new WCGeometricLine(*line);
					//Add line to list
					this->_lines.push_back(newLine);
					//Set the line layer
//					newLine->Layer( this->_part->LinesLayer() );
					newCurve = newLine;
				}
				//Otherwise
				else {
					//Try casting to a nurb
					nurbs = dynamic_cast<WCNurbsCurve*>( (*curveIter).first );
					//Check error case
					if (nurbs == NULL) {
						CLOGGER_ERROR(WCLogManager::RootLogger(), "WCShaft::GenerateFrontFace - Unknown curve type.");
						//throw error
						return trimProfiles;
					}
					//Duplicate the curve
					newNurb = new WCNurbsCurve(*nurbs);
					//Add curve to list
					this->_curves.push_back(newNurb);
					//Set the curve layer
//					newNurb->Layer( this->_part->CurvesLayer() );
					newCurve = newNurb;
				}
				//Translate curve to take ray.base into account
				newCurve->ApplyTranslation(invMove);
				//Project the curve onto the front face
				newCurve->ApplyTransform(transform);
				//Translate curve back from taking take ray.base into account
				newCurve->ApplyTranslation(forMove);
				//Based on side either maintain or reverse profiles
				if (this->_profilesOnRight)
					//Add curve to trim profile in reverse order
					trimProfile.first.push_front( std::make_pair( newCurve, !(*curveIter).second ) );
				else 
					//Add curve to trim profile and maintain order
					trimProfile.first.push_back( std::make_pair( newCurve, (*curveIter).second ) );
			}
			//Set the trimProfile outside state
			trimProfile.second = (*profileIter).second;
			//Add the trim profile into the list
			trimProfiles.push_back(trimProfile);
			//Clear the trimProfile list
			trimProfile.first.clear();
		}		
	}

/*** DEBUG ***
	WCGeometricLine* line;
	WCNurbsCurve* nurb;
	std::cout << "Matrix: " << transform;
	for (trimProfileIter = trimProfiles.begin(); trimProfileIter != trimProfiles.end(); trimProfileIter++) {
		curveList = (*trimProfileIter).first;
		for (curveIter = curveList.begin(); curveIter != curveList.end(); curveIter++) {
			//Print the curve info
			std::cout << "Curve: " << (*curveIter).second << std::endl;
			line = dynamic_cast<WCGeometricLine*>( (*curveIter).first );
			if (line != NULL) std::cout << *line;
			else {
				nurb = dynamic_cast<WCNurbsCurve*>((*curveIter).first);
				std::cout << *nurb;
			}
		}
	}
/*** DEBUG ***/

	//Build list of boundary points for exterior profiles (use original profiles)
	std::list<WCVector4> inputList, tmpList;
	for (profileIter = this->_profiles.begin(); profileIter != this->_profiles.end(); profileIter++) {
		//Only use profile if it is an outside profile
		if ((*profileIter).second) {
			//Build the boundary list (true is for detailed)
			tmpList = (*profileIter).first->BoundaryList(true);
//			for(std::list<WCVector4>::iterator iter = tmpList.begin(); iter != tmpList.end(); iter++) std::cout << *iter << std::endl;
			//Splice it into the end of the overall list
			inputList.splice(inputList.end(), tmpList);
		}
	}
	//Get reference plane
	WCPartPlane *refPlane = this->_profiles.front().first->Sketch()->ReferencePlane();
	//Find minimum bounding rectangle for bounding points
	std::list<WCVector4> baseCorners = MinimumBoundingRectangle(inputList, refPlane->InverseTransformMatrix(), refPlane->TransformMatrix());
	//Place into array
	WCVector4 corners[4];
	corners[0] = baseCorners.front(); baseCorners.pop_front();		// Lower-left
	corners[1] = baseCorners.front(); baseCorners.pop_front();		// Upper-left
	corners[2] = baseCorners.front(); baseCorners.pop_front();		// Upper-right
	corners[3] = baseCorners.front(); baseCorners.pop_front();		// Lower-right
	//Translate, transform, translate back to front position
	corners[0] = forMove + transform * (corners[0] + invMove);
	corners[1] = forMove + transform * (corners[1] + invMove);
	corners[2] = forMove + transform * (corners[2] + invMove);
	corners[3] = forMove + transform * (corners[3] + invMove);
	//Create controlPoint array (take side into account)
	std::vector<WCVector4> controlPoints;
	if (this->_profilesOnRight) {
		controlPoints.push_back(corners[0]);
		controlPoints.push_back(corners[1]);
		controlPoints.push_back(corners[3]);
		controlPoints.push_back(corners[2]);
	}
	else {
		controlPoints.push_back(corners[1]);
		controlPoints.push_back(corners[0]);
		controlPoints.push_back(corners[2]);
		controlPoints.push_back(corners[3]);
	}
	//Generate trimmed surface using profiles and corners
	WCTrimmedNurbsSurface *front = new WCTrimmedNurbsSurface(this->_document->Scene()->GeometryContext(), 
		trimProfiles, 1, 1, 2, 2, controlPoints, WCNurbsMode::Default(), WCNurbsMode::Default());
	//Make sure front is valid
	if (front != NULL) {
		front->TextureSize(1024);
		front->RenderProgram( this->_part->Scene()->ShaderManager()->ProgramFromName("scn_basiclight_trim") );
		this->_surfaces.push_back(front);
		//Set the layer for the surface
//		front->Layer(this->_part->SurfacesLayer());
	}
	
	/*** Clean up and Finish ***/
	
	//Return the trim Profiles
	return trimProfiles;
}


void WCShaft::GenerateBackProfile(const WCRay &ray) {
	//Make sure there is some angle
	if (this->_cwAngle - this->_ccwAngle < 0.01) return;
	//Make sure not a full circle
	if (this->_cwAngle - this->_ccwAngle >= 2.0*M_PI) return;
	
	std::list<WCTrimProfile> trimProfiles;
	std::list<WCTrimProfile>::iterator trimProfileIter;
	WCTrimProfile trimProfile;
	std::list<std::pair<WCSketchProfile*,bool> >::iterator profileIter;
	std::list< std::pair<WCGeometricCurve*,bool> > curveList;
	std::list< std::pair<WCGeometricCurve*,bool> >::iterator curveIter;	
	WCMatrix4 transform(true);
	WCVector4 forMove(ray.Base().I(), ray.Base().J(), ray.Base().K(), 0.0);
	WCVector4 invMove(-ray.Base().I(), -ray.Base().J(), -ray.Base().K(), 0.0);
	
	//ccwAngle = 0 case
	if (this->_cwAngle == 0.0){
		//Loop through all profiles
		for (profileIter = this->_profiles.begin(); profileIter != this->_profiles.end(); profileIter++) {
			//Get the profile's list of curves
			curveList = (*profileIter).first->CurveList();
			//Loop through all curves and copy into trimProfiles
			for (curveIter = curveList.begin(); curveIter != curveList.end(); curveIter++) {
				//Based on side either maintain or reverse profiles
				if (this->_profilesOnRight)
					//Add curve to trim profile and maintain order
					trimProfile.first.push_back( std::make_pair( (*curveIter).first, (*curveIter).second ) );
				else 
					//Add curve to trim profile and reverse order
					trimProfile.first.push_front( std::make_pair( (*curveIter).first, !(*curveIter).second ) );
			}
			//Set the trimProfile outside state
			trimProfile.second = (*profileIter).second;
			//Add the trim profile into the list
			trimProfiles.push_back(trimProfile);
			//Clear the trimProfile list
			trimProfile.first.clear();
		}
	}
	//General case (project points around curve
	else {
		//Determine transform for back face projection
		WCQuaternion rotQuat;
		//Convert angle to degrees
		WPFloat angle = this->_cwAngle * R2D;
		rotQuat.FromEulerAxis(ray.Direction().I(), ray.Direction().J(), ray.Direction().K(), -angle);
		transform = rotQuat.ToDCM();
		
		WCNurbsCurve *nurbs, *newNurb;
		WCGeometricLine *line, *newLine;
		WCGeometricCurve* newCurve;
		//Loop through all profiles
		for (profileIter = this->_profiles.begin(); profileIter != this->_profiles.end(); profileIter++) {
			//Get the profile's list of curves
			curveList = (*profileIter).first->CurveList();
			//Loop through all curves in the profile
			for (curveIter = curveList.begin(); curveIter != curveList.end(); curveIter++) {
				//Try casting to a line
				line = dynamic_cast<WCGeometricLine*>( (*curveIter).first );
				//If it is a line
				if (line != NULL) {
					//Duplicate the line
					newLine = new WCGeometricLine(*line);
					//Add line to list
					this->_lines.push_back(newLine);
					//Set the line layer
//					newLine->Layer( this->_part->LinesLayer() );
					newCurve = newLine;
				}
				//Otherwise
				else {
					//Try casting to a nurb
					nurbs = dynamic_cast<WCNurbsCurve*>( (*curveIter).first );
					//Check error case
					if (nurbs == NULL) {
						CLOGGER_ERROR(WCLogManager::RootLogger(), "WCShaft::GenerateFrontFace - Unknown curve type.");
						//throw error
						return;
					}
					//Duplicate the curve
					newNurb = new WCNurbsCurve(*nurbs);
					//Add curve to list
					this->_curves.push_back(newNurb);
					//Set the curve layer
//					newNurb->Layer( this->_part->CurvesLayer() );
					newCurve = newNurb;
				}
				//Translate curve to take ray.base into account
				newCurve->ApplyTranslation(invMove);
				//Project the curve onto the back face
				newCurve->ApplyTransform(transform);
				//Translate curve back from taking take ray.base into account
				newCurve->ApplyTranslation(forMove);
				//Based on side either maintain or reverse profiles
				if (this->_profilesOnRight)
					//Add curve to trim profile and maintain order
					trimProfile.first.push_back( std::make_pair( newCurve, (*curveIter).second ) );
				else 
					//Add curve to trim profile in reverse order
					trimProfile.first.push_front( std::make_pair( newCurve, !(*curveIter).second ) );
			}
			//Set the trimProfile outside state
			trimProfile.second = (*profileIter).second;
			//Add the trim profile into the list
			trimProfiles.push_back(trimProfile);
			//Clear the trimProfile list
			trimProfile.first.clear();
		}
	}

	//Build list of boundary points for exterior profiles (use original profiles)
	std::list<WCVector4> inputList, tmpList;
	for (profileIter = this->_profiles.begin(); profileIter != this->_profiles.end(); profileIter++) {
		//Only use profile if it is an outside profile
		if ((*profileIter).second) {
			//Build the boundary list (true is for detailed)
			tmpList = (*profileIter).first->BoundaryList(true);
//			for(std::list<WCVector4>::iterator iter = tmpList.begin(); iter != tmpList.end(); iter++) std::cout << *iter << std::endl;
			//Splice it into the end of the overall list
			inputList.splice(inputList.end(), tmpList);
		}
	}
	//Get reference plane
	WCPartPlane *refPlane = this->_profiles.front().first->Sketch()->ReferencePlane();
	//Find minimum bounding rectangle for bounding points
	std::list<WCVector4> baseCorners = MinimumBoundingRectangle(inputList, refPlane->InverseTransformMatrix(), refPlane->TransformMatrix());
	//Place into array
	WCVector4 corners[4];
	corners[0] = baseCorners.front(); baseCorners.pop_front();		// Lower-left
	corners[1] = baseCorners.front(); baseCorners.pop_front();		// Upper-left
	corners[2] = baseCorners.front(); baseCorners.pop_front();		// Upper-right
	corners[3] = baseCorners.front(); baseCorners.pop_front();		// Lower-right
	//Translate, transform, translate back to front position
	corners[0] = forMove + transform * (corners[0] + invMove);
	corners[1] = forMove + transform * (corners[1] + invMove);
	corners[2] = forMove + transform * (corners[2] + invMove);
	corners[3] = forMove + transform * (corners[3] + invMove);
	//Create controlPoint array (take side into account)
	std::vector<WCVector4> controlPoints;
	if (this->_profilesOnRight) {
		controlPoints.push_back(corners[1]);
		controlPoints.push_back(corners[0]);
		controlPoints.push_back(corners[2]);
		controlPoints.push_back(corners[3]);
	}
	else {
		controlPoints.push_back(corners[0]);
		controlPoints.push_back(corners[1]);
		controlPoints.push_back(corners[3]);
		controlPoints.push_back(corners[2]);
	}
	//Generate trimmed surface using profiles and corners
	WCTrimmedNurbsSurface *back = new WCTrimmedNurbsSurface(this->_document->Scene()->GeometryContext(),
		trimProfiles, 1, 1, 2, 2, controlPoints, WCNurbsMode::Default(), WCNurbsMode::Default());
	//Make sure back is valid
	if (back != NULL) {
		back->TextureSize(1024);
		back->RenderProgram( this->_part->Scene()->ShaderManager()->ProgramFromName("scn_basiclight_trim") );
		//Add to list of surfaces
		this->_surfaces.push_back(back);
		//Set the layer for the surface
//		back->Layer(this->_part->SurfacesLayer());

	}
}


void WCShaft::GenerateSurfaces(const WCRay &ray, const std::list<WCTrimProfile> &profileList) {
	//Make sure there is some angle
	if (this->_cwAngle - this->_ccwAngle < 0.01) return;

	WCNurbsSurface *surface;
	std::list<WCTrimProfile>::const_iterator profileIter;
	std::list< std::pair<WCGeometricCurve*,bool> > curveList;
	std::list< std::pair<WCGeometricCurve*,bool> >::iterator curveIter;
	//Determine angle
	WPFloat angle = this->_cwAngle - this->_ccwAngle;
	//Loop through all profiles
	for (profileIter = profileList.begin(); profileIter != profileList.end(); profileIter++) {
		//Get the profile's list of curves
		curveList = (*profileIter).first;
		//Loop through all curves and generate
		for (curveIter = curveList.begin(); curveIter != curveList.end(); curveIter++) {
			//Attempt to create a revolution surface (take profile direction into account)
			if ((*profileIter).second)
				surface = WCNurbsSurface::RevolveCurve(this->_document->Scene()->GeometryContext(),
					(*curveIter).first, &ray, angle, !(*curveIter).second );
			else
				surface = WCNurbsSurface::RevolveCurve(this->_document->Scene()->GeometryContext(),
					(*curveIter).first, &ray, angle, (*curveIter).second ); 
			//Make sure is valid, and add to list of surfaces
			if (surface != NULL) this->_surfaces.push_back(surface);
		}
	}
	
	/*** Set properties for surfaces ***/

	std::list<WCNurbsSurface*>::iterator surfIter;
	for (surfIter = this->_surfaces.begin(); surfIter != this->_surfaces.end(); surfIter++) {
		(*surfIter)->Color( WCPartFeature::DefaultSurfaceColor );
		//Only set if needs setting (trimmed surfaces have already been set)
		if ((*surfIter)->RenderProgram() == 0)
			(*surfIter)->RenderProgram( this->_part->Scene()->ShaderManager()->ProgramFromName("scn_basiclight"));
//		(*surfIter)->Layer(this->_part->SurfacesLayer());
		//Try doubling the LOD
		(*surfIter)->LevelOfDetail( (*surfIter)->LevelOfDetailU() * 3, (*surfIter)->LevelOfDetailV() * 3 );
	}
	
	/*** Done with Surfaces ***/
}


void WCShaft::GeneratePoints(const WCRay &ray) {
	//Make sure there is some angle
	if (this->_cwAngle - this->_ccwAngle < 0.01) return;
}


void WCShaft::GenerateCurves(const WCRay &ray, const std::list<WCTrimProfile> &profileList) {
	//Make sure there is some angle
	if (this->_cwAngle - this->_ccwAngle < 0.01) return;

	//Calculate xUnit
	WCVector4 xUnit = this->_axis->Sketch()->ReferencePlane()->InverseTransformMatrix() * ray.Direction();
	xUnit.Set(xUnit.J(), -xUnit.I(), 0.0, 0.0);
	xUnit = this->_axis->Sketch()->ReferencePlane()->TransformMatrix() * xUnit;
	//Calculate yUnit
	WCVector4 yUnit = ray.Direction().CrossProduct(xUnit);
	//Determine positive and negative angles (take side into account)
	WPFloat negAngle;
	WPFloat posAngle;
	if (this->_profilesOnRight) {
		negAngle = -this->_cwAngle * R2D;
		posAngle = -this->_ccwAngle * R2D;
	}
	else {
		negAngle = 180.0 - (this->_cwAngle * R2D);
		posAngle = 180.0 - (this->_ccwAngle * R2D);
	}

	std::list<WCTrimProfile>::const_iterator profileIter;
	std::list< std::pair<WCGeometricCurve*,bool> > curveList;
	std::list< std::pair<WCGeometricCurve*,bool> >::iterator curveListIter;

	WCNurbsCurve *curve;
	WCVector4 endPoint, center;
	WPFloat radius;
	//Loop through all trim profiles
	for (profileIter = profileList.begin(); profileIter != profileList.end(); profileIter++) {
		//Get the profile's list of curves
		curveList = (*profileIter).first;
		//Only do profiles with more than one curve
		if (curveList.size() > 1) {
			//Loop through all curves and generate curve from end
			for (curveListIter = curveList.begin(); curveListIter != curveList.end(); curveListIter++) {
				//Make sure to get end based on alignment
				if ((*curveListIter).second) endPoint = (*curveListIter).first->Evaluate(1.0);
				else endPoint = (*curveListIter).first->Evaluate(0.0);
				//Project the endPoint onto the axis
				center = ProjectPointToLine3D(ray.Base(), ray.Direction(), endPoint);
				//Calculate the radius of the curve
				radius = center.Distance(endPoint);
				//Only create circle if point is not on axis
				if (radius > 0.001) {
					//Attempt to create a circular curve
					curve = WCNurbsCurve::CircularArc(this->_document->Scene()->GeometryContext(), center, xUnit, yUnit, radius, negAngle, posAngle);
					//Make sure is valid, and add to list of curves
					if (curve != NULL) this->_curves.push_back(curve);
				}
			}
		}
	}

	/*** Set properties for curves ***/

	std::list<WCNurbsCurve*>::iterator curveIter;
	for (curveIter = this->_curves.begin(); curveIter != this->_curves.end(); curveIter++) {
		(*curveIter)->Color( WCPartFeature::DefaultCurveColor );
		(*curveIter)->Thickness( PARTFEATURE_LINE_THICKNESS );
//		(*curveIter)->Layer(this->_part->CurvesLayer());
		//Try doubling the LOD
//		(*curveIter)->LevelOfDetail( (*curveIter)->LevelOfDetail() * 3 );
	}
}


void WCShaft::GenerateTopology(const WCRay &ray) {
}


/***********************************************~***************************************************/


WCShaft::WCShaft(WCPartBody *body, const std::string &name, const std::list<std::pair<WCSketchProfile*,bool> > &profiles,
	WCSketchAxis *axis, const bool profilesOnRight, const WPFloat &cwAngle, const WPFloat &ccwAngle) : 
	::WCPartFeature(body, name), _profiles(profiles), _axis(axis), _profilesOnRight(profilesOnRight),
	_cwAngle(cwAngle), _ccwAngle(ccwAngle), _points(), _lines(), _curves(), _surfaces() {
	//Check feature name
	if (this->_name == "") this->_name = this->_part->GenerateFeatureName(this);
	//Create event handler
	this->_controller = new WCShaftController(this);
	//Create tree element
	WSTexture* shaftIcon = this->_document->Scene()->TextureManager()->TextureFromName("shaft32");
	this->_treeElement = new WCTreeElement(this->_document->TreeView(), this->_name, this->_controller, shaftIcon);
	//Add profiles as children
	std::list< std::pair<WCSketchProfile*,bool> >::iterator profIter;
	for (profIter = this->_profiles.begin(); profIter != this->_profiles.end(); profIter++) {
		this->_treeElement->AddLastChild( (*profIter).first->TreeElement() );
	}
	this->_treeElement->IsOpen(false);
	//Add tree view element
	this->_creator->TreeElement()->AddLastChild(this->_treeElement);
	
	//Add the shaft to the part (true for visualize and select)
	this->_part->AddFeature(this, false);
	
	//Generate the points, curves, and surfaces
	WCMatrix4 planeMatrix = this->_axis->Sketch()->ReferencePlane()->TransformMatrix();
	//Get axis base
	WCVector4 base = planeMatrix * this->_axis->Begin();
	//Get axis direction
	WCVector4 direction = (planeMatrix * this->_axis->End()) - base;
	direction.Normalize(true);
	//Create ray
	WCRay ray(base, direction);
	//Generate the front profile
	std::list<WCTrimProfile> profileList = this->GenerateFrontProfile(ray);
	//Generate the back profile
	this->GenerateBackProfile(ray);
	//Generate the other surfaces
	this->GenerateSurfaces(ray, profileList);
	//Generate all curves
	this->GenerateCurves(ray, profileList);
	//Generate all points
//	this->GeneratePoints(ray);
	//Generate topological model
//	this->GenerateTopology(ray);
	
	//Add all points into the part
	//...
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


WCShaft::~WCShaft() {
}

	
void WCShaft::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
}


xercesc::DOMElement* WCShaft::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	return NULL;
}


void WCShaft::Render(const GLuint defaultProg, const WCColor color) {
}


void WCShaft::OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) {
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


void WCShaft::OnDeselection(const bool fromManager) {
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


/***********************************************~***************************************************/


bool WCShaft::QualifyProfiles(const std::list<WCSketchProfile*> &inputProfiles, std::list<std::pair<WCSketchProfile*,bool> > &outputProfiles, 
	const WCSketchAxis *axis) {
	WCSketch *sketch;
	WPUInt tmpFloat;
	bool isOnRight;
	
	//Make sure output is empty
	outputProfiles.clear();

	//Validate each profile
	std::list<WCSketchProfile*>::const_iterator iter;
	std::list<WCSketchProfile*> profileList;
	for (iter = inputProfiles.begin(); iter != inputProfiles.end(); iter++) {
		//Make sure profile is linear, closed, and not self-intersecting
		if ((!(*iter)->IsLinear()) || (!(*iter)->IsClosed()) || ((*iter)->IsSelfIntersecting())) {
			//Validate that the sketch is closed, linear, and non-intersecting
			CLOGGER_WARN(WCLogManager::RootLogger(), "WCShaft::QualifyProfiles - Invalid profile in list of profiles.");
		}
		else {
			//Get first valid profile's sketch and side
			if (profileList.size() == 0) {
				//Get the first profile's side
				tmpFloat = (*iter)->IsOnRight(axis);
				//If tmpFloat = 0, then profile intersects the axis, and should be discarded
				if (tmpFloat == 0) {
					CLOGGER_WARN(WCLogManager::RootLogger(), "WCShaft::QualifyProfiles - Initial profile intersects axis.");
				}
				//Otherwise...
				else {
					//Get the sketch the profile belongs to
					sketch = (*iter)->Sketch();
					//Set side appropriately
					if (tmpFloat == 1) isOnRight = true;
					else isOnRight = false;
					//Add profile to list
					profileList.push_back(*iter);
				}
			}
			//Second 
			else {
				//Make sure is in same sketch
				if ((*iter)->Sketch() != sketch) {
					CLOGGER_WARN(WCLogManager::RootLogger(), "WCShaft::QualifyProfiles - Profile sketches to not match.");
				}
				else {
					//Test to see which side profile is on
					tmpFloat = (*iter)->IsOnRight(axis);
					//Make sure profile does not intersect axis
					if (tmpFloat == 0) {
						CLOGGER_WARN(WCLogManager::RootLogger(), "WCShaft::QualifyProfiles - Profile intersects axis.");
					}
					//Make sure profile is on same side as initial profile
					else if ((tmpFloat == 1) == isOnRight) {
						//Put good profiles into local list
						profileList.push_back(*iter);
					}
					else
						CLOGGER_WARN(WCLogManager::RootLogger(), "WCShaft::QualifyProfiles - Profile on wrong side of axis.");
				}
			}
		}
	}
	
	//Categorize the profiles
	std::list<WCProfileTreeNode*> rootList = WCSketchProfile::CategorizeIntoTree(profileList);
	outputProfiles = WCSketchProfile::FlattenCategorizationTree(rootList);
	//Otherwise, return the side
	return isOnRight;
}


/***********************************************~***************************************************/


WCDrawingMode* WCShaft::ModeCreate(WCPartWorkbench *wb) {
	//Create a new drawing mode
	return new WCModeShaftCreate(wb);
}


WCActionShaftCreate* WCShaft::ActionCreate(WCPartBody *body, const std::string &shaftName, const std::list< std::pair<WCSketchProfile*,bool> > &profiles, 
	WCSketchAxis *axis, const bool profilesOnRight, const WPFloat &cwAngle, const WPFloat &ccwAngle) {
	//Create a new shaft create action
	return new WCActionShaftCreate(body, shaftName, profiles, axis, profilesOnRight, cwAngle, ccwAngle);
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCShaft &shaft) {
	return out;
}


/***********************************************~***************************************************/

