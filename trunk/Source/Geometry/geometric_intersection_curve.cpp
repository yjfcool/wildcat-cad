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
#include "Geometry/geometric_intersection.h"
#include "Geometry/geometry_context.h"
#include "Geometry/geometric_point.h"
#include "Geometry/geometric_line.h"
#include "Geometry/nurbs_curve.h"


/*** Locally Defined Values ***/
#define CLI_LINE_CUTOFF							0.075
#define CCI_CURVE_CUTOFF						3


/***********************************************~***************************************************/


/*** GeometricIntersection -- Curve and Point ***
 * This algorithm tries to intersect a point with a NURBS curve.  The result can be zero to many hits.  The approach is to
 *	generate the curve into a number of line segments and intersect the point with each line segment.  There are a few things to remember:
 *		1) Point must intersect a given line segment within tolerance
 *		2) Point must be > tol parametric value (u) after the last point - avoids case of point being very close to where segments meet
 *		3) Must check flags for CULL_BOUNDARY case and only add hit as appropriate
 *		4) Make sure to delete generated segement data
***/
std::list<WCIntersectionResult> __WILDCAT_NAMESPACE__::GeometricIntersection(WCNurbsCurve *left,
	WCGeometricPoint *right, const WPFloat &tol, const unsigned int &flags) {
	std::list<WCIntersectionResult> results;
	//See if point is within curve bounding box
	//...

	//Determine estimated length of curve and LOD
	WPUInt lod = 128; //(WPUInt)(left->EstimateLength() / tol);
	GLfloat *data = left->GenerateClientBuffer(lod, true);

	//Initialize the p0 vector and other variables
	WCVector4 p0((WPFloat)data[0], (WPFloat)data[1], (WPFloat)data[2], 1.0);
	WCVector4 p1, direction, tmpVec;
	WPFloat u, dist, lastU=-1.0;
	WPUInt index=4;
	WPFloat paraFactor = 1.0 / (lod - 1);

	//Loop through remaining vertices and test each
	for (WPUInt i=1; i<lod; i++) {
		//Determine p1
		p1.Set( (WPFloat)data[index], (WPFloat)data[index+1], (WPFloat)data[index+2], 1.0 );
		//See if there is an intersection between line and point segment
		direction = p1 - p0;
		dist = p0.Distance(p1);
		//Get point vector
		tmpVec = right->Data() - p0;
		u = direction.DotProduct(tmpVec) / (dist * dist);
		//Bounds check u
		if ((u > -tol) && (u < (1.0 + tol))) {
			//Bound u
			u = STDMAX( STDMIN(u, 1.0), 0.0);
			//Get point on segment
			tmpVec = p0 + direction * u;
			//See distance is within tolerance
			if (tmpVec.Distance(right->Data()) <= tol) {
				//Convert u from segment to curve
				u = (i - 1 + u) * paraFactor;
				//Do consecutive hit check (u is always greater than lastU)
				if (u - lastU > tol) {
					//Create a hit result
					WCIntersectionResult hit;
					hit.type = IntersectPoint;
					hit.leftBoundary = (u < tol) || (1.0 - u < tol);
					hit.rightBoundary = true;
					hit.leftParam.I(u);
					hit.rightParam = right->Data();
					//Do boundary cull check, insert if ok
					if (!(flags * INTERSECT_CULL_BOUNDARY) || !hit.leftBoundary) {
//						std::cout << hit;
						//Add the hit to the list
						results.push_back(hit);
					}
					//Set the new last u
					lastU = u;
				}
			}
		}
		//Move p1 to p0
		p0 = p1;
		index += 4;
	}
	//Clean up and return the hit list
	left->ReleaseBuffer(data);
	return results;
}


/***********************************************~***************************************************/


