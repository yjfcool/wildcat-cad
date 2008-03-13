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
#include "Geometry/nurbs_surface.h"
#include "Geometry/nurbs.h"
#include "Geometry/geometry_context.h"
#include "Geometry/geometric_line.h"
#include "Geometry/nurbs_curve.h"
#include "Geometry/geometric_algorithms.h"
#include "Geometry/ray.h"


/*** Local macros ***/
#define MIN(a,b) (a<b?a:b)
#define MAX(a,b) (a>b?a:b)
#define ABS(a)	 (a>0?a:-a)


/*** Extern Variables ***/
extern WPFloat* __bezier_coef[8];


/***********************************************~***************************************************/


void WCNurbsSurface::ValidateClosure(void) {
	//Not implemented for now
	this->_isClosedU = false;
	this->_isClosedV = false;
}


void WCNurbsSurface::ValidateSelfIntersection(void) {
	//Not implemented for now
	this->_isSelfIntersecting = false;
}


void WCNurbsSurface::GenerateKnotPointsVBOs(void) {
	//Determine actual number of bytes needed in the buffer
	WPUInt size = this->_kpU * 4 * sizeof(GLfloat);
	GLfloat data[size / sizeof(GLfloat)];
	//Check to see if shader array is undersize
	if ((WPUInt)this->_context->SurfaceMinKPBufferSize() < size)
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateKnotPointsVBO - Undersized array in shader.");
	//Otherwise, copy the kp array into the vbo (Cast WPFloat to GLfloat)
	for (WPUInt i=0; i<this->_kpU; i++) data[i*4] = (GLfloat)this->_knotPointsU[i];
	//Bind the knot point buffer and load it
	glBindBuffer(GL_ARRAY_BUFFER, this->_context->SurfaceKPUBuffer());
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
/*** Debug ***
	std::cout << "Knot Points U VBO: " << this->_kpU << std::endl;
	GLfloat *data2 = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	for (int i=0; i<this->_kpU; i++) printf("\t%d: %f\n", i, data2[i*4]);
	glUnmapBuffer(GL_ARRAY_BUFFER);
/*** Debug ***/
	//Unbind the buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Now load the V buffer
	for (WPUInt i=0; i<this->_kpV; i++) data[i*4] = (GLfloat)this->_knotPointsV[i];
	//Bind the knot point buffer and load it
	glBindBuffer(GL_ARRAY_BUFFER, this->_context->SurfaceKPVBuffer());
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
/*** Debug ***
	std::cout << "Knot Points V VBO: " << this->_kpV << std::endl;
	data2 = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	for (int i=0; i<this->_kpV; i++) printf("\t%d: %f\n", i, data2[i*4]);
	glUnmapBuffer(GL_ARRAY_BUFFER);
/*** Debug ***/
	//Unbind the buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Check for GL errors here
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateKnotPointsVBO - Unspecified GL Error.");
}


void WCNurbsSurface::GenerateKnotPointsTextures(void) {
	//Create temporary array for data
	GLfloat dataU[4 * this->_kpU];
	//Copy knot points into array (cast WPFloat to GLfloat)
	for (WPUInt i=0; i<this->_kpU; i++) dataU[i*4] = (GLfloat)this->_knotPointsU[i];
	//Set up some parameters
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//Set up texture
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_context->SurfaceKPUTex());	
	glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, this->_kpU, 1, GL_RGBA, GL_FLOAT, dataU);
	//Check for GL errors
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateKnotPointsTexture - U Texture Setup.");
	//Create temporary array for data
	GLfloat dataV[4 * this->_kpU];
	for (WPUInt i=0; i<this->_kpV; i++) dataV[i*4] = (GLfloat)this->_knotPointsV[i];
	//Set up some parameters
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//Set up texture
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_context->SurfaceKPVTex());
	glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, this->_kpV, 1, GL_RGBA, GL_FLOAT, dataV);
	//Check for GL errors here
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateKnotPointsTexture - V Texture Setup.");
}


void WCNurbsSurface::GenerateControlPointsVBO(void) {
	//Determine the needed size of the buffer
	GLuint size = this->_cpU * this->_cpV * 4 * sizeof(GLfloat);
	GLfloat data[size / sizeof(GLfloat)];
	//Check for shader array undersize
	if ((GLuint)this->_context->SurfaceMinCPBufferSize() < size)
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateControlPointsVBO - Undersized array in shader.");
	//Otherwise, convert all control points to GLfloat arrays
	WCVector4 controlPoint;
	for (WPUInt i=0; i<(this->_cpU * this->_cpV); i++) {
		controlPoint = this->_controlPoints.at(i);
		//Cast each control point to a GLfloat array
		data[i*4]	= (GLfloat)controlPoint.I();
		data[i*4+1] = (GLfloat)controlPoint.J();
		data[i*4+2] = (GLfloat)controlPoint.K();
		data[i*4+3] = (GLfloat)controlPoint.L();
	}
	//Bind to buffer and copy data in
	glBindBuffer(GL_ARRAY_BUFFER, this->_context->SurfaceCPBuffer());
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
/*** Debug ***
	std::cout << "Control Point VBO: " << this->_cpU * this->_cpV << std::endl;
	GLfloat *data2 = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	for (int i=0; i<(this->_cpU * this->_cpV); i++) printf("\t%d: %f %f %f %f\n", i, data2[i*4], data2[i*4+1], data2[i*4+2], data2[i*4+3]);
	glUnmapBuffer(GL_ARRAY_BUFFER);
/*** Debug ***/
	//Clean up and check for errors
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Check for GL errors here
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateControlPointsVBO - Unspecified GL Error.");
}


void WCNurbsSurface::GenerateControlPointsTexture(void) {
	//Create temporary array for data
	GLfloat data[4*NURBSSURFACE_MAX_CONTROLPOINTS*NURBSSURFACE_MAX_CONTROLPOINTS];
	WCVector4 point;
	int index = 0;
	//Copy control points into array
	for (WPUInt i=0; i<this->_cpV; i++) {
		for (WPUInt j=0; j<this->_cpU; j++) {
			//Cast each control point to a GLfloat array
			point = this->_controlPoints.at(i*this->_cpU + j);
			data[index++] = (GLfloat)point.I();
			data[index++] = (GLfloat)point.J();
			data[index++] = (GLfloat)point.K();
			data[index++] = (GLfloat)point.L();
		}
	}
	//Set up some parameters
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//Set up texture
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_context->SurfaceCPTex());
	glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, this->_cpU, this->_cpV, GL_RGBA, GL_FLOAT, data);
	//Check for GL errors here
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateControlPointsTexture Error - Texture Setup.");
}


void WCNurbsSurface::LoadKnotPoints(const std::vector<WPFloat> &uKP, const std::vector<WPFloat> &vKP) {
	//Load the U knot point array
	if (this->_modeU == WCNurbsMode::Default())
		this->_knotPointsU = WCNurbs::LoadDefaultKnotPoints(this->_kpU, this->_degreeU);
	else if (this->_modeU == WCNurbsMode::Bezier())
		this->_knotPointsU = WCNurbs::LoadBezierKnotPoints(this->_kpU);
	else if (this->_modeU == WCNurbsMode::Custom())
		this->_knotPointsU = WCNurbs::LoadCustomKnotPoints(uKP);

	//Load the V knot point array
	if (this->_modeV == WCNurbsMode::Default())
		this->_knotPointsV = WCNurbs::LoadDefaultKnotPoints(this->_kpV, this->_degreeV);
	else if (this->_modeV == WCNurbsMode::Bezier())
		this->_knotPointsV = WCNurbs::LoadBezierKnotPoints(this->_kpV);
	else if (this->_modeV == WCNurbsMode::Custom())
		this->_knotPointsV = WCNurbs::LoadCustomKnotPoints(vKP);
}


