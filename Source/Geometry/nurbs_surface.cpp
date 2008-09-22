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
 

/*** Extern Variables ***/
extern WPFloat* __bezier_coef[8];


/*** Locally Defined Values ***/
#define NURBSSURFACE_INVERSION_MAX_ITERATIONS	12
#define NURBSSURFACE_EPSILON_ONE				0.0001
#define NURBSSURFACE_EPSILON_TWO				0.0001
#define NURBSSURFACE_EQUALITY_EPSILON			0.001
#define NURBSSURFACE_RENDER_ACCURACY			0.20
#define NURBSSURFACE_RENDER_LOWER				0.85
#define NURBSSURFACE_RENDER_UPPER				1.55


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
	GLfloat *data = new GLfloat[size / sizeof(GLfloat)];
	//Check to see if shader array is undersize
	if ((WPUInt)this->_context->SurfaceMinKPBufferSize() < size)
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateKnotPointsVBO - Undersized array in shader.");
	//Otherwise, copy the kp array into the vbo (Cast WPFloat to GLfloat)
	for (WPUInt i=0; i<this->_kpU; i++) data[i*4] = (GLfloat)this->_knotPointsU[i];
	//Bind the knot point buffer and load it
	glBindBuffer(GL_ARRAY_BUFFER, this->_context->SurfaceKPUBuffer());
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	//Delete the array
	delete data;
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
	if (glGetError() != GL_NO_ERROR)
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateKnotPointsVBO - Unspecified GL Error.");
}


void WCNurbsSurface::GenerateKnotPointsTextures(void) {
	//Create temporary array for data
	GLfloat *dataU = new GLfloat[4 * this->_kpU];
	//Copy knot points into array (cast WPFloat to GLfloat)
	for (WPUInt i=0; i<this->_kpU; i++) dataU[i*4] = (GLfloat)this->_knotPointsU[i];
	//Set up some parameters
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//Set up texture
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_context->SurfaceKPUTex());	
	glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, this->_kpU, 1, GL_RGBA, GL_FLOAT, dataU);
	//Check for GL errors
	if (glGetError() != GL_NO_ERROR)
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateKnotPointsTexture - U Texture Setup.");

	//Create temporary array for data
	GLfloat *dataV = new GLfloat[4 * this->_kpU];
	for (WPUInt i=0; i<this->_kpV; i++) dataV[i*4] = (GLfloat)this->_knotPointsV[i];
	//Set up some parameters
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//Set up texture
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_context->SurfaceKPVTex());
	glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, this->_kpV, 1, GL_RGBA, GL_FLOAT, dataV);
	//Delete arrays
	delete dataU;
	delete dataV;
	//Check for GL errors here
	if (glGetError() != GL_NO_ERROR) 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateKnotPointsTexture - V Texture Setup.");
}


