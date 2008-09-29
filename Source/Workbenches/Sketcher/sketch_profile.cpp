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
#include "Sketcher/sketch_profile.h"
#include "Sketcher/sketch.h"
#include "Sketcher/sketch_axis.h"
#include "Kernel/document.h"
#include "PartDesign/part_plane.h"


/*** Locally Defined Values ***/
#define SKETCHPROFILE_TOLERANCE					0.001


/***********************************************~***************************************************/


bool WCSketchProfile::DetermineIsLinear(const std::list<WCGeometricCurve*> &list) {
	WCGeometricCurve* curve;
	
	//Copy the curve list and clear it
	std::list<WCGeometricCurve*> toBeProcessed = list;
	this->_curveList.clear();
	//Add back in the first curve and remove from TBP list
	this->_curveList.push_back( std::make_pair(toBeProcessed.front(), true) );
	toBeProcessed.pop_front();
	
	WCVector4 front, back, ptFront, ptBack;
	WPUInt count;
	
	//While toBeProcessed has elements, try to add them onto the front or back of the list
	count = (WPUInt)toBeProcessed.size();
	while (toBeProcessed.size() > 0) {
		//Get element to be processed
		curve = toBeProcessed.front();
		toBeProcessed.pop_front();
		//Evaluate at front and end
		ptFront = curve->Evaluate(0.0);
		WCVector4 ptMid = curve->Evaluate(0.5);
		ptBack = curve->Evaluate(1.0);
		
		//Check to see if connects to front of list (remember to look at curve orientation)
		if (this->_curveList.front().second) front = this->_curveList.front().first->Evaluate(0.0);
		else front = this->_curveList.front().first->Evaluate(1.0);
//		std::cout << "ListFront: " << front << std::endl;
		//Matched front of curve to front of list
		if (front.Distance(ptFront) < SKETCH_ANALYSIS_TOLERANCE) {
			//Add to front of curveList with orientation backwards
			this->_curveList.push_front( std::make_pair(curve, false) );
			//Reset the count
			count = (WPUInt)toBeProcessed.size();
		}
		//Matched back of curve to front of list
		else if (front.Distance(ptBack) < SKETCH_ANALYSIS_TOLERANCE) {
			//Add to front of curveList with orientation forwards
			this->_curveList.push_front( std::make_pair(curve, true) );		
			//Reset the count
			count = (WPUInt)toBeProcessed.size();
		}
		
		//Check back matches
		else {
			if (this->_curveList.back().second) back = this->_curveList.back().first->Evaluate(1.0);
			else back = this->_curveList.back().first->Evaluate(0.0);
//			std::cout << "ListBack: " << back << std::endl;
			//Matched front of curve to back of list
			if (back.Distance(ptFront) < SKETCH_ANALYSIS_TOLERANCE) {
				//Add to back of curveList with orientation forwards
				this->_curveList.push_back( std::make_pair(curve, true) );
				//Reset the count
				count = (WPUInt)toBeProcessed.size();
			}
			//Matched back of curve to back of list
			else if (back.Distance(ptBack) < SKETCH_ANALYSIS_TOLERANCE) {
				//Add to back of curveList with orientation backwards
				this->_curveList.push_back( std::make_pair(curve, false) );		
				//Reset the count
				count = (WPUInt)toBeProcessed.size();
			}
			
			//No match found
			else {
				//Check that count > 1
				if (count == 1) {
					//Put back the current curve
					this->_curveList.push_back( std::make_pair(curve, true) );
					//Empty remaining toBeProcessed into curveList
					while(toBeProcessed.size() > 0) {
						//Add to the curveList
						this->_curveList.push_back( std::make_pair(toBeProcessed.front(), true) );
						//Remove from toBeProcessed
						toBeProcessed.pop_front();
					}
					//Return false
					return false;
				}
				//Otherwise
				else {
					//Put curve back into TBP
					toBeProcessed.push_back(curve);
					//Decrease count
					count--;
				}
			}
		}
	}
	//If here, then all segments are linear
	return true;
}


bool WCSketchProfile::DetermineIsClosed(void) {
	//Don't bother if not linear
	if (!this->_isLinear) {
		return false;
	}
	WCVector4 firstBegin, lastEnd;
	
	//Determine starting point
	if (this->_curveList.front().second) {
//		std::cout << "Front Forwards\n";
		firstBegin = this->_curveList.front().first->Evaluate(0.0);
	}
	else {
//		std::cout << "Front Backwards\n";
		firstBegin = this->_curveList.front().first->Evaluate(1.0);
	}
	//Determine ending point
	if (this->_curveList.back().second) {
//		std::cout << "Back Forwards\n";
		lastEnd = this->_curveList.back().first->Evaluate(1.0);
	}
	else {
//		std::cout << "Back Backwards\n";
		lastEnd = this->_curveList.back().first->Evaluate(0.0);
	}
	
	//If start and end are same, it is closed, otherwise, it is not
	if (firstBegin.Distance(lastEnd) > SKETCH_ANALYSIS_TOLERANCE) return false;
	//Profile is closed
	return true;
}