void WCNurbsSurface::GenerateSurfaceHigh(void) {
	//Check for special generation cases, otherwise set the program
	if ((this->_controlPoints.size() == 4) && (this->_numVerts <= 225)) return this->GenerateSurfaceSize4();
//	if ((this->_degreeU == 1) && (this->_degreeV = 1)) return this->GenerateSurfaceOne();

	/*** Setup programs and bindable uniforms ***/

	//Now generate the control points VBO
	this->GenerateControlPointsVBO();
	//Create a knotpoint VBO
	this->GenerateKnotPointsVBOs();

	//Set the program and some uniforms
	if ((this->_degreeU <= 3) && (this->_degreeV <= 3) && (this->_modeU == WCNurbsMode::Bezier()) && (this->_modeV == WCNurbsMode::Bezier())) {
		std::cout << "Using Bezier23\n";
		glUseProgram(this->_context->SurfaceBezier23Program());
		glUniform4i(this->_context->SurfaceLocations()[NURBSSURFACE_LOC_PARAMSU_BEZIER23], this->_degreeU, this->_cpU, this->_kpU, 0);
		glUniform4i(this->_context->SurfaceLocations()[NURBSSURFACE_LOC_PARAMSV_BEZIER23], this->_degreeV, this->_cpV, this->_kpV, 0);
	//Otherwise use the default case
	} else {
		std::cout << "Using Default\n";
		glUseProgram(this->_context->SurfaceDefaultProgram());
		glUniform4i(this->_context->SurfaceLocations()[NURBSSURFACE_LOC_PARAMSU_DEFAULT], this->_degreeU, this->_cpU, this->_kpU, 0);
		glUniform4i(this->_context->SurfaceLocations()[NURBSSURFACE_LOC_PARAMSV_DEFAULT], this->_degreeV, this->_cpV, this->_kpV, 0);
	}
	//Make sure no problems so far
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateSurfaceHigh - Setup Problems.");

	/*** Setup Output Buffers ***/

	//Setup vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_VERTEX_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, this->_numVerts*NURBSSURFACE_FLOATS_PER_VERTEX*sizeof(GLfloat), 0, GL_STATIC_DRAW);
	glBindBufferBaseEXT(GL_TRANSFORM_FEEDBACK_BUFFER_EXT, 0, this->_buffers[NURBSSURFACE_VERTEX_BUFFER]);
	//Setup normal buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_NORMAL_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, this->_numVerts*NURBSSURFACE_FLOATS_PER_NORMAL*sizeof(GLfloat), 0, GL_STATIC_DRAW);
	glBindBufferBaseEXT(GL_TRANSFORM_FEEDBACK_BUFFER_EXT, 1, this->_buffers[NURBSSURFACE_NORMAL_BUFFER]);
	//Setup texcoord buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_TEXCOORD_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, this->_numVerts*NURBSSURFACE_FLOATS_PER_TEXCOORD*sizeof(GLfloat), 0, GL_STATIC_DRAW);
	glBindBufferBaseEXT(GL_TRANSFORM_FEEDBACK_BUFFER_EXT, 2, this->_buffers[NURBSSURFACE_TEXCOORD_BUFFER]);
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateSurfaceHigh - Output buffers setup.");

	/*** Setup Input Buffer ***/

	//Determine step size per direction
	WPFloat u = this->_knotPointsU[0];
	WPFloat range = this->_knotPointsU[this->_kpU-1] - this->_knotPointsU[0];
	WPFloat du = range / ((GLfloat)(this->_lodU-1));
	WPFloat v = this->_knotPointsV[0];
	range = this->_knotPointsV[this->_kpV-1] - this->_knotPointsV[0];
	WPFloat dv = range / ((GLfloat)(this->_lodV-1));
	//Create a temporary array (numVerts vertices)
	GLfloat data[this->_numVerts * 4];
	int index = 0;
	//Loop through each line
	for (WPUInt r=0; r <this->_lodV; r++) {
		//Reset u each loop
		u = this->_knotPointsU[0];
		//Loop through each batch on each line
		for (WPUInt i=0; i<this->_lodU; i++) {
			//Load array with value for each batch
			data[index++]	= (GLfloat)u;				//Set first position to u
			data[index++]	= (GLfloat)v;				//Set second position to du
			data[index++]	= (GLfloat)0.0;				//Set third position to nothing
			data[index++]	= (GLfloat)0.0;				//Set fourth position to nothing
			//Increment u
			u += du;
		}
		//Increment dv
		v += dv;
	}
//	for (int i=0; i<this->_numVerts; i++) printf("%d: %f %f %f %f\n", i, data[i*4], data[i*4+1], data[i*4+2], data[i*4+3]);
	//Create a temporary input buffer
	GLuint inputBuffer;
	glGenBuffers(1, &inputBuffer);
	//Set up transform feedback	buffer
	glBindBuffer(GL_ARRAY_BUFFER, inputBuffer);
	//Allocate space for the batch data and load data value array
	glBufferData(GL_ARRAY_BUFFER, this->_numVerts * 4 * sizeof(GLfloat), data, GL_STATIC_DRAW);
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateSurfaceHigh - Input buffer setup.");

	/*** Time to Generate ***/

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnable(GL_RASTERIZER_DISCARD_EXT);
	glBeginTransformFeedbackEXT(GL_POINTS);
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateSurfaceHigh - Begin Transform Feedback.");

	glVertexPointer(4, GL_FLOAT, 4 * sizeof(GLfloat), 0);
	glDrawArrays(GL_POINTS, 0, this->_numVerts);
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateSurfaceHigh - Draw Arrays.");

	glEndTransformFeedbackEXT();
	glDisable(GL_RASTERIZER_DISCARD_EXT);
	glDisableClientState(GL_VERTEX_ARRAY);

	/*** Clean up and exit ***/

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &inputBuffer);
/*** Debug ***
	CLOGGER_DEBUG(WCLogManager::RootLogger(), "High Feedback Verts: " << this->_numVerts);
	//Show Vertex Data
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_VERTEX_BUFFER]);
	GLfloat *data2 = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	for (int i=0; i<this->_numVerts; i++) printf("\t%d: %f %f %f %f\n", i, data2[i*4], data2[i*4+1], data2[i*4+2], data2[i*4+3]);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Show Normal Data
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_NORMAL_BUFFER]);
	data2 = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	for (int i=0; i<this->_numVerts; i++) printf("\t%d: %f %f %f\n", i, data2[i*4], data2[i*4+1], data2[i*4+2]);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Show TexCoord Data
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_TEXCOORD_BUFFER]);
	data2 = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	for (int i=0; i<this->_numVerts; i++) printf("\t%d: %f %f\n", i, data2[i*2], data2[i*2+1]);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
/*** Debug ***/
	//Clean up and check on errors
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateSurfaceHigh - Unspecified GL Error.");
}


