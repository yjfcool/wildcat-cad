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
#include "Geometry/trimmed_nurbs_surface.h"
#include "Geometry/geometry_context.h"
#include "Geometry/nurbs_curve.h"
#include "Geometry/nurbs.h"
#include "Geometry/geometric_line.h"
#include "Geometry/geometric_algorithms.h"
#include "Geometry/ray.h"


/*** Locally Defined Values ***/
#define TRIMSURFACE_INVERSION_MAX_ITERATIONS	12
#define TRIMSURFACE_EPSILON_ONE					0.0001
#define TRIMSURFACE_EPSILON_TWO					0.0001
#define TRIMSURFACE_EQUALITY_EPSILON			0.001
#define TRIMSURFACE_RENDER_ACCURACY				0.20
#define TRIMSURFACE_RENDER_LOWER				0.85
#define TRIMSURFACE_RENDER_UPPER				1.55


/***********************************************~***************************************************/


void WCTrimmedNurbsSurface::ClearTriangulations(std::list<WCTrimTriangulation> &triList) {
	std::list<WCTrimTriangulation>::iterator triIter;
	//Loop through all triangulations
	for (triIter = triList.begin(); triIter != triList.end(); triIter++) {
		glDeleteBuffers(1, &((*triIter).vertexBuffer));
		glDeleteBuffers(1, &((*triIter).indexBuffer));
	}
	//Clear the list
	triList.clear();
}


void WCTrimmedNurbsSurface::GeneratePISurfaceTexture(const GLfloat* buffer, const WPUInt &lodU, const WPUInt &lodV) {
	//Set up some parameters
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//Set up texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_context->TrimSurfTex());
	glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, lodU, lodV, GL_RGBA, GL_FLOAT, buffer);
	//Check for GL errors here
	if (glGetError() != GL_NO_ERROR) 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTrimmedNurbsSurface::GeneratePISurfaceTexture Error - Surface Texture Setup.");
}