bool WCSketchProfile::DetermineIsSelfIntersecting(void) {
	std::list< std::pair<WCGeometricCurve*,bool> >::iterator firstIter, secondIter;
	std::list<WCIntersectionResult> hitList;
	WCGeometricLine *firstLine, *secondLine;
	WCNurbsCurve *firstCurve, *secondCurve;
	int flags = INTERSECT_GEN_NONE | INTERSECT_CULL_BOUNDARY;
	//Outer loop
	for(firstIter = this->_curveList.begin(); firstIter != this->_curveList.end(); firstIter++) {
		//Set type of first curve
		firstLine = dynamic_cast<WCGeometricLine*>((*firstIter).first);;
		if (!firstLine) firstCurve = dynamic_cast<WCNurbsCurve*>((*firstIter).first);
		//Inner loop
		secondIter = firstIter;
		for (secondIter++; secondIter != this->_curveList.end(); secondIter++) {
			//Set type of second curve
			secondLine = dynamic_cast<WCGeometricLine*>((*secondIter).first);
			if (!secondLine) secondCurve = dynamic_cast<WCNurbsCurve*>((*secondIter).first);
			//Intersect first and second based on type
			if (firstLine && secondLine)		hitList = GeometricIntersection(firstLine, secondLine, SKETCH_PROFILE_DEFAULT_TOLERANCE, flags);
			else if (firstLine && !secondLine)	hitList = GeometricIntersection(secondCurve, firstLine, SKETCH_PROFILE_DEFAULT_TOLERANCE, flags);
			else if (!firstLine && secondLine)	hitList = GeometricIntersection(firstCurve, secondLine, SKETCH_PROFILE_DEFAULT_TOLERANCE, flags);
			else								hitList = GeometricIntersection(firstCurve, secondCurve, SKETCH_PROFILE_DEFAULT_TOLERANCE, flags);
			//If there are any intersections, return true
//			if (hitList.size() != 0) return true;
		}
	}
	//No intersections means that is not self-intersecting
	return false;
}


/*** Force Clockwise Algorithm
 *	I went through many interations of this algorithm on how to determine that the profile is has a CW winding.
 *	Here is the approach:
 *		1) Find the midpoint tangent of the first curve
 *		2) Project the tangent onto the reference plane, save as norm
 *		3) Based on curve direction, adjust #2 to get the normal to the tangent (normal should point into the interior of profile)
 *		4) Project the normal back into regular space
 *		5) Create a point just off of the curve in the direction of the normal
 *		6) Check to see if point is inside profile or not (this shoots a ray from point to some far distant place and intersects all curves)
 *		7) If IsInside count is even, then profile is CW, otherwise profile is CCW
***/
void WCSketchProfile::ForceClockwise(void) {
//	std::cout << *this;
	bool ccw = false;
	//Check on size 1 special case
	if (this->_curveList.size() == 1) {
//		std::list<WCVector4> boundaryPoints;
//		BuildBoundaryList(this->_curveList, boundaryPoints, true, 0.01);
		ccw = true;
	}
	//Otherwise...
	else {
		//Find point and tangent at curve mid-point
		WCRay tangent = this->_curveList.front().first->Tangent(0.5);
		//Project tangent onto reference plane
		WCVector4 norm = this->_sketch->ReferencePlane()->InverseTransformMatrix() * tangent.Direction();
		//Get norm to tangent (take orientation into account)
		if (this->_curveList.front().second) norm.Set(-norm.J(), norm.I(), 0.0, 0.0);
		else norm.Set(norm.J(), -norm.I(), 0.0, 0.0);
		//Project back into regular space
		norm = this->_sketch->ReferencePlane()->TransformMatrix() * norm;
		//Create reference point just off of point using norm
		WCVector4 refPoint = tangent.Base() + norm * 0.01;
		WPUInt hitCount = this->IsInside(refPoint);
		//Determine if ccw
		ccw = ((hitCount % 2) != 0);
	}
	//If ccw, need to invert the profile
	if (ccw) {
//		CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCSketchProfile::ForceClockwise - Reversing profile.");
		//Iteratre through the curveList
		std::list< std::pair<WCGeometricCurve*,bool> >::iterator iter;
		std::list< std::pair<WCGeometricCurve*,bool> > newList;
		//Reverse the order of the list, and invert all of the direction bools
		for (iter = this->_curveList.begin(); iter != this->_curveList.end(); iter++) {
			newList.push_front( std::make_pair((*iter).first, !(*iter).second) );
		}
		//Set the new list
		this->_curveList = newList;
	}	
}