void WCNurbsSurface::GenerateControlPointsVBO(void) {
	//Determine the needed size of the buffer
	GLuint size = this->_cpU * this->_cpV * 4 * sizeof(GLfloat);
	GLfloat *data = new GLfloat[size / sizeof(GLfloat)];
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
	//Delete array
	delete data;
/*** Debug ***
	std::cout << "Control Point VBO: " << this->_cpU * this->_cpV << std::endl;
	GLfloat *data2 = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	for (int i=0; i<(this->_cpU * this->_cpV); i++) printf("\t%d: %f %f %f %f\n", i, data2[i*4], data2[i*4+1], data2[i*4+2], data2[i*4+3]);
	glUnmapBuffer(GL_ARRAY_BUFFER);
/*** Debug ***/
	//Clean up and check for errors
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Check for GL errors here
	if (glGetError() != GL_NO_ERROR)
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateControlPointsVBO - Unspecified GL Error.");
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
	if (glGetError() != GL_NO_ERROR) 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateControlPointsTexture Error - Texture Setup.");
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


std::vector<GLfloat*>
WCNurbsSurface::GenerateSurfaceHigh(const WPFloat &uStart, const WPFloat &uStop, const WPUInt &lodU, 
	const WPFloat &vStart, const WPFloat &vStop, const WPUInt &lodV, const bool &server, std::vector<GLuint> &buffers) {
//Conditional compilation as long as transform feedback is present
#ifdef GL_EXT_transform_feedback
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

	WPUInt numVerts = lodU * lodV;
	//Setup vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[NURBSSURFACE_VERTEX_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, numVerts * NURBSSURFACE_FLOATS_PER_VERTEX * sizeof(GLfloat), 0, GL_STATIC_DRAW);
	glBindBufferBaseEXT(GL_TRANSFORM_FEEDBACK_BUFFER_EXT, 0, this->_buffers[NURBSSURFACE_VERTEX_BUFFER]);
	//Setup normal buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[NURBSSURFACE_NORMAL_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, numVerts * NURBSSURFACE_FLOATS_PER_NORMAL * sizeof(GLfloat), 0, GL_STATIC_DRAW);
	glBindBufferBaseEXT(GL_TRANSFORM_FEEDBACK_BUFFER_EXT, 1, this->_buffers[NURBSSURFACE_NORMAL_BUFFER]);
	//Setup texcoord buffer
	glBindBuffer(GL_ARRAY_BUFFER, buffers[NURBSSURFACE_TEXCOORD_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, numVerts * NURBSSURFACE_FLOATS_PER_TEXCOORD * sizeof(GLfloat), 0, GL_STATIC_DRAW);
	glBindBufferBaseEXT(GL_TRANSFORM_FEEDBACK_BUFFER_EXT, 2, buffers[NURBSSURFACE_TEXCOORD_BUFFER]);
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateSurfaceHigh - Output buffers setup.");

	/*** Setup Input Buffer ***/

	//Determine step size per direction
	WPFloat u = this->_knotPointsU[0];
	WPFloat range = this->_knotPointsU[this->_kpU-1] - this->_knotPointsU[0];
	WPFloat du = range / ((GLfloat)(lodU-1));
	WPFloat v = this->_knotPointsV[0];
	range = this->_knotPointsV[this->_kpV-1] - this->_knotPointsV[0];
	WPFloat dv = range / ((GLfloat)(lodV-1));
	//Create a temporary array (numVerts vertices)
	GLfloat *data = new GLfloat[numVerts * 4];
	int index = 0;
	//Loop through each line
	for (WPUInt r=0; r <lodV; r++) {
		//Reset u each loop
		u = this->_knotPointsU[0];
		//Loop through each batch on each line
		for (WPUInt i=0; i<lodU; i++) {
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
	glBufferData(GL_ARRAY_BUFFER, numVerts * 4 * sizeof(GLfloat), data, GL_STATIC_DRAW);
	//Delete array
	delete data;
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateSurfaceHigh - Input buffer setup.");

	/*** Time to Generate ***/

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnable(GL_RASTERIZER_DISCARD_EXT);
	glBeginTransformFeedbackEXT(GL_POINTS);
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateSurfaceHigh - Begin Transform Feedback.");

	glVertexPointer(4, GL_FLOAT, 4 * sizeof(GLfloat), 0);
	glDrawArrays(GL_POINTS, 0, numVerts);
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
#endif // GL_EXT_transform_feedback
	//Return client buffers
	return std::vector<GLfloat*>();
}


std::vector<GLfloat*>
WCNurbsSurface::GenerateSurfaceMedium(const WPFloat &uStart, const WPFloat &uStop, const WPUInt &lodU,
	const WPFloat &vStart, const WPFloat &vStop, const WPUInt &lodV, const bool &server, std::vector<GLuint> &buffers) {
	WPUInt numVerts = lodU * lodV;
	GLfloat uStep = (GLfloat)1.0 / (GLfloat)(lodU - 1);
	GLfloat vStep = (GLfloat)1.0 / (GLfloat)(lodV - 1);
	//Determine proper program and setup
	if ((this->_degreeU <= 3) && (this->_degreeV <= 3)) {
		//Degree 2 & 3 Bezier
		if ((this->_modeU == WCNurbsMode::Bezier()) && (this->_modeV == WCNurbsMode::Bezier())) {
			glUseProgram(this->_context->SurfaceBezier23Program());
			glUniform4i(this->_context->SurfaceLocations()[NURBSSURFACE_LOC_PARAMSU_BEZIER23], this->_degreeU, this->_cpU, this->_kpU, 0);
			glUniform4i(this->_context->SurfaceLocations()[NURBSSURFACE_LOC_PARAMSV_BEZIER23], this->_degreeV, this->_cpV, this->_kpV, 0);			
		}
		//Degree 2 & 3 Default and Custom
		else {
			glUseProgram(this->_context->SurfaceDefault23Program());
			glUniform4i(this->_context->SurfaceLocations()[NURBSSURFACE_LOC_PARAMSU_DEFAULT23], this->_degreeU, this->_cpU, this->_kpU, 0);
			glUniform4i(this->_context->SurfaceLocations()[NURBSSURFACE_LOC_PARAMSV_DEFAULT23], this->_degreeV, this->_cpV, this->_kpV, 0);
			glUniform4f(this->_context->SurfaceLocations()[NURBSSURFACE_LOC_PARAMS_DEFAULT23], 0.0, uStep, 0.0, vStep);
		}
	//Otherwise use the default case
	} else {
		glUseProgram(this->_context->SurfaceDefaultProgram());
		glUniform4i(this->_context->SurfaceLocations()[NURBSSURFACE_LOC_PARAMSU_DEFAULT], this->_degreeU, this->_cpU, this->_kpU, 0);		
		glUniform4i(this->_context->SurfaceLocations()[NURBSSURFACE_LOC_PARAMSV_DEFAULT], this->_degreeV, this->_cpV, this->_kpV, 0);								
	}

	//Save the viewport and polygon mode bits
	glPushAttrib(GL_VIEWPORT_BIT | GL_POLYGON_BIT | GL_ENABLE_BIT | GL_TEXTURE_BIT);
	glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
	//Set some GL state settings
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glEnableClientState(GL_VERTEX_ARRAY);
	//Now generate the control points texture
	this->GenerateControlPointsTexture();
	//Create a knotpoint texture
	this->GenerateKnotPointsTextures();
	//Bind to framebuffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->_context->SurfaceFramebuffer());
	//Setup draw buffers
	GLenum genBuffers[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_COLOR_ATTACHMENT2_EXT };
	glDrawBuffers(3, genBuffers);
	//Set up the viewport and polygon mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glViewport(0, 0, lodU, lodV);
	//Ready the input quad
	GLfloat quad[] = { -1.0, -1.0, -1.0, 1.0, 1.0, 1.0, 1.0, -1.0 };
	glVertexPointer(2, GL_FLOAT, 0, quad);
	//Render the quad
	glDrawArrays(GL_QUADS, 0, 4);

	GLfloat *vertData = NULL;
	GLfloat *normData = NULL;
	GLfloat *uvData = NULL;
	//See if server or client
	if (server) {
		GLuint tmpBuffer;
		//Setup all of the buffers if necessary
		if (buffers.at(NURBSSURFACE_VERTEX_BUFFER) == 0) { glGenBuffers(1, &tmpBuffer); buffers.at(NURBSSURFACE_VERTEX_BUFFER) = tmpBuffer; }
		if (buffers.at(NURBSSURFACE_NORMAL_BUFFER) == 0) { glGenBuffers(1, &tmpBuffer); buffers.at(NURBSSURFACE_NORMAL_BUFFER) = tmpBuffer; }
		if (buffers.at(NURBSSURFACE_TEXCOORD_BUFFER) == 0) { glGenBuffers(1, &tmpBuffer); buffers.at(NURBSSURFACE_TEXCOORD_BUFFER) = tmpBuffer; }
		//Read the vertex texture (using PBO)
		glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, buffers.at(NURBSSURFACE_VERTEX_BUFFER));
		glBufferData(GL_PIXEL_PACK_BUFFER, numVerts * 4 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
		glReadPixels(0, 0, lodU, lodV, GL_RGBA, GL_FLOAT, NULL);
		//Read the normal texture (using PBO)
		glReadBuffer(GL_COLOR_ATTACHMENT1_EXT);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, buffers.at(NURBSSURFACE_NORMAL_BUFFER));
		glBufferData(GL_PIXEL_PACK_BUFFER, numVerts * 4 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
		glReadPixels(0, 0, lodU, lodV, GL_RGBA, GL_FLOAT, NULL);
		//Clean up the PBO
		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

		//Read the tex coord data (using basic glReadPixels)
		GLfloat *tmpUV = new GLfloat[numVerts * 4];
		uvData = new GLfloat[numVerts * 2];
		glReadBuffer(GL_COLOR_ATTACHMENT2_EXT);
		glReadPixels(0, 0, lodU, lodV, GL_RGBA, GL_FLOAT, tmpUV);
		//Need to convert from 4-component to 2 component
		for (WPUInt i=0; i<numVerts; i++) memcpy(&uvData[i*2], &tmpUV[i*4], 2 * sizeof(GLfloat));
		//Upload texCoords into vbo
		glBindBuffer(GL_ARRAY_BUFFER, buffers.at(NURBSSURFACE_TEXCOORD_BUFFER));
		glBufferData(GL_ARRAY_BUFFER, numVerts * 2 * sizeof(GLfloat), uvData, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		//Delete temp array and set to NULL
		glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
		delete uvData;
		uvData = NULL;
	}
	//Must be client
	else {
		//Do we need to restore the default FBO state
		if (!buffers.empty()) {
			//Attach default texture to FBO
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, this->_context->SurfaceVerticesTex(), 0);
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_RECTANGLE_ARB, this->_context->SurfaceNormTex(), 0);
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_TEXTURE_RECTANGLE_ARB, this->_context->SurfaceTexCoorTex(), 0);
		}
		//Just copy data out to GLfloat arrays
		else {
			//Read the vertex data
			vertData = new GLfloat[numVerts * 4];
			glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
			glReadPixels(0, 0, lodU, lodV, GL_RGBA, GL_FLOAT, vertData);
			//Read the normal data
			normData = new GLfloat[numVerts * 4];
			glReadBuffer(GL_COLOR_ATTACHMENT1_EXT);
			glReadPixels(0, 0, lodU, lodV, GL_RGBA, GL_FLOAT, normData);
			//Read the tex coord data
			GLfloat *tmpUV = new GLfloat[numVerts * 4];
			uvData = new GLfloat[numVerts * 2];
			glReadBuffer(GL_COLOR_ATTACHMENT2_EXT);
			glReadPixels(0, 0, lodU, lodV, GL_RGBA, GL_FLOAT, tmpUV);
			//Need to convert from 4-component to 2 component
			for (WPUInt i=0; i<numVerts; i++) memcpy(&uvData[i*2], &tmpUV[i*4], 2 * sizeof(GLfloat));
			//Delete tmpUV
			glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
			delete tmpUV;
/*** Debug ***
			std::cout << "Medium Generation Verts (client): " << lodU << ", " << lodV << std::endl;
			for (int i=0; i<numVerts; i++) printf("\t%d: %f %f %f %f\n", i, vertData[i*4], vertData[i*4+1], vertData[i*4+2], vertData[i*4+3]);
			for (int i=0; i<numVerts; i++) printf("\t%d: %f %f %f %f\n", i, normData[i*4], normData[i*4+1], normData[i*4+2], normData[i*4+3]);
			for (int i=0; i<numVerts; i++) printf("\t%d: %f %f\n", i, uvData[i*2], uvData[i*2+1]);
/*** Debug ***/
		}
	}
	//Clean up the GL state
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glPopAttrib();
	glPopClientAttrib();
	//Should check for errors here
	GLenum err = glGetError();
	if (err != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateSurfaceMedium - At Cleanup:" << err);
	//Return client buffers
	std::vector<GLfloat*> retVals;
	retVals.push_back(vertData);			// Must be first
	retVals.push_back(normData);			// Must be second
	retVals.push_back(uvData);				// Must be third
	return retVals;
}


std::vector<GLfloat*>
WCNurbsSurface::GenerateSurfaceLow(const WPFloat &uStart, const WPFloat &uStop, const WPUInt &lodU,
	const WPFloat &vStart, const WPFloat &vStop, const WPUInt &lodV, const bool &server, std::vector<GLuint> &buffers) {
	int vIndex, nIndex, tIndex, index;
	WPFloat u = this->_knotPointsU[0];
	WPFloat v = this->_knotPointsV[0];
	WPFloat range = this->_knotPointsU[this->_kpU-1] - this->_knotPointsU[0];
	WPFloat du = range / ((GLfloat)(lodU-1));
	range = this->_knotPointsV[this->_kpV-1] - this->_knotPointsV[0];
	WPFloat dv = range / ((GLfloat)(lodV-1));
	WPUInt numVerts = lodU * lodV;

	//Create a temporary array (NumVert vertices)
	GLfloat *vData = new GLfloat[numVerts * NURBSSURFACE_FLOATS_PER_VERTEX];
	GLfloat *nData = new GLfloat[numVerts * NURBSSURFACE_FLOATS_PER_NORMAL];
	GLfloat *tData = new GLfloat[numVerts * NURBSSURFACE_FLOATS_PER_TEXCOORD];
	WPFloat *bvU, *bvV;
	WCVector4 pt;
	int spanU, spanV;
	WCVector4 S, Su, Sv, norm;
	WPFloat D, Du, Dv;
	
	//Zero all of the indices
	vIndex = nIndex = tIndex = 0;
	//Loop through v
	for (WPUInt i=0; i<lodV; i++) {
		//Reset u each loop
		u = this->_knotPointsU[0];
		//Loop through u
		for (WPUInt j=0; j<lodU; j++) {
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

	//Check to see if server side buffers
	if (server) {
		GLuint tmpBuffer;
		//Allocate space for the output vertex data
		if (buffers.at(NURBSSURFACE_VERTEX_BUFFER) == 0) { glGenBuffers(1, &tmpBuffer); buffers.at(NURBSSURFACE_VERTEX_BUFFER) = tmpBuffer; }
		glBindBuffer(GL_ARRAY_BUFFER, buffers.at(NURBSSURFACE_VERTEX_BUFFER));	
		glBufferData(GL_ARRAY_BUFFER, numVerts * NURBSSURFACE_FLOATS_PER_VERTEX * sizeof(GLfloat), vData, GL_STATIC_DRAW);
		//Setup normal buffer	
		if (buffers.at(NURBSSURFACE_NORMAL_BUFFER) == 0) { glGenBuffers(1, &tmpBuffer); buffers.at(NURBSSURFACE_NORMAL_BUFFER) = tmpBuffer; }
		glBindBuffer(GL_ARRAY_BUFFER, buffers.at(NURBSSURFACE_NORMAL_BUFFER));	
		glBufferData(GL_ARRAY_BUFFER, numVerts * NURBSSURFACE_FLOATS_PER_NORMAL * sizeof(GLfloat), nData, GL_STATIC_DRAW);
		//Setup texcoord buffer
		if (buffers.at(NURBSSURFACE_TEXCOORD_BUFFER) == 0) { glGenBuffers(1, &tmpBuffer); buffers.at(NURBSSURFACE_TEXCOORD_BUFFER) = tmpBuffer; }
		glBindBuffer(GL_ARRAY_BUFFER, buffers.at(NURBSSURFACE_TEXCOORD_BUFFER));
		glBufferData(GL_ARRAY_BUFFER, numVerts * NURBSSURFACE_FLOATS_PER_TEXCOORD * sizeof(GLfloat), tData, GL_STATIC_DRAW);
/*** Debug ***
		 std::cout << "Generate Low Verts: " << numVerts << std::endl;
		 glBindBuffer(GL_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_VERTEX_BUFFER]);	
		 GLfloat *data2= (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
		 for (int i=0; i<numVerts; i++) printf("\t%d V: %f %f %f %f\n", i, data2[i*4], data2[i*4+1], data2[i*4+2], data2[i*4+3]);
		 glUnmapBuffer(GL_ARRAY_BUFFER);
		 //Show Normal Data
		 glBindBuffer(GL_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_NORMAL_BUFFER]);
		 data2 = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
		 for (int i=0; i<numVerts; i++) printf("\t%d: %f %f %f\n", i, data2[i*4], data2[i*4+1], data2[i*4+2]);
		 glUnmapBuffer(GL_ARRAY_BUFFER);
		 //Show TexCoord Data
		 glBindBuffer(GL_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_TEXCOORD_BUFFER]);
		 data2 = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
		 for (int i=0; i<numVerts; i++) printf("\t%d: %f %f\n", i, data2[i*2], data2[i*2+1]);
		 glUnmapBuffer(GL_ARRAY_BUFFER);
 /*** Debug ***/
		//Clean up and report errors
		glBindBuffer(GL_ARRAY_BUFFER, 0);	
		if (glGetError() != GL_NO_ERROR) 
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateSurfaceLow - Error at Loading Buffers.");
		//Delete arrays
		delete vData;
		delete nData;
		delete tData;
		//Return nothing
		return std::vector<GLfloat*>();
	}
	//Return client buffers
	std::vector<GLfloat*> retVals;
	retVals.push_back(vData);			// Must be first
	retVals.push_back(nData);			// Must be second
	retVals.push_back(tData);			// Must be third
	return retVals;
}


std::vector<GLfloat*>
WCNurbsSurface::GenerateSurfaceSize4(const WPFloat &uStart, const WPFloat &uStop, const WPUInt &lodU,
	const WPFloat &vStart, const WPFloat &vStop, const WPUInt &lodV, const bool &server, std::vector<GLuint> &buffers) {
	//Get the four corners
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
	WPUInt numVerts = lodU * lodV;
	WPFloat u = this->_knotPointsU[0];
	WPFloat v = this->_knotPointsV[0];
	WPFloat range = this->_knotPointsU[this->_kpU-1] - this->_knotPointsU[0];
	WPFloat du = range / ((GLfloat)(lodU-1));
	range = this->_knotPointsV[this->_kpV-1] - this->_knotPointsV[0];
	WPFloat dv = range / ((GLfloat)(lodV-1));
	//Set aside data for the vertices, normals, and texcoords
	GLfloat *vData = new GLfloat[numVerts*4];
	GLfloat *nData = new GLfloat[numVerts*4];
	GLfloat *tData = new GLfloat[numVerts*2];
	//Loop on V
	for(WPUInt i=0; i<lodV; i++) {
		//Reset u each loop
		u = this->_knotPointsU[0];
		for (WPUInt j=0; j<lodU; j++) {
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

	//See if server side
	if (server) {
		GLuint tmpBuffer;
		//Allocate space for the output vertex data
		if (buffers.at(NURBSSURFACE_VERTEX_BUFFER) == 0) { glGenBuffers(1, &tmpBuffer); buffers.at(NURBSSURFACE_VERTEX_BUFFER) = tmpBuffer; }
		glBindBuffer(GL_ARRAY_BUFFER, buffers.at(NURBSSURFACE_VERTEX_BUFFER));	
		glBufferData(GL_ARRAY_BUFFER, numVerts * NURBSSURFACE_FLOATS_PER_VERTEX * sizeof(GLfloat), vData, GL_STATIC_DRAW);
		//Setup normal buffer	
		if (buffers.at(NURBSSURFACE_NORMAL_BUFFER) == 0) { glGenBuffers(1, &tmpBuffer); buffers.at(NURBSSURFACE_NORMAL_BUFFER) = tmpBuffer; }
		glBindBuffer(GL_ARRAY_BUFFER, buffers.at(NURBSSURFACE_NORMAL_BUFFER));	
		glBufferData(GL_ARRAY_BUFFER, numVerts * NURBSSURFACE_FLOATS_PER_NORMAL * sizeof(GLfloat), nData, GL_STATIC_DRAW);
		//Setup texcoord buffer
		if (buffers.at(NURBSSURFACE_TEXCOORD_BUFFER) == 0) { glGenBuffers(1, &tmpBuffer); buffers.at(NURBSSURFACE_TEXCOORD_BUFFER) = tmpBuffer; }
		glBindBuffer(GL_ARRAY_BUFFER, buffers.at(NURBSSURFACE_TEXCOORD_BUFFER));
		glBufferData(GL_ARRAY_BUFFER, numVerts * NURBSSURFACE_FLOATS_PER_TEXCOORD * sizeof(GLfloat), tData, GL_STATIC_DRAW);
		//Clean up and report errors
		glBindBuffer(GL_ARRAY_BUFFER, 0);	
		if (glGetError() != GL_NO_ERROR)
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateSurfaceSize4 - Error buffering server data.");
		//Delete arrays
		delete vData;
		delete nData;
		delete tData;
		//Return nothing
		return std::vector<GLfloat*>();
	}
	//See if texture buffers
	else if (!buffers.empty()) {
/*** DEBUG ***
		std::cout << "Debug for Surface Texture Buffers: " << lodU * lodV << std::endl;
		for (int i=0; i<lodV*lodU; i++)
			printf("%d: %f %f %f %f\n", i, vData[i*4], vData[i*4+1], vData[i*4+2], vData[i*4+3]);
/*** DEBUG ***/
		//Bind to the vertices texture
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, buffers.at(NURBSSURFACE_VERTEX_BUFFER));
		glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, lodU, lodV, GL_RGBA, GL_FLOAT, vData);
		//Bind to the normals texture
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, buffers.at(NURBSSURFACE_NORMAL_BUFFER));
		glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, lodU, lodV, GL_RGBA, GL_FLOAT, nData);
		//Bind to the texCoords texture
//		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, buffers.at(NURBSSURFACE_TEXCOORD_BUFFER));
//		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA32F_ARB, lodU, lodV, 0, GL_RGBA, GL_FLOAT, tData);
		//Clean up and report errors
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);	
		if (glGetError() != GL_NO_ERROR)
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateSurfaceSize4 - Error buffering texture data.");
		//Delete arrays
		delete vData;
		delete nData;
		delete tData;
		//Return nothing
		return std::vector<GLfloat*>();
	}
	//Return client buffers
	std::vector<GLfloat*> retVals;
	retVals.push_back(vData);			// Must be first
	retVals.push_back(nData);			// Must be second
	retVals.push_back(tData);			// Must be third
	return retVals;
}


GLuint* WCNurbsSurface::GenerateIndex(const WPFloat &uStart, const WPFloat &uStop, const WPUInt &lodU,
	const WPFloat &vStart, const WPFloat &vStop, const WPUInt &lodV, const bool &server, GLuint &buffer) {
	//Allocate space for local array of data
	GLuint size = (lodU - 1) * (lodV - 1) * 2 * 3;  //Two triangles per lod, 3 verts per triangle
	GLuint *data = new GLuint[size];			
	//Check to make sure data was allocated
	if (data == NULL) {	
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateIndex - Unable to allocate space for index data.");
		return NULL;
	}

	int ll, index = 0;
	//Loop through each line
	for(WPUInt v=0; v<lodV-1; v++) {
		//Loop through each element in the line
		for (WPUInt u=0; u<lodU-1; u++) {
			//Calculate vertex index of lower left vertex
			ll = lodU * v + u;
			//Upper triangle
			data[index++] = ll;							//	1--2
			data[index++] = ll + lodU;					//	|/
			data[index++] = data[index-1] + 1;			//	0
			//Lower triangle
			data[index++] = ll;							//	   2
			data[index++] = data[index-2];				//	 / |
			data[index++] = ll + 1;						//	0--3
		}
	}

	//See if server or client
	if (server) {
		//Make sure buffer exists
		if (buffer == 0) glGenBuffers(1, &buffer);
		//Allocate space for index array
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
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
		if (glGetError() != GL_NO_ERROR) 
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateIndex - Unspecified error.");
		//Delete array
		delete data;
		data = NULL;
	}
	//Return client index buffer
	return data;
}


/***********************************************~***************************************************/


WCNurbsSurface::WCNurbsSurface(WCGeometryContext *context, const WPUInt &degreeU, const WPUInt &degreeV, const WPUInt &cpU, const WPUInt &cpV, 
	const std::vector<WCVector4> &controlPoints, const WCNurbsMode &modeU, const WCNurbsMode &modeV, const std::vector<WPFloat> &kpU, const std::vector<WPFloat> &kpV) : 
	::WCGeometricSurface(), _context(context), _degreeU(degreeU), _degreeV(degreeV), _modeU(modeU), _modeV(modeV), 
	_cpU(cpU), _cpV(cpV), _controlPoints(controlPoints), _kpU(0), _kpV(0), _knotPointsU(NULL), _knotPointsV(NULL),
	_lengthU(0.0), _lengthV(0.0), _lodU(0), _lodV(0), _buffers(), _altBuffers() {
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

	//Use knot mode to setup knot array
	this->LoadKnotPoints(kpU, kpV);
	//Estimate length values
	this->_lengthU = WCNurbs::EstimateLengthU(this->_controlPoints, this->_cpU);
	this->_lengthV = WCNurbs::EstimateLengthV(this->_controlPoints, this->_cpV);	
	//Validate surface flags
	this->ValidateClosure();
	this->ValidateSelfIntersection();
	//Establish aligned bounding box
	this->_bounds = new WCAlignedBoundingBox(this->_controlPoints);
}


WCNurbsSurface::WCNurbsSurface(const WCNurbsSurface &surf) : ::WCGeometricSurface(surf),
	_context(surf._context), _degreeU(surf._degreeU), _degreeV(surf._degreeV), _modeU(surf._modeU), _modeV(surf._modeV), 
	_cpU(surf._cpU), _cpV(surf._cpV), _controlPoints(surf._controlPoints), _kpU(surf._kpU), _kpV(surf._kpV), _knotPointsU(NULL), _knotPointsV(NULL),
	_lengthU(surf._lengthU), _lengthV(surf._lengthV), _lodU(0), _lodV(0), _buffers(), _altBuffers() {
	//Need to load knot points
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface:: Copy Constructor - Not yet implemented.");
	//Establish aligned bounding box
	this->_bounds = new WCAlignedBoundingBox(this->_controlPoints);	
}


WCNurbsSurface::WCNurbsSurface(xercesc::DOMElement *element, WCSerialDictionary *dictionary) :
	::WCGeometricSurface( WCSerializeableObject::ElementFromName(element,"GeometricSurface"), dictionary ),
	_context(NULL), _degreeU(0), _degreeV(0), _modeU(WCNurbsMode::Default()), _modeV(WCNurbsMode::Default()), _cpU(0), _cpV(0),
	_controlPoints(), _kpU(0), _kpV(0), _knotPointsU(NULL), _knotPointsV(NULL), _lengthU(0.0), _lengthV(0.0), _buffers(), _altBuffers() {
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::WCNurbsSurface - Restore from XML is not implemented.");
}


WCNurbsSurface::~WCNurbsSurface() {
	//See if need to Delete the buffers
	if (!this->_buffers.empty()) this->ReleaseBuffers(this->_buffers);
	if (!this->_altBuffers.empty()) this->ReleaseBuffers(this->_altBuffers);
	//Clear the collection of control points
	this->_controlPoints.clear();
	//Delete the knot point array
	if (this->_knotPointsU != NULL) delete this->_knotPointsU;
	if (this->_knotPointsV != NULL) delete this->_knotPointsV;
	//Check for errors
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::~WCNurbsSurface - Unspecified error.");
}


void WCNurbsSurface::Degree(const WPUInt &degreeU, const WPUInt &degreeV) {
	WPUInt degU = degreeU;
	WPUInt degV = degreeV;
	if ((degU > NURBSSURFACE_MAX_DEGREE) || (degV > NURBSSURFACE_MAX_DEGREE)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::Degree - Degree must be less than NURBSSURFACE_MAX_DEGREE.");
		degU = STDMIN((WPUInt)NURBSSURFACE_MAX_DEGREE, degU);
		degV = STDMIN((WPUInt)NURBSSURFACE_MAX_DEGREE, degV);
	}
	//Check to make sure degree is less than the number of control points
	if ((degU >= this->_cpU) || (degV >= this->_cpV)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::Degree - Degree must be less than the number of control points.");
		degU = STDMIN(this->_cpU - 1, degU);
		degV = STDMIN(this->_cpV - 1, degV);
	}
	if ((degU < 1) || (degV < 1)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::Degree - Degree must be greater than 1.");
		degU = STDMAX(degreeU, (WPUInt)1);
		degV = STDMAX(degV, (WPUInt)1);
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
	this->IsVisualDirty(true);
	this->IsSerialDirty(true);
}


WPFloat WCNurbsSurface::Area(const WPFloat &tolerance) {
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::Area Error - Not yet implemented.");
	return 0.0;
}


WCVisualObject* WCNurbsSurface::HitTest(const WCRay &ray, const WPFloat &tolerance) {
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::HitTest Error - Not yet implemented.");
	return NULL;
}


void WCNurbsSurface::ApplyTransform(const WCMatrix4 &transform) {
	//Loop through all control points and apply transform
	for (WPUInt i=0; i<this->_controlPoints.size(); i++)
		this->_controlPoints.at(i) = transform * this->_controlPoints.at(i);
	//Make sure curve is regenerated
	this->IsVisualDirty(true);
	this->IsSerialDirty(true);
}


void WCNurbsSurface::ApplyTranslation(const WCVector4 &translation) {
	//Loop through all control points and apply translation
	for (WPUInt i=0; i<this->_controlPoints.size(); i++)
		this->_controlPoints.at(i) = this->_controlPoints.at(i) + translation;
	//Make sure curve is regenerated
	this->IsVisualDirty(true);
	this->IsSerialDirty(true);
}


void WCNurbsSurface::Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom) {
	//Make sure surface is visible
	if (!this->_isVisible) return;

	//Determine best LOD values
	WPFloat lengthU = WCNurbs::EstimateLengthU(this->_controlPoints, this->_cpU);
	WPUInt lodU = STDMAX((WPUInt)(this->_cpU * 5), (WPUInt)(lengthU * sqrt(zoom) / NURBSSURFACE_RENDER_ACCURACY));
	WPFloat lengthV = WCNurbs::EstimateLengthV(this->_controlPoints, this->_cpV);
	WPUInt lodV = STDMAX((WPUInt)(this->_cpV * 5), (WPUInt)(lengthV * sqrt(zoom) / NURBSSURFACE_RENDER_ACCURACY));
	WPFloat factorU = (WPFloat)this->_lodU / (WPFloat)lodU;
	WPFloat factorV = (WPFloat)this->_lodV / (WPFloat)lodV;
	//See if need to regenerate
	if ((factorU < NURBSSURFACE_RENDER_LOWER) || (factorU > NURBSSURFACE_RENDER_UPPER) ||
		(factorV < NURBSSURFACE_RENDER_LOWER) || (factorV > NURBSSURFACE_RENDER_UPPER))  {
		//Set the new lod
		this->_lodU = lodU;
		this->_lodV = lodV;
		//Mark as dirty
		this->IsVisualDirty(true);
	}

	//Check to see if surface needs to be generated
	if (this->IsVisualDirty()) {
		//Generate the server buffer of data
		this->GenerateServerBuffers(0.0, 1.0, this->_lodU, 0.0, 1.0, this->_lodV, this->_buffers, true);
		//Mark as clean
		this->IsVisualDirty(false);
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
	//Report them errors
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::Render - Unspecified error.");
}


void WCNurbsSurface::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
	//Mark the surface as dirty
	this->IsVisualDirty(true);
	this->IsSerialDirty(true);
	//Estimate length values
	this->_lengthU = WCNurbs::EstimateLengthU(this->_controlPoints, this->_cpU);
	this->_lengthV = WCNurbs::EstimateLengthV(this->_controlPoints, this->_cpV);	
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
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::Derivative - Not yet implemented.");
	return WCVector4();
}


WCRay WCNurbsSurface::Tangent(const WPFloat &u, const WPFloat &v) {
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::Tangent - Not yet implemented.");	
	return WCRay(WCVector4(), WCVector4());
}


std::pair<WCVector4,WCVector4> WCNurbsSurface::PointInversion(const WCVector4 &point) {
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::PointInversion - Not yet implemented.");
	//Special case for degree=1 curves
	if ((this->_degreeU == 1) && (this->_degreeV == 1)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::PointInversion - Not yet implemented for degree=1 surfaces.");
		return std::make_pair(WCVector4(), WCVector4());
	}
	//Check for special generation cases, otherwise set the program
//	if ((this->_controlPoints.size() == 4) && (this->_numVerts <= NURBSSURFACE_SIZE4_CUTOFF)) return this->GenerateSurfaceSize4();
	//	if ((this->_degreeU == 1) && (this->_degreeV = 1)) return this->GenerateSurfaceOne();
//	if ((this->_lodU > (WPUInt)this->_context->SurfaceMaxTextureSize()) || (this->_lodV > (WPUInt)this->_context->SurfaceMaxTextureSize())) {
//		CLOGGER_WARN(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateSurfaceMedium - LOD exceeds hardware Maximum Texture Size.");
//		return this->GenerateSurfaceLow();
//	}

	//Check to see if surface needs to be generated
	if (this->IsVisualDirty()) {
		//Generate the curve - switch on performance level
//		switch(this->_context->SurfacePerformanceLevel()) {
//			case NURBSSURFACE_PERFLEVEL_HIGH: this->GenerateSurfaceHigh(); break;
//			case NURBSSURFACE_PERFLEVEL_MEDIUM: this->GenerateSurfaceMedium(); break;
//			case NURBSSURFACE_PERFLEVEL_LOW: this->GenerateSurfaceLow(); break;
//		}
		//Generate the index for the curve
//		this->GenerateIndex();		
		//Mark as clean
		this->IsVisualDirty(true);
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
	for (i=0; i<(this->_lodU+1)*(this->_lodV+1); i++) {
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
	u = (index % (this->_lodU+1)) * this->_knotPointsU[this->_kpU-1] / (this->_lodU+1);
	v = (index / (this->_lodV+1)) * this->_knotPointsV[this->_kpV-1] / (this->_lodV+1);

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
		if (dist < NURBSSURFACE_EPSILON_ONE) return std::make_pair(s, WCVector4(u,v,0.0));
		
		//Do some interim calculations
		f = sU.DotProduct(r);
		suMag = sU.Magnitude();
		//Check condition 2 - part 1
		if (( STDFABS(f) / (suMag * dist) ) <= NURBSSURFACE_EPSILON_TWO) return std::make_pair(s, WCVector4(u,v,0.0));					
		//Now the other part
		g = sV.DotProduct(r);
		svMag = sV.Magnitude();
		//Check condition 2 - part 2
		if (( STDFABS(g) / (svMag * dist) ) <= NURBSSURFACE_EPSILON_TWO) return std::make_pair(s, WCVector4(u,v,0.0));		
		
		//Create 
		fu = (suMag * suMag) + r.DotProduct(sUU);
		fv = sU.DotProduct(sV) + r.DotProduct(sUV);
		gu = fv;
		gv = (svMag * svMag) + r.DotProduct(sVV);
		bottom = (fu * gv) - (gu * fv);
		//Check to make sure no div by zero
		if (bottom == 0.0) {
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::PointInverse - |A| is zero.");
			return std::make_pair(s, WCVector4(u,v,0.0));
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
		if ( (sU*uNx + sV*vNx).Magnitude() <= NURBSSURFACE_EPSILON_ONE) return std::make_pair(s, WCVector4(u,v,0.0));
		
		//Otherwise, increment u and v
		u = u + uNx;
		v = v + vNx;
		//Increment loop interation counter
		iterCount++;
	}
	return std::make_pair(s, WCVector4(u,v,0.0));
}


std::vector<GLfloat*>
WCNurbsSurface::GenerateClientBuffers(const WPFloat &uStart, const WPFloat &uStop, WPUInt &lodU,
	const WPFloat &vStart, const WPFloat &vStop, WPUInt &lodV, const bool &managed) {
	//Make sure LOD >= 2
	lodU = STDMAX(lodU, (WPUInt)2);
	lodV = STDMAX(lodV, (WPUInt)2);

	std::vector<GLuint> dummy;
	std::vector<GLfloat*> buffers;
	//See if numVerts < NURBSSURFACE_SIZE4_CUTOFF
	if (lodU * lodV < NURBSSURFACE_SIZE4_CUTOFF) {
		//Generate the buffers of data using size4 method
		buffers = this->GenerateSurfaceSize4(0.0, 1.0, lodU, 0.0, 1.0, lodV, false, dummy);
	}
	//Low generation only on LOD > MaxTextureSize || PerfLevel == Low
	else if ( (lodU > (WPUInt)this->_context->CurveMaxTextureSize()) ||
			 (lodV > (WPUInt)this->_context->CurveMaxTextureSize()) ||
			 (this->_context->SurfacePerformanceLevel() == NURBSSURFACE_PERFLEVEL_LOW) ) {
		//Generate the buffers of data using low method
		buffers = this->GenerateSurfaceLow(0.0, 1.0, lodU, 0.0, 1.0, lodV, false, dummy);
	}
	//Medium generation only if PerfLevel == Med
	else if (this->_context->SurfacePerformanceLevel() == NURBSSURFACE_PERFLEVEL_MEDIUM) {
		//Generate the buffer of data using medium method
		buffers = this->GenerateSurfaceMedium(0.0, 1.0, lodU, 0.0, 1.0, lodV, false, dummy);
	}
	//High generation only if perf level == High
	else if (this->_context->SurfacePerformanceLevel() == NURBSSURFACE_PERFLEVEL_HIGH) {
		//Generate the buffer of data using high method
		buffers = this->GenerateSurfaceHigh(0.0, 1.0, lodU, 0.0, 1.0, lodV, false, dummy);
	}
	//Error path
	else {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateClientBuffer - Unknown generation path.");
		//throw error
	}
	//Generate index buffer
	GLuint buffer = 0;
	GLuint *indexBuffer = GenerateIndex(0.0, 1.0, lodU, 0.0, 1.0, lodV, false, buffer);
	buffers.push_back((GLfloat*)indexBuffer);
/*** Debug ***
	 std::cout << "Generate Client Buffers (" << lodU << " x " << lodV << ")\n";
	 GLfloat *data = buffers.at(0);
	 for (int i=0; i<lodU*lodV; i++) 
		printf("\t%d: %f %f %f %f\n", i, data[i*4], data[i*4+1], data[i*4+2], data[i*4+3]);
 /*** Debug ***/
	
	//Return the buffers
	return buffers;
}


void WCNurbsSurface::ReleaseBuffers(std::vector<GLfloat*> &buffers) {
	//Make sure there are atleast 4 to delete
	if (buffers.size() >= 4) {
		delete buffers.at(NURBSSURFACE_VERTEX_BUFFER);
		delete buffers.at(NURBSSURFACE_INDEX_BUFFER);
		delete buffers.at(NURBSSURFACE_NORMAL_BUFFER);
		delete buffers.at(NURBSSURFACE_TEXCOORD_BUFFER);
	}
	//Clear the list
	buffers.clear();
}


void WCNurbsSurface::GenerateServerBuffers(const WPFloat &uStart, const WPFloat &uStop, WPUInt &lodU,
	const WPFloat &vStart, const WPFloat &vStop, WPUInt &lodV, std::vector<GLuint> &buffers, const bool &managed) {
	//Make sure LOD >= 2
	lodU = STDMAX(lodU, (WPUInt)2);
	lodV = STDMAX(lodV, (WPUInt)2);
	//Make sure buffers has 4 elements
	if (buffers.size() < 4) {
		//Clear the buffer
		buffers.clear();
		//Put in four 0 elements
		buffers = std::vector<GLuint>(4, 0);
	}

	//See if numVerts < NURBSSURFACE_SIZE4_CUTOFF or degree 1
	if ((lodU * lodV < NURBSSURFACE_SIZE4_CUTOFF) || (this->_degreeU == 1) || (this->_degreeV == 1)) {
		//Generate the buffers of data using size4 method
		this->GenerateSurfaceSize4(0.0, 1.0, lodU, 0.0, 1.0, lodV, true, buffers);
	}
	//Low generation only on LOD > MaxTextureSize || PerfLevel == Low
	else if ( (lodU > (WPUInt)this->_context->CurveMaxTextureSize()) ||
			  (lodV > (WPUInt)this->_context->CurveMaxTextureSize()) ||
			  (this->_context->SurfacePerformanceLevel() == NURBSSURFACE_PERFLEVEL_LOW) ) {
		//Generate the buffers of data using low method
		this->GenerateSurfaceLow(0.0, 1.0, lodU, 0.0, 1.0, lodV, true, buffers);
	}
	//Medium generation only if PerfLevel == Med
	else if (this->_context->SurfacePerformanceLevel() == NURBSSURFACE_PERFLEVEL_MEDIUM) {
		//Generate the buffer of data using medium method
		this->GenerateSurfaceMedium(0.0, 1.0, lodU, 0.0, 1.0, lodV, true, buffers);
	}
	//High generation only if perf level == High
	else if (this->_context->SurfacePerformanceLevel() == NURBSSURFACE_PERFLEVEL_HIGH) {
		//Generate the buffer of data using high method
		this->GenerateSurfaceHigh(0.0, 1.0, lodU, 0.0, 1.0, lodV, true, buffers);
	}
	//Error path
	else {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateServerBuffer - Unknown generation path.");
		//throw error
	}
	//Generate index buffer
	GLuint buffer = 0;
	GenerateIndex(0.0, 1.0, lodU, 0.0, 1.0, lodV, true, buffer);
	buffers.at(NURBSSURFACE_INDEX_BUFFER) = buffer;
}


void WCNurbsSurface::ReleaseBuffers(std::vector<GLuint> &buffers) {
	//Make sure there are some values
	if (buffers.size() >= 4) {
		glDeleteBuffers(1, &(buffers.at(NURBSSURFACE_VERTEX_BUFFER)));
		glDeleteBuffers(1, &(buffers.at(NURBSSURFACE_INDEX_BUFFER)));
		glDeleteBuffers(1, &(buffers.at(NURBSSURFACE_NORMAL_BUFFER)));
		glDeleteBuffers(1, &(buffers.at(NURBSSURFACE_TEXCOORD_BUFFER)));
	}
	//Clear the buffers
	buffers.clear();
}


void WCNurbsSurface::GenerateTextureBuffers(const WPFloat &uStart, const WPFloat &uStop, WPUInt &lodU,
	const WPFloat &vStart, const WPFloat &vStop, WPUInt &lodV, std::vector<GLuint> &textures, const bool &managed) {
	//Make sure LOD >= 2
	lodU = STDMAX(lodU, (WPUInt)2);
	lodV = STDMAX(lodV, (WPUInt)2);
	//Make sure textures has 4 elements
	if (textures.size() < 4) {
		//Clear the texture
		textures.clear();
		//Put in four 0 elements
		textures = std::vector<GLuint>(4, 0);
	}

	//Bind to the texture for vertices
	GLuint tmpBuffer;
	glActiveTexture(GL_TEXTURE0);
	if (textures.at(NURBSSURFACE_VERTEX_BUFFER) == 0) glGenTextures(1, &tmpBuffer); textures.at(NURBSSURFACE_VERTEX_BUFFER) = tmpBuffer;
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, textures.at(NURBSSURFACE_VERTEX_BUFFER));
	//Set its parameters
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA32F_ARB, lodU, lodV, 0, GL_RGBA, GL_FLOAT, NULL);
	//Bind to the texture for normals
	if (textures.at(NURBSSURFACE_NORMAL_BUFFER) == 0) glGenTextures(1, &tmpBuffer); textures.at(NURBSSURFACE_NORMAL_BUFFER) = tmpBuffer;
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, textures.at(NURBSSURFACE_NORMAL_BUFFER));
	//Set its parameters
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA32F_ARB, lodU, lodV, 0, GL_RGBA, GL_FLOAT, NULL);
	//Bind to the texture for texcoords
	if (textures.at(NURBSSURFACE_TEXCOORD_BUFFER) == 0) glGenTextures(1, &tmpBuffer); textures.at(NURBSSURFACE_TEXCOORD_BUFFER) = tmpBuffer;
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, textures.at(NURBSSURFACE_TEXCOORD_BUFFER));
	//Set its parameters
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA32F_ARB, lodU, lodV, 0, GL_RGBA, GL_FLOAT, NULL);
	//Bind back to no texture
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
	
	//See if numVerts < NURBSSURFACE_SIZE4_CUTOFF or degree 1
	if ((lodU * lodV < NURBSSURFACE_SIZE4_CUTOFF) || (this->_degreeU == 1) || (this->_degreeV == 1)) {
		//Generate the textures of data using size4 method
		this->GenerateSurfaceSize4(0.0, 1.0, lodU, 0.0, 1.0, lodV, false, textures);
	}
	//Low generation only on LOD > MaxTextureSize || PerfLevel == Low
	else if ( (lodU > (WPUInt)this->_context->CurveMaxTextureSize()) ||
			 (lodV > (WPUInt)this->_context->CurveMaxTextureSize()) ||
			 (this->_context->SurfacePerformanceLevel() == NURBSSURFACE_PERFLEVEL_LOW) ) {
		//Generate the textures of data using low method
		this->GenerateSurfaceLow(0.0, 1.0, lodU, 0.0, 1.0, lodV, false, textures);
	}
	//Medium generation only if PerfLevel == Med
	else if (this->_context->SurfacePerformanceLevel() == NURBSSURFACE_PERFLEVEL_MEDIUM) {	
		//Attach texture to framebuffer
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, textures.at(NURBSSURFACE_VERTEX_BUFFER), 0);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_RECTANGLE_ARB, textures.at(NURBSSURFACE_NORMAL_BUFFER), 0);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT2_EXT, GL_TEXTURE_RECTANGLE_ARB, textures.at(NURBSSURFACE_TEXCOORD_BUFFER), 0);
		//Bind back to no texture
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
		GLenum retVal = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		//Check for errors
		if (retVal != GL_FRAMEBUFFER_COMPLETE_EXT) 
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateTextureBuffers - Framebuffer is not complete.");
		//Generate the textures of data using medium method
		this->GenerateSurfaceMedium(0.0, 1.0, lodU, 0.0, 1.0, lodV, false, textures);
	}
	//High generation only if perf level == High
	else if (this->_context->SurfacePerformanceLevel() == NURBSSURFACE_PERFLEVEL_HIGH) {
		std::cout << "SurfaceGenerateTexture-High no workie!\n";
		exit(0);
		//Generate the textures of data using high method
		this->GenerateSurfaceHigh(0.0, 1.0, lodU, 0.0, 1.0, lodV, false, textures);
	}
	//Error path
	else {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCNurbsSurface::GenerateServerBuffer - Unknown generation path.");
		//throw error
	}
	//Generate index buffer
	GLuint tex = 0;
	GenerateIndex(0.0, 1.0, lodU, 0.0, 1.0, lodV, false, tex);
	textures.at(NURBSSURFACE_INDEX_BUFFER) = tex;
}


