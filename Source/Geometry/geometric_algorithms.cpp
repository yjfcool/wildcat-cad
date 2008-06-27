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
#include "Geometry/geometric_algorithms.h"
#include "Geometry/geometric_line.h"
#include "Geometry/nurbs_curve.h"


/***********************************************~***************************************************/


/*** Line-Line intersection algorithm taken from http://local.wasp.uwa.edu.au/~pbourke/geometry/lineline3d/ ***/
WPUInt __WILDCAT_NAMESPACE__::LineLineIntersection(const WCVector4 &p1, const WCVector4 &p2, const WCVector4 &p3, 
	const WCVector4 &p4, const bool &includeEnds, const WPFloat &tol, WCVector4 &value, WPFloat &uValue, WPFloat &vValue) {
	//Make sure both lines have some minimal length
	if ((p4.Distance(p3) < tol) || (p2.Distance(p1) < tol)) return INTERSECTION_NONE;
	//Some setup subtractions
	WCVector4 p43(p4 - p3);
	WCVector4 p21(p2 - p1);
	//Check the cross product to make sure not parallel
	WCVector4 cross = p43.CrossProduct(p21);
	WPFloat denom = cross.Magnitude();
	if (denom < tol) {
//		CLOGGER_WARN(WCLogManager::RootLogger(), "LineLineIntersection -  Parallel lines: denominator < Tolerance.");
		//Need to check for overlap
		//...
		return INTERSECTION_PARALLEL;
	}		
	//Many dot products
	WPFloat d121 = p1.DotProduct(p21);			// L
	WPFloat d2121 = p21.DotProduct(p21);		// M
	WPFloat d321 = p3.DotProduct(p21);			// N
	WPFloat d4321 = p43.DotProduct(p21);		// O
	WPFloat d143 = p1.DotProduct(p43);			// Q
	WPFloat d343 = p3.DotProduct(p43);			// R
	WPFloat d4343 = p43.DotProduct(p43);		// S
	denom = (d4343 * d2121) - (d4321 * d4321);
	if (denom == 0.0) {
		CLOGGER_WARN(WCLogManager::RootLogger(), "LineLineIntersection -  Denominator == 0.0.");
		return INTERSECTION_NONE;
	}
	WPFloat numer = d4343 * (d321 - d121) + d4321 * (d143 - d343);
	WPFloat mua = numer / denom;
	WPFloat mub = (d143 + d4321 * mua - d343) / d4343;
	//Calculate points and distance between them
	WCVector4 pointOnFirst = p1 + p21 * mua;
	WCVector4 pointOnSecond = p3 + p43 * mub;
	//Make sure mua and mub are (0.0 1.0)
	if ( (mua >= 1.0) || (mua <= 0.0) || (mub >= 1.0) || (mub <= 0.0) )
		return INTERSECTION_NONE;
	//Get tolerance measures of mua
	bool firstStart = fabs(mua) < tol;
	bool firstEnd = fabs(mua-1.0) < tol;
	bool secondStart = fabs(mub) < tol;
	bool secondEnd = fabs(mub-1.0) < tol;
	//No intersection if at ends of lines
	if ( (firstStart && secondStart) || (firstStart && secondEnd) || (firstEnd && secondStart) || (firstEnd && secondEnd) ) 
		return INTERSECTION_NONE;
	//Make sure lines are not > tol apart
	WPFloat dist = pointOnFirst.Distance(pointOnSecond);
	if (dist < tol) {
		//Create a hit record
		value = pointOnFirst;
		uValue = mua;
		vValue = mub;
		//Return the hit
		return INTERSECTION_INTERSECTION;
	}
	//No intersection, return NULL
	return INTERSECTION_NONE;
}


/***********************************************~***************************************************/