std::list<WCIntersectionResult> __WILDCAT_NAMESPACE__::GeometricIntersection(WCNurbsCurve *left,
	 WCGeometricLine *right, const WPFloat &tol, const unsigned int &flags) {
	std::list<WCIntersectionResult> results;
	//See if line intersects curve bounding box
	//...

	/*** Setup programs and texture locations ***/
	
	WPUInt lod = 512;// (WPUInt)(left->EstimateLength() / tol);
	WPFloat paraFactor = 1.0 / lod, mua, mub;
	WCVector4 point;
	GLfloat *leftData = left->GenerateClientBuffer(lod, true);

	//Set program values
	glUseProgram(left->Context()->CurveLineProgram());
	glUniform2f(left->Context()->IntersectionLocations()[INTERSECTION_CLI_PARAMS], (GLfloat)lod, (GLfloat)tol);
	glUniform4f(left->Context()->IntersectionLocations()[INTERSECTION_CLI_LBEGIN], 
				(GLfloat)right->Begin().I(), (GLfloat)right->Begin().J(), (GLfloat)right->Begin().K(), 1.0);
	glUniform4f(left->Context()->IntersectionLocations()[INTERSECTION_CLI_LEND],
				(GLfloat)right->End().I(), (GLfloat)right->End().J(), (GLfloat)right->End().K(), 1.0);
	
	/*** Bind to framebuffer object ***/
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, left->Context()->CurveCurveFramebuffer());
	//Check to make sure the framebuffer is ready
	GLenum retVal = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	//Check the status of the framebuffer object
	if (retVal != GL_FRAMEBUFFER_COMPLETE_EXT) { 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "_CurveLine Intersection - Framebuffer is not complete.");
		return results;
	}

	/*** Setup Left texture ***/
	
	//Setup and copy the data into the texture
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//Set up texture
	glActiveTexture(GL_TEXTURE0);	
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, left->Context()->CCILeftTex());	
	glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, lod, 1, GL_RGBA, GL_FLOAT, leftData);

	/*** Setup Viewport and Render***/
	
	//Save the viewport setting
	glPushAttrib(GL_VIEWPORT_BIT);
	//Disable some settings
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	//Set the viewport
	glViewport(0, 0, lod, 1);
	//Draw into the framebuffer
	GLfloat vertData[] = { -1.0, -1.0, -1.0, 1.0, 1.0, 1.0, 1.0, -1.0 };
	//Turn on vertex arrays
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, vertData);
	//--- Draw ---
	glDrawArrays(GL_QUADS, 0, 4);
	//Clean up
	glDisableClientState(GL_VERTEX_ARRAY);
	glPopAttrib();
	//Re-enable some settings
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	//Check for errors
	if (glGetError() != GL_NO_ERROR)
		CLOGGER_ERROR(WCLogManager::RootLogger(), "_CurveLine Intersection - Render.");

	/*** Save output texture into array using simple memory read ***/
	
	GLfloat *cciData = new GLfloat[lod * 4];
	glReadPixels(0, 0, lod, 1, GL_RGBA, GL_FLOAT, cciData);
	//Clean up the framebuffer object and cp/kp textures
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	//Stop using a program
	glUseProgram(0);
	//Check for errors
	if (glGetError() != GL_NO_ERROR)
		CLOGGER_ERROR(WCLogManager::RootLogger(), "_CurveLine Intersection - Clean up.");