void WCNurbsSurface::GenerateSurfaceMedium(void) {
	//Check for special generation cases, otherwise set the program
	if ((this->_controlPoints.size() == 4) && (this->_numVerts <= NURBSSURFACE_SIZE4_CUTOFF)) return this->GenerateSurfaceSize4();
//	if ((this->_degreeU == 1) && (this->_degreeV = 1)) return this->GenerateSurfaceOne();
	if ((this->_lodU > (WPUInt)this->_context->SurfaceMaxTextureSize()) || (this->_lodV > (WPUInt)this->_context->SurfaceMaxTextureSize())) {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateSurfaceMedium - LOD exceeds hardware Maximum Texture Size.");
		return this->GenerateSurfaceLow();
	}

	/*** Setup programs and texture locations ***/
	
	if ((this->_degreeU <= 3) && (this->_degreeV <= 3)) {
		//Degree 2 & 3 Bezier
		if ((this->_modeU == WCNurbsMode::Bezier()) && (this->_modeV == WCNurbsMode::Bezier())) {
//			std::cout << "Using Bezier23\n";
			glUseProgram(this->_context->SurfaceBezier23Program());
			glUniform4i(this->_context->SurfaceLocations()[NURBSSURFACE_LOC_PARAMSU_BEZIER23], this->_degreeU, this->_cpU, this->_kpU, 0);
			glUniform4i(this->_context->SurfaceLocations()[NURBSSURFACE_LOC_PARAMSV_BEZIER23], this->_degreeV, this->_cpV, this->_kpV, 0);			
		}
		//Degree 2 & 3 Default and Custom
		else {
//			std::cout << "Using Default23\n";
			glUseProgram(this->_context->SurfaceDefault23Program());
			glUniform4i(this->_context->SurfaceLocations()[NURBSSURFACE_LOC_PARAMSU_DEFAULT23], this->_degreeU, this->_cpU, this->_kpU, 0);
			glUniform4i(this->_context->SurfaceLocations()[NURBSSURFACE_LOC_PARAMSV_DEFAULT23], this->_degreeV, this->_cpV, this->_kpV, 0);
		}
	//Otherwise use the default case
	} else {
//		std::cout << "Using Default\n";
		glUseProgram(this->_context->SurfaceDefaultProgram());
		glUniform4i(this->_context->SurfaceLocations()[NURBSSURFACE_LOC_PARAMSU_DEFAULT], this->_degreeU, this->_cpU, this->_kpU, 0);		
		glUniform4i(this->_context->SurfaceLocations()[NURBSSURFACE_LOC_PARAMSV_DEFAULT], this->_degreeV, this->_cpV, this->_kpV, 0);								
	}
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateCurveMedium - Programs did not initialize.");
	//Now generate the control points texture
	this->GenerateControlPointsTexture();
	//Create a knotpoint texture
	this->GenerateKnotPointsTextures();

	/*** Setup framebuffer object ***/

	//Bind to framebuffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->_context->SurfaceFramebuffer());
	//Setup draw buffers
	GLenum buffers[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT };
	glDrawBuffers(2, buffers);
	//Check to make sure the framebuffer is ready
	GLenum retVal = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	//Check the status of the framebuffer object
	if (retVal != GL_FRAMEBUFFER_COMPLETE_EXT) { 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateCurveMedium - Framebuffer is not complete."); return; }	
	
	/*** Setup Input Texture and TexCoord Buffer ***/
	
	//Allocate space for the texture (w * h * RGBA)
	GLfloat data[this->_numVerts * 4];
	GLfloat uvData[this->_numVerts * 2];
	WPFloat u = this->_knotPointsU[0];
	WPFloat range = this->_knotPointsU[this->_kpU-1] - this->_knotPointsU[0];
	WPFloat du = range / ((GLfloat)(this->_lodU-1));
	WPFloat v = this->_knotPointsV[0];
	range = this->_knotPointsV[this->_kpV-1] - this->_knotPointsV[0];
	WPFloat dv = range / ((GLfloat)(this->_lodV-1));
	int index=0, uvIndex=0;	
	//Initialize data in the array
	for (WPUInt i=0; i<this->_lodV; i++) {
		//Make sure to reset u
		u = 0;
		for (WPUInt j=0; j<this->_lodU; j++) {
			data[index++] = (GLfloat)u;				//Set first position to u
			data[index++] = (GLfloat)v;				//Set second position to v
			data[index++] = (GLfloat)0.0;			//Set third position to 0.0
			data[index++] = (GLfloat)0.0;			//Set fourth position to 0.0
			uvData[uvIndex++] = (GLfloat)u;			//Set the u coordinate
			uvData[uvIndex++] = (GLfloat)v;			//Set the v coordinate
			//Increment u
			u = STDMIN(u+du, this->_knotPointsU[this->_kpU-1]);
		}
		//Selectively Increment v
		if (i < this->_lodV) v = STDMIN(v+dv, this->_knotPointsV[this->_kpV-1]);
	}
//	for (int i=0; i<this->_numVerts; i++) printf("%d: %f %f %f %f\n", i, data[i*4], data[i*4+1], data[i*4+2], data[i*4+3]);
	//Setup and copy the data into the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_context->SurfaceInTex());
	glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, this->_lodU, this->_lodV, GL_RGBA, GL_FLOAT, data);
	//Copy data into TexCoord Buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_TEXCOORD_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, this->_numVerts * 2 * sizeof(GLfloat), uvData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsSurface::GenerateSurfaceMedium Error - Setup Input Texture.\n";
	
	/*** Setup and render ***/

	//Save the viewport and polygon mode bits
	glPushAttrib(GL_VIEWPORT_BIT | GL_POLYGON_BIT);
	//Disable some settings
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	//Set up the viewport and polygon mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glViewport(0, 0, this->_context->SurfaceMaxTextureSize(), this->_context->SurfaceMaxTextureSize());
	GLfloat w = (GLfloat)(this->_lodU * 2.0 / this->_context->SurfaceMaxTextureSize() - 1.0);
	GLfloat h = (GLfloat)(this->_lodV * 2.0 / this->_context->SurfaceMaxTextureSize() - 1.0);
	GLfloat quad[] = { -1.0, -1.0, -1.0, h, w, h, w, -1.0 };
	//Turn on vertex arrays
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, quad);
	glDrawArrays(GL_QUADS, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
	//Restore the viewport and polygon mode
	glPopAttrib();
	//Re-enable some settings
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);	
	//Do some error checking
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateSurfaceMedium - At Render.");

	/*** Save output textures into vertex VBO and normal VBO using simple memory read ***/

	//Read the vertex data
	GLfloat vertData[this->_numVerts * 4];
	glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glReadPixels(0, 0, this->_lodU, this->_lodV, GL_RGBA, GL_FLOAT, vertData);
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_VERTEX_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, this->_numVerts * 4 * sizeof(GLfloat), vertData, GL_STATIC_DRAW);

	//Read the normal data
	GLfloat normData[this->_numVerts * 4];
	glReadBuffer(GL_COLOR_ATTACHMENT1_EXT);
	glReadPixels(0, 0, this->_lodU, this->_lodV, GL_RGBA, GL_FLOAT, normData);
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_NORMAL_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, this->_numVerts * 4 * sizeof(GLfloat), normData, GL_STATIC_DRAW);

	/*** Save output textures into vertex and normal VBOs using a PBO ***

	//Read the vertex texture
	glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, this->_buffers[NURBSSURFACE_VERTEX_BUFFER]);
	glBufferData(GL_PIXEL_PACK_BUFFER, this->_numVerts * 4 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
	glReadPixels(0, 0, this->_lodU, this->_lodV, GL_RGBA, GL_FLOAT, NULL);
	//Read the normal texture
	glReadBuffer(GL_COLOR_ATTACHMENT1_EXT);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, this->_buffers[NURBSSURFACE_NORMAL_BUFFER]);
	glBufferData(GL_PIXEL_PACK_BUFFER, this->_numVerts * 4 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
	glReadPixels(0, 0, this->_lodU, this->_lodV, GL_RGBA, GL_FLOAT, NULL);
	//Clean up the PBO
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
//	if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsSurface::GenerateSurfaceMedium Error - FBO -> PBO -> VBO.\n";
	
	/*** Restore the framebuffer ***/
		
	//Clean up the framebuffer object
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
/*** Debug ***
	std::cout << "Medium Generation Verts: " << this->_numVerts << std::endl;
	//Show Vertex Data
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_VERTEX_BUFFER]);
	GLfloat *data2 = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	for (int i=0; i<this->_numVerts; i++) printf("\t%d: %f %f %f %f\n", i, data2[i*4], data2[i*4+1], data2[i*4+2], data2[i*4+3]);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Show Normal Data
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_NORMAL_BUFFER]);
	data2 = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	for (int i=0; i<this->_numVerts; i++) printf("\t%d: %f %f %f %f\n", i, data2[i*4], data2[i*4+1], data2[i*4+2], data2[i*4+3]);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	//Show TexCoord Data
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_TEXCOORD_BUFFER]);
	data2 = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	for (int i=0; i<this->_numVerts; i++) printf("\t%d: %f %f\n", i, data2[i*2], data2[i*2+1]);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
/*** Debug ***/
	//Should check for errors here
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateSurfaceMedium - At Cleanup.");
}


void WCNurbsSurface::GenerateSurfaceLow(void) {
	//Check for special generation cases, otherwise set the program
	if (this->_controlPoints.size() == 4) return this->GenerateSurfaceSize4();
//	if ((this->_degreeU == 1) && (this->_degreeV = 1)) return this->GenerateSurfaceOne();

	int vIndex, nIndex, tIndex, index;
	WPFloat u = this->_knotPointsU[0];
	WPFloat v = this->_knotPointsV[0];
	WPFloat range = this->_knotPointsU[this->_kpU-1] - this->_knotPointsU[0];
	WPFloat du = range / ((GLfloat)(this->_lodU-1));
	range = this->_knotPointsV[this->_kpV-1] - this->_knotPointsV[0];
	WPFloat dv = range / ((GLfloat)(this->_lodV-1));
	
	//Create a temporary array (NumVert vertices)
	GLfloat vData[this->_numVerts*4];
	GLfloat nData[this->_numVerts*4];
	GLfloat tData[this->_numVerts*2];
	WPFloat *bvU, *bvV;
	WCVector4 pt;
	int spanU, spanV;
	WCVector4 S, Su, Sv, norm;
	WPFloat D, Du, Dv;
	
	//Zero all of the indices
	vIndex = nIndex = tIndex = 0;
	//Loop through v
	for (WPUInt i=0; i<this->_lodV; i++) {
		//Reset u each loop
		u = this->_knotPointsU[0];
		//Loop through u
		for (WPUInt j=0; j<this->_lodU; j++) {
			/*** Calculate point and normal ***/
			//Init the point values
			S.Set(0.0, 0.0, 0.0, 0.0);
			Su.Set(0.0, 0.0, 0.0, 0.0);
			Sv.Set(0.0, 0.0, 0.0, 0.0);
			D = Du = Dv = 0.0;
			//Find the span and basis values up to second derivative
			spanU = WCNurbs::FindSpan(this->_cpU, this->_degreeU, u, this->_knotPointsU);
			bvU = WCNurbs::BasisValues(spanU, u, this->_degreeU, this->_knotPointsU, 1);
			spanV = WCNurbs::FindSpan(this->_cpV, this->_degreeV, v, this->_knotPointsV);
			bvV = WCNurbs::BasisValues(spanV, v, this->_degreeV, this->_knotPointsV, 1);
			//Evaluate the point
			for(WPUInt k=0; k<=this->_degreeV; k++) {
				//Determine the index of the control point
				index = (this->_cpU * (spanV - this->_degreeV + k)) + spanU - this->_degreeU;
				for (WPUInt l=0; l<=this->_degreeU; l++) {
					pt = this->_controlPoints.at(index);
					S += pt * (pt.L() * bvU[l] * bvV[k]);
					D += pt.L() * bvU[l] * bvV[k];
					Su += pt * (pt.L() * bvU[l+this->_degreeU+1] * bvV[k]);
					Du += pt.L() * bvU[l+this->_degreeU+1] * bvV[k];
					Sv += pt * (pt.L() * bvU[l] * bvV[k+this->_degreeV+1]);
					Dv += pt.L() * bvU[l] * bvV[k+this->_degreeV+1];
					index++;
				}
			}
			//Calculate final values
			S /= D;
			Su = (Su * D - S * Du) / (D * D);
			Sv = (Sv * D - S * Dv) / (D * D);
			//Make sure to delete the basis values
			delete bvU;
			delete bvV;
			//Cross sU and sV and normalize to get normal vector
			norm = (Su.CrossProduct(Sv)).Normalize(true);
			//Place data into array
			vData[vIndex++] = (GLfloat)S.I();
			vData[vIndex++] = (GLfloat)S.J();
			vData[vIndex++] = (GLfloat)S.K();
			vData[vIndex++] = 1.0;
			nData[nIndex++] = (GLfloat)norm.I();
			nData[nIndex++] = (GLfloat)norm.J();
			nData[nIndex++] = (GLfloat)norm.K();
			nData[nIndex++] = 0.0;
			tData[tIndex++] = (GLfloat)u;
			tData[tIndex++] = (GLfloat)v;		
			//Increment u (up to kp-1)
			u = STDMIN(u+du, this->_knotPointsU[this->_kpU-1]);				
		}
		v = STDMIN(v+dv, this->_knotPointsV[this->_kpV-1]);
	}
	//Allocate space for the output vertex data
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_VERTEX_BUFFER]);	
	glBufferData(GL_ARRAY_BUFFER, this->_numVerts*4*sizeof(GLfloat), vData, GL_STATIC_DRAW);
	//Setup normal buffer	
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_NORMAL_BUFFER]);	
	glBufferData(GL_ARRAY_BUFFER, this->_numVerts*4*sizeof(GLfloat), nData, GL_STATIC_DRAW);
	//Setup texcoord buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_TEXCOORD_BUFFER]);	
	glBufferData(GL_ARRAY_BUFFER, this->_numVerts*2*sizeof(GLfloat), tData, GL_STATIC_DRAW);		