WPUInt __WILDCAT_NAMESPACE__::RayRayIntersection(const WCVector4 &p0, const WCVector4 &t0, const WCVector4 &p2, const WCVector4 &t2,
	WPFloat &alf0, WPFloat &alf2, WCVector4 &p) {
	//Make sure t0 and t2 are normalized
	WCVector4 tn0( t0 );
	WCVector4 tn2( t2 );
	tn0.Normalize(true);
	tn2.Normalize(true);
	WCVector4 cross = tn0.CrossProduct(tn2);
	WPFloat denom = cross.Magnitude();
	denom *= denom;
	if (denom < 0.001) {
//		CLOGGER_WARN(WCLogManager::RootLogger(), "RayRayIntersection - denominator ~= 0.");
		p = p0;
		return INTERSECTION_NONE;
	}
	WCVector4 top = p2 - p0;
	//Find the point of intersection
	WPFloat u = ( (top.I() * tn2.J() * cross.K()) + 
				  (top.J() * tn2.K() * cross.I()) + 
				  (top.K() * tn2.I() * cross.J()) ) -
				( (cross.I() * tn2.J() * top.K()) +
				  (cross.J() * tn2.K() * top.I()) +
				  (cross.K() * tn2.I() * top.J()) );
	//Final result
	u = u / denom;
	WCVector4 result = p0 + (tn0 * u);
	result.L(1.0);
	WCVector4 inter = result - p2;
	WPFloat v;
	if (tn2.I() != 0.0) v = inter.I() / tn2.I();
	else if (tn2.J() != 0.0) v = inter.J() / tn2.J();
	else v = inter.K() / tn2.K();
	alf0 = u;
	alf2 = v;
	p = result;
	return INTERSECTION_INTERSECTION;
}


/***********************************************~***************************************************/


bool __WILDCAT_NAMESPACE__::OneArcConic(const WCVector4 &p0, const WCVector4 &t0, const WCVector4 &p2,
	const WCVector4 &t2, const WCVector4 &p, WCVector4 &p1) {
	WCVector4 p2p0 = p2 - p0;
	WCVector4 dumVec;
	WPFloat alf0, alf2, w1, dummy;
	WPUInt retVal = RayRayIntersection(p0, t0, p2, t2, dummy, dummy, p1);
	//If lines are not parallel (finite control point)
	if (retVal == INTERSECTION_INTERSECTION) {
		WCVector4 pp1 = p - p1;
		RayRayIntersection(p1, pp1, p0, p2p0, alf0, alf2, dumVec);
//		std::cout << "Alf0: " << alf0 << std::endl;
//		std::cout << "Alf2: " << alf2 << std::endl;
		WPFloat num = p2p0.Magnitude() - alf2;
//		std::cout << "Num: " << num << std::endl;
		//Check to make sure not div by zero
		if ((num < 0.0001)||(alf2 <= 0.0)) return false;
		WPFloat a = sqrt(alf2 / num);
//		std::cout << "A: " << a << std::endl;
		WPFloat u = a / (1.0 + a);
		WPFloat omu = 1.0 - u;
		WCVector4 p1p = p1 - p;
		WCVector4 pp0 = p - p0;
		WCVector4 pp2 = p - p2;
		num = omu * omu * pp0.DotProduct( p1p ) + u * u * pp2.DotProduct( p1p );
		WPFloat den = 2.0 * u * omu * p1p.DotProduct(p1p);
		w1 = num / den;
	}
	//Lines are parallel (infinite point)
	else {
		std::cout << "Parallel Lines path\n";
		RayRayIntersection(p, t0, p0, p2p0, alf0, alf2, dumVec);
//		std::cout << "Alf0: " << alf0 << std::endl;
//		std::cout << "Alf2: " << alf2 << std::endl;
		WPFloat num = p2p0.Magnitude() - alf2;
//		std::cout << "Num: " << num << std::endl;
		//Check to make sure not div by zero
		if ((num < 0.0001)||(alf2 <= 0.0)) return false;
		WPFloat a = sqrt( alf2 / num );
//		std::cout << "A: " << a << std::endl;
		WPFloat u = a / (1.0 + a);
//		std::cout << "U: " << u << std::endl;
		WPFloat b = 2.0 * u * (1.0 - u);
		b = -alf0 * (1.0 - b) / b;
//		std::cout << "B: " << b << std::endl;
		p1 = t0 * b;
		p1.L(0.0);
	}
	//Set p1's w value
	p1.L(w1);
	return true;
}


/***********************************************~***************************************************/


WPFloat __WILDCAT_NAMESPACE__::Angle(const WCVector4 &p0, const WCVector4 &p1, const WCVector4 &p2) {
	//Temp variables
	WCVector4 p2p1 = p2 - p1;
	WCVector4 p0p1 = p0 - p1;
	p2p1.L(0.0);
	p0p1.L(0.0);
	WPFloat num = p2p1.DotProduct(p0p1);
	WPFloat den = p2p1.Magnitude() * p0p1.Magnitude();
	WPFloat angle = acos( num / den );
	//Make sure to return in degrees
	return angle * (180.0 / M_PI);
}