/*** DEBUG ***
//	std::cout << "Debug output for CLI\n";
	for (int i=0; i<lod; i++) {
		 if (cciData[i*4] != -1.0) {
			 std::cout << i << ": " << leftData[i*4] << ", " << leftData[i*4+1] << ", " << leftData[i*4+2] << ", " << leftData[i*4+3] << std::endl;
			 std::cout << i << ": " << cciData[i*4] << ", " << cciData[i*4+1] << ", " << cciData[i*4+2] << ", " << cciData[i*4+3] << std::endl;
		 }
	}
/*** DEBUG ***/

	/*** Scan output for intersection results ***/
	
	WCIntersectionResult hit;
	WPUInt lookAhead, diff;
	std::vector<WCVector4> curvePoints;
	//Loop through the output
	for (WPUInt i=0; i<lod; i++) {
		//A non -1.0 in the first place indicates a hit
		if (cciData[i*4] != -1.0) {
			//Look ahead to see how many hits in a row there are
			lookAhead = i + 1;
			while ((cciData[lookAhead * 4] != -1.0) && (lookAhead <lod)) lookAhead++;
			diff = lookAhead - i;
			//Get the first and last points
			WCVector4 start(cciData[i*4+1], cciData[i*4+2], cciData[i*4+3], 1.0);
			WCVector4 end(cciData[lookAhead*4-3], cciData[lookAhead*4-2], cciData[lookAhead*4-1], 1.0);
			WPFloat dist = start.Distance(end);
			//See how long the overlap is
			if (dist > CLI_LINE_CUTOFF) {
				//Determine left and right start and stop parametric values
				WPFloat leftStart = i * paraFactor;
				WPFloat leftEnd = (lookAhead-1) * paraFactor;
				WPFloat rightStart = cciData[i*4];
				WPFloat rightEnd = cciData[lookAhead*4-4];
				//Set hit param values
				hit.leftParam.I(leftStart);
				hit.leftParam.J(leftEnd);
				hit.rightParam.I(rightStart);
				hit.rightParam.J(rightEnd);
				//Set hit type
				hit.type = IntersectLine;
				//Set hit boundary values
				hit.leftBoundary = ( (leftStart < paraFactor) || (leftEnd > 1.0-paraFactor) );
				hit.rightBoundary = ( (rightStart < paraFactor) || (rightEnd > 1.0-paraFactor) );
				//Check if culling end-point intersections
				if (!(flags & INTERSECT_CULL_BOUNDARY) ||  (!hit.leftBoundary && !hit.rightBoundary)) {
					//See if genObj
					if (flags & INTERSECT_GEN_LINES) {
						//Determine line start and end values
						WCVector4 ptA = right->Evaluate(rightStart);
						WCVector4 ptB = right->Evaluate(rightEnd);
						//Create new line
						WCGeometricLine *newLine = new WCGeometricLine(ptA, ptB);
						//Add to result struct
						hit.object = newLine;
					}
					else hit.object = NULL;
					//Add the intersection to the list
//					std::cout << "CLI - " << hit << std::endl;
					results.push_back(hit);
				}
			}
			//Diff <= CLI_CURVE_CUTOFF is a point
			else {
				//Find average of mua, mub, and point
				mua = 0.0;
				mub = 0.0;
				point.Set(0.0, 0.0, 0.0, 0.0);
				//Accumulate the values
				for (WPUInt j=i; j<lookAhead; j++) {
					mua += (j * paraFactor);
					mub += cciData[j*4];
					point += WCVector4(cciData[j*4+1], cciData[j*4+2], cciData[j*4+3], 1.0);
				}
				//Average the values
				mua = STDMAX(0.0, STDMIN(mua / diff, 1.0));
				//Determine actual overall parametric value on right
				mub = STDMAX(0.0, STDMIN(mub / diff, 1.0));
				//Get intersection point
				point /= diff;
				//Create intersection result
				hit.type = IntersectPoint;
				hit.leftParam.I(mua);
				hit.rightParam.I(mub);
				hit.leftBoundary = (mua <= paraFactor) || (mua >= 1.0 - paraFactor);
				hit.rightBoundary = (mub <= paraFactor) || (mub >= 1.0 - paraFactor);
				//Check if culling end-point intersections
				if (!(flags & INTERSECT_CULL_BOUNDARY) ||  (!hit.leftBoundary && !hit.rightBoundary)) {
					//See if genObj
					if (flags & INTERSECT_GEN_POINTS) {
						//Create new point
						WCGeometricPoint *newPoint = new WCGeometricPoint(point);
						//Add to result struct
						hit.object = newPoint;
					}
					else hit.object = NULL;
					//Add the intersection to the list
//					std::cout << "CLI - " << hit << point << std::endl;
					results.push_back(hit);
				}
			}
			//Increment i to lookAhead
			i = lookAhead;
		}
	}
	//Delete data array
	left->ReleaseBuffer(leftData);
	//Delete the data buffer
	delete cciData;
	//Return the results
	return results;
}

/***********************************************~***************************************************/