GLfloat* WCTrimmedNurbsSurface::PointInversionHigh(std::list<WCVector4> &boundaryList) {
	GLuint numVerts = (GLuint)boundaryList.size();
	//Get the surface data
	WPUInt lodU=TRIMSURFACE_PI_TEX_SIZE, lodV=TRIMSURFACE_PI_TEX_SIZE;
	std::vector<GLfloat*> buffers = this->WCNurbsSurface::GenerateClientBuffers(lodU, lodV, true);
	if ((lodU != TRIMSURFACE_PI_TEX_SIZE) || (lodV != TRIMSURFACE_PI_TEX_SIZE))
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTrimmedNurbsSurface::PointInversionHigh - Altered LOD.");

	//Setup program, parameters, and surface texture
	glUseProgram(this->_context->TrimInversionProgram());
	glUniform2i(this->_context->TrimLocations()[TRIMSURFACE_LOC_PI_PARAMS], lodU, lodV);
	//Copy surface data into a texture
	this->GeneratePISurfaceTexture(buffers[0], lodU, lodV);
	//Release the surface buffers
	this->ReleaseBuffers(buffers);

	/*** Setup framebuffer object ***/
	
	//Bind to framebuffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->_context->TrimFramebuffer());
	//Check to make sure the framebuffer is ready
	GLenum retVal = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	//Check the status of the framebuffer object
	if (retVal != GL_FRAMEBUFFER_COMPLETE_EXT) { 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTrimmedNurbsSurface::PointInversionHigh - Framebuffer is not complete: " << retVal); 
		return NULL;
	}

	/*** Setup input texture ***/
	
	//Allocate space for the input texture (w * h * RGBA)
	GLuint texWidth = this->_context->TrimMaxTextureSize();
	GLuint texHeight = (GLuint)ceil((GLfloat)numVerts / (GLfloat)texWidth);
	GLfloat *pointData = new GLfloat[texWidth * texHeight * 4];
	GLuint index = 0;
	WCVector4 pt;
	//Initialize data in the array
	for (std::list<WCVector4>::iterator listIter=boundaryList.begin(); listIter != boundaryList.end(); listIter++) {
		pt = (*listIter);
		pointData[index*4] =   (GLfloat)pt.I();					//Set first position to x
		pointData[index*4+1] = (GLfloat)pt.J();					//Set second position to y
		pointData[index*4+2] = (GLfloat)pt.K();					//Set third position to z
		pointData[index*4+3] = (GLfloat)1.0;					//Set fourth position to 1.0
		index++;
	}
	//Setup and copy the data into the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_context->TrimInTex());
	glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, texWidth, texHeight, GL_RGBA, GL_FLOAT, pointData);
	//Check for errors
	if (glGetError() != GL_NO_ERROR) 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTrimmedNurbsSurface::PointInversionHigh - Setup Input Texture.");
	//Delete data array
	delete pointData;

	 /*** Setup and render ***/
	
	//Save the viewport and polygon mode bits
	glPushAttrib(GL_VIEWPORT_BIT | GL_POLYGON_BIT);
	//Disable some settings
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	//Set up the viewport and polygon mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glViewport(0, 0, texWidth, texHeight);
	GLfloat vertData[] = { -1.0, -1.0, -1.0, 1.0, 1.0, 1.0, 1.0, -1.0 };
	GLfloat texData[] = { -1.0, -1.0, -1.0, 1.0, 1.0, 1.0, 1.0, -1.0 };
	//Turn on vertex arrays
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, vertData);
	//Turn on texturing
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, texData);
	//--- Draw ---
	glDrawArrays(GL_QUADS, 0, 4);
	//Clean up
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	//Restore the viewport and polygon mode
	glPopAttrib();
	//Re-enable some settings
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);	
	//Do some error checking
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateSurfaceMedium - At Render.");
	
	/*** Save output textures into vertex VBO and normal VBO using simple memory read ***/
	
	//Read the vertex data
	GLfloat *inverseData = new GLfloat[texHeight * texWidth * 4];
	glReadPixels(0, 0, texWidth, texHeight, GL_RGBA, GL_FLOAT, inverseData);
	//Do some error checking
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateSurfaceMedium - At Read Data.");

	/*** Restore the framebuffer ***/

	//Clean up the framebuffer object
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	//Should check for errors here
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::PointInversionHigh - At Cleanup.");
/*** Debug ***
	 CLOGGER_DEBUG(WCLogManager::RootLogger(), "PointInversion High: " << numVerts);
	 for (int i=0; i<numVerts; i++)
		 printf("\t%d: %f %f %f %f\n", i, inverseData[i*4], inverseData[i*4+1], inverseData[i*4+2], inverseData[i*4+3]);
/*** Debug ***/
	//Return the array of inverted data
	return inverseData;
}