/***********************************************~***************************************************/


void __WILDCAT_NAMESPACE__::SplitArc(const WCVector4 &p0, const WCVector4 &p1, const WCVector4 &p2,
	WCVector4 &q1, WCVector4 &s, WCVector4 &r1) {
	//Set up q1 and r1
	WPFloat w1 = p1.L();
	q1 = p0 + (p1 * w1);
	r1 = p2 + (p1 * w1);
	WPFloat wqr = 1.0 + w1;
	q1.L( 1.0 + w1 );
	r1.L( 1.0 + w1 );
	q1 = q1 / wqr;
	r1 = r1 / wqr;
	//Setup S, Q2, R0
	s = (q1 + r1) * 0.5;
	s.L(1.0);
	//Reparameterize
	wqr = sqrt( (1.0 + w1) / 2.0);
	q1.L(wqr);
	r1.L(wqr);
}
                                                                                             

/***********************************************~***************************************************/


struct WCHeapNode {
	WCVector4 point;
	WPFloat angle;
	WPFloat distance;
	WCHeapNode *left, *right;
};


void _ConvexHullFlattenRecursive(WCHeapNode *node, std::list<WCVector4> &list) {
	//Add left children (if any)
	if (node->left != NULL) _ConvexHullFlattenRecursive(node->left, list);
	//Add this node
	list.push_back(node->point);
//	std::cout << "ConvexSort: " << node->point << std::endl;
	//Add right children (if any)
	if (node->right != NULL) _ConvexHullFlattenRecursive(node->right, list);
	//Delete the node
	delete node;
}


void _ConvexHullRecursivePlaceNode(WCHeapNode *refNode, WCHeapNode *node, const WPFloat &tol) {
	//Check same angle case
	if ( fabs(node->angle - refNode->angle) < tol) {
		//Check distances
		if (node->distance > refNode->distance) {
			//If node distance is greater, replace contents of refNode
			refNode->point = node->point;
			refNode->distance = node->distance;
		}
		//Delete the new node
		delete node;
		return;
	}
	//Check left child
	if (node->angle < refNode->angle) {
		//Check to see if there is a left-child
		if (refNode->left == NULL) refNode->left = node;
		//If there is, recurse down the tree
		else _ConvexHullRecursivePlaceNode(refNode->left, node, tol);
		return;
	}
	//Must be right child
	if (refNode->right == NULL) refNode->right = node;
	else _ConvexHullRecursivePlaceNode(refNode->right, node, tol);
}


/*** Modified version of a heap sort - sort criteria is the angle from reference point.
	Duplicate angle points have the nearest discarded ***/
void _ConvexHull2DSort(const WCVector4 &refPoint, std::list<WCVector4> &pointList, const WPFloat &tol) {
	std::list<WCVector4>::iterator pointIter = pointList.begin();
	WCHeapNode *root = new WCHeapNode(), *node;
	root->point = *pointIter;
	root->angle = atan2((*pointIter).J() - refPoint.J(), (*pointIter).I() - refPoint.I());
	root->distance = refPoint.Distance( *pointIter );
	root->left = NULL;
	root->right = NULL;
	//Loop through all of the points and build heap
	for (pointIter++ ; pointIter != pointList.end(); pointIter++) {
		node = new WCHeapNode();
		node->point = *pointIter;
		node->angle = atan2((*pointIter).J() - refPoint.J(), (*pointIter).I() - refPoint.I());
		node->distance = refPoint.Distance( *pointIter );
		node->left = NULL;
		node->right = NULL;
		_ConvexHullRecursivePlaceNode(root, node, tol);
	}
	//Flatten heap into pointList (flatten deletes the nodes)
	pointList.clear();
	_ConvexHullFlattenRecursive(root, pointList);
/*** DEBUG ***
	for (pointIter = pointList.begin(); pointIter != pointList.end(); pointIter++) 
		std::cout << "CB2DSort Point: " << *pointIter << std::endl;
/*** DEBUG ***/
}


/*** ConvexHull2D algorithm taken from "Introduction to Algorithms" by T. Cormen, C. Leiserson, R. Rivest, and C. Stein 
	 Referred to as Graham's Scan ***/