void WCSketchProfile::Initialize(void) {
	//Create the profile controller
	this->_controller = new WCSketchProfileController(this);
	//Add into sketch
	if (!this->_sketch->AddFeature(this)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchProfile::WCSketchProfile - Problem adding profile to sketch.");
		//throw error
		return;
	}
	//Create tree element and add into the tree (beneath the sketch profiles element)
	WSTexture *profileIcon = this->_document->Scene()->TextureManager()->TextureFromName("profile32");
	this->_treeElement = new WCTreeElement(this->_sketch->Document()->TreeView(), this->_name, this->_controller, profileIcon);
	this->_sketch->ProfilesTreeElement()->AddLastChild(this->_treeElement);
	
	//Retain curves
	std::list< std::pair<WCGeometricCurve*,bool> >::iterator listIter;
	for (listIter = this->_curveList.begin(); listIter != this->_curveList.end(); listIter++) {
		//Retain the curve
		(*listIter).first->Retain(*this);
	}
}


/***********************************************~***************************************************/


WCSketchProfile::WCSketchProfile(WCSketch *sketch, const std::string &name, std::list<WCGeometricCurve*> &list) :
	::WCSketchFeature(sketch), _curveList(), _isLinear(false), _isClosed(false), _isSelfIntersecting(false) {
	//Check feature name
	if (this->_name == "") this->_name = this->_sketch->GenerateFeatureName(this);
	//Determine linearity
	this->_isLinear = this->DetermineIsLinear(list);
	//Determine if Closed
	this->_isClosed = this->DetermineIsClosed();
	//Determine if self-intersecting
	this->_isSelfIntersecting = this->DetermineIsSelfIntersecting();
	//If linear, closed, and not self-intersecting, then force clockwise
	if (this->_isLinear && this->_isClosed && !this->_isSelfIntersecting) this->ForceClockwise();
	//Complete initialization
	this->Initialize();
}