GLfloat* WCTrimmedNurbsSurface::PointInversionLow(std::list<WCVector4> &boundaryList) {
	//Setup a whole lot of variables
	unsigned int numPoints = boundaryList.size();
	GLint tmpVal = TRIMSURFACE_PI_TEX_SIZE * 4;
	GLfloat minDist, dist, leftDist, rightDist, topDist, bottomDist, hSign, vSign, vValue, uValue, uDot, vDot, hDirMag, vDirMag;
	WPUInt lodU=TRIMSURFACE_PI_TEX_SIZE, lodV=TRIMSURFACE_PI_TEX_SIZE;
	//Get the surface data
	std::vector<GLfloat*> buffers = this->WCNurbsSurface::GenerateClientBuffers(lodU, lodV, true);
	if ((lodU != TRIMSURFACE_PI_TEX_SIZE) || (lodV != TRIMSURFACE_PI_TEX_SIZE))
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTrimmedNurbsSurface::PointInversionLow - Altered LOD.");
	GLfloat paraU = (GLfloat)(1.0 / (lodU - 1.0));
	GLfloat paraV = (GLfloat)(1.0 / (lodV - 1.0));
	GLfloat* surfData = buffers.at(0);
	GLfloat* buffer = new GLfloat[numPoints * 4];
	GLuint markIndex, markU, markV, ptIndex=0, surfIndex;
	WCVector4 pt, surfPt, center, left, right, top, bottom, hPt, vPt, vDir, hDir, pDir;

	//Loop through each point - trying to find surface vertex closest to point
	for (std::list<WCVector4>::iterator ptIter=boundaryList.begin(); ptIter != boundaryList.end(); ptIter++) {
		//Get the point
		pt = *ptIter;
		//Reset surfIndex
		surfIndex = 0;
		minDist = 100000.0;
		//Loop and find the closest point on the surface
		for (GLuint v=0; v<lodV; v++) {
			for (GLuint u=0; u<lodU; u++) {
				//Determine distance from point to surface point
				surfPt.Set(surfData[surfIndex], surfData[surfIndex + 1], surfData[surfIndex + 2], 1.0);
				dist = (GLfloat)surfPt.Distance(pt);
				//Is this smaller than current smallest
				if (dist <= minDist) {
					//Capture the location
					markIndex = surfIndex;
					markU = u;
					markV = v;
					minDist = dist;
				}
				//Increment surfIndex
				surfIndex += 4;
			}
		}
		//Now determine quadrant
		center.Set(surfData[markIndex], surfData[markIndex+1], surfData[markIndex+2], 1.0);
		//Set left point
		if (markU > 0) {
			left.Set(surfData[markIndex-4], surfData[markIndex-3], surfData[markIndex-2], 1.0);
			leftDist = (GLfloat)pt.Distance(left);
		}
		else leftDist = 10000.0;
		//Set the right point
		if (markU <lodU-1) {
			right.Set(surfData[markIndex+4], surfData[markIndex+5], surfData[markIndex+6], 1.0);
			rightDist = (GLfloat)pt.Distance(right);
		}
		else rightDist = 10000.0;
		//Set the bottom point
		if (markV > 0) {
			bottom.Set(surfData[markIndex-tmpVal], surfData[markIndex-tmpVal+1], surfData[markIndex-tmpVal+2], 1.0);
			bottomDist = (GLfloat)pt.Distance(bottom);
		}
		else bottomDist = 10000.0;
		//Set the top point
		if (markV < lodV-1) {
			top.Set(surfData[markIndex+tmpVal], surfData[markIndex+tmpVal+1], surfData[markIndex+tmpVal+2], 1.0);
			topDist = (GLfloat)pt.Distance(top);
		}
		else topDist = 10000.0;
		//Set the signs
		if (leftDist < rightDist) { hSign = -1.0; hPt = left; }
		else { hSign = 1.0; hPt = right; }
		if (bottomDist < topDist) { vSign = -1.0; vPt = bottom; }
		else { vSign = 1.0; vPt = top; }
		//Convert from triangle values to [uv]
		vDir = vPt - center;
		vDirMag = (GLfloat)vDir.Magnitude();
		hDir = hPt - center;
		hDirMag = (GLfloat)hDir.Magnitude();
		pDir = pt - center;
		uDot = (GLfloat)hDir.DotProduct(pDir) / (hDirMag * hDirMag);
		vDot = (GLfloat)vDir.DotProduct(pDir) / (vDirMag * vDirMag);
		uValue = (paraU * (GLfloat)markU) + (uDot * paraU * hSign);
		vValue = (paraV * (GLfloat)markV) + (vDot * paraV * vSign);
		//Bound u and v [0,1]
		uValue = (GLfloat)STDMAX(0.0, STDMIN(1.0, uValue));
		vValue = (GLfloat)STDMAX(0.0, STDMIN(1.0, vValue));
		//Record [u,v] value into buffer
		buffer[ptIndex*4] = uValue;			// u value
		buffer[ptIndex*4 + 1] = vValue;		// v value
		buffer[ptIndex*4 + 2] = 0.0;		// non-value
		buffer[ptIndex*4 + 3] = 1.0;		// non-value
//		std::cout << ptIndex << ": " << pt << " --> U: " << uValue << ", V: " << vValue << ", UDot: " << uDot << ", VDot: " << vDot << std::endl;
		ptIndex++;
	}
	//Release the surface buffers
	this->ReleaseBuffers(buffers);
/*** DEBUG ***
	for (int i=0; i<numPoints; i++)
		std::cout << i << ": " << buffer[i*4] << ", " << buffer[i*4+1] << std::endl;
/*** DEBUG ***/
	//Return the buffer
	return buffer;
}