std::list<WCVector4> __WILDCAT_NAMESPACE__::ConvexHull2D(std::list<WCVector4> pointList, const WPFloat &tol) {
	//Check for size 1, 2, or 3
	if (pointList.size() < 3) return pointList;
	std::list<WCVector4> outputList;
	
	//Find the left-most lowest point
	WCVector4 lowPoint = pointList.front();
	std::list<WCVector4>::iterator iter, lowIter = pointList.begin();
	for (iter = pointList.begin(); iter != pointList.end(); iter++) {
		//Check if lowest outright, or if equal in lowness then more left
		if (((*iter).J() < lowPoint.J()) || (((*iter).J() == lowPoint.J()) && ((*iter).I() < lowPoint.I()))) {
			//Copy the point
			lowPoint = *iter;
			//Mark the iterator
			lowIter = iter;
		}
	}
	//Add low point to the output list and remove from point list
	pointList.erase(lowIter);
	outputList.push_back(lowPoint);

	//Sort the remaining points using lowPoint as reference
	_ConvexHull2DSort(lowPoint, pointList, tol);
	
	//Remove next two points from pointList and add to outputList
	WCVector4 nextToTop = pointList.front();
	outputList.push_front( pointList.front() );
	pointList.pop_front();
	WCVector4 top = pointList.front();
	outputList.push_front( pointList.front() );
	pointList.pop_front();
	
	//Process remaining points in pointList
	WPFloat angle1, angle2;
	for (iter = pointList.begin(); iter != pointList.end(); iter++) {
		//Repeat until turn is to the left
		do {
			//Calculate angle's 1 and 2
			angle1 = atan2(top.J() - nextToTop.J(), top.I() - nextToTop.I());
			if (angle1 < 0.0) angle1 += 2.0 * M_PI;
			angle2 = atan2((*iter).J() - top.J(), (*iter).I() - top.I());
			if (angle2 < 0.0) angle2 += 2.0 * M_PI;
			//If there is a non-left angle
			if (angle2 <= (angle1+tol)) {
				//Remove the no-longer-needed last element
				outputList.pop_front();
				//Get new top and nextToTop values
				top = outputList.front();
				outputList.pop_front();
				nextToTop = outputList.front();
				outputList.push_front(top);
			}
		} while ((angle2 <= (angle1+tol)) && (outputList.size() >= 2));
		//Left turn
		outputList.push_front( *iter );
		nextToTop = top;
		top = *iter;
	}
	
	//Special Case - Selected lowPoint is in line with point to the left and point to the right (due to floating point issues)
	//   0 ---- LP ---- LP-1
	//Get 0
	WCVector4 first = outputList.front();
	//Get LP-1
	iter = outputList.end(); iter--; iter--;
	WCVector4 nextLast = *iter;
	//Calculate angle from first to lowPoint
	angle1 = atan2(first.I() - lowPoint.I(), first.J() - lowPoint.J());
	if (angle1 < 0.0) angle1 += 2.0 * M_PI;
	angle2 = atan2(lowPoint.I() - nextLast.I(), lowPoint.J() - nextLast.J());
	if (angle2 < 0.0) angle2 += 2.0 * M_PI;
	//If angles are within tolerance
	if (fabs(angle1 - angle2) < tol) {
		//Remove the last point
		outputList.pop_back();
	}
/*** DEBUG ***
	//Print out the output list
	for (iter = outputList.begin(); iter != outputList.end(); iter++) std::cout << *iter << std::endl;
/*** DEBUG ***/
	//Return the convex hull
	return outputList;
}


/***********************************************~***************************************************/