/*** Debug ***
	std::cout << "Generate Low Verts: " << this->_numVerts << std::endl;
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_VERTEX_BUFFER]);	
	GLfloat *data2= (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	for (int i=0; i<this->_numVerts; i++) printf("\t%d V: %f %f %f %f\n", i, data2[i*4], data2[i*4+1], data2[i*4+2], data2[i*4+3]);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	//Show Normal Data
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_NORMAL_BUFFER]);
	data2 = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	for (int i=0; i<this->_numVerts; i++) printf("\t%d: %f %f %f\n", i, data2[i*4], data2[i*4+1], data2[i*4+2]);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	//Show TexCoord Data
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_TEXCOORD_BUFFER]);
	data2 = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	for (int i=0; i<this->_numVerts; i++) printf("\t%d: %f %f\n", i, data2[i*2], data2[i*2+1]);
	glUnmapBuffer(GL_ARRAY_BUFFER);
/*** Debug ***/
	//Clean up and report errors
	glBindBuffer(GL_ARRAY_BUFFER, 0);	
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateSurfaceLow - Unspecified error.");
}


void WCNurbsSurface::GenerateSurfaceOne(void) {
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateSurfaceOne - Not yet implemented.");
}


void WCNurbsSurface::GenerateSurfaceSize4(void) {
	WCVector4 p0 = this->_controlPoints.at(0);
	WCVector4 p1 = this->_controlPoints.at(1);
	WCVector4 p2 = this->_controlPoints.at(2);
	WCVector4 p3 = this->_controlPoints.at(3);
	//Calcuate the corner normals
	WCVector4 n0 = (p1 - p0).CrossProduct(p2 - p0);
	WCVector4 n1 = (p3 - p1).CrossProduct(p0 - p1);
	WCVector4 n2 = (p0 - p2).CrossProduct(p3 - p2);
	WCVector4 n3 = (p2 - p3).CrossProduct(p1 - p3);
	//Make sure to normalize
	n0.Normalize(true);
	n1.Normalize(true);
	n2.Normalize(true);
	n3.Normalize(true);
	//Loop and create 
	int vIndex=0, nIndex=0, tIndex=0;
	WCVector4 pt;
	WPFloat u = this->_knotPointsU[0];
	WPFloat v = this->_knotPointsV[0];
	WPFloat range = this->_knotPointsU[this->_kpU-1] - this->_knotPointsU[0];
	WPFloat du = range / ((GLfloat)(this->_lodU-1));
	range = this->_knotPointsV[this->_kpV-1] - this->_knotPointsV[0];
	WPFloat dv = range / ((GLfloat)(this->_lodV-1));
	//Set aside data for the vertices, normals, and texcoords
	GLfloat vData[this->_numVerts*4];
	GLfloat nData[this->_numVerts*4];
	GLfloat tData[this->_numVerts*2];
	//Loop on V
	for(WPUInt i=0; i<this->_lodV; i++) {
		//Reset u each loop
		u = this->_knotPointsU[0];
		for (WPUInt j=0; j<this->_lodU; j++) {
			//Calculate vertex values (interpolate the corner points)
			pt = (p0 * u * v) + (p1 * (1.0-u) * v) + (p2 * u * (1.0-v)) + (p3 * (1.0-u) * (1.0-v));
			vData[vIndex++] = (GLfloat)pt.I();
			vData[vIndex++] = (GLfloat)pt.J();
			vData[vIndex++] = (GLfloat)pt.K();
			vData[vIndex++] = 1.0;						
			//Calc and load normal data (interpolate the corner normals)
			pt = (n0 * u * v) + (n1 * (1.0-u) * v) + (n2 * u * (1.0-v)) + (n3 * (1.0-u) * (1.0-v));
			nData[nIndex++] = (GLfloat)pt.I();
			nData[nIndex++] = (GLfloat)pt.J();
			nData[nIndex++] = (GLfloat)pt.K();
			nData[nIndex++] = 0.0;			
			//Load TexCoord Info
			tData[tIndex++] = (GLfloat)u;
			tData[tIndex++] = (GLfloat)v;
			//Increment u (up to kp-1)
			u = STDMIN(u+du, this->_knotPointsU[this->_kpU-1]);				
		}
		v = STDMIN(v+dv, this->_knotPointsV[this->_kpV-1]);
	}
	//Allocate space for the output vertex data
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_VERTEX_BUFFER]);	
	glBufferData(GL_ARRAY_BUFFER, this->_numVerts*4*sizeof(GLfloat), vData, GL_STATIC_DRAW);
	//Setup normal buffer	
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_NORMAL_BUFFER]);	
	glBufferData(GL_ARRAY_BUFFER, this->_numVerts*4*sizeof(GLfloat), nData, GL_STATIC_DRAW);
	//Setup texcoord buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_TEXCOORD_BUFFER]);	
	glBufferData(GL_ARRAY_BUFFER, this->_numVerts*2*sizeof(GLfloat), tData, GL_STATIC_DRAW);		
/*** Debug ***
	std::cout << "Generate Size4 Verts: " << this->_numVerts << std::endl;
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_VERTEX_BUFFER]);	
	GLfloat *data2= (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	for (int i=0; i<this->_numVerts; i++) printf("\t%d V: %f %f %f %f\n", i, data2[i*4], data2[i*4+1], data2[i*4+2], data2[i*4+3]);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	//Show Normal Data
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_NORMAL_BUFFER]);
	data2 = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	for (int i=0; i<this->_numVerts; i++) printf("\t%d: %f %f %f\n", i, data2[i*4], data2[i*4+1], data2[i*4+2]);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	//Show TexCoord Data
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_TEXCOORD_BUFFER]);
	data2 = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	for (int i=0; i<this->_numVerts; i++) printf("\t%d: %f %f\n", i, data2[i*2], data2[i*2+1]);
	glUnmapBuffer(GL_ARRAY_BUFFER);
/*** Debug ***/
	//Clean up and report errors
	glBindBuffer(GL_ARRAY_BUFFER, 0);	
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateSurfaceSize4 - Unspecified error.");
}


void WCNurbsSurface::GenerateIndex(void) {
	//Allocate space for local array of data
	GLuint size = (this->_lodU-1) * (this->_lodV-1) * 2 * 3;  //Two triangles per lod, 3 verts per triangle
	GLuint data[size];			
	//Check to make sure data was allocated
	if (data == NULL) {	
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateIndex - Unable to allocate space for index data."); return; }
	int ll, index = 0;
	//Loop through each line
	for(WPUInt v=0; v<this->_lodV-1; v++) {
		//Loop through each element in the line
		for (WPUInt u=0; u<this->_lodU-1; u++) {
			//Calculate vertex index of lower left vertex
			ll = this->_lodU * v + u;
			//Upper triangle
			data[index++] = ll;							//	1--2
			data[index++] = ll + this->_lodU;			//	|/
			data[index++] = data[index-1] + 1;			//	0
			//Lower triangle
			data[index++] = ll;							//	   2
			data[index++] = data[index-2];				//	 / |
			data[index++] = ll + 1;						//	0--3
		}
	}
	//Allocate space for index array
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_INDEX_BUFFER]);
	//Copy data into buffer object
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(GLuint), data, GL_STATIC_DRAW);	
/*** Debug ***
	std::cout << "Generate Index Values: " << size << std::endl;	
	GLint *data2 = (GLint*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_ONLY);
	for (int i=0; i<size/3; i++) printf("\t%d: %d %d %d\n", i, data2[i*3], data2[i*3+1], data2[i*3+2]);
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);	
/*** Debug ***/
	//Clean up and report errors
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);	
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateIndex - Unspecified error.");
}


