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


GLuint WCTrimmedNurbsSurface::PointInversionHigh(std::list<WCVector4> &boundaryList) {
	GLuint numVerts = (GLuint)boundaryList.size();
	//Get the surface data
	WPUInt lodU=TRIMSURFACE_PI_TEX_SIZE, lodV=TRIMSURFACE_PI_TEX_SIZE;
	std::vector<GLuint> buffers;
	this->WCNurbsSurface::GenerateTextureBuffers(0.0, 1.0, lodU, 0.0, 1.0, lodV, buffers, true);
	if ((lodU != TRIMSURFACE_PI_TEX_SIZE) || (lodV != TRIMSURFACE_PI_TEX_SIZE))
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTrimmedNurbsSurface::PointInversionHigh - Altered LOD.");
	//Setup program, parameters, and surface texture
	glUseProgram(this->_context->TrimInversionProgram());
	glUniform2i(this->_context->TrimLocations()[TRIMSURFACE_LOC_PI_PARAMS], lodU, lodV);
	//Set up texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, buffers.at(NURBSSURFACE_VERTEX_BUFFER));

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
	glPushAttrib(GL_VIEWPORT_BIT | GL_POLYGON_BIT | GL_TEXTURE_BIT | GL_ENABLE_BIT);
	glPushClientAttrib(GL_ALL_ATTRIB_BITS);
	//Disable some settings
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	//Bind to framebuffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->_context->TrimFramebuffer());
	//Set up the viewport and polygon mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glViewport(0, 0, texWidth, texHeight);
	GLfloat vertData[] = { -1.0, -1.0, -1.0, 1.0, 1.0, 1.0, 1.0, -1.0 };
	//Turn on vertex arrays
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, vertData);
	//--- Draw ---
	glDrawArrays(GL_QUADS, 0, 4);
	//Restore the viewport and polygon mode
	glPopAttrib();
	glPopClientAttrib();
	//Do some error checking
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateSurfaceMedium - At Render.");
	
	/*** Save output textures into a VBO using PBO ***/

	GLuint buffer;
	glGenBuffers(1, &buffer);
	//Bind to the PBO
	glBindBuffer(GL_PIXEL_PACK_BUFFER, buffer);
	glBufferData(GL_PIXEL_PACK_BUFFER, texHeight * texWidth * 4 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
	//Read the pixel data
	glReadPixels(0, 0, texWidth, texHeight, GL_RGBA, GL_FLOAT, NULL);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

	/*** Restore the framebuffer and cleanup ***/

	//Clean up the framebuffer object
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	//Release the surface buffers
	this->ReleaseTextures(buffers);
	//Bind back to no textures
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
/*** DEBUG ***
	std::cout << "Point Inversion High: " << boundaryList.size() << std::endl;	
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	GLfloat *data2 = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	for (int i=0; i<boundaryList.size(); i++) printf("\t%d: %f %f %f %f\n", i, data2[i*4], data2[i*4+1], data2[i*4+2], data2[i*4+3]);
	glUnmapBuffer(GL_ARRAY_BUFFER);	
/*** DEBUG ***/
	//Should check for errors here
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::PointInversionHigh - At Cleanup.");
	//Return the array of inverted data
	return buffer;
}


GLuint WCTrimmedNurbsSurface::PointInversionLow(std::list<WCVector4> &boundaryList) {
	//Setup a whole lot of variables
	unsigned int numPoints = boundaryList.size();
	GLint tmpVal = TRIMSURFACE_PI_TEX_SIZE * 4;
	GLfloat minDist, dist, leftDist, rightDist, topDist, bottomDist, hSign, vSign, vValue, uValue, uDot, vDot, hDirMag, vDirMag;
	WPUInt lodU=TRIMSURFACE_PI_TEX_SIZE, lodV=TRIMSURFACE_PI_TEX_SIZE;
	//Get the surface data
	std::vector<GLfloat*> buffers = this->WCNurbsSurface::GenerateClientBuffers(0.0, 1.0, lodU, 0.0, 1.0, lodV, true);
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
		uValue = STDMAX((GLfloat)0.0, STDMIN((GLfloat)1.0, uValue));
		vValue = STDMAX((GLfloat)0.0, STDMIN((GLfloat)1.0, vValue));
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

	//Buffer the data into the VBO
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numPoints * 4, buffer, GL_STATIC_DRAW);
	//Return the vbo
	return vbo;
}