WCSketchProfile::WCSketchProfile(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	WCSketchFeature( WCSerializeableObject::ElementFromName(element,"SketchFeature"), dictionary ), 
	_curveList(), _isLinear(false), _isClosed(false), _isSelfIntersecting(false) {
	//Make sure element if not null
	if (element == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchProfile::WCSketchProfile - NULL Element passed.");
		//throw error
		return;
	}
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

	//Setup booleans
	this->_isLinear = WCSerializeableObject::GetBoolAttrib(element,"linear");
	this->_isClosed = WCSerializeableObject::GetBoolAttrib(element,"closed");
	this->_isSelfIntersecting = WCSerializeableObject::GetBoolAttrib(element,"intersecting");

	//Setup curve list
	XMLCh* xmlString = xercesc::XMLString::transcode("Curves");
	xercesc::DOMNodeList *curveList = element->getElementsByTagName(xmlString)->item(0)->getChildNodes();
	xercesc::XMLString::release(&xmlString);
	xercesc::DOMNode *tmpNode;
	xercesc::DOMElement *curveElement;
	//Loop through all curves
	for (WPUInt curveIndex=0; curveIndex < curveList->getLength(); curveIndex++) {
		//Get the indexed node
		tmpNode = curveList->item(curveIndex);
		//Make sure node is element
		if (tmpNode->getNodeType() == xercesc::DOMNode::ELEMENT_NODE) {
			//Cast node to element
			curveElement = (xercesc::DOMElement*)tmpNode;
			//Get the curve address
			WCGeometricCurve* curve = (WCGeometricCurve*)WCSerializeableObject::GetGUIDAttrib(curveElement, "address", dictionary);
			//Get the curve orientation
			bool orientation = WCSerializeableObject::GetBoolAttrib(curveElement, "orientation");
			//Add curve to list
			this->_curveList.push_back( std::make_pair(curve, orientation) );
		}
	}
	//Complete initialization
	this->Initialize();
}


WCSketchProfile::~WCSketchProfile() {
	//Remove from the sketch
	if (!this->_sketch->RemoveFeature(this)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchProfile::~WCSketchProfile - Problem removing profile from sketch.");	
	}
	//Release curves
	std::list< std::pair<WCGeometricCurve*,bool> >::iterator listIter;
	for (listIter = this->_curveList.begin(); listIter != this->_curveList.end(); listIter++) {
		//Retain the curve
		(*listIter).first->Release(*this);
	}
}


/*** Positive is to the right, zero is on both sides, and negative is to left
	Axis always has zero or positive slope ***/
WPInt WCSketchProfile::IsOnRight(const WCSketchAxis *axis) {
	//Project axis begin and end points into 3D space
	WCVector4 axisBegin = this->_sketch->ReferencePlane()->TransformMatrix() * axis->Begin();
	WCVector4 axisEnd = this->_sketch->ReferencePlane()->TransformMatrix() * axis->End();
	//Form a near infinite geometric line using the axis points
	WCVector4 dir = axis->End() - axis->Begin();
	dir.Normalize(true);
	WCVector4 p1 = axis->Begin() + dir * 1000000.0;
	WCVector4 p2 = axis->Begin() - dir * 1000000.0;
	WCGeometricLine *line = new WCGeometricLine(p1, p2);
	//Determine slope of axis
	WPFloat lineSlope = (axis->End().J() - axis->Begin().J()) / (axis->End().I() - axis->Begin().I());

	//Try intersecting that line with all curves in the profile
	WCGeometricLine* tmpLine;
	WCNurbsCurve* tmpCurve;
	std::list<WCIntersectionResult> hits;
	std::list< std::pair<WCGeometricCurve*,bool> >::iterator iter;
	WPFloat testLineSlope;
	for (iter = this->_curveList.begin(); iter != this->_curveList.end(); iter++) {
		//Figure out what type of curve
		tmpLine = dynamic_cast<WCGeometricLine*> ((*iter).first);
		//If it is a line
		if (tmpLine) {
			//Try the intersection
			hits = GeometricIntersection(line, tmpLine, SKETCH_PROFILE_DEFAULT_TOLERANCE, INTERSECT_GEN_NONE);
		}
		//Must be a curve
		else {
			//Make it a curve
			tmpCurve = dynamic_cast<WCNurbsCurve*> ((*iter).first);
			//Try the intersection
			hits = GeometricIntersection(tmpCurve, line, SKETCH_PROFILE_DEFAULT_TOLERANCE, INTERSECT_GEN_NONE);
		}

		//Check to see if any hits, if so return 0.0
		if (hits.size() != 0) {
			//Check to see if is a line
			if (tmpLine) {
				//Determine slope of test line
				testLineSlope = 0;
				//See if slopes are the same				
				if (fabs(fabs(lineSlope) - fabs(testLineSlope)) > 0.01) return 0;
				//Otherwise, we can ignore this intersection - since it must be along the axis
			}
			//Case of curve intersecting the axis
			else return 0;
		}
	}
	//Delete the line
	delete line;
	//If here, then no intersections, so test points to see on which side
	WCVector4 testPoint, projPoint;
	for (iter = this->_curveList.begin(); iter != this->_curveList.end(); iter++) {
		//Get a point on the curve
		testPoint = (*iter).first->Evaluate(0.5);
		//Make sure point is not on axis
		projPoint = ProjectPointToLine3D(axis->Begin(), dir, testPoint);
		if (projPoint.Distance(testPoint) > 0.01) {
			//Project the point onto the plane
			testPoint = this->_sketch->ReferencePlane()->InverseTransformMatrix() * testPoint;
			//See if point is left or right of line
			if (IsOnRight2D(axis->Begin().I(), axis->Begin().J(), axis->End().I(), axis->End().J(), testPoint.I(), testPoint.J())) return 1;
			else return -1;
		}
		//If here, then must try next curve to see if left or right
	}
	return 1;
}


/*** IsInside Algorithm ***
 * This algorithm determines if the passed point is inside or outside of the primary profile.  The caller is responsible
 *	for checking to make sure profile is closed.  Here are the details of the algorithm:
 *		1) A line is created from the passed point to a point very far away (assumed to always be outside of the profile)
 *		2) The line is intersected with all of the curves of the profile
 *		3) The number of intersections is returned.  Even is outside, odd is inside.
***/
WPUInt WCSketchProfile::IsInside(const WCVector4 &point) {
	//Form a geometric line from way far away to the point
	WCVector4 p2(100000.0, 8128.0, 0.0, 1.0);
	p2 = this->_sketch->ReferencePlane()->TransformMatrix() * p2;
	WCGeometricLine *line = new WCGeometricLine(point, p2);
	WCGeometricLine *iterLine;
	WCNurbsCurve* iterCurve;
	unsigned int flags = INTERSECT_GEN_NONE;
	//Try intersecting that line with all curves in the profile
	WPFloat count = 0.0;
	std::list<WCIntersectionResult> hits;
	std::list<WCIntersectionResult>::iterator hitsIter;
	std::list< std::pair<WCGeometricCurve*,bool> >::iterator iter;
	for (iter = this->_curveList.begin(); iter != this->_curveList.end(); iter++) {
		//Determine type of curve
		iterLine = dynamic_cast<WCGeometricLine*>((*iter).first);
		if (!iterLine) iterCurve = dynamic_cast<WCNurbsCurve*>((*iter).first);
		//Try the intersection
		if (iterLine) hits = GeometricIntersection(line, iterLine, SKETCH_PROFILE_DEFAULT_TOLERANCE, flags);
		else hits = GeometricIntersection(iterCurve, line, SKETCH_PROFILE_DEFAULT_TOLERANCE, flags);
		//Adjust hits count
		for (hitsIter = hits.begin(); hitsIter != hits.end(); hitsIter++) {
			//Adjust for boundary hit
			if ((iterLine && (*hitsIter).rightBoundary) || ((!iterLine && (*hitsIter).leftBoundary)) ) count += 0.5;
			//Half for a boundary hit, whole for a non-boundary hit
			else count++;
		}
	}
	//Delete the line
	delete line;
	//Return the count
	return (WPUInt)ceil(count);
}


/*** Categorize Algorithm ***
 * This algorithm tries to determine the relationship between two profiles.  They can be intersecting or distinct.  Even if
 *	they do not intersect two profiles can be distinct or one profile can also be either inside or outside of the other.  All
 *	return values are the status of the passed profile as is relates to the primary profile.
 *	Here is the algorithm:
 *		1) Intersect all curves from profile A with all curves from profile B
 *		2) Any intersection results in a return type of Intersect
 *		3) If no intersections then check to see if any point from profile B is inside A, likewise a point from A is in B
 *		4) Returns from these tests are used to categorize as distinct, inside, or outside
***/
WCProfileType WCSketchProfile::Categorize(WCSketchProfile *profile) {
	std::list<WCIntersectionResult> hits;
	WCGeometricLine *firstLine, *secondLine;
	WCNurbsCurve *firstCurve, *secondCurve;

	//Intersect all curves of profile with this
	std::list< std::pair<WCGeometricCurve*,bool> >::iterator firstIter, secondIter;
	//Outer loop through this curve list
	for (firstIter = this->_curveList.begin(); firstIter != this->_curveList.end(); firstIter++) {
		//Figure out what type of curve
		firstLine = dynamic_cast<WCGeometricLine*> ((*firstIter).first);
		if (!firstLine) firstCurve = dynamic_cast<WCNurbsCurve*> ((*firstIter).first);
		//Inner loop through profile curve list
		for (secondIter = profile->_curveList.begin(); secondIter != profile->_curveList.end(); secondIter++) {
			//Figure out type of second curve
			secondLine = dynamic_cast<WCGeometricLine*> ((*secondIter).first);
			if (!secondLine) secondCurve = dynamic_cast<WCNurbsCurve*> ((*secondIter).first);
			//Itersect the two based on type
			if (firstLine && secondLine)		hits = GeometricIntersection(firstLine, secondLine, SKETCHPROFILE_TOLERANCE, INTERSECT_GEN_NONE);
			else if (firstLine && !secondLine)	hits = GeometricIntersection(secondCurve, firstLine, SKETCHPROFILE_TOLERANCE, INTERSECT_GEN_NONE);
			else if (!firstLine && secondLine)	hits = GeometricIntersection(firstCurve, secondLine, SKETCHPROFILE_TOLERANCE, INTERSECT_GEN_NONE);
			else								hits = GeometricIntersection(firstCurve, secondCurve, SKETCHPROFILE_TOLERANCE, INTERSECT_GEN_NONE);
			//If there are hits, then there is some intersection
			if (hits.size() > 0) return WCProfileType::Intersect();
		}
	}
	
	//If no intersections, check point from profile to see if inside or out of this
	WCVector4 pt0 = profile->_curveList.front().first->Evaluate(0.0);
	WCVector4 pt1 = this->_curveList.front().first->Evaluate(0.0);
	WPUInt BinA = this->IsInside(pt0);
	WPUInt AinB = profile->IsInside(pt1);
	bool p0Even = (BinA % 2) == 0;
	bool p1Even = (AinB % 2) == 0;

	//If both are even, A and B are distinct
	if (p0Even && p1Even) return WCProfileType::Distinct();		// A abd B are distinct
	else if (p0Even) return WCProfileType::Outside();			// B is outside A
	return WCProfileType::Inside();								// B is inside A
}


/*** Triangulate Algorithm ***
 * This fundamental algorithm generates a vertex buffer and index buffer that triangulate the closed linear profile.
 *	First a detailed boundary list of the profile is generated.  This list is processed by the TriangulatePolygon routine.  See
 *	the details of this routine for more information.  It simply returns the index data which is then loaded into a buffer.
 *	The boundary point data is also loaded into a buffer.
***/
GLuint WCSketchProfile::Triangulate(GLuint &vertexBuffer, GLuint &indexBuffer) {
	//Put all vertices into a list
	std::list<WCVector4> boundaryList;
	this->BoundaryList(true, boundaryList);
	std::list<WCVector4>::iterator boundaryIter;
	//Define some arrays and variables
	int numVerts = (int)boundaryList.size();
	WCVector4 tmpVec;
	std::list<WCVector4> vertList;
	int vertIndex = 0;
	GLfloat *vertData = new GLfloat[numVerts * 3];
	GLint* indexData;
	WCMatrix4 toPlaneMatrix( this->_sketch->ReferencePlane()->InverseTransformMatrix() );
	
	//Process list
	for (boundaryIter = boundaryList.begin(); boundaryIter != boundaryList.end(); boundaryIter++) {
		//Project onto plane and put into list
		tmpVec = toPlaneMatrix * (*boundaryIter);
		vertList.push_back(tmpVec);
		//Also place into vertex buffer array
		vertData[vertIndex*3] = (GLfloat)(*boundaryIter).I();
		vertData[vertIndex*3+1] = (GLfloat)(*boundaryIter).J();
		vertData[vertIndex*3+2] = (GLfloat)(*boundaryIter).K();
		//Increment vertIndex
		vertIndex++;
	}

	//Expects verts in CW order, outputs triangles in CW order
	indexData = TriangulatePolygon(vertList);

	//Put vertices and index array into VBOs
	glGenBuffers(1, &vertexBuffer);
	glGenBuffers(1, &indexBuffer);
	//Load vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * numVerts, vertData, GL_STATIC_DRAW);
	//Load index buffer
	glBindBuffer(GL_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLint) * 3 * (numVerts-2), indexData, GL_STATIC_DRAW);
	//Make sure to delete the index array
	delete indexData;
	delete vertData;
/*** DEBUG ***
	std::cout << "Triangulate Verts(" << vertexBuffer << "): " << numVerts << std::endl;
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);	
	GLfloat *data2 = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	for (int i=0; i<numVerts; i++) printf("\t%d V: %f %f %f\n", i, data2[i*3], data2[i*3+1], data2[i*3+2]);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	std::cout << "Triangulate Index(" << indexBuffer << "): " << numVerts-2 << std::endl;
	glBindBuffer(GL_ARRAY_BUFFER, indexBuffer);	
	GLint *data3 = (GLint*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	for (int i=0; i<numVerts-2; i++) printf("\t%d I: %d %d %d\n", i, data3[i*3], data3[i*3+1], data3[i*3+2]);
	glUnmapBuffer(GL_ARRAY_BUFFER);
/*** DEBUG ***/
	//Return to unbound state
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Check for errors here
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << error << std::endl;
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCSketchProfile::Triangulate - Unspecified GL Error.");
	}
	//Return the number of triangles
	return numVerts - 2;
}