void WCTrimmedNurbsSurface::GenerateTriangulations(std::list<WCTrimTriangulation> &triList) {
	//Only perform is there are trim profiles
	if (this->_profileList.size() == 0) return;
	//Make sure there are not current items in the list
	if (!triList.empty()) this->ClearTriangulations(triList);
	
	std::list<WCTrimProfile>::iterator profileIter;
	std::list<WCVector4> boundaryList;
	std::list<WCVector4>::iterator boundaryIter;
	GLfloat *vertData;
	WCTrimTriangulation triangulation;
	GLint *indexData, numVerts;
	GLuint vertexBuffer, indexBuffer;

	//Go through each profile
	for (profileIter = this->_profileList.begin(); profileIter != this->_profileList.end(); profileIter++) {
		//Generate complete boundary list
		BuildBoundaryList( *profileIter, boundaryList, true );
		numVerts = (GLint)boundaryList.size();

		//Invert all of the points (only u,v values)
		if (boundaryList.size() > 32)
			vertData = this->PointInversionHigh(boundaryList);
		else
			vertData = this->PointInversionLow(boundaryList);
		//Triangulate (expects verts in CW order, outputs triangles in CW order)
		indexData = TriangulatePolygon(vertData, numVerts);

		//Put vertices and index array into VBOs
		glGenBuffers(1, &vertexBuffer);
		glGenBuffers(1, &indexBuffer);
		//Create WCTrimTriangulation
		triangulation.numTriangles = numVerts - 2;
		triangulation.vertexBuffer = vertexBuffer;
		triangulation.indexBuffer = indexBuffer;		
		//Add to triList
		triList.push_back(triangulation);
		
		//Load vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * numVerts, vertData, GL_STATIC_DRAW);
		//Load index buffer
		glBindBuffer(GL_ARRAY_BUFFER, indexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLint) * 3 * triangulation.numTriangles, indexData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		//Make sure to delete the vert and index arrays
		delete indexData;
		delete vertData;
/*** DEBUG ***
		std::cout << "Triangulate Verts(" << vertexBuffer << "): " << numVerts << std::endl;
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);	
		GLfloat *data2 = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
		for (int i=0; i<numVerts; i++) printf("\t%d V: %f %f\n", i, data2[i*2], data2[i*2+1]);
		glUnmapBuffer(GL_ARRAY_BUFFER);
		std::cout << "Triangulate Index(" << indexBuffer << "): " << numVerts-2 << std::endl;
		glBindBuffer(GL_ARRAY_BUFFER, indexBuffer);	
		GLint *data3 = (GLint*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
		for (int i=0; i<numVerts-2; i++) printf("\t%d I: %d %d %d\n", i, data3[i*3], data3[i*3+1], data3[i*3+2]);
		glUnmapBuffer(GL_ARRAY_BUFFER);
/*** DEBUG ***/
	}
	
	/*** Clean Up ***/
	
	//Check for errors here
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTrimmedNurbsSurface::GenerateTrimList - GL Error at cleanup: " << error);
	}
}


/***********************************************~***************************************************/


WCTrimmedNurbsSurface::WCTrimmedNurbsSurface(WCGeometryContext *context, const std::list<WCTrimProfile> &profileList, 
	const WPUInt &degreeU, const WPUInt &degreeV, const WPUInt &cpU, const WPUInt &cpV, 
	const std::vector<WCVector4> &controlPoints, const WCNurbsMode &modeU, const WCNurbsMode &modeV,
	const std::vector<WPFloat> &kpU, const std::vector<WPFloat> &kpV) : 
	:: WCNurbsSurface(context, degreeU, degreeV, cpU, cpV, controlPoints, modeU, modeV, kpU, kpV), _context(context),
	_profileList(profileList), _isTextureDirty(false), _trimMatrix(), _invTrimMatrix(),
	_trimTexture(0),  _texWidth(0), _texHeight(0) {
	//Make sure there are some profiles
	if (this->_profileList.size() == 0) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTrimmedNurbsSurface::WCTrimmedNurbsSurface - No profiles attached.");
		//throw error
		return;
	}
	//Determine orientation matrix and aspect ratio
	WCVector4 base = this->_controlPoints.at(0);
	WCVector4 xAxis = this->_controlPoints.at(this->_cpU-1) - base;
	WCVector4 yAxis = this->_controlPoints.at(this->_controlPoints.size() - this->_cpU) - base;
	WCVector4 zAxis = xAxis.CrossProduct(yAxis);