GLuint WCTrimmedNurbsSurface::GenerateTriangulation(std::list<GLuint> &triList) {
	//Only perform is there are trim profiles
	if (this->_profileList.size() == 0) return 0;
	//Make sure there are not current items in the list
	triList.clear();
	
	std::list<WCTrimProfile>::iterator profileIter;
	std::list<WCVector4> boundaryList, fullList;
	std::list<WCVector4>::iterator boundaryIter;
	GLuint numVerts;//, *indexData;

	//Go through each profile
	for (profileIter = this->_profileList.begin(); profileIter != this->_profileList.end(); profileIter++) {
		//Generate complete boundary list for that profile
		BuildBoundaryList( *profileIter, boundaryList, true );
		numVerts = (GLint)boundaryList.size();
		//Concatenate boundaryList to fullList
		fullList.splice(fullList.end(), boundaryList);
		//NumVerts is number of vertices from BuildBoundaryList
		triList.push_back(numVerts);
	}
	//Invert all of the points (only u,v values)
	GLuint vertBuffer;
	if ((fullList.size() > 1) && (fullList.size() < 262144))
		vertBuffer = this->PointInversionHigh(fullList);
	else
		vertBuffer = this->PointInversionLow(fullList);
	
	//Check for errors here
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTrimmedNurbsSurface::GenerateTrimList - GL Error at cleanup: " << error);
	}
	//Return the buffer of vertices
	return vertBuffer;
}


/***********************************************~***************************************************/


WCTrimmedNurbsSurface::WCTrimmedNurbsSurface(WCGeometryContext *context, const std::list<WCTrimProfile> &profileList, 
	const WPUInt &degreeU, const WPUInt &degreeV, const WPUInt &cpU, const WPUInt &cpV, 
	const std::vector<WCVector4> &controlPoints, const WCNurbsMode &modeU, const WCNurbsMode &modeV,
	const std::vector<WPFloat> &kpU, const std::vector<WPFloat> &kpV) : 
	:: WCNurbsSurface(context, degreeU, degreeV, cpU, cpV, controlPoints, modeU, modeV, kpU, kpV),
	_profileList(profileList), _isTextureDirty(true), _trimTexture(0),  _texWidth(0), _texHeight(0) {
	//Make sure there are some profiles
	if (this->_profileList.size() == 0) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTrimmedNurbsSurface::WCTrimmedNurbsSurface - No profiles attached.");
		//throw error
		return;
	}
}


WCTrimmedNurbsSurface::WCTrimmedNurbsSurface(const WCTrimmedNurbsSurface &surf) : ::WCNurbsSurface(surf),
	_profileList(surf._profileList), _isTextureDirty(true), _trimTexture(0),  _texWidth(0), _texHeight(0) {
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTrimmedNurbsSurface::WCTrimmedNurbsSurface - Copy constructor not implemented.");
}


