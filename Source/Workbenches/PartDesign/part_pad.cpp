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


void WCPartPad::GeneratePoints(void) {
	//Check to make sure there is a depth
	if (this->_firstOffset - this->_secondOffset < 0.01) return;
}


void WCPartPad::GenerateCurves(void) {
	//Check to make sure there is a depth
	if (this->_firstOffset - this->_secondOffset < 0.01) return;

	//Create a line from each curve end-point in each profile
	WCGeometricLine* line;
	WCVector4 pt, lower, upper;
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
			//Make sure curveList is > 1
			if (curveList.size() > 1) {
				//Iterate through all curves in list
				for (curveIter = curveList.begin(); curveIter != curveList.end(); curveIter++) {
					//See if curve is forward
					if ((*curveIter).second) pt = (*curveIter).first->Evaluate(1.0);
					//...Or backwards
					else pt = (*curveIter).first->Evaluate(0.0);
					//Create a line using the point
					lower = pt + this->_direction * this->_secondOffset;
					upper = pt + this->_direction * this->_firstOffset;
					line = new WCGeometricLine(lower, upper);
					//Set the properties for the line
					line->Color( WCPartFeature::DefaultCurveColor );
					line->Thickness( PARTFEATURE_LINE_THICKNESS );
					//Add the line to the list of all lines
					this->_lines.push_back(line);			
				}
			}
		}
	}
}


void WCPartPad::GenerateSurfaces(void) {
	//Check to make sure there is a depth
	if (this->_firstOffset - this->_secondOffset < 0.01) return;
	
	//Create a surface from each curve in each profile
	bool exterior;
	WCGeometricCurve* curve;
	WCNurbsSurface* surface;
	std::list<WCSketchProfile*>::iterator profilesIter;
	std::list<std::list<WCSketchProfile*> >::iterator listIter;
	std::list< std::pair<WCGeometricCurve*,bool> > curveList;
	std::list< std::pair<WCGeometricCurve*,bool> >::iterator curveIter;
	//Loop through all profiles and extrude all surfaces
	for (listIter = this->_profiles.begin(); listIter != this->_profiles.end(); listIter++) {
		//Mark as exterior
		exterior = true;
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
			}
			//Mark as interior
			exterior = false;
		}
	}
}