/*** GeometricIntersection -- Curve and Line ***
 * This algorithm tries to intersect a line with a NURBS curve.  The result can be zero to many hits and may include a linear overlaps.
 * General approach is as follows:
 *		1) Generate curve as multiple line segments
 *		2) Intersect each segment with the line.  Store all hit values.
 *		3) If there is a parallel intersection (overlap), need to 
 ***
std::list<WCIntersectionResult> __WILDCAT_NAMESPACE__::GeometricIntersection(WCNurbsCurve *left,
	WCGeometricLine *right, const WPFloat &tol, const unsigned int &flags) {
	std::list<WCIntersectionResult> results;
	//See if line intersects curve bounding box
	//...

	//Determine estimated length of curve and LOD
	WPUInt lod = 256;// (WPUInt)(left->EstimateLength() / tol);
	GLfloat *data = left->GenerateClientBuffer(lod, true);
	//Evaluate both lines at both ends
	WCVector4 p3 = right->Begin();
	WCVector4 p4 = right->End();

	//Set up lots of variables
	WCVector4 p1((WPFloat)data[0], (WPFloat)data[1], (WPFloat)data[2], 1.0), p2;
	//Get length vectors
	WCVector4 p21, cross, pointOnFirst, pointOnSecond;
	WCVector4 p43(p4 - p3);
	WPFloat dist, d121, d2121, d321, d4321, d143, d343, d4343, denom, numer, mua, mub;
	WPUInt index=4;
	WPFloat paraFactor = 1.0 / (lod - 1);
	WPFloat onePlusTol = 1.0 + tol;
	//Loop through all curve segments
	for (WPUInt i=0; i<lod-1; i++) {
		//Determine secVec
		p2.Set( (WPFloat)data[index], (WPFloat)data[index+1], (WPFloat)data[index+2], 1.0 );
		p21 = p2 - p1;
		p43 = p4 - p3;
		//Check for parallel
		cross = p43.CrossProduct(p21);
		dist = cross.Magnitude();
		if (dist < 0.001) {
			//For now assume parallel is no intersection
//			CLOGGER_WARN(WCLogManager::RootLogger(), "GeometricIntersection::CurveLine - Parallel Intersection.");
		}
		//Non-parallel case
		else {
			//Many dot products
			d121 = p1.DotProduct(p21);				// L
			d2121 = p21.DotProduct(p21);			// M
			d321 = p3.DotProduct(p21);				// N
			d4321 = p43.DotProduct(p21);			// O
			d143 = p1.DotProduct(p43);				// Q
			d343 = p3.DotProduct(p43);				// R
			d4343 = p43.DotProduct(p43);			// S
			denom = (d4343 * d2121) - (d4321 * d4321);
			//Some further calculations...
			numer = d4343 * (d321 - d121) + d4321 * (d143 - d343);
			mua = STDMAX(0.0, STDMIN(numer / denom, 1.0));
			mub = STDMAX(0.0, STDMIN((d143 + d4321 * mua - d343) / d4343, 1.0));
			//Make sure mua and mub are in (0.0 +- tol, 1.0 +- tol)
			if ( (mua < onePlusTol) && (mua > -tol) && (mub < onePlusTol) && (mub > -tol) ) {
				//Calculate points and distance between them
				pointOnFirst = p1 + p21 * mua;
				pointOnSecond = p3 + p43 * mub;
				//Make sure lines are not > tol apart
				dist = pointOnFirst.Distance(pointOnSecond);
				if (dist < tol) {
					//Determine actual overall parametric value on left
					mua = (i - 1 + mua) * paraFactor;
					//Create intersection result
					WCIntersectionResult hit;
					hit.type = IntersectPoint;
					hit.leftParam.I(mua);
					hit.rightParam.I(mub);
					hit.leftBoundary = (fabs(mua) < tol) || (fabs(mua-1.0) < tol);
					hit.rightBoundary = (fabs(mub) < tol) || (fabs(mub-1.0) < tol);
					
					//Check if culling end-point intersections
					if (!(flags & INTERSECT_CULL_BOUNDARY) ||  (!hit.leftBoundary && !hit.rightBoundary)) {
						//See if genObj
						if (flags & INTERSECT_GEN_POINTS) {
							//Create new point
							WCGeometricPoint *newPoint = new WCGeometricPoint(pointOnFirst);
							//Add to result struct
							hit.object = newPoint;
						}
						else hit.object = NULL;
						//Add the intersection to the list
						std::cout << "CLI - " << hit << pointOnFirst << std::endl;
						results.push_back(hit);
					}
				}
			}
		}
		//Move p2 to p1
		p1 = p2;
		index += 4;
	}	
	//Clean up and return the hit list
	left->ReleaseBuffer(data);
	return results;
}


/***********************************************~***************************************************/


/*** GeometricIntersection -- Curve and Curve ***
 * This algorithm tries to intersect a NURBS curve with a NURBS curve.  The result can be zero to many hits and may include an extended overlap.
 ***/