WCTrimmedNurbsSurface::WCTrimmedNurbsSurface(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCNurbsSurface( WCSerializeableObject::ElementFromName(element,"NurbsSurface"), dictionary ),
	_profileList(), _isTextureDirty(true), _trimTexture(0), _texWidth(0), _texHeight(0) {
	//Make sure element if not null
	if (element == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTrimmedNurbsSurface::WCTrimmedNurbsSurface - NULL Element passed.");
		//throw error
		return;
	}
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

	//Restore trim profile list
	XMLCh *xmlString = xercesc::XMLString::transcode("Profile");
	xercesc::DOMNodeList *profileList = element->getElementsByTagName(xmlString);
	xercesc::XMLString::release(&xmlString);
	xmlString = xercesc::XMLString::transcode("Curve");
	xercesc::DOMNodeList* curveList;
	xercesc::DOMElement *profileElement, *curveElement;
	std::list<std::pair<WCGeometricCurve*,bool> > tmpList;
	//Loop through all profiles
	for (int i = 0; i < profileList->getLength(); i++) {
		//Get the profile element
		profileElement = (xercesc::DOMElement*)profileList->item(i);
		//Get the list of all curves in the profile
		curveList = profileElement->getElementsByTagName(xmlString);
		//Loop through all curves
		for (int j = 0; j < curveList->getLength(); j++) {
			//Get the curve element
			curveElement = (xercesc::DOMElement*)curveList->item(j);
			//Get the curve address
			WCGeometricCurve *curve = (WCGeometricCurve*)WCSerializeableObject::GetGUIDAttrib(curveElement, "address", dictionary);
			//Get the curve orientation
			bool orientation = WCSerializeableObject::GetBoolAttrib(curveElement, "orientation");
			//Add curve to tmp list
			tmpList.push_back( std::make_pair(curve, orientation) );
		}
		//If there are curves in tmpList add tmpList to _profileList
		if (tmpList.size()) {
			this->_profileList.push_back(tmpList);
			//Clear the list
			tmpList.clear();
		}
	}
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
		this->GenerateServerBuffers(0.0, 1.0, this->_lodU, 0.0, 1.0, this->_lodV, this->_buffers, true);
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
		(factorV < TRIMSURFACE_RENDER_LOWER) || (factorV > TRIMSURFACE_RENDER_UPPER) || this->IsTextureDirty()) {
		//Set the new texture sizes
		this->_texWidth = texU;
		this->_texHeight = texV;
		//Generate the trim texture
		this->GenerateTrimTexture(this->_texWidth, this->_texHeight, this->_trimTexture, true);
		//Mark as clean
		this->_isTextureDirty = false;
	}
	//Set the rendering program
	if (this->_renderProg != 0) {
		glUseProgram(this->_renderProg);
	}
	else if (defaultProg != 0)glUseProgram(defaultProg);
	
	//Save the client state
	glPushClientAttrib(GL_ALL_ATTRIB_BITS);
	//Make sure that vertex, index, normal, and texcoord arrays are enabled
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_VERTEX_BUFFER]);
	glVertexPointer(4, GL_FLOAT, 4 * sizeof(GLfloat), 0);
	//Enable index arrays
	glEnableClientState(GL_INDEX_ARRAY);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_INDEX_BUFFER]);
	glIndexPointer(GL_INT, sizeof(GLint), 0);
	//Enable normal arrays
	glEnableClientState(GL_NORMAL_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_NORMAL_BUFFER]);
	glNormalPointer(GL_FLOAT, 4 * sizeof(GLfloat), 0);
	//Enable texture coord arrays
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_TEXCOORD_BUFFER]);
	glTexCoordPointer(2, GL_FLOAT, 2 * sizeof(GLfloat), 0);
	//Set color appropriately
	if (color == WCColor::Default()) {
		this->_color.Enable();
	}
	else {
		color.Enable();
		glUseProgram( this->_context->ProgramFromName("scn_selection_trim") );
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
	glPopClientAttrib();
	//Restore the default prog
	if (this->_renderProg != 0) glUseProgram(defaultProg);
	//Report them errors
	if (glGetError() != GL_NO_ERROR)
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTrimmedNurbsSurface::Render Error - Unspecified error.");
}
	

void WCTrimmedNurbsSurface::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
	//Mark the surface as dirty
	this->IsVisualDirty(true);
	this->IsSerialDirty(true);
	//Update any parents about dirtyness
	this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);
}


void WCTrimmedNurbsSurface::GenerateTrimTexture(GLuint &texWidth, GLuint &texHeight, GLuint &texture, const bool &managed) {
	//Try to generate all of the tessellated trim profiles
	std::list<GLuint> triList;
	GLuint vertBuffer;
	try {
		//Generate the triangulations
		vertBuffer = this->GenerateTriangulation(triList);
	}
	//Handle the error
	catch (...) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTrimmedNurbsSurface::GenerateTrimTexture - Exception caught in GenerateTriangulations.");
		return;
	}

	/*** Setup output texture and framebuffer ***/

	//Generate texture if needed
	if (texture == 0) glGenTextures(1, &texture);
	//Setup output texture
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, texture);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	//Make sure to unbind from the texture
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
	//Create and bind to the framebuffer
	GLuint framebuffer, stencilRB;
	glGenFramebuffersEXT(1, &framebuffer);
	glGenRenderbuffersEXT(1, &stencilRB);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, texture, 0);
	//Initialize stencil renderbuffer
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, stencilRB);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_STENCIL_EXT, texWidth, texHeight);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, stencilRB);
	//Clear the framebuffer and stencil renderbuffer
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	/*** Setup GL Environment ***/

	//Make sure no programs are active
	glUseProgram(0);
	//Save the viewport and polygon mode bits
	glPushAttrib(GL_VIEWPORT_BIT | GL_POLYGON_BIT | GL_ENABLE_BIT | GL_TEXTURE_BIT);
	glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
	//Set some GL state settings
	glEnableClientState(GL_VERTEX_ARRAY);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	//Set up the viewport and polygon mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glViewport(0, 0, texWidth, texHeight);
	//Push and clear the projection and modelview matrices
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	
	/*** Draw all of the trim triangulations ***/
	
	//Loop through each triangulation
	GLuint index = 0;
	std::list<GLuint>::iterator triIter;
	for(triIter = triList.begin(); triIter != triList.end(); triIter++) {
		//Setup vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, vertBuffer );
		glVertexPointer(4, GL_FLOAT, 0, NULL);
		//Draw the trim
		glDrawArrays(GL_TRIANGLE_FAN, index, *triIter );
		//Increment the index
		index += *triIter;
	}
	//Bind back to nothing
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Delete the vertex buffer
	glDeleteBuffers(1, &vertBuffer);

	/*** Draw a covering rectangle ***/

	//Prepare to draw into color buffers
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glStencilFunc(GL_NOTEQUAL, 0, 1);
	GLfloat vertData[] = { -1.0, -1.0, -1.0, 1.0, 1.0, 1.0, 1.0, -1.0 };
	//Turn on vertex arrays
	glVertexPointer(2, GL_FLOAT, 0, vertData);
	//--- Draw ---
	glDrawArrays(GL_QUADS, 0, 4);

	/*** Restore the GL state ***/
	
	//Clean up the GL state
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glDeleteFramebuffersEXT(1, &framebuffer);
	glDeleteRenderbuffersEXT(1, &stencilRB);
	glPopAttrib();
	glPopClientAttrib();
	//Pop projection and modelview matrices
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
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