/***********************************************~***************************************************/


WCNurbsSurface::WCNurbsSurface(WCGeometryContext *context, const WPUInt &degreeU, const WPUInt &degreeV, const WPUInt &cpU, const WPUInt &cpV, 
	const std::vector<WCVector4> &controlPoints, const WCNurbsMode &modeU, const WCNurbsMode &modeV,
	const std::vector<WPFloat> &kpU, const std::vector<WPFloat> &kpV) : ::WCGeometricSurface(), _context(context),
	_lodU(0), _lodV(0), _numVerts(0), _degreeU(degreeU), _degreeV(degreeV), _modeU(modeU), _modeV(modeV), 
	_cpU(cpU), _cpV(cpV), _controlPoints(controlPoints), _kpU(0), _kpV(0), _knotPointsU(), _knotPointsV() {
	
	//Check to make sure a CP collection was passed
	if (this->_controlPoints.size() == 0) { 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::WCNurbsSurface - Invalid control points collection."); return;	}
	//Check to make sure there are at least 2 control points
	if ((this->_cpU < 2) || (this->_cpV < 2)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::WCNurbsSurface - Must have at least two control points in each direction.");
		return; }
	//Make sure not too many control points
	if ((this->_cpU * this->_cpV) > NURBSSURFACE_MAX_CONTROLPOINTS) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::WCNurbsSurface - Too many control points.");
		return; }
	//Check to make sure that degree is greater than 0
	if ((this->_degreeU == 0) || (this->_degreeV == 0)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::WCNurbsSurface - Degree must be greater than 0.");
		if (this->_degreeU == 0) this->_degreeU = 1;
		if (this->_degreeV == 0) this->_degreeU = 1;
	}
	//Check to make sure degree is less than the number of control points
	if ((this->_degreeU >= this->_cpU) || (this->_degreeV >= this->_cpV)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::WCNurbsSurface - Degree must be less than the number of control points.");
		this->_degreeU = STDMIN(this->_cpU - 1, this->_degreeU);
		this->_degreeV = STDMIN(this->_cpV - 1, this->_degreeV);	
	}
	//Define the number of knot points
	this->_kpU = this->_degreeU + this->_cpU + 1;
	this->_kpV = this->_degreeV + this->_cpV + 1;
	//Perform some addtional checks if mode is custom
	if (this->_modeU == WCNurbsMode::Custom()) {
		//See if the collection is null
		if (kpU.size() == 0) { 
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::WCNurbsSurface - Invalid U knot points collection."); return; }
		//Make sure the collection is the right size
		if (kpU.size() != this->_kpU) { 
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::WCNurbsSurface - Invalid U knot points collection size."); return; }
	}
	if (this->_modeV == WCNurbsMode::Custom()) {
		//See if the collection is null
		if (kpV.size() == 0) { 
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::WCNurbsSurface - Invalid V knot points collection."); return; }
		//Make sure the collection is the right size
		if (kpV.size() != this->_kpV) { 
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::WCNurbsSurface - Invalid V knot points collection size."); return; }
	}

	//Set up buffers
	glGenBuffers(4, this->_buffers);
	//Use knot mode to setup knot array
	this->LoadKnotPoints(kpU, kpV);
	//Estimate LOD values
	WPFloat length = WCNurbs::EstimateLengthU(this->_controlPoints, this->_cpU);
	this->_lodU = STDMAX( (int)(length / NURBSSURFACE_GENERATE_ACCURACY) + 1, 2);
	length = WCNurbs::EstimateLengthV(this->_controlPoints, this->_cpV);
	this->_lodV = STDMAX( (int)(length / NURBSSURFACE_GENERATE_ACCURACY) + 1, 2);
	this->_numVerts = this->_lodU * this->_lodV;
	//Validate surface flags
	this->ValidateClosure();
	this->ValidateSelfIntersection();
	//Establish aligned bounding box
	this->_bounds = new WCAlignedBoundingBox(this->_controlPoints);
	//Doe some error checking
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::WCNurbsSurface - Unspecified error.");
}


WCNurbsSurface::~WCNurbsSurface() {
	//Delete the buffers
	glDeleteBuffers(5, this->_buffers);
	//Clear the collection of control points
	this->_controlPoints.clear();
	//Delete the knot point array
	if (this->_knotPointsU != NULL) delete this->_knotPointsU;
	if (this->_knotPointsV != NULL) delete this->_knotPointsV;
	//Check for errors
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::~WCNurbsSurface - Unspecified error.");
}


GLuint WCNurbsSurface::VertexBuffer(void) {
	//Check to see if surface needs to be generated
	if (this->_isVisualDirty) {
		//Generate the curve - switch on performance level
		switch(this->_context->SurfacePerformanceLevel()) {
			case NURBSSURFACE_PERFLEVEL_HIGH:	this->GenerateSurfaceHigh(); break;
			case NURBSSURFACE_PERFLEVEL_MEDIUM: this->GenerateSurfaceMedium(); break;
			case NURBSSURFACE_PERFLEVEL_LOW:	this->GenerateSurfaceLow(); break;
		}
		//Generate the index for the curve
		this->GenerateIndex();
		//Mark as clean
		this->_isVisualDirty = false;
	}
	//Return the buffer
	return this->_buffers[NURBSSURFACE_VERTEX_BUFFER];
}


void WCNurbsSurface::Degree(const WPUInt &degreeU, const WPUInt &degreeV) {
	WPUInt degU = degreeU;
	WPUInt degV = degreeV;
	if ((degU > NURBSSURFACE_MAX_DEGREE) || (degV > NURBSSURFACE_MAX_DEGREE)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::Degree - Degree must be less than NURBSSURFACE_MAX_DEGREE.");
		degU = MIN(NURBSSURFACE_MAX_DEGREE, degU);
		degV = MIN(NURBSSURFACE_MAX_DEGREE, degV);
	}
	//Check to make sure degree is less than the number of control points
	if ((degU >= this->_cpU) || (degV >= this->_cpV)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::Degree - Degree must be less than the number of control points.");
		degU = STDMIN(this->_cpU - 1, degU);
		degV = STDMIN(this->_cpV - 1, degV);
	}
	if ((degU < 1) || (degV < 1)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::Degree - Degree must be greater than 1.");
		degU = MAX(degreeU, 1);
		degV = MAX(degV, 1);
	}
	//Don't let degree change if mode is Custom
	if ( (this->_modeU == WCNurbsMode::Custom()) || (this->_modeU == WCNurbsMode::Custom())) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::Degree - Degree change not allowed in custom knot mode.");
		return;
	}
	//Set the new degree
	this->_degreeU = degU;
	this->_degreeV = degV;
	//Determine the correct number of knot points
	this->_kpU = this->_degreeU + this->_cpU + 1;
	this->_kpV = this->_degreeV + this->_cpV + 1;
	//Use knot mode to setup knot array
	this->LoadKnotPoints();
/*** Debug ***
	std::cout << "Degrees: " << this->_degreeU << " " << this->_degreeV << std::endl;
/*** Debug ***/
	//Mark the object as dirty
	this->_isVisualDirty = true;
}


void WCNurbsSurface::LevelOfDetail(const WPUInt &lodU, const WPUInt &lodV) {
	WPUInt levelU = lodU;
	WPUInt levelV = lodV;
	//Check to make sure lodU is valid
	if ((levelU < 2) || (levelU > NURBSSURFACE_MAX_LOD)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::LevelOfDetail - LOD U(" << levelU << ") out of bounds.");
		//throw error
		return;
	}
	//Check to make sure lodV is valid
	if ((levelV < 2) || (levelV > NURBSSURFACE_MAX_LOD)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::LevelOfDetail - LOD V(" << levelV << ") out of bounds.");
		//throw error
		return;
	}
	//Set the new lod
	this->_lodU = levelU;
	this->_lodV = levelV;
	this->_numVerts = this->_lodU * this->_lodV;
/*** Debug ***/
//	std::cout << "LOD: " << this->_lodU << " " << this->_lodV << std::endl;	
/*** Debug ***/
	//Mark the object as dirty
	this->_isVisualDirty = true;
}


bool WCNurbsSurface::Intersect(const WCGeometricPoint &point, const WPFloat &tolerance) {
	return false;
}


bool WCNurbsSurface::Intersect(const WCGeometricCurve &curve, const WPFloat &tolerance) {
	return false;
}


bool WCNurbsSurface::Intersect(const WCGeometricSurface &surface, const WPFloat &tolerance) {
	return false;
}


WPFloat WCNurbsSurface::Area(void) {
	return 0.0;
}


WCVisualObject* WCNurbsSurface::HitTest(const WCRay &ray, const WPFloat &tolerance) {
	std::cout << "WCNurbsSurface::HitTest Error - Not yet implemented.\n";
	return NULL;
}


void WCNurbsSurface::ApplyTransform(const WCMatrix4 &transform) {
	std::cout << "WCNurbsSurface::ApplyTransform Error - Not yet implemented.\n";
}


void WCNurbsSurface::ApplyTranslation(const WCVector4 &translation) {
	std::cout << "WCNurbsSurface::ApplyTranslation Error - Not yet implemented.\n";
}


void WCNurbsSurface::Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom) {
	//Make sure surface is visible
	if (!this->_isVisible) return;
	//Check to see if surface needs to be generated
	if (this->_isVisualDirty) {
		//Generate the curve - switch on performance level
		switch(this->_context->SurfacePerformanceLevel()) {
			case NURBSSURFACE_PERFLEVEL_HIGH:	this->GenerateSurfaceHigh(); break;
			case NURBSSURFACE_PERFLEVEL_MEDIUM: this->GenerateSurfaceMedium(); break;
			case NURBSSURFACE_PERFLEVEL_LOW:	this->GenerateSurfaceLow(); break;
		}
		//Generate the index for the curve
		this->GenerateIndex();
		//Mark as clean
		this->_isVisualDirty = false;
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
		glUseProgram(0);
	}
	//Draw the geometry
	glDrawElements(GL_TRIANGLES, (this->_lodU-1)*(this->_lodV-1)*2*3, GL_UNSIGNED_INT, 0);
	
	/*** Draw done - now clean up ***/
	
	//Bind back to nothing
	glBindBuffer(GL_ARRAY_BUFFER, 0);	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//Make sure that vertex and normal arrays are disabled
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_INDEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	//Restore the default prog
//	if (this->_renderProg != 0) glUseProgram(defaultProg);
	//Report them errors
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::Render - Unspecified error.");
}


void WCNurbsSurface::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
	//Mark the surface as dirty
	this->_isVisualDirty = true;
	//Estimate LOD values
	WPFloat length = WCNurbs::EstimateLengthU(this->_controlPoints, this->_cpU);
	this->_lodU = (WPUInt)(length / NURBSSURFACE_GENERATE_ACCURACY) + 1;
	length = WCNurbs::EstimateLengthV(this->_controlPoints, this->_cpV);
	this->_lodV = (WPUInt)(length / NURBSSURFACE_GENERATE_ACCURACY) + 1;
	this->_numVerts = this->_lodU * this->_lodV;	
	//Make sure all dependent objects know about it
	this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);	
}

	
WCVector4 WCNurbsSurface::Evaluate(const WPFloat &u, const WPFloat &v) {
	WPFloat evalU = u;
	WPFloat evalV = v;
	//Bounds check the u and v values
	if ((evalU < this->_knotPointsU[0]) || (evalU > this->_knotPointsU[this->_kpU-1]) || 
		(evalV < this->_knotPointsV[0]) || (evalV > this->_knotPointsV[this->_kpV-1])) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::Evaluate - U or V out of bounds.");
		return WCVector4();
	}
	//Otherwise, find the span for the u and v values
	WPUInt spanU = WCNurbs::FindSpan(this->_cpU, this->_degreeU, evalU, this->_knotPointsU);
	WPUInt spanV = WCNurbs::FindSpan(this->_cpV, this->_degreeV, evalV, this->_knotPointsV);
	//Calculate basis values for U and V
	WPFloat* basisValuesU = WCNurbs::BasisValues(spanU, evalU, this->_degreeU, this->_knotPointsU);
	if (basisValuesU == NULL) return WCVector4();		
	WPFloat* basisValuesV = WCNurbs::BasisValues(spanV, evalV, this->_degreeV, this->_knotPointsV);	
	if (basisValuesV == NULL) { delete basisValuesU; return WCVector4(); }
	//Declare some temparary values
	WCVector4 c;
	int index;
	WPFloat w=0;
	WCVector4 point;
	//Evaluate the point
	for(WPUInt i=0; i<=this->_degreeV; i++) {
		//Determine the index of the control point
		index = (this->_cpU * (spanV - this->_degreeV + i)) + spanU - this->_degreeU;
		for (WPUInt j=0; j<=this->_degreeU; j++) {
			point = this->_controlPoints.at(index);			
			w = w + (point.L() * basisValuesU[j] * basisValuesV[i]);
			c = c + point * point.L() * basisValuesU[j] * basisValuesV[i];
			index++;
		}
	}
	//Do the w-divide
	c /= w;
	//Set W = 1.0
	c.L(1.0);
	//Make sure to delete basis values
	delete basisValuesU;
	delete basisValuesV;
	//Return the evaluated point
	return c;
}