//	this->_width = xAxis.Magnitude();
//	this->_height = yAxis.Magnitude();
//	WPFloat aspectRatio = this->_height / this->_width;
//	this->_texHeight = (WPUInt)(this->_texWidth * aspectRatio);
	xAxis.Normalize(true);
	yAxis.Normalize(true);
	zAxis.Normalize(true);
	//Determine the trim matrix and inverse trim matrix
	this->_invTrimMatrix = WCMatrix4(xAxis.I(), yAxis.I(), zAxis.I(), base.I(),
								  xAxis.J(), yAxis.J(), zAxis.J(), base.J(),
								  xAxis.K(), yAxis.K(), zAxis.K(), base.K(),
								  0.0, 0.0, 0.0, 1.0);
	this->_trimMatrix = this->_invTrimMatrix.Inverse();
}

WCTrimmedNurbsSurface::~WCTrimmedNurbsSurface() {
	//Release texture
	this->ReleaseTrimTexture(this->_trimTexture);
}


WPFloat WCTrimmedNurbsSurface::Area(const WPFloat &tolerance) {
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTrimmedNurbsSurface::Area - Not yet implemented.");
	return 0.0;
}


WCVector4 WCTrimmedNurbsSurface::Evaluate(const WPFloat &u, const WPFloat &v) {
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTrimmedNurbsSurface::Evaluate - Not yet implemented.");
	return WCVector4();
}


WCVector4 WCTrimmedNurbsSurface::Derivative(const WPFloat &u, const WPUInt &uDer, const WPFloat &v, const WPUInt &vDer) {
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTrimmedNurbsSurface::Derivative - Not yet implemented.");
	return WCVector4();
}


WCRay WCTrimmedNurbsSurface::Tangent(const WPFloat &u, const WPFloat &v) {
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTrimmedNurbsSurface::Tangent - Not yet implemented.");
	return WCRay(WCVector4(), WCVector4());
}


std::pair<WCVector4,WCVector4> WCTrimmedNurbsSurface::PointInversion(const WCVector4 &point) {
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTrimmedNurbsSurface::PointInversion - Not yet implemented.");
	//Get point inversion from base surface
	//...
	//Look up [uv] coordinates in texture to make sure point is in trimmed surface
	//...
	return std::make_pair(WCVector4(), WCVector4());
}


WCVisualObject* WCTrimmedNurbsSurface::HitTest(const WCRay &ray, const WPFloat &tolerance) {
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTrimmedNurbsSurface::HitTest - Not yet implemented.");
	return NULL;
}


