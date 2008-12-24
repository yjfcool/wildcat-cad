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
#include <Geometry/geometric_algorithms.h>


/***********************************************~***************************************************/


struct TriangulateVertex {
	int id;
	WCVector4 vertex;
	WPFloat internalAngle;
	TriangulateVertex *left, *right;
};


/***********************************************~***************************************************/


WPFloat _TriangulateInternalAngle(TriangulateVertex* vert) {
	WCVector4 v0 = vert->left->vertex - vert->vertex;
	WCVector4 v1 = vert->right->vertex - vert->vertex;
	WCVector4 cross = v1.CrossProduct(v0);
	//Determine the angle
	WPFloat retVal = acos( v0.DotProduct(v1) / (v0.Magnitude() * v1.Magnitude()) );
	//If cross.K is negative, invert the angle
	if (cross.K() < 0.0) retVal =  2.0 * M_PI - retVal;
	return retVal;
}


bool _TriangulateIsInside(WCVector4 &ear, WCVector4 &left, WCVector4 &right, WCVector4 &pt) {
	//Check if on right of ear to left, left to right, right to ear
	bool earToLeft = IsOnRight2D(ear.I(), ear.J(), left.I(), left.J(), pt.I(), pt.J());
	bool leftToRight = IsOnRight2D(left.I(), left.J(), right.I(), right.J(), pt.I(), pt.J());
	bool rightToEar = IsOnRight2D(right.I(), right.J(), ear.I(), ear.J(), pt.I(), pt.J());
	//Must be on right side of all three to be inside
	if (earToLeft && leftToRight && rightToEar) return true;
	else return false;
}


std::list<TriangulateVertex*>::iterator _TriangulateFindEar(std::list<TriangulateVertex*> &vertexList) {
	std::list<TriangulateVertex*>::iterator outerIter, innerIter;
	TriangulateVertex* tmpVert;
	bool noneInside;
	
	//Outer loop
	for (outerIter = vertexList.begin(); outerIter != vertexList.end(); outerIter++) {
		//Reset none inside
		noneInside = true;
		//Only consider ears that are reflex
		if ( (*outerIter)->internalAngle < M_PI ) {
			tmpVert = (*outerIter)->left->left;
			//Inner loop
			do {
				//Test to see if this vertex is inside the triangle formed with outerIter as the ear
				if (_TriangulateIsInside( (*outerIter)->vertex, (*outerIter)->left->vertex, (*outerIter)->right->vertex, tmpVert->vertex ))
					noneInside = false;
				//Otherwise, move to the next vert
				tmpVert = tmpVert->left;
			//Keep going until right is reached
			} while (tmpVert != (*outerIter)->right);
			//Check to see if noneInside is true
			if (noneInside) return outerIter;
		}
	}
	//Error - should not reach here
	CLOGGER_ERROR(WCLogManager::RootLogger(), "TriangulateFindEar - no ear found in entire profile:" << vertexList.size());
	for (outerIter = vertexList.begin(); outerIter != vertexList.end(); outerIter++) {
		std::cout << "E: " << (*outerIter)->vertex << std::endl;
	}
	return outerIter;
}


/***********************************************~***************************************************/


GLint* __WILDCAT_NAMESPACE__::TriangulatePolygon(const std::list<WCVector4> &pointList) {
/*** DEBUG ***
	std::list<WCVector4>::const_iterator plIter;
	for (plIter = pointList.begin(); plIter != pointList.end(); plIter++) std::cout << *plIter << std::endl;
/*** DEBUG ***/
	//Make sure there are at least 4 points
	if (pointList.size() < 4) {
		//Trivial case
		if (pointList.size() == 3) {
			GLint *tris = new GLint[3];
			tris[0] = 0;
			tris[1] = 1;
			tris[2] = 2;
			return tris;
		}
		//Error case
		CLOGGER_ERROR(WCLogManager::RootLogger(), "TriangulatePolygon - Too few points.");
		//throw error
		return NULL;
	}
	
	//Create vertex structs for each point
	std::list<WCVector4>::const_iterator pointIter = pointList.begin();
	std::list<TriangulateVertex*> vertexList;
	int id = 0;
	TriangulateVertex *vert, *rightVert = NULL;
	//Process remaining vertices
	for (pointIter = pointList.begin(); pointIter != pointList.end(); pointIter++) {
		//Create the new vertex
		vert = new TriangulateVertex();
		//See about cleaning up last vert
		if (rightVert != NULL) rightVert->left = vert;
		vert->id = id++;
		vert->vertex = *pointIter;
		vert->right = rightVert;
		//Add vertex to list and vector
		vertexList.push_back(vert);
		//Swap vert into rightVert
		rightVert = vert;
	}
	//Clean up first and last verts
	vertexList.front()->right = vertexList.back();
	vertexList.back()->left = vertexList.front();

	//Determine the type of each vertex (and list it)
	std::list<TriangulateVertex*>::iterator vertIter;
	for (vertIter = vertexList.begin(); vertIter != vertexList.end(); vertIter++) {
		(*vertIter)->internalAngle = _TriangulateInternalAngle( *vertIter );
//		std::cout << "Vert(" << *vertIter << "): " << (*vertIter)->id << ", " << (*vertIter)->internalAngle << " :: " << (*vertIter)->vertex << std::endl;
//		std::cout << "\tLeft: " << (*vertIter)->left << ", Right: " << (*vertIter)->right << std::endl;
	}

	//Triangle list
	GLint *triangles = new GLint[(pointList.size() - 2) * 3];
	GLint triIndex = 0;
	std::list<TriangulateVertex*>::iterator earIter;
	
	do {
		//Find an ear
		earIter = _TriangulateFindEar(vertexList);
		//Add a triangle
		triangles[triIndex*3] = (*earIter)->right->id;
		triangles[triIndex*3+1] = (*earIter)->id;
		triangles[triIndex*3+2] = (*earIter)->left->id;
		triIndex++;
		//Remove the ear (both in list and in linked-list
		(*earIter)->left->right = (*earIter)->right;
		(*earIter)->right->left = (*earIter)->left;
		//Recompute angles for left and right points
		(*earIter)->left->internalAngle = _TriangulateInternalAngle( (*earIter)->left );
		(*earIter)->right->internalAngle = _TriangulateInternalAngle( (*earIter)->right );
		vertexList.erase(earIter);
	//Keep going until only 3 vertices are left
	} while (vertexList.size() > 3);
	//Add the last triangle
	earIter = vertexList.begin();
	triangles[triIndex*3] = (*earIter)->right->id;
	triangles[triIndex*3+1] = (*earIter)->id;
	triangles[triIndex*3+2] = (*earIter)->left->id;	
	//Return the list of triangles
	return triangles;
}


GLint* __WILDCAT_NAMESPACE__::TriangulatePolygon(GLfloat *pointList, const GLuint &numPoints) {
	//Really really hacky -- create list of points
	std::list<WCVector4> pList;
	WCVector4 pt;
	for (GLuint i=0; i<numPoints; i++) {
		//Create point
		pt.Set(pointList[i*4], pointList[i*4+1], 0.0, 1.0);
		pList.push_back(pt);
	}
	//Return the pointer to the list
	return TriangulatePolygon(pList);
}