WCVector4 WCNurbsSurface::Derivative(const WPFloat &u, const WPUInt &uDer, const WPFloat &v, const WPUInt &vDer) {
	std::cout << "WCNUrbsSurface::Derivative Error - Not yet implemented.\n";
	return WCVector4();
}


WCRay WCNurbsSurface::Tangent(const WPFloat &u, const WPFloat &v) {
	std::cout << "WCNUrbsSurface::Tangent Error - Not yet implemented.\n";	
	return WCRay(WCVector4(), WCVector4());
}


WCVector4 WCNurbsSurface::PointInversion(const WCVector4 &point) {
	//Special case for degree=1 curves
	if ((this->_degreeU == 1) && (this->_degreeV == 1)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::PointInversion - Not yet implemented for degree=1 surfaces.");
		return WCVector4();
	}
	//Check to see if surface needs to be generated
	if (this->_isVisualDirty) {
		//Generate the curve - switch on performance level
		switch(this->_context->SurfacePerformanceLevel()) {
			case NURBSSURFACE_PERFLEVEL_HIGH: this->GenerateSurfaceHigh(); break;
			case NURBSSURFACE_PERFLEVEL_MEDIUM: this->GenerateSurfaceMedium(); break;
			case NURBSSURFACE_PERFLEVEL_LOW: this->GenerateSurfaceLow(); break;
		}
		//Generate the index for the curve
		this->GenerateIndex();		
		//Mark as clean
		this->_isVisualDirty = false;		
	}
	//Get base vector for point
	WCVector4 refPoint = point;
	refPoint.K(0.0);
	WCVector4 curvePoint;
	WPUInt i, j;
	int index=-1;
	WPFloat u, v, dist, minDist=10000000.0;

	//Find the closest point on the surface
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_VERTEX_BUFFER]);
	GLfloat *data = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	//Loop through all points and find closest
	for (i=0; i<(this->_lod+1)*(this->_lod+1); i++) {
		//Create a point from the data set
		j = i * NURBSSURFACE_FLOATS_PER_VERTEX;
		curvePoint.Set(data[j], data[j+1], data[j+2], data[j+3]);
		//Calculate distance from the reference point
		curvePoint = refPoint - curvePoint;
		dist = curvePoint.Magnitude();
		if (dist < minDist) {
			//Set the min distance
			minDist = dist;
			//Save the point index
			index = i;
		}
	}
	//Make sure to unmap the buffer
	glUnmapBuffer(GL_ARRAY_BUFFER);
	//Calculate the u,v value for the initial minimum point
	u = (index % (this->_lod+1)) * this->_knotPointsU[this->_kpU-1] / (this->_lod+1);
	v = (index / (this->_lod+1)) * this->_knotPointsV[this->_kpV-1] / (this->_lod+1);

	//Initialize metrics
	WPFloat *bvU, *bvV;
	WCVector4 pt;
	dist = minDist;
	int spanU, spanV;
	WCVector4 s, sU, sV, sUU, sVV, sUV, r;
	WPFloat uNx, vNx, f, g, suMag, svMag, fu, fv, gu, gv, bottom, top;
	//Loop until condition 1, 2, or 4 is met
	int iterCount = 0;
	while (iterCount <= NURBSSURFACE_INVERSION_MAX_ITERATIONS) {
		//Init the point values
		s.Set(0.0, 0.0, 0.0, 0.0);
		sU.Set(0.0, 0.0, 0.0, 0.0);
		sUU.Set(0.0, 0.0, 0.0, 0.0);
		sUV.Set(0.0, 0.0, 0.0, 0.0);
		sV.Set(0.0, 0.0, 0.0, 0.0);
		sVV.Set(0.0, 0.0, 0.0, 0.0);
		//Find the span and basis values up to second derivative
		spanU = WCNurbs::FindSpan(this->_cpU, this->_degreeU, u, this->_knotPointsU);
		bvU = WCNurbs::BasisValues(spanU, u, this->_degreeU, this->_knotPointsU, 2);
		spanV = WCNurbs::FindSpan(this->_cpV, this->_degreeV, v, this->_knotPointsV);
		bvV = WCNurbs::BasisValues(spanV, v, this->_degreeV, this->_knotPointsV, 2);
		//Evaluate the point
		for(i=0; i<=this->_degreeV; i++) {
			//Determine the index of the control point
			index = (this->_cpU * (spanV - this->_degreeV + i)) + spanU - this->_degreeU;
			for (j=0; j<=this->_degreeU; j++) {
				pt = this->_controlPoints.at(index);			
				s = s + (pt * bvU[j] * bvV[i]);
				sU = sU + (pt * bvU[j+this->_degreeU+1] * bvV[i]);
				sV = sV + (pt * bvU[j] * bvV[i+this->_degreeV+1]);
				sUU = sUU + (pt * bvU[j+2*(this->_degreeU+1)] * bvV[i]);
				sVV = sVV + (pt * bvU[j] * bvV[i+2*(this->_degreeV+1)]);
				sUV = sUV + (pt * bvU[j+this->_degreeU+1] * bvV[i+this->_degreeV+1]);
				index++;
			}
		}
		//Make sure to delete the basis values
		delete bvU;
		delete bvV;					
		//Calculate metrics
		r = s - refPoint;
		dist = r.Magnitude();	
		//Check for condition 1
		if (dist < NURBSSURFACE_EPSILON_ONE) return s;
		
		//Do some interim calculations
		f = sU.DotProduct(r);
		suMag = sU.Magnitude();
		//Check condition 2 - part 1
		if (( ABS(f) / (suMag * dist) ) <= NURBSSURFACE_EPSILON_TWO) return s;					
		//Now the other part
		g = sV.DotProduct(r);
		svMag = sV.Magnitude();
		//Check condition 2 - part 2
		if (( ABS(g) / (svMag * dist) ) <= NURBSSURFACE_EPSILON_TWO) return s;		
		
		//Create 
		fu = (suMag * suMag) + r.DotProduct(sUU);
		fv = sU.DotProduct(sV) + r.DotProduct(sUV);
		gu = fv;
		gv = (svMag * svMag) + r.DotProduct(sVV);
		bottom = (fu * gv) - (gu * fv);
		//Check to make sure no div by zero
		if (bottom == 0.0) {
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::PointInverse - |A| is zero.");
			return s;
		}
		//Calculate delta u
		top = (-f * gv) - (-g * fv);
		uNx = top / bottom;
		//Calculate delta v
		top = (-g * fu) - (-f * gu);
		vNx = top / bottom;												
		
		//Check condition 3
		if (u + uNx < this->_knotPointsU[spanU]) uNx = 0;
		if (u + uNx > this->_knotPointsU[spanU+1]) uNx = this->_knotPointsU[spanU+1] - u;	
		if (v + vNx < this->_knotPointsV[spanV]) vNx = 0;
		if (v + vNx > this->_knotPointsV[spanV+1]) vNx = this->_knotPointsV[spanV+1] - u;
		
		//Check condition 4
		if ( (sU*uNx + sV*vNx).Magnitude() <= NURBSSURFACE_EPSILON_ONE) return s;
		
		//Otherwise, increment u and v
		u = u + uNx;
		v = v + vNx;
		//Increment loop interation counter
		iterCount++;
	}
	return s;
}
	
	
void WCNurbsSurface::InsertKnotU(const WPFloat &u, const WPUInt &multiplicity) {
}