void WCNurbsSurface::ReleaseTextures(std::vector<GLuint> &textures) {
	//Make sure there are some values
	if (textures.size() >= 4) {
		glDeleteTextures(1, &(textures.at(NURBSSURFACE_VERTEX_BUFFER)));
		glDeleteTextures(1, &(textures.at(NURBSSURFACE_INDEX_BUFFER)));
		glDeleteTextures(1, &(textures.at(NURBSSURFACE_NORMAL_BUFFER)));
		glDeleteTextures(1, &(textures.at(NURBSSURFACE_TEXCOORD_BUFFER)));
	}
	//Clear the textures
	textures.clear();

}


void WCNurbsSurface::InsertKnotU(const WPFloat &u, const WPUInt &multiplicity) {
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::InsertKnotU - Not yet implemented.");
}


void WCNurbsSurface::InsertKnotV(const WPFloat &v, const WPUInt &multiplicity) {
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::InsertKnotV - Not yet implemented.");
}


void WCNurbsSurface::RefineKnot(void) {
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::RefineKnot - Not yet implemented.");
}


void WCNurbsSurface::RemoveKnot(void) {
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::RemoveKnot - Not yet implemented.");
}


void WCNurbsSurface::ElevateDegree(void) {
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::ElevateDegree - Not yet implemented.");
}