bool WCSketchProfile::operator==(const WCSketchProfile &profile) {
	//Make sure they have the same number of elements
	if (profile._curveList.size() != this->_curveList.size()) return false;
	return true;
}


bool WCSketchProfile::operator==(const std::list<WCGeometricCurve*> &curveList) {
	//Make sure they have the same number of elements
	if (curveList.size() != this->_curveList.size()) return false;

	std::list<WCGeometricCurve*>::const_iterator iter;
	std::list<std::pair<WCGeometricCurve*,bool> >::iterator iter2;
	
	//Now check each element in curveList is in _curveList
	for (iter = curveList.begin(); iter != curveList.end(); iter++) {
		iter2 = this->_curveList.begin();
		//Loop until match is found or end is hit
		while ((*iter != (*iter2).first) && (iter2 != this->_curveList.end())) iter2++; 
		//If entire list was processed and no match, then not the same lists
		if (iter2 == this->_curveList.end()) return false;
	}
	//If here, then lists are matched
	return true;
}


bool WCSketchProfile::operator!=(const WCSketchProfile &profile) {
	return !(*this == profile);
}


void WCSketchProfile::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
	if (msg == OBJECT_NOTIFY_DELETE)
		std::cout << "I heard about you being deleted.\n";
}


bool WCSketchProfile::Regenerate(void) {
	return false;
}