void WCPartPad::GenerateTopBottom(void) {
//	std::list<WCSketchProfile*>::iterator profileIter;
//	std::list< std::pair<WCGeometricCurve*,bool> > curveList;
//	std::list< std::pair<WCGeometricCurve*,bool> >::iterator curveIter;
//	WCGeometricCurve* curve;
//	std::list< std::pair<WCGeometricCurve*,bool> > topList, bottomList;
//	std::list<std::list<std::pair<WCGeometricCurve*,bool> > > topProfiles, bottomProfiles;
//	WCGeometricLine *line, *newLine;
//	WCVector4 lineStart, lineEnd, tmpPt1, tmpPt2;
//	WCNurbsCurve *nurbs, *newNurbs;
//	std::vector<WCVector4> controlPoints, topVec, bottomVec;
//	std::vector<WCVector4>::iterator cpIter;

		
	/*** Create Top and Bottom CurveLists ***
	
	//Loop through all profiles and duplicate for top and bottom
	for (profileIter = this->_profiles.begin(); profileIter != this->_profiles.end(); profileIter++) {
		//Get the list of curves
		curveList = (*profileIter)->CurveList();
		//Iterate through all curves in list
		for (curveIter = curveList.begin(); curveIter != curveList.end(); curveIter++) {
			curve = (*curveIter).first;
			//Try casting to line
			line = dynamic_cast<WCGeometricLine*>(curve);
			//If is is a line
			if (line != NULL) {
				//Get line points
				lineStart = line->Begin();
				lineEnd = line->End();
				//Make sure pos depth is large enough
				if (this->_firstOffset > 0.01) {
					//Create line for top (remember to project up to the new upper plane)
					tmpPt1 = lineStart + this->_direction * this->_firstOffset;
					tmpPt2 = lineEnd + this->_direction * this->_secondOffset;
					newLine = new WCGeometricLine(tmpPt1, tmpPt2);
					newLine->Color( WCPartFeature::DefaultCurveColor );
					newLine->Thickness(PARTFEATURE_LINE_THICKNESS);
					//Add line to toplist and line list
					topList.push_back( std::make_pair(newLine, (*curveIter).second) );
					this->_lines.push_back( newLine );
				}
				else topList.push_back( *curveIter );

				//Make sure neg depth is large enough
				if (this->_secondOffset < 0.01) {
					//Create line for bottom (remember to project up to the new lower plane)
					tmpPt1 = lineStart + this->_direction * this->_secondOffset;
					tmpPt2 = lineEnd + this->_direction * this->_secondOffset;
					newLine = new WCGeometricLine(tmpPt1, tmpPt2);
					newLine->Color( WCPartFeature::DefaultCurveColor );
					newLine->Thickness(PARTFEATURE_LINE_THICKNESS);
					//Add line to bottomlist and line list
					bottomList.push_back( std::make_pair(newLine, (*curveIter).second) );
					this->_lines.push_back( newLine );
				}
				else bottomList.push_back( *curveIter );
			}
			//Otherwise...
			else {
				//Cast to a nurbs curve
				nurbs = dynamic_cast<WCNurbsCurve*>(curve);
				//Make sure this is a nurbs curve
				if (nurbs != NULL) {
					//Make sure pos depth is large enough
					if (this->_firstOffset > 0.01) {
						//Create new top curve - and translate up
						newNurbs = new WCNurbsCurve( *nurbs );
						newNurbs->ApplyTranslation( this->_direction * this->_firstOffset );
						newNurbs->Color( WCPartFeature::DefaultCurveColor );
						//Add nurbs to topList and curve list
						topList.push_back( std::make_pair(newNurbs, (*curveIter).second) );
						this->_curves.push_back( newNurbs );
					}
					else topList.push_back( *curveIter );
					
					//Make sure neg depth is large enough
					if (this->_secondOffset < 0.01) {
						//Create new bottom curve - and translate down
						newNurbs = new WCNurbsCurve( *nurbs );
						newNurbs->ApplyTranslation( this->_direction * this->_secondOffset );
						newNurbs->Color( WCPartFeature::DefaultCurveColor );
						//Add nurbs to bottomList and curve list
						bottomList.push_back( std::make_pair(newNurbs, (*curveIter).second) );
						this->_curves.push_back( newNurbs );
					}
					else bottomList.push_back( *curveIter );
				}
				//Error condition
				else {
					CLOGGER_ERROR(WCLogManager::RootLogger(), "WCPad::GenerateTopBottom - Unrecognized curve type.");
					//throw error
					return;
				}
			}
		}
		//Clear the curve list
		curveList.clear();
		//Reverse the order of the list, and invert all of the direction bools
		for (curveIter = bottomList.begin(); curveIter != bottomList.end(); curveIter++)
			curveList.push_front( std::make_pair((*curveIter).first, !(*curveIter).second) );
		bottomList = curveList;
		
		//Add lists to topProfiles and bottomProfiles
		topProfiles.push_back( topList );
		bottomProfiles.push_back( bottomList );
		//Clear top and bottom list
		topList.clear();
		bottomList.clear();
	}

	//Build list of boundary points for exterior profiles
	std::list<WCVector4> inputList, tmpList;
	for (profileIter = this->_profiles.begin(); profileIter != this->_profiles.end(); profileIter++) {
		if ((*profileIter) == this->_profiles.front()) {
			tmpList = (*profileIter)->BoundaryList(false);
			inputList.splice(inputList.begin(), tmpList);
		}
	}
	GLuint prog = this->_part->Scene()->ShaderManager()->ProgramFromName("scn_basiclight_trim");
	//Find minimum bounding rectangle for bounding points
	WCPartPlane *refPlane = this->_profiles.front().front()->Sketch()->ReferencePlane();
	std::list<WCVector4> baseCorners = MinimumBoundingRectangle(inputList, refPlane->InverseTransformMatrix(), refPlane->TransformMatrix());
	WCVector4 corners[4];
	corners[0] = baseCorners.front(); baseCorners.pop_front();		// Lower-left
	corners[1] = baseCorners.front(); baseCorners.pop_front();		// Upper-left
	corners[2] = baseCorners.front(); baseCorners.pop_front();		// Upper-right
	corners[3] = baseCorners.front(); baseCorners.pop_front();		// Lower-right

	/*** Top Trim Surface ***
	
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
		topProfiles, 1, 1, 2, 2, controlPoints, WCNurbsMode::Default(), WCNurbsMode::Default());
//	top->TextureSize(1024);
	top->Color( WCPartFeature::DefaultSurfaceColor );
	top->RenderProgram(prog);
	this->_surfaces.push_back(top);

	/*** Bottom Trim Surface ***

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
		bottomProfiles, 1, 1, 2, 2, controlPoints, WCNurbsMode::Default(), WCNurbsMode::Default());
//	bottom->TextureSize(1024);
	bottom->Color( WCPartFeature::DefaultSurfaceColor );
	bottom->RenderProgram(prog);
	this->_surfaces.push_back(bottom);

	/*** End Top-Bottom Surface Generation ***/
}


void WCPartPad::GenerateTopology(void) {
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


/***********************************************~***************************************************/


WCPartPad::WCPartPad(WCPartBody *body, const std::string &name, std::list<std::list<WCSketchProfile*> > &profiles, 
	const bool &reverseDir, const WCPartPadType &firstType, const WCPartPadType &secondType, const WPFloat &firstOffset, const WPFloat &secondOffset) : 
	::WCPartFeature(body, name), _profiles(profiles), _isReversed(reverseDir), _firstType(firstType), _secondType(secondType),
	_firstOffset(firstOffset), _secondOffset(secondOffset), _points(), _lines(), _curves(), _surfaces() {
	//Determine direction
	WCPartPlane *refPlane = this->_profiles.front().front()->Sketch()->ReferencePlane();
	//Determine direction vector
	WCVector4 dir(0.0, 0.0, 1.0, 0.0);
	dir = refPlane->TransformMatrix() * dir;
	dir.Normalize(true);
	this->_direction = dir;

	//Generate the points, curves, and surfaces
	this->GeneratePoints();
	this->GenerateCurves();
	this->GenerateSurfaces();
	this->GenerateTopBottom();
	this->GenerateTopology();

	//Finish initialization
	this->Initialize();
}


WCPartPad::WCPartPad(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCPartFeature( WCSerializeableObject::ElementFromName(element,"PartFeature"), dictionary),
	_profiles(), _isReversed(false), _firstType(WCPartPadType::Dimension()), _secondType(WCPartPadType::Dimension()),
	_firstOffset(0.0), _secondOffset(0.0), _points(), _lines(), _curves(), _surfaces() {
	//Restore the pad here
	//...
}


WCPartPad::~WCPartPad() {
	//Remove from the part
	if (!this->_part->RemoveFeature(this, true)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCPartShaft::~WCPartShaft - Problem removing feature from part.");	
	}
	//Remove all points into the part
	//...
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
	//...
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
	//...
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