void WCTrimmedNurbsSurface::Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom) {
	//Check to see if the surface is visible
	if (!this->_isVisible) return;

	//Determine best LOD values
	WPFloat lengthU = WCNurbs::EstimateLengthU(this->_controlPoints, this->_cpU);
	WPUInt lodU = STDMAX((WPUInt)(this->_cpU * 5), (WPUInt)(lengthU * sqrt(zoom) / TRIMSURFACE_RENDER_ACCURACY));
	WPFloat lengthV = WCNurbs::EstimateLengthV(this->_controlPoints, this->_cpV);
	WPUInt lodV = STDMAX((WPUInt)(this->_cpV * 5), (WPUInt)(lengthV * sqrt(zoom) / TRIMSURFACE_RENDER_ACCURACY));
	WPFloat factorU = (WPFloat)this->_lodU / (WPFloat)lodU;
	WPFloat factorV = (WPFloat)this->_lodV / (WPFloat)lodV;
	//See if need to regenerate underlying surface
	if ((factorU < TRIMSURFACE_RENDER_LOWER) || (factorU > TRIMSURFACE_RENDER_UPPER) ||
		(factorV < TRIMSURFACE_RENDER_LOWER) || (factorV > TRIMSURFACE_RENDER_UPPER) ||
		this->IsVisualDirty())  {
		//Set the new lod
		this->_lodU = lodU;
		this->_lodV = lodV;
		//Generate the server buffer of data
		this->GenerateServerBuffers(this->_lodU, this->_lodV, this->_buffers, true);
		//Mark as clean
		this->IsVisualDirty(false);
	}
	//See if texture forces regen.
	WPUInt texU = STDMIN((WPUInt)TRIMSURFACE_MAX_TEX_SIZE, (WPUInt)(lengthU * sqrt(zoom) / TRIMSURFACE_RENDER_ACCURACY) * 24);
	WPUInt texV = STDMIN((WPUInt)TRIMSURFACE_MAX_TEX_SIZE, (WPUInt)(lengthV * sqrt(zoom) / TRIMSURFACE_RENDER_ACCURACY) * 24);
	texU = STDMAX(texU, (WPUInt)16);
	texV = STDMAX(texV, (WPUInt)16);
	factorU = (WPFloat)this->_texWidth / (WPFloat)texU;
	factorV = (WPFloat)this->_texHeight / (WPFloat)texV;
	//Check to see if trim texture is dirty
	if ((factorU < TRIMSURFACE_RENDER_LOWER) || (factorU > TRIMSURFACE_RENDER_UPPER) ||
		(factorV < TRIMSURFACE_RENDER_LOWER) || (factorV > TRIMSURFACE_RENDER_UPPER) ||
		this->IsTextureDirty()) {
//		std::cout << "TexU: " << texU << ", TexV: " << texV << std::endl;
		//Set the new texture sizes
		this->_texWidth = texU;
		this->_texHeight = texV;
		//Generate the trim texture
		this->GenerateTrimTexture(this->_texWidth, this->_texHeight, this->_trimTexture, true);		
		//Mark as clean
		this->IsTextureDirty(false);
	}
	//Set the rendering program
	if (this->_renderProg != 0) {
		glUseProgram(this->_renderProg);
	}
	else if (defaultProg != 0)glUseProgram(defaultProg);
	
	//Make sure that vertex, index, normal, and texcoord arrays are enabled
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_VERTEX_BUFFER]);
	glVertexPointer(4, GL_FLOAT, 4 * sizeof(GLfloat), 0);
	
	glEnableClientState(GL_INDEX_ARRAY);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_INDEX_BUFFER]);
	glIndexPointer(GL_INT, sizeof(GLint), 0);

	glEnableClientState(GL_NORMAL_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_NORMAL_BUFFER]);
	glNormalPointer(GL_FLOAT, 4 * sizeof(GLfloat), 0);
	
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_TEXCOORD_BUFFER]);
	glTexCoordPointer(2, GL_FLOAT, 2 * sizeof(GLfloat), 0);

	//Set color appropriately
	if (color == WCColor::Default()) {
		this->_color.Enable();
	}
	else {
		color.Enable();
		glUseProgram( this->_context->ShaderManager()->ProgramFromName("scn_selection_trim") );
	}

	//Set up the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_trimTexture);
	GLint loc = glGetUniformLocation(this->_renderProg, "alphaTexture");
	glUniform1i(loc, 0);
	loc = glGetUniformLocation(this->_renderProg, "texSize");
	glUniform2f(loc, (GLfloat)this->_texWidth, (GLfloat)this->_texHeight);

	//Draw elements (*2 for each triangle in a lod box, *3 for each vertex in a triangle)
	glDrawElements(GL_TRIANGLES, (this->_lodU-1)*(this->_lodV-1)*2*3, GL_UNSIGNED_INT, 0);
	
	/*** Draw done - now clean up ***/
	
	//Bind back to nothing
	glBindBuffer(GL_ARRAY_BUFFER, 0);	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
	//Make sure that vertex and normal arrays are disabled
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_INDEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	//Restore the default prog
	if (this->_renderProg != 0) glUseProgram(defaultProg);
	//Report them errors
	if (glGetError() != GL_NO_ERROR) std::cout << "WCTrimmedNurbsSurface::Render Error - Unspecified error.\n";
}
	

void WCTrimmedNurbsSurface::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
	//Mark the surface as dirty
	this->IsVisualDirty(true);
	this->IsSerialDirty(true);
	//Update any parents about dirtyness
	this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);
}