void WCNurbsSurface::InsertKnotV(const WPFloat &v, const WPUInt &multiplicity) {
}


void WCNurbsSurface::RefineKnot(void) {
}


void WCNurbsSurface::RemoveKnot(void) {
}


void WCNurbsSurface::ElevateDegree(void) {
}


void WCNurbsSurface::ReduceDegree(void) {
}


WCNurbsSurface& WCNurbsSurface::operator=(const WCNurbsSurface &surface) {
	std::cout << "WCNUrbsSurface::operator= Error - Not yet implemented.\n";
	return *this;
}


bool WCNurbsSurface::operator==(const WCNurbsSurface &surface) {
	std::cout << "WCNUrbsSurface::operator== Error - Not yet implemented.\n";
	return false;
}


xercesc::DOMElement* WCNurbsSurface::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Create the base element for the object
	xercesc::DOMElement*  surfaceElem = document->createElement(xercesc::XMLString::transcode("NURBSSurface"));
	return surfaceElem;
}


/***********************************************~***************************************************/


WCNurbsSurface* WCNurbsSurface::ExtrudeCurve(WCGeometryContext *context, WCGeometricCurve* curve, const WCVector4 &direction, const WPFloat &posDepth, 
	const WPFloat &negDepth, const bool &aligned) {
//	std::cout << "Aligned: " << aligned << std::endl;
	//Make sure curve is not null
	if (curve == NULL) return NULL;
	WCGeometricLine* line = dynamic_cast<WCGeometricLine*>(curve);
	//See if curve is a simple line
	if (line != NULL) {
		//Create vector of control points
		std::vector<WCVector4> cp;
		//Get the four corners
		WCVector4 pt0, pt1;
		if (aligned) {
			pt0 = line->Evaluate(0.0) + (direction * negDepth);
			pt1 = line->Evaluate(1.0) + (direction * negDepth);
		} else {
			pt0 = line->Evaluate(1.0) + (direction * negDepth);
			pt1 = line->Evaluate(0.0) + (direction * negDepth);			
		}
		WCVector4 pt2 = pt0 + (direction * (posDepth - negDepth));
		WCVector4 pt3 = pt1 + (direction * (posDepth - negDepth));
		cp.push_back(pt0);
		cp.push_back(pt1);
		cp.push_back(pt2);
		cp.push_back(pt3);
		//Create the curve
		WCNurbsSurface *surface = new WCNurbsSurface(context, 1, 1, 2, 2, cp, WCNurbsMode::Default(), WCNurbsMode::Default());
		surface->LevelOfDetail(2, 2);
		return surface;
	}
	//Otherwise it is a nurbs curve
	else {
		WCNurbsCurve* nurbs = dynamic_cast<WCNurbsCurve*>(curve);
		//Check to make sure cast is valid
		if (nurbs == NULL) {
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::ExtrudeCurve - Unknown geometric curve passed.");
			return NULL;
		}
		//Get the control point collection
		WPUInt curveDegree = nurbs->Degree();
		WPUInt curveNumCP = nurbs->NumberControlPoints();
		std::vector<WCVector4> oldCP = nurbs->ControlPoints();
		std::vector<WCVector4> cp;
		WPFloat *knotPoints = nurbs->KnotPoints();
		std::vector<WPFloat> kp;
		for (WPUInt i=0; i<nurbs->NumberKnotPoints(); i++) kp.push_back(knotPoints[i]);
		WCVector4 data;
		//Check alignment
		if (aligned) {
			//First loop
			for (WPUInt j=0; j<curveNumCP; j++) {
				data = oldCP.at(j) + (direction * negDepth);
				cp.push_back(data);
			}
			//Second loop
			for (WPUInt j=0; j<curveNumCP; j++) {
				data = oldCP.at(j) + ((direction * negDepth * 0.5) + (direction * posDepth * 0.5));
				cp.push_back(data);
			}
			//Third loop
			for (WPUInt j=0; j<curveNumCP; j++) {
				data = oldCP.at(j) + (direction * posDepth);
				cp.push_back(data);
			}
		}
		//Reverse alignment
		else {
			//First loop
			for (int j=curveNumCP-1; j>=0; j--) {
				data = oldCP.at(j) + (direction * negDepth);
				cp.push_back(data);
			}
			//Second loop
			for (int j=curveNumCP-1; j>=0; j--) {
				data = oldCP.at(j) + ((direction * negDepth * 0.5) + (direction * posDepth * 0.5));
				cp.push_back(data);
			}
			//Third loop
			for (int j=curveNumCP-1; j>=0; j--) {
				data = oldCP.at(j) + (direction * posDepth);
				cp.push_back(data);
			}
		}

		WCNurbsMode curveMode = nurbs->Mode();
		WCNurbsSurface *surface = new WCNurbsSurface(context, curveDegree, 2, curveNumCP, 3, cp, curveMode, WCNurbsMode::Default(), kp);
		return surface;
	}
}


/***********************************************~***************************************************/