std::list<WCObject*> WCSketchProfile::DeletionDependencies(void) {
	return std::list<WCObject*>();
}


void WCSketchProfile::OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) {
	//Check to see what workbench we are in
	WCColor color;
	if (this->_sketch->Workbench()->IsActive()) color = WCSketchFeature::SelectedColor;
	else color = WCPartFeature::SelectedColor;
	//Set the color of the base curves
	std::list< std::pair<WCGeometricCurve*,bool> >::iterator iter;
	for (iter = this->_curveList.begin(); iter != this->_curveList.end(); iter++) {
		(*iter).first->Color(color);
	}
	//Mark this as selected
	this->_isSelected = true; 
}


void WCSketchProfile::OnDeselection(const bool fromManager) {
	//Check to see what workbench we are in
	WCColor color;
	if (this->_sketch->Workbench()->IsActive()) color = WCSketchFeature::DefaultColor;
	else color = WCPartFeature::DefaultCurveColor;
	//Set the color of the base curves
	std::list< std::pair<WCGeometricCurve*,bool> >::iterator iter;
	for (iter = this->_curveList.begin(); iter != this->_curveList.end(); iter++) {
		(*iter).first->Color(color);
	}
	//Mark this as not selected
	this->_isSelected = false; 
}

	
xercesc::DOMElement* WCSketchProfile::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create the base element for the object
	XMLCh* xmlString = xercesc::XMLString::transcode("SketchProfile");
	xercesc::DOMElement* element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Include the sketch feature element
	xercesc::DOMElement* featureElement = this->WCSketchFeature::Serialize(document, dictionary);
	element->appendChild(featureElement);
	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);
	//Add boolean attributes
	WCSerializeableObject::AddBoolAttrib(element, "linear", this->_isLinear);
	WCSerializeableObject::AddBoolAttrib(element, "closed", this->_isClosed);
	WCSerializeableObject::AddBoolAttrib(element, "intersecting", this->_isSelfIntersecting);

	//Loop through all curves and add them
	xmlString = xercesc::XMLString::transcode("Curves");
	XMLCh* curveStr = xercesc::XMLString::transcode("Curve");
	xercesc::DOMElement *curves = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	element->appendChild(curves);
	xercesc::DOMElement *curveElem;
	std::list<std::pair<WCGeometricCurve*,bool> >::iterator curvesIter;
	WPUInt index = 0;
	for (curvesIter = this->_curveList.begin(); curvesIter != this->_curveList.end(); curvesIter++) {
		//Create an entry for each curve
		curveElem = document->createElement(curveStr);
		//Add curve address
		WCSerializeableObject::AddGUIDAttrib(curveElem, "address", (*curvesIter).first, dictionary);
		//Add curve orientation flag
		WCSerializeableObject::AddBoolAttrib(curveElem, "orientation", (*curvesIter).second);
		//Add index of the curve
		WCSerializeableObject::AddFloatAttrib(curveElem, "index", index++);
		//Add curve to list
		curves->appendChild(curveElem);
	}
	xercesc::XMLString::release(&curveStr);
	//Return the element
	return element;
}