std::list<WCVector4> _MinimumBoundingRectangleArea(std::list<WCVector4>::iterator firstIter,
	std::list<WCVector4> &convexHull, WPFloat &area) {
	//Give a starting point (and always moving clockwise) determine the area of the bounding rectangle
	WPFloat posDist, negDist = 0.0, thirdDist = 0.0, tmpDist = 1.0;
	std::list<WCVector4>::iterator tmpIter;
	WCVector4 initPt(*firstIter), firstPt(initPt), secondPt, thirdPt, fourthPt;

	//Find the next point (wrap if necessary)
	tmpIter = firstIter;
	tmpIter++;
	if (tmpIter == convexHull.end()) tmpIter = convexHull.begin();
	secondPt = *tmpIter;
	WCVector4 baseVec = secondPt - firstPt;
	baseVec.Normalize(true);

	//Determine base angle
	WPFloat otherAngle, baseAngle = atan2(secondPt.J() - firstPt.J(), secondPt.I() - firstPt.I());
	//Make sure no negative angles
	if(baseAngle < 0.0) baseAngle = 2.0 * M_PI + baseAngle;
	//Determine initial posValue
	posDist = firstPt.Distance(secondPt);
	
	//Loop through all remaining points to determine primary axis start and end
	for (tmpIter++; tmpDist > 0.0001 ; tmpIter++) {
		//Wrap tmpIter if necessary
		if (tmpIter == convexHull.end()) tmpIter = convexHull.begin();
		//Figure out angle
		otherAngle = atan2( (*tmpIter).J() - initPt.J(), (*tmpIter).I() - initPt.I() );
		//Make sure no negative angles
		if (otherAngle < 0.0) otherAngle = 2.0 * M_PI + otherAngle;
		//Difference between base and other angle
		otherAngle = baseAngle - otherAngle;
		tmpDist = initPt.Distance( *tmpIter );
		//Determine distance along 1st-2nd axis
		tmpDist = tmpDist * cos(otherAngle);
		//See if it falls further than posValue 
		if (tmpDist > posDist) {
			posDist = tmpDist;
			//Need to project new point onto base line
			secondPt = initPt + baseVec * tmpDist;
		}
		if (tmpDist < negDist) {
			negDist = tmpDist;
			//Need to project new point onto base line
			firstPt = initPt + baseVec * tmpDist;
		}
		//Determine third distance
		tmpDist = tmpDist * tan(otherAngle);
		//Increase thirdDist if necessary
		if (tmpDist > thirdDist) thirdDist = tmpDist;
	}
	//Calculate area
	area = (posDist - negDist) * thirdDist;

	//Calculate third and fourth points
	WCVector4 normVec(baseVec.J(), -baseVec.I(), 0.0, 0.0);
	thirdPt = secondPt + normVec * thirdDist;
	fourthPt = firstPt + normVec * thirdDist;
	//Load up the list
	std::list<WCVector4> outputList;
	outputList.push_back(firstPt);
	outputList.push_back(secondPt);
	outputList.push_back(thirdPt);
	outputList.push_back(fourthPt);
//	std::cout << "First: " << firstPt << std::endl;
//	std::cout << "Second: " << secondPt << std::endl;
//	std::cout << "Third: " << thirdPt << std::endl;
//	std::cout << "Fourth: " << fourthPt << std::endl;
	//Return the list
	return outputList;
}


/*** Algorithm taken from http://cgm.cs.mcgill.ca/~orm/maer.html called rotating calipers,
 * More detail information taken from "Computational Geometry with the Rotating Calipers",
 *  submitted as Master's Thesis by Hormoz Pirzadeh, 1999
 * Algorithm actually is just brute force for now.  Want to move to above algorithm...
 * It is important to note that the winding order of the points is maintained
***/
std::list<WCVector4> __WILDCAT_NAMESPACE__::MinimumBoundingRectangle(const std::list<WCVector4> &pointList,
	const WCMatrix4 &toPlane, const WCMatrix4 &fromPlane) {
	//Check for size 1 or 2
	if (pointList.size() <= 2) return pointList;
	
	std::list<WCVector4>::const_iterator citer;
	WCVector4 vec;
	//Convert all points to planar form
	std::list<WCVector4> planarPoints;
	for(citer = pointList.begin(); citer != pointList.end(); citer++) {
		vec = toPlane * (*citer);
		planarPoints.push_back( vec );
//		std::cout << "MinRec: " << vec << std::endl;
	}

	//Generate the convex hull of the planar points
	std::list<WCVector4> convexHull = ConvexHull2D(planarPoints, 0.0001);
	std::list<WCVector4>::iterator convexIter;
/*** DEBUG ***
	for(convexIter = convexHull.begin(); convexIter != convexHull.end(); convexIter++)
		std::cout << "Convex: " << *convexIter << std::endl;
/*** DEBUG ***/

	//Find the smallest bounding rectangle
	std::list<WCVector4> tmpList, minList;
	WPFloat tmpArea, minArea = 10000000000.0;
	//Test each point as a starting point for a rectangle
	for(convexIter = convexHull.begin(); convexIter != convexHull.end(); convexIter++) {
		tmpList = _MinimumBoundingRectangleArea(convexIter, convexHull, tmpArea);
//		std::cout << "TmpArea: " << tmpArea << std::endl;
		//Check to see if this one wins
		if (tmpArea < minArea) {
			minList = tmpList;
			minArea = tmpArea;
		}
	}
	
	//Convert all points back to world-form
	for(std::list<WCVector4>::iterator rectIter = minList.begin(); rectIter != minList.end(); rectIter++) {
/*** DEBUG ***
		std::cout << "MinRec: " << *rectIter << std::endl;
/*** DEBUG ***/
		(*rectIter) = fromPlane * (*rectIter);
	}
	//Rectangle found, return the value
	return minList;
}