WCNurbsSurface* WCNurbsSurface::RevolveCurve(WCGeometryContext *context, WCGeometricCurve *curve, const WCRay *axis, 
	const WPFloat &angle, const bool &aligned) {
	
	//Make sure curve is not null
	if (curve == NULL) return NULL;

	/*** Set up new surface knot points ***/

	//Determine number of arcs for the rotation
	WPUInt numArcs, numCPU, numKPU;
	WPFloat knotPoints[12];
	//If angle <= 90 degree, only one arc needed
	if (angle <= M_PI * 0.5) {
		numArcs = 1;
		numKPU = 6;
		numCPU = 3;
	}
	//If less than 180 degrees, two arcs needed
	else if (angle <= M_PI) {
		numArcs = 2;
		knotPoints[3] = 0.5;
		knotPoints[4] = 0.5;
		numKPU = 8;
		numCPU = 5;
	}
	//If angle less than 270 degrees, three arcs needed
	else if (angle <= M_PI * 1.5) {
		numArcs = 3;
		knotPoints[3] = 1.0 / 3.0;
		knotPoints[4] = knotPoints[3];
		knotPoints[5] = 2.0 / 3.0;
		knotPoints[6] = knotPoints[5];
		numKPU = 10;
		numCPU = 7;
	}
	//Otherwise, four arcs needed
	else {
		numArcs = 4;
		knotPoints[3] = 0.25;
		knotPoints[4] = knotPoints[3];
		knotPoints[5] = 0.5;
		knotPoints[6] = knotPoints[5];
		knotPoints[7] = 0.75;
		knotPoints[8] = knotPoints[7];
		numKPU = 12;
		numCPU = 9;
	}
	//Determine angle per arc
	WPFloat deltaAngle = angle / numArcs;
	int j = 3 + 2 * (numArcs - 1);
	//Set the first and last knotPoints
	for (int i=0; i<3; j++, i++) {
		knotPoints[i] = 0.0;
		knotPoints[j] = 1.0;
	}

	/*** Set up new surface control points ***/

	//Setup curve controlPoint, degree, and knot point values
	WPUInt curveDegree;
	std::vector<WCVector4> curveCP;
	std::vector<WPFloat> curveKP;
	WCNurbsMode curveMode = WCNurbsMode::Default();
	WCGeometricLine* line = dynamic_cast<WCGeometricLine*>(curve);
	//See if curve is a simple line
	if (line != NULL) {
		//Make sure line does not lie along axis
		WCVector4 beginTest = ProjectPointToLine3D(axis->Base(), axis->Direction(), line->Begin());
		WCVector4 endTest = ProjectPointToLine3D(axis->Base(), axis->Direction(), line->End());
		//If both are the same after being projected then no valid rev surf.
		if ((beginTest.Distance(line->Begin()) < 0.0001) && (endTest.Distance(line->End()) < 0.0001)) {
//			CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCNurbsSurface::RevolveCurve - Axis degenerate point found.");
			return NULL;
		}
		//set curve degree to 2
		curveDegree = 2;
		//Setup line control points (take aligned into account)
		if (!aligned) {
			//Get first point
			curveCP.push_back( line->Begin() );
			//Get mid-point
			curveCP.push_back( line->Evaluate(0.5) );
			//Get last point
			curveCP.push_back( line->End() );
		}
		else {
			//Get first point
			curveCP.push_back( line->End() );
			//Get mid-point
			curveCP.push_back( line->Evaluate(0.5) );
			//Get last point
			curveCP.push_back( line->Begin() );
		}
		//Knot point array (3 0.0, and 3 1.0)
		curveKP.insert(curveKP.end(), 3, 0.0);
		curveKP.insert(curveKP.end(), 3, 1.0);
	}
	//Otherwise, try to get a NURBS curve
	else {
		WCNurbsCurve* nurbs = dynamic_cast<WCNurbsCurve*>(curve);
		//Check to make sure cast is valid
		if (nurbs == NULL) {
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::RevolveCurve - Unknown geometric curve passed.");
			return NULL;
		}
		//Get the degree
		curveDegree = nurbs->Degree();
		curveCP = nurbs->ControlPoints();
		curveMode = nurbs->Mode();
		//Setup curveKP
		WPFloat *kpArray = nurbs->KnotPoints();
		for (WPUInt i=0; i<nurbs->NumberKnotPoints(); i++)
			curveKP.push_back(kpArray[i]);
		//Reverse curve values if necessary
		if (aligned) {
			std::vector<WCVector4> tmpVec = curveCP;
			curveCP.clear();
			//Reverse tmpVec
			for (int i=tmpVec.size()-1; i>=0; i--) curveCP.push_back( tmpVec.at(i) );
		}
	}
	WPUInt numCPV = curveCP.size();

	//Holding arrays for sines and cosines
	WPFloat theta = 0.0;
	WPFloat cosines[numArcs+1];
	WPFloat sines[numArcs+1];
	//Determine all angle values
	for (WPUInt i=1; i<=numArcs; i++) {
		theta = theta + deltaAngle;
		cosines[i] = cos(theta);
		sines[i] = sin(theta);
	}

	WPFloat r, uDummy, vDummy;
	WPFloat wm = cos(deltaAngle / 2.0), weight;
	WCVector4 Pij[numCPU][numCPV];
	WCVector4 O, x, y, t, P0, P2, T0, T2, pt, tmpVec;
	int index;
	//Loop and compute each u row of control points and weights
	for (WPUInt j=0; j<numCPV; j++) {
		//Project curveCP[j] onto axis
		O = ProjectPointToLine3D(axis->Base(), axis->Direction(), curveCP[j]);
		x = curveCP[j] - O;
		//Make sure there is no weight on x
		x.L(0.0);
		r = x.Magnitude();
		//If r ~ 0, then there will be issues with the normal at that point
		if (fabs(r - 0.0) < 0.0001) {
//			CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCNurbsSurface::RevolveCurve - Point on axis degenerate case.");
		}
		x.Normalize(true);
		y = x.CrossProduct( axis->Direction() );
		P0 = curveCP[j];
		Pij[0][j] = curveCP[j];
		T0 = y;
		index = 0;
		//Compute u row
		for (WPUInt i=1; i<=numArcs; i++) {
			//Determine the second point
			P2 = O + (x * r * cosines[i]) + (y * r * sines[i]);
			Pij[index+2][j] = P2;
			//Make sure to get the control point weight
			Pij[index+2][j].L( curveCP[j].L() );
			T2 = (x * -1.0 * sines[i]) + (y * cosines[i]);
			//Intersect P0-T0 and P2-T2
			RayRayIntersection(P0, T0, P2, T2, uDummy, vDummy, pt);
			Pij[index+1][j] = pt;
			//Set the right weight
			weight = wm * curveCP[j].L();
			Pij[index+1][j].L( weight );
			index += 2;
			//If not last arc
			if (i < numArcs) { P0 = P2; T0 = T2; }
		}
	}

	//Convert Pij into vector
	std::vector<WCVector4> controlPoints;
	for (WPUInt j=0; j<numCPV; j++)
		for (int i=numCPU-1; i>=0; i--)
			controlPoints.push_back( Pij[i][j] );
	
	//Convert knotPoints to vector
	std::vector<WPFloat> knotPointsU;
	for (WPUInt i=0; i<numKPU; i++) knotPointsU.push_back(knotPoints[i]);
	
/*** DEBUG ***
	std::cout << "DegreeU: 2\n";
	std::cout << "DegreeV: " << curveDegree << std::endl;
	std::cout << "NumCPU: " << numCPU << std::endl;
	std::cout << "NumCPV: " << numCPV << std::endl;
	for (int i=0; i<knotPointsU.size(); i++)
		std::cout << "KPU(" << i << "): " << knotPointsU.at(i) << std::endl;
	for (int i=0; i<curveKP.size(); i++)
		std::cout << "KPV(" << i << "): " << curveKP.at(i) << std::endl;
	for (int i=0; i<controlPoints.size(); i++)
		std::cout << "CP(" << i << "): " << controlPoints.at(i) << std::endl;
/*** DEBUG ***/

	/*** Create new surface knot points ***/

	//Create revolution surface
	WCNurbsSurface *surface = new WCNurbsSurface(context, 2, curveDegree, numCPU, numCPV, controlPoints,
		WCNurbsMode::Custom(), curveMode, knotPointsU, curveKP);
	return surface;
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCNurbsSurface &surface) {
	//Print out basic surface info
	out << "NurbsSurface(" << (void*)&surface << "):\n";
	out << "\t Verts: " << surface._numVerts << std::endl;
	out << "\t LOD U:" << surface._lodU << " V: " << surface._lodV << std::endl;
	out << "\t Degree(" << surface._degreeU << " & " << surface._degreeV << ")\n";	

	//Only print if collection has items
	if (surface._controlPoints.size() != 0) {
		//Print out control point information
		out << "\t Control Points(" << surface._cpU << " x " <<  surface._cpV << ")\n";
		for (WPUInt i=0; i<surface._controlPoints.size(); i++) {
			std::cout << "\t\t{" << surface._controlPoints.at(i).I() << ", \t";
			std::cout << surface._controlPoints.at(i).J() << ", \t";
			std::cout << surface._controlPoints.at(i).K() << ", \t";
			std::cout << surface._controlPoints.at(i).L() << "}\n";
		}
	}
	
	//Print out knot point information
	out << "\t Knot Points(" << surface._kpU << " & " << surface._kpV << ")\n";
	if (surface._knotPointsU != NULL)
		out << "\t\t U: "; for (WPUInt i=0; i < surface._kpU; i++) out << surface._knotPointsU[i] << " "; out << std::endl;
	if (surface._knotPointsV != NULL)
		out << "\t\t V: "; for (WPUInt i=0; i < surface._kpV; i++) out << surface._knotPointsV[i] << " "; out << std::endl;
	return out;
}


/***********************************************~***************************************************/