/***********************************************~***************************************************/


std::list<std::list<WCSketchProfile*> > _RecursiveFlatten(WCProfileTreeNode *node, bool outside) {
	std::list<WCSketchProfile*> tmpList;
	std::list<std::list<WCSketchProfile*> > outputList, tmpListList;
	std::list<WCProfileTreeNode*>::const_iterator childIter;

	//If an outside profile
	if (outside) {
		//Add node to list
		tmpList.push_back( node->profile);
		//Add all children first if node is an outside node
		for (childIter = node->children.begin(); childIter != node->children.end(); childIter++) {
			//Add children to list
			tmpList.push_back( (*childIter)->profile );
		}
		//Add this list to the output list
		outputList.push_back(tmpList);
	}
	//Recursively process children
	for (childIter = node->children.begin(); childIter != node->children.end(); childIter++) {
		//Get the child's list
		tmpListList = _RecursiveFlatten( *childIter, !outside );
		//Merge it with this level's list
		outputList.splice(outputList.end(), tmpListList);
	}
	//Return the list
	return outputList;
}


WCProfileType _RecursiveTreeAddition(WCProfileTreeNode *node, WCProfileTreeNode *newNode) {
	//Check the profile vs. this node
	WCProfileType type = node->profile->Categorize(newNode->profile);
	//If B is outside of A, move A into B
	if (type == WCProfileType::Outside()) {
		newNode->children.push_back(node);
	}
	//If B is inside A, see about pushing B below A's children
	else if (type == WCProfileType::Inside()) {
		std::list<WCProfileTreeNode*>::iterator childrenIter;
		std::list<WCProfileTreeNode*> insideChildren, outsideChildren;
		//Iterate through all children
		for (childrenIter = node->children.begin(); childrenIter != node->children.end(); childrenIter++) {
			type = _RecursiveTreeAddition(*childrenIter, newNode);
			//Return immediately if intersect
			if (type == WCProfileType::Intersect()) return type;
			//Otherwise, catalog all children
			if (type == WCProfileType::Inside()) insideChildren.push_back(*childrenIter);
			else if (type == WCProfileType::Outside())outsideChildren.push_back(*childrenIter);
		}
		//Remove all outside profiles
		for (childrenIter = outsideChildren.begin(); childrenIter != outsideChildren.end(); childrenIter++) {
			node->children.remove(*childrenIter);
		}
		//If not inside any root nodes, new node is child node
		if (insideChildren.size() == 0) {
			node->children.push_back(newNode);
		}
		//Set the return type
		type = WCProfileType::Inside();
	}
	return type;
}