void WCTrimmedNurbsSurface::GenerateTessellation(WPUInt &lodU, WPUInt &lodV, std::vector<GLfloat*> &buffers) {
	//Generate dense trim texture
	//...
	//Generate lodU x lodV surface mesh
	//...
	//Generate all trim boundary points
	//...

	//Add all trim boundary points into triangle.c input structure (marked as boundary)
	//and into output buffer
	//...

	//Look up all surface mesh point in texture to see if they are in or out
	//add in vertices to both triangle.c (marked as interior) and output structures
	//...

	//Run triangle.c and get index buffer
	//...

	//Output correct data
	//...
}


xercesc::DOMElement* WCTrimmedNurbsSurface::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create primary element for this object
	XMLCh* xmlString = xercesc::XMLString::transcode("TrimmedNURBSSurface");
	xercesc::DOMElement*  element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);
	//Include the parent element
	xercesc::DOMElement* surfElement = this->WCNurbsSurface::Serialize(document, dictionary);
	element->appendChild(surfElement);

	//Save lists of TrimProfiles
	xmlString = xercesc::XMLString::transcode("TrimProfiles");
	xercesc::DOMElement* trimProfilesElement = document->createElement(xmlString);
	element->appendChild(trimProfilesElement);
	xercesc::XMLString::release(&xmlString);
	xercesc::DOMElement *profileElement;
	xmlString = xercesc::XMLString::transcode("Profile");
	XMLCh* curveString = xercesc::XMLString::transcode("Curve");
	std::list<WCTrimProfile>::iterator profileIter;
	WPUInt profileIndex = 0;
	for (profileIter = this->_profileList.begin(); profileIter != this->_profileList.end(); profileIter++) {
		//Add an element for the overall trim profile
		profileElement = document->createElement(xmlString);
		//Add the order index of the profile
		WCSerializeableObject::AddFloatAttrib(profileElement, "index", profileIndex++);
		//Append the element
		trimProfilesElement->appendChild(profileElement);
		//Inner loop for profile curves
		WPUInt curveIndex = 0;
		std::list<std::pair<WCGeometricCurve*,bool> >::iterator curveIter;
		for (curveIter = (*profileIter).begin(); curveIter != (*profileIter).end(); curveIter++) {
			//Add an element for the curve
			xercesc::DOMElement* curveElement = document->createElement(curveString);
			//Add the curve address
			WCSerializeableObject::AddGUIDAttrib(curveElement, "address", (*curveIter).first, dictionary);
			//Add curve direction
			WCSerializeableObject::AddBoolAttrib(curveElement, "orientation", (*curveIter).second);
			//Add the order index of the curve
			WCSerializeableObject::AddFloatAttrib(curveElement, "index", curveIndex++);
			//Append the element
			profileElement->appendChild(curveElement);
		}
	}
	//Make sure to release the strings
	xercesc::XMLString::release(&xmlString);
	xercesc::XMLString::release(&curveString);

	//Return the element
	return element;
}


/***********************************************~***************************************************/


std::ostream& __WILDCAT_NAMESPACE__::operator<<(std::ostream& out, const WCTrimmedNurbsSurface &surface) {
	out << "Trimmed Nurbs Surface\n";
	return out;
}	


/***********************************************~***************************************************/