std::list<WCIntersectionResult> __WILDCAT_NAMESPACE__::GeometricIntersection(WCNurbsCurve *left, WCNurbsCurve *right,
	const WPFloat &tol, const unsigned int &flags) {
	std::list<WCIntersectionResult> results;
	//Check if self intersection
	if (left == right) return results;
	
	//See if bounding boxes intersect
	//...
	
	/*** Setup programs and texture locations ***/
	
	WPUInt lod = 512;// (WPUInt)(left->EstimateLength() / tol);
	WPFloat paraFactor = 1.0 / lod, mua, mub;
	WCVector4 point;
	GLfloat *leftData = left->GenerateClientBuffer(lod, true);
	GLfloat *rightData = right->GenerateClientBuffer(lod, true);
	//Set program values
	glUseProgram(left->Context()->CurveCurveProgram());
	glUniform2f(left->Context()->IntersectionLocations()[INTERSECTION_CCI_PARAMS], (GLfloat)lod, (GLfloat)tol);

	/*** Bind to framebuffer object ***/
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, left->Context()->CurveCurveFramebuffer());
	//Check to make sure the framebuffer is ready
	GLenum retVal = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	//Check the status of the framebuffer object
	if (retVal != GL_FRAMEBUFFER_COMPLETE_EXT) { 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "_CurveCurve Intersection - Framebuffer is not complete.");
		return results;
	}
	
	/*** Setup Left texture ***/
	
	//Setup and copy the data into the texture
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//Set up texture
	glActiveTexture(GL_TEXTURE0);	
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, left->Context()->CCILeftTex());	
	glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, lod, 1, GL_RGBA, GL_FLOAT, leftData);
	//Delete data array
	left->ReleaseBuffer(leftData);
	
	/*** Setup Right texture ***/
	
	//Setup and copy the data into the texture
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//Set up texture
	glActiveTexture(GL_TEXTURE1);	
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, left->Context()->CCIRightTex());	
	glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, lod, 1, GL_RGBA, GL_FLOAT, rightData);
	//Delete data array
	right->ReleaseBuffer(rightData);
	
	
	/*** Setup Viewport and Render***/
	
	//Save the viewport setting
	glPushAttrib(GL_VIEWPORT_BIT);
	//Disable some settings
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	//Set the viewport
	glViewport(0, 0, lod, 1);
	//Draw into the framebuffer
	GLfloat vertData[] = { -1.0, -1.0, -1.0, 1.0, 1.0, 1.0, 1.0, -1.0 };
	//Turn on vertex arrays
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, vertData);
	//--- Draw ---
	glDrawArrays(GL_QUADS, 0, 4);
	//Clean up
	glDisableClientState(GL_VERTEX_ARRAY);
	glPopAttrib();
	//Re-enable some settings
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	//Check for errors
	if (glGetError() != GL_NO_ERROR)
		CLOGGER_ERROR(WCLogManager::RootLogger(), "_CurveCurve Intersection - Render.");
	
	/*** Save output texture into array using simple memory read ***/
	
	GLfloat *cciData = new GLfloat[lod * 4];
	glReadPixels(0, 0, lod, 1, GL_RGBA, GL_FLOAT, cciData);
	//Clean up the framebuffer object and cp/kp textures
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	//Stop using a program
	glUseProgram(0);
	//Check for errors
	if (glGetError() != GL_NO_ERROR)
		CLOGGER_ERROR(WCLogManager::RootLogger(), "_CurveCurve Intersection - Clean up.");