void _AddProfileToTree(std::list<WCProfileTreeNode*> &rootList, WCProfileTreeNode *newNode) {
	std::list<WCProfileTreeNode*>::iterator listIter;
	std::list<WCProfileTreeNode*> insideChildren, outsideChildren;
	WCProfileType type = WCProfileType::Distinct();
	
	//Check profile against all root nodes
	for (listIter = rootList.begin(); listIter != rootList.end(); listIter++) {
		//Categorize vs. the root nodes
		type = _RecursiveTreeAddition( *listIter, newNode);
		//Make sure no intersection
		if (type == WCProfileType::Intersect()) {
			delete newNode;
			return;
		}
		//Otherwise, build lists of child types
		if (type == WCProfileType::Inside()) insideChildren.push_back(*listIter);
		else if (type == WCProfileType::Outside()) outsideChildren.push_back(*listIter);
	}
	//Remove all outside profiles
	for (listIter = outsideChildren.begin(); listIter != outsideChildren.end(); listIter++) {
		rootList.remove(*listIter);
	}
	//If not inside any root nodes, new node is root node
	if (insideChildren.size() == 0) {
		rootList.push_back(newNode);
	}
}


void _RecursiveNodePrint(WCProfileTreeNode* node, int depth) {
	//Print the profile name
	for (int i=0; i<depth; i++) std::cout << "\t";
	std::cout << node->profile->GetName() << std::endl;
	std::list<WCProfileTreeNode*>::iterator childIter;
	for (childIter = node->children.begin(); childIter != node->children.end(); childIter++)
		_RecursiveNodePrint(*childIter, depth+1);
}


std::list<WCProfileTreeNode*> WCSketchProfile::CategorizeIntoTree(const std::list<WCSketchProfile*> &profiles) {
	//Check for no profiles case
	if (profiles.size() == 0) return std::list<WCProfileTreeNode*>();
	
	std::list<WCSketchProfile*>::const_iterator profileIter = profiles.begin();
	std::list<WCProfileTreeNode*> rootList;
	WCProfileTreeNode *firstNode, *node;
	firstNode = new WCProfileTreeNode();
	firstNode->profile = *profileIter;
//	std::cout << *(firstNode->profile);
	rootList.push_back(firstNode);

	//Process all of the profiles
	for (profileIter++; profileIter != profiles.end(); profileIter++) {
		node = new WCProfileTreeNode();
		node->profile = *profileIter;
//		std::cout << *(node->profile);
		_AddProfileToTree(rootList, node);
	}
/*** DEBUG ***
	std::list<WCProfileTreeNode*>::iterator rlIter;
	for (rlIter = rootList.begin(); rlIter != rootList.end(); rlIter++) {
		_RecursiveNodePrint(*rlIter, 0);
	}
/*** DEBUG ***/
	//Return rootList
	return rootList;
}


std::list<std::list<WCSketchProfile*> > WCSketchProfile::FlattenCategorizationTree(std::list<WCProfileTreeNode*> &rootList) {
	std::list<std::list<WCSketchProfile*> > outputList, tmpList;
	std::list<WCProfileTreeNode*>::const_iterator rootListIter;
	//Merge each root node list with outputList
	for(rootListIter = rootList.begin(); rootListIter != rootList.end(); rootListIter++) {
		//Get list from root node down
		tmpList = _RecursiveFlatten(*rootListIter, true);
		//Add list into master list
		outputList.splice(outputList.end(), tmpList);
	}
/*** DEBUG ***
	std::list<WCSketchProfile*> innerList;
	std::list<WCSketchProfile*>::iterator innerIter;
	std::list<std::list<WCSketchProfile*> >::iterator outerIter;
	for (outerIter = outputList.begin(); outerIter != outputList.end(); outerIter++) {
		innerList = *outerIter;
		std::cout << "****";
		for (innerIter = innerList.begin(); innerIter != innerList.end(); innerIter++) {
			std::cout << (*innerIter)->GetName() << std::endl;
		}
	}
/*** DEBUG ***/
	return outputList;
}


/***********************************************~***************************************************/


std::ostream& __WILDCAT_NAMESPACE__::operator<<(std::ostream& out, const WCSketchProfile &profile) {
	out << "Sketch Profile(" << &profile << ")\n";
	out << "\tLinear: " << profile._isLinear << std::endl;
	out << "\tClosed: " << profile._isClosed << std::endl;
	out << "\tSelfIntersecting: " << profile._isSelfIntersecting << std::endl;
	std::list<std::pair<WCGeometricCurve*,bool> >::const_iterator iter;
	for (iter = profile._curveList.begin(); iter != profile._curveList.end(); iter++) {
		if ((*iter).second)
			std::cout << "\tCurve Forwards: " << (*iter).first->Evaluate(0.0) << " : " << (*iter).first->Evaluate(1.0) << std::endl;
		else
			std::cout << "\tCurve Backwards: " << (*iter).first->Evaluate(1.0) << " : " << (*iter).first->Evaluate(0.0) << std::endl;
	}
	return out;
}


/***********************************************~***************************************************/