void WCTrimmedNurbsSurface::GenerateTrimTexture(GLuint &texWidth, GLuint &texHeight, GLuint &texture, const bool &managed) {
	//Generate all of the tessellated trim profiles
	std::list<WCTrimTriangulation> triList;
	this->GenerateTriangulations(triList);

	//Make sure no programs are active
	glUseProgram(0);
	//Generate texture if needed
	if (texture == 0) glGenTextures(1, &texture);
	//Setup output texture
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texture);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	//Make sure to unbind from the texture
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
	
	/*** Setup framebuffer ***/
	
	//Create framebuffer
	//Bind to the custom framebuffer
	GLuint framebuffer;
	glGenFramebuffersEXT(1, &framebuffer);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer);
	//Bind the output texture to the framebuffer object
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, texture, 0);
	//Check to make sure the framebuffer is ready
	GLenum retVal = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	//Check the status of the framebuffer object
	if (retVal != GL_FRAMEBUFFER_COMPLETE_EXT) { 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTrimmedNurbsSurface::GenerateTexture - Framebuffer is not complete: " << retVal); 
		return;
	}
	
	/*** Setup viewport and matrices and render state ***/
	
	//Get the current viewport
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	//Set up the viewport
	glViewport(0, 0, texWidth, texHeight);
	
	//Push and setup the projection and modelview matrices
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	//Just clear the projection matrix
	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	//Load the trim matrix for modelview
	glLoadIdentity();
	//this->_trimMatrix.GLLoadMatrix();
	
	//Set render state
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_INDEX_ARRAY);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	
	//Clear the framebuffer
	glClear(GL_COLOR_BUFFER_BIT);
	
	/*** Draw all of the trim triangulations ***/
	
	std::list<WCTrimTriangulation>::iterator triIter = triList.begin();
	//Set the color to fill ones
	glColor3f(1.0, 0.0, 0.0);
	//Setup vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, (*triIter).vertexBuffer );
	glVertexPointer(4, GL_FLOAT, 0, NULL);
	//Set up index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*triIter).indexBuffer );
	glIndexPointer(GL_INT, 0, NULL);
	//Draw the trim
	glDrawElements(GL_TRIANGLES, (*triIter).numTriangles*3 , GL_UNSIGNED_INT, 0);

	//Loop through each (starting with second tri)
	for(triIter++; triIter != triList.end(); triIter++) {
		//Set the color to fill zeros
		glColor3f(0.0, 0.0, 0.0);
		//Setup vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, (*triIter).vertexBuffer );
		glVertexPointer(4, GL_FLOAT, 0, NULL);
		//Set up index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*triIter).indexBuffer );
		glIndexPointer(GL_INT, 0, NULL);
		//Draw the trim
		glDrawElements(GL_TRIANGLES, (*triIter).numTriangles*3 , GL_UNSIGNED_INT, 0);
	}
	
	/*** Restore the viewport matrices and render state ***/
	
	//Restore render state
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_INDEX_ARRAY);
	//Bind back to nothing
	glBindBuffer(GL_ARRAY_BUFFER, 0);	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	//Pop projection and modelview matrices
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	
	//Restore viewport and polygon mode
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	
	/*** Clean up framebuffer and textures ***/
	
	//Unbind the framebuffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glDeleteFramebuffersEXT(1, &framebuffer);
	//Check for errors
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTrimmedNurbsSurface::GenerateTexture - Unspecified GL Error: " << error);
	}
}


void WCTrimmedNurbsSurface::ReleaseTrimTexture(GLuint &texture) {
	//For now just delete it
	glDeleteTextures(1, &texture);
}


xercesc::DOMElement* WCTrimmedNurbsSurface::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Create the base element for the object
	xercesc::DOMElement*  trimmedElem = document->createElement(xercesc::XMLString::transcode("TrimmedSurface"));
	return trimmedElem;
}


/***********************************************~***************************************************/


std::ostream& __WILDCAT_NAMESPACE__::operator<<(std::ostream& out, const WCTrimmedNurbsSurface &surface) {
	out << "Trimmed Nurbs Surface\n";
	return out;
}	


/***********************************************~***************************************************/