/*** DEBUG ***
	for (int i=0; i<lod; i++) {
		if (cciData[i*4] != -1.0) {
			CLOGGER_ERROR(WCLogManager::RootLogger(), i << ": " << cciData[i*4] << ", " << cciData[i*4+1] << ", " << cciData[i*4+2] << ", " << cciData[i*4+3]);
		}
	}
/*** DEBUG ***/

	/*** Scan output for intersection results ***/

	WCIntersectionResult hit;
	WPUInt lookAhead, diff;
	std::vector<WCVector4> curvePoints;
	//Loop through the output
	for (WPUInt i=0; i<lod; i++) {
		//A non -1.0 in the first place indicates a hit
		if (cciData[i*4] != -1.0) {
			//Look ahead to see how many hits in a row there are
			lookAhead = i + 1;
			while ((cciData[lookAhead * 4] != -1.0) && (lookAhead <lod)) lookAhead++;
			diff = lookAhead - i;
			//Diff > CCI_CURVE_CUTOFF is a curve
			if (diff > CCI_CURVE_CUTOFF) {
				//Determine start and stop parameters for left
				hit.type = IntersectCurve;
				hit.leftParam.I( STDMAX(0.0, STDMIN(i * paraFactor, 1.0)) );
				hit.leftParam.J( STDMAX(0.0, STDMIN((lookAhead-1) * paraFactor, 1.0)) );
				//Determine start and stop parameters for right
				hit.rightParam.I(0.0);
				hit.rightParam.J(1.0);
				//Determine boundary values
				hit.leftBoundary = false;
				hit.rightBoundary = false;
				//Check if culling end-point intersections
				if (!(flags & INTERSECT_CULL_BOUNDARY) ||  (!hit.leftBoundary && !hit.rightBoundary)) {
					//See if genObj
					if (flags & INTERSECT_GEN_CURVES) {
						//Accumulate the points into a vector
						for (WPUInt j=i; j<lookAhead; j++) {
							//Get the point data
							point = WCVector4(cciData[j*4+1], cciData[j*4+2], cciData[j*4+3], 1.0);
							//Add to the vector
							curvePoints.push_back(point);
						}
						//Create the curve
						WCNurbsCurve* newCurve = WCNurbsCurve::GlobalInterpolation(curvePoints, 3);
						//Add to result struct
						hit.object = newCurve;
					}
					else hit.object = NULL;
					//Add the intersection to the list
//					std::cout << "CCI - " << hit << point << std::endl;
					results.push_back(hit);
				}
			}
			// Diff <= CCI_CURVE_CUTOFF, must be a point
			else {
				//Find average of mua, mub, and point
				mua = 0.0;
				mub = 0.0;
				point.Set(0.0, 0.0, 0.0, 0.0);
				//Accumulate the values
				for (WPUInt j=i; j<lookAhead; j++) {
					mua += (j * paraFactor);
					mub += (cciData[j*4] * paraFactor);
					point += WCVector4(cciData[j*4+1], cciData[j*4+2], cciData[j*4+3], 1.0);
				}
				//Average the values
				mua = STDMAX(0.0, STDMIN(mua / diff, 1.0));
				//Determine actual overall parametric value on right
				mub = STDMAX(0.0, STDMIN(mub / diff, 1.0));
				//Get intersection point
				point /= diff;
				//Create intersection result
				hit.type = IntersectPoint;
				hit.leftParam.I(mua);
				hit.rightParam.I(mub);
				hit.leftBoundary = (mua <= paraFactor) || (mua >= 1.0 - paraFactor);
				hit.rightBoundary = (mub <= paraFactor) || (mub >= 1.0 - paraFactor);
				//Check if culling end-point intersections
				if (!(flags & INTERSECT_CULL_BOUNDARY) ||  (!hit.leftBoundary && !hit.rightBoundary)) {
					//See if genObj
					if (flags & INTERSECT_GEN_POINTS) {
						//Create new point
						WCGeometricPoint *newPoint = new WCGeometricPoint(point);
						//Add to result struct
						hit.object = newPoint;
					}
					else hit.object = NULL;
					//Add the intersection to the list
//					std::cout << "CCI - " << hit << point << std::endl;
					results.push_back(hit);
				}
			}
			//Increment i to lookAhead
			i = lookAhead;
		}
	}
	//Delete the data buffer
	delete cciData;
	
	//Return the results
	return results;
}


/***********************************************~***************************************************/


/*** ***
 *
 *
 ***/
std::list<WCIntersectionResult> __WILDCAT_NAMESPACE__::GeometricIntersection(WCNurbsCurve *left,
	WCNurbsSurface *right, const WPFloat &tol, const unsigned int &flags) {
	//Get intersection list
	std::list<WCIntersectionResult> results = GeometricIntersection(right, left, tol, flags);
	//Return the reversed the list
	return IntersectionReverse( results );
}


/***********************************************~***************************************************/


/*** ***
 *
 *
 ***/
std::list<WCIntersectionResult> __WILDCAT_NAMESPACE__::GeometricIntersection(WCNurbsCurve *left,
	WCTrimmedNurbsSurface *right, const WPFloat &tol, const unsigned int &flags) {
	//Get intersection list
	std::list<WCIntersectionResult> results = GeometricIntersection(right, left, tol, flags);
	//Return the reversed the list
	return IntersectionReverse( results );
}


/***********************************************~***************************************************/