/*** Boundary List Algorithm ***
 * This algorithm builds a list of boundary points (in WCVector4 format) for the list of curves.  The list must be clockwise oriented.
 *	The only tricky part of this algorithm is that the end point of one curve is the same as the starting point of the next curve.  So,
 *	in order to not have duplicate points the last point (remember to keep curve direction in mind) is ignored.
 ***/
void __WILDCAT_NAMESPACE__::BuildBoundaryList(std::list<std::pair<WCGeometricCurve*,bool> > &curveList, std::list<WCVector4> &outputList,
	const bool &detailed, const WPFloat &tolerance) {
//	std::list<WCVector4> pointList;
	WCGeometricLine *line;
	WCNurbsCurve *nurb;
	std::vector<WCVector4> controlPoints;
	WCVector4 point;

	//Make sure the output list is clear
	outputList.clear();

	//Go through each curve and get boundary points
	std::list< std::pair<WCGeometricCurve*,bool> >::iterator curveIter;
	for(curveIter = curveList.begin(); curveIter != curveList.end(); curveIter++) {
		//Try to cast to a line
		line = dynamic_cast<WCGeometricLine*>((*curveIter).first);
		//If it is a line, process as such
		if (line != NULL) {
			//Process forwards line (only get end of line)
			if ((*curveIter).second) outputList.push_back( line->End() );
			//Process backwards line
			else outputList.push_back( line->Begin() );
		}
		//Try to cast to a nurbs curve
		else {
			nurb = dynamic_cast<WCNurbsCurve*>((*curveIter).first);
			//Make sure it is a nurbs curve
			if (nurb != NULL) {
				//If detailed, get all evaluated points
				if (detailed) {
					//Need to determine best LOD
					WPUInt lod = 256; //(WPUInt)(nurb->Length(tolerance) / tolerance);
					GLfloat* data = nurb->GenerateClientBuffer(lod, true);
					//Process forwards
					if ((*curveIter).second) {
						for (WPUInt i=1; i<lod; i++) {
							//Get point data
							point.Set(data[i*4], data[i*4+1], data[i*4+2], 1.0);
							//Put point into list
							outputList.push_back(point);
						}
					}
					//Process backwards
					else {
						for (int i=lod-2; i>=0; i--) {
							//Get point data
							point.Set(data[i*4], data[i*4+1], data[i*4+2], 1.0);
							//Put point into list
							outputList.push_back(point);
						}
					}
					//Clean up the data from the curve
					nurb->ReleaseBuffer(data);
				}
				//Gather just the control points
				else {
					//Get the list of control points
					controlPoints = nurb->ControlPoints();
					//Process curve forwards
					if ((*curveIter).second) {
						//Go through all control points (make sure to skip the first)
						for(WPUInt i=1; i<controlPoints.size(); i++) {
							//Copy the vector base
							point = controlPoints.at(i);
							//Set the weight to 1.0
							point.L(1.0);
							//Add point to point line
							outputList.push_back(point);
						}
					}
					//Process curve backwards
					else {
						//Go through all control points (make sure to skip the first)
						for(int i=(int)controlPoints.size()-2; i>=0; i--) {
							//Copy the vector base
							point = controlPoints.at(i);
							//Set the weight to 1.0
							point.L(1.0);
							//Add point to point line
							outputList.push_back(point);
						}				
					}
				}
			}
			//Error case (unknown curve type)
			else {
				CLOGGER_ERROR(WCLogManager::RootLogger(), "GeometricAlgorithms::BuildBoundaryList - Unknown curve type.");
				//throw error
				return;
			}
		}
	}
/*** DEBUG ***
	std::cout << "Boundary Points --->\n";
	for (std::list<WCVector4>::iterator ptIter=outputList.begin(); ptIter != outputList.end(); ptIter++) {
		std::cout << *ptIter << std::endl;
	}
/*** DEBUG ***/
}


/***********************************************~***************************************************/