void WCNurbsSurface::ReduceDegree(void) {
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::ReduceDegree - Not yet implemented.");
}


WCNurbsSurface& WCNurbsSurface::operator=(const WCNurbsSurface &surface) {
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::operator= - Not yet implemented.");
	return *this;
}


bool WCNurbsSurface::operator==(const WCNurbsSurface &surface) {
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::operator== - Not yet implemented.");
	return false;
}


xercesc::DOMElement* WCNurbsSurface::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create primary element for this object
	XMLCh* xmlString = xercesc::XMLString::transcode("NURBSSurface");
	xercesc::DOMElement*  element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);
	//Include the parent element
	xercesc::DOMElement* curveElement = this->WCGeometricSurface::Serialize(document, dictionary);
	element->appendChild(curveElement);

	//Add Context attrib
	WCSerializeableObject::AddGUIDAttrib(element, "context", this->_context, dictionary);
	//Degree
	WCSerializeableObject::AddFloatAttrib(element, "degreeU", this->_degreeU);
	WCSerializeableObject::AddFloatAttrib(element, "degreeV", this->_degreeV);
	//Number of control point
	WCSerializeableObject::AddFloatAttrib(element, "cpU", this->_cpU);
	WCSerializeableObject::AddFloatAttrib(element, "cpV", this->_cpV);
	//Number of knot points
	WCSerializeableObject::AddFloatAttrib(element, "kpU", this->_kpU);
	WCSerializeableObject::AddFloatAttrib(element, "kpV", this->_kpV);
	//Modes
	this->_modeU.ToElement(element, "modeU");
	this->_modeV.ToElement(element, "modeV");

	//Add list of control points
	for (WPUInt i=0; i<this->_cpU * this->_cpV; i++)
		this->_controlPoints.at(i).ToElement(element, "ControlPoint");

	//List of knot points for U
	xmlString = xercesc::XMLString::transcode("KnotPointU");
	xercesc::DOMElement *kpElement;
	for (WPUInt i=0; i<this->_kpU; i++) {
		kpElement = document->createElement(xmlString);
		WCSerializeableObject::AddFloatAttrib(kpElement, "value", this->_knotPointsU[i]);
		element->appendChild(kpElement);
	}
	xercesc::XMLString::release(&xmlString);
	//List of knot points for V
	xmlString = xercesc::XMLString::transcode("KnotPointV");
	for (WPUInt i=0; i<this->_kpV; i++) {
		kpElement = document->createElement(xmlString);
		WCSerializeableObject::AddFloatAttrib(kpElement, "value", this->_knotPointsU[i]);
		element->appendChild(kpElement);
	}
	xercesc::XMLString::release(&xmlString);

	//Return the element
	return element;
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
			for (int i=(int)tmpVec.size()-1; i>=0; i--) curveCP.push_back( tmpVec.at(i) );
		}
	}
	WPUInt numCPV = (WPUInt)curveCP.size();

	//Holding arrays for sines and cosines
	WPFloat theta = 0.0;
	WPFloat *cosines = new WPFloat[numArcs+1];
	WPFloat *sines = new WPFloat[numArcs+1];
	//Determine all angle values
	for (WPUInt i=1; i<=numArcs; i++) {
		theta = theta + deltaAngle;
		cosines[i] = cos(theta);
		sines[i] = sin(theta);
	}

	//Allocate array of vectors
	WCVector4 **Pij = new WCVector4*[numCPU];
	for (WPUInt i=0; i<numCPU; i++) Pij[i] = new WCVector4[numCPV];

	WPFloat r, uDummy, vDummy;
	WPFloat wm = cos(deltaAngle / 2.0), weight;
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

	//Delete arrays
	delete cosines;
	delete sines;
//	for (WPUInt i=0; i<numCPU; i++) delete Pij[i];
	delete Pij;
	//Return the new surface
	return surface;
}


/***********************************************~***************************************************/


std::ostream& __WILDCAT_NAMESPACE__::operator<<(std::ostream& out, const WCNurbsSurface &surface) {
	//Print out basic surface info
	out << "NurbsSurface(" << (void*)&surface << "):\n";
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

