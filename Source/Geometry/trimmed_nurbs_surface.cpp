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


/***********************************************~***************************************************/


void WCTrimmedNurbsSurface::ClearTriangulations(void) {
	std::list<WCTrimTriangulation>::iterator triIter;
	//Loop through all triangulations
	for (triIter = this->_triList.begin(); triIter != this->_triList.end(); triIter++) {
		glDeleteBuffers(1, &((*triIter).vertexBuffer));
		glDeleteBuffers(1, &((*triIter).indexBuffer));
	}
	this->_triList.clear();
}


void WCTrimmedNurbsSurface::GenerateTrimList(void) {
	//Only perform is there are trim profiles
	if (this->_profileList.size() == 0) return;
	
	std::list<WCTrimProfile>::iterator profileIter;
	std::list<WCVector4> boundaryList, vertList;
	std::list<WCVector4>::iterator boundaryIter;
	WCVector4 tmpVec;
	GLfloat *vertData;
	WCTrimTriangulation triangulation;
	GLint *indexData, numVerts, vertIndex;
	GLuint vertexBuffer, indexBuffer;
	
	//Clear the triangulation list
	this->ClearTriangulations();

	//Go through each profile
	for (profileIter = this->_profileList.begin(); profileIter != this->_profileList.end(); profileIter++) {
		//Generate complete boundary list
		boundaryList = BuildBoundaryList( *profileIter, true );
		numVerts = (GLint)boundaryList.size();
//		std::cout << "NumVerts: " << numVerts << std::endl;
		//Allocate space for vertices
		vertData = new GLfloat[numVerts * 3];
		vertIndex = 0;
		//Process list
		for (boundaryIter = boundaryList.begin(); boundaryIter != boundaryList.end(); boundaryIter++) {
			//Project onto plane and put into list
			tmpVec = this->_trimMatrix * (*boundaryIter);
			vertList.push_back(tmpVec);
			//Also place into vertex buffer array
			vertData[vertIndex*3] = (GLfloat)(*boundaryIter).I();
			vertData[vertIndex*3+1] = (GLfloat)(*boundaryIter).J();
			vertData[vertIndex*3+2] = (GLfloat)(*boundaryIter).K();
			//Increment vertIndex
			vertIndex++;
		}
		
		//Triangulate (expects verts in CW order, outputs triangles in CW order)
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
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		//Make sure to delete the vert and index arrays
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
		//Create WCTrimTriangulation
		triangulation.numTriangles = numVerts - 2;
		triangulation.vertexBuffer = vertexBuffer;
		triangulation.indexBuffer = indexBuffer;
		triangulation.outside = (*profileIter) == this->_profileList.front();
		//Add to triList
		this->_triList.push_back(triangulation);
		//Clear the vertList
		vertList.clear();
	}
	
	/*** Clean Up ***/
	
	//Check for errors here
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cout << error << std::endl;
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTrimmedNurbsSurface::GenerateTrimList - Unspecified GL Error at cleanup.");
	}
}


void WCTrimmedNurbsSurface::GenerateTexture(void) {
	//Make sure no programs are active
	glUseProgram(0);
	//Setup output texture
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_trimTexture);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, this->_texWidth, this->_texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	//Make sure to unbind from the texture
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
	
	/*** Setup framebuffer ***/
	
	//Create framebuffer
	//Bind to the custom framebuffer
	GLuint framebuffer;
	glGenFramebuffersEXT(1, &framebuffer);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer);
	//Bind the output texture to the framebuffer object
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, this->_trimTexture, 0);
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
	glViewport(0, 0, this->_texWidth, this->_texHeight);
	
	//Push and setup the projection and modelview matrices
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	//Just clear the projection matrix
	glLoadIdentity();
	glOrtho(0.0, this->_width, 0.0, this->_height, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	//Load the trim matrix for modelview
	this->_trimMatrix.GLLoadMatrix();

	//Set render state
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_INDEX_ARRAY);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	
	//Clear the framebuffer
	glClear(GL_COLOR_BUFFER_BIT);

	/*** If there are any, draw all of the trim triangulations ***/

	if (this->_profileList.size() > 0) {
		std::list<WCTrimTriangulation>::iterator triIter;
		//Loop through each 
		for(triIter = this->_triList.begin(); triIter != this->_triList.end(); triIter++) {
			//Set color based on outside vs. inside
			if ( (*triIter).outside ) {
//				std::cout << "Drawing outside trim profile\n";
				glColor3f(1.0, 0.0, 0.0);
			}
			else {
//				std::cout << "Drawing inside trim profile\n";
				glColor3f(0.0, 0.0, 0.0);
			}

			//Setup vertex buffer
			glBindBuffer(GL_ARRAY_BUFFER, (*triIter).vertexBuffer );
			glVertexPointer(3, GL_FLOAT, 0, NULL);

			//Set up index buffer
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*triIter).indexBuffer );
			glIndexPointer(GL_INT, 0, NULL);
			
			//Draw the trim
			glDrawElements(GL_TRIANGLES, (*triIter).numTriangles*3 , GL_UNSIGNED_INT, 0);
		}
	}
	//Otherwise, just draw covering quad
	else {
//		std::cout << "Trim: Drawing nothing.\n";
		//Set color to fill
		glColor3f(1.0, 0.0, 0.0);
		//Switch this to vertex arrays at some point
		glBegin(GL_QUADS);
			//Lower left
			WCVector4 tmpVec = this->_controlPoints[0];
			glVertex3f((GLfloat)tmpVec.I(), (GLfloat)tmpVec.J(), (GLfloat)tmpVec.K());
			//Upper left
			tmpVec = this->_controlPoints[2];
			glVertex3f((GLfloat)tmpVec.I(), (GLfloat)tmpVec.J(), (GLfloat)tmpVec.K());
			//Upper right
			tmpVec = this->_controlPoints[3];
			glVertex3f((GLfloat)tmpVec.I(), (GLfloat)tmpVec.J(), (GLfloat)tmpVec.K());
			//Lower right
			tmpVec = this->_controlPoints[1];
			glVertex3f((GLfloat)tmpVec.I(), (GLfloat)tmpVec.J(), (GLfloat)tmpVec.K());
		glEnd();
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


/***********************************************~***************************************************/


WCTrimmedNurbsSurface::WCTrimmedNurbsSurface(WCGeometryContext *context, const std::list<WCTrimProfile> &profileList, 
	const WPUInt &degreeU, const WPUInt &degreeV, const WPUInt &cpU, const WPUInt &cpV, 
	const std::vector<WCVector4> &controlPoints, const WCNurbsMode &modeU, const WCNurbsMode &modeV,
	const std::vector<WPFloat> &kpU, const std::vector<WPFloat> &kpV) : 
	:: WCNurbsSurface(context, degreeU, degreeV, cpU, cpV, controlPoints, modeU, modeV, kpU, kpV), _context(context),
	_triList(), _profileList(profileList), _trimMatrix(), _trimTexture(0),  _texWidth(0), _texHeight(0) {

	//Determine orientation matrix and aspect ratio
	WCVector4 base = this->_controlPoints.at(0);
	WCVector4 xAxis = this->_controlPoints.at(this->_cpU-1) - base;
	WCVector4 yAxis = this->_controlPoints.at(this->_controlPoints.size() - this->_cpU) - base;
	WCVector4 zAxis = xAxis.CrossProduct(yAxis);
	this->_width = xAxis.Magnitude();
	this->_height = yAxis.Magnitude();
	WPFloat aspectRatio = this->_height / this->_width;
	this->_texHeight = (WPUInt)(this->_texWidth * aspectRatio);
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
	//Delete texture
	glDeleteTextures(1, &(this->_trimTexture));
	//Clear the trim triangulation list
	this->ClearTriangulations();
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
	return std::make_pair(WCVector4(), WCVector4());
}


WCVisualObject* WCTrimmedNurbsSurface::HitTest(const WCRay &ray, const WPFloat &tolerance) {
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTrimmedNurbsSurface::HitTest - Not yet implemented.");
	return NULL;
}


void WCTrimmedNurbsSurface::Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom) {
	//Check to see if the surface is visible
	if (!this->_isVisible) return;
	//Check to see if surface or texture needs to be generated
	if (this->IsVisualDirty()) {
		//Generate trimList
//		this->GenerateTrimList();
		//Set up trim texture object
//		glGenTextures(1, &(this->_trimTexture));
		//Generate the surface - switch on performance level
		switch(this->_context->SurfacePerformanceLevel()) {
//			case TRIMSURFACE_PERFLEVEL_HIGH:	
//				this->GenerateSurfaceHigh();
				break;
//			case TRIMSURFACE_PERFLEVEL_MEDIUM:	
//				this->GenerateSurfaceMedium(); 
				break;
//			case TRIMSURFACE_PERFLEVEL_LOW:		
//				this->GenerateSurfaceLow(); 
				break;
		}
		//Generate the trim texture
		this->GenerateTexture();		
		//Generate the index for the curve
//		this->GenerateIndex();
		//Update the bounding box
//		this->_bounds->Set(this->_buffers[NURBSSURFACE_VERTEX_BUFFER], this->_numVerts);
		//Mark as clean
		this->IsVisualDirty(false);
	}
	//Set the rendering program
	if (this->_renderProg != 0) {
		glUseProgram(this->_renderProg);
	}
	else if (defaultProg != 0)glUseProgram(defaultProg);
	
/*** Debug ***/
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


std::vector<GLfloat*> WCTrimmedNurbsSurface::GenerateClientBuffers(WPUInt &lodU, WPUInt &lodV) {
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTrimmedNurbsSurface::GenerateClientBuffers - Not yet implemented.");
	return std::vector<GLfloat*>();
}


void WCTrimmedNurbsSurface::GenerateServerBuffers(WPUInt &lodU, WPUInt &lodV, std::vector<GLuint> &buffers) {
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCTrimmedNurbsSurface::GenerateServerBuffers - Not yet implemented.");
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

