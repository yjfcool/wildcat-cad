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


/*** Included header files ***/
#include "Geometry/nurbs_curve.h"
#include "Geometry/geometric_point.h"
#include "Geometry/geometry_context.h"
#include "Geometry/geometric_algorithms.h"
#include "Geometry/nurbs.h"
#include "Geometry/geometric_line.h"
#include "Geometry/ray.h"


/*** Extern Variables ***/
extern WPFloat* __bezier_coef[8];

/*** Locally Defined Values ***/
//Accuracy Constants
#define NURBSCURVE_INVERSION_MAX_ITERATIONS		12
#define NURBSCURVE_EPSILON_ONE					0.0001
#define NURBSCURVE_EPSILON_TWO					0.0001
#define NURBSCURVE_EQUALITY_EPSILON				0.001
#define NURBSCURVE_RENDER_ACCURACY				0.20
#define NURBSCURVE_RENDER_LOWER					0.85
#define NURBSCURVE_RENDER_UPPER					1.55


/***********************************************~***************************************************/


void WCNurbsCurve::GenerateKnotPointsVBO(void) {
	//Determine actual number of bytes needed in the buffer
	WPUInt size = this->_kp * 4 * sizeof(GLfloat);
	GLfloat *data = new GLfloat[size / sizeof(GLfloat)];
	//Check to see if shader array is undersize	
	if ((WPUInt)this->_context->CurveMinKPBufferSize() < size) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::GenerateKnotPointsVBO - Undersized array in shader.");
	}	
	//Otherwise, copy the kp array into the vbo (Cast WPFloat to GLfloat)	
	for (WPUInt i=0; i<this->_kp; i++) data[i*4] = (GLfloat)this->_knotPoints[i];
	//Bind the knot point buffer and load it
	glBindBuffer(GL_ARRAY_BUFFER, this->_context->CurveKPBuffer());	
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	//Unbind the buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Check for errors here
	if (glGetError() != GL_NO_ERROR) 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::GenerateKnotPointsVBO - Unspecified GL Error.");
/*** Debug ***
	std::cout << "Knot Points VBO: " << this->_kp << std::endl;	
	GLfloat *data2 = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	for (int i=0; i<this->_kp; i++) printf("\t%d: %f\n", i, data2[i*4]);
	glUnmapBuffer(GL_ARRAY_BUFFER);	
/*** Debug ***/
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	delete data;
}


void WCNurbsCurve::GenerateKnotPointsTexture(void) {
	//Create temporary array for data
	GLfloat *data = new GLfloat[4 * this->_kp];
	//Copy knot points into array (cast WPFloat to GLfloat)
	for (WPUInt i=0; i<this->_kp; i++) data[i*4] = (GLfloat)this->_knotPoints[i];
	//Set up some parameters
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//Set up texture
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_context->CurveKPTex());	
	glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, this->_kp, 1, GL_RGBA, GL_FLOAT, data);
	//Delete array and check for errors
	if (glGetError() != GL_NO_ERROR) 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::GenerateKnotPointsTexture Error - Texture Setup.");
	delete data;
}


void WCNurbsCurve::GenerateControlPointsVBO(void) {
	//Determine the needed size of the buffer
	GLuint size = this->_cp * 4 * sizeof(GLfloat);
	GLfloat *data = new GLfloat[size / sizeof(GLfloat)];
	//Check for shader array undersize
	if ((WPUInt)this->_context->CurveMinCPBufferSize() < size) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::GenerateControlPointsVBO - Undersized array in shader.");
	}
	//Otherwise, convert all control points to GLfloat arrays
	WCVector4 controlPoint;
	for (WPUInt i=0; i<this->_cp; i++) {
		//Cast each control point to a GLfloat array
		controlPoint = this->_controlPoints.at(i);
		data[i*4]	= (GLfloat)controlPoint.I();
		data[i*4+1] = (GLfloat)controlPoint.J();
		data[i*4+2] = (GLfloat)controlPoint.K();
		data[i*4+3] = (GLfloat)controlPoint.L();
	}
	//Bind to buffer and copy data in
	glBindBuffer(GL_ARRAY_BUFFER, this->_context->CurveCPBuffer());
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	//Unbind the buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Should check for errors here
	if (glGetError() != GL_NO_ERROR) 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::GenerateControlPointsVBO - Unspecified GL Error.");
/*** Debug ***
	std::cout << "Control Point VBO: " << this->_cp << std::endl;
	GLfloat *data2 = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	for (int i=0; i<this->_cp; i++) printf("\t%d: %f %f %f %f\n", i, data2[i*4], data2[i*4+1], data2[i*4+2], data2[i*4+3]);
	glUnmapBuffer(GL_ARRAY_BUFFER);
/*** Debug ***/
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Delete data array
	delete data;
}


void WCNurbsCurve::GenerateControlPointsTexture(void) {
	//Create temporary array for data
	GLfloat *data = new GLfloat[this->_cp * 4];
	//Copy control points into array
	WCVector4 controlPoint;
	for (WPUInt i=0; i<this->_cp; i++) {
		//Cast each control point to a GLfloat array
		controlPoint = this->_controlPoints.at(i);
		data[i*4]	= (GLfloat)controlPoint.I();
		data[i*4+1] = (GLfloat)controlPoint.J();
		data[i*4+2] = (GLfloat)controlPoint.K();
		data[i*4+3] = (GLfloat)controlPoint.L();	
	}
	//Set up some parameters
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//Set up texture
	glActiveTexture(GL_TEXTURE1);	
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_context->CurveCPTex());	
	glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, this->_cp, 1, GL_RGBA, GL_FLOAT, data);
	if (glGetError() != GL_NO_ERROR) 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::GenerateControlPointsTexture Error - Texture Setup.");
	//Delete data array
	delete data;
}


void WCNurbsCurve::LoadKnotPoints(const std::vector<WPFloat> &knotPoints) {
	//Default Knot Points
	if (this->_mode == WCNurbsMode::Default()) {
		//Make sure not actually Bezier
		if (this->_cp == this->_degree+1) {
			this->_mode = WCNurbsMode::Bezier();
			CLOGGER_WARN(WCLogManager::RootLogger(), "WCNurbsCurve::LoadKnotPoints - Converting mode to Bezier.");
			this->_knotPoints = WCNurbs::LoadBezierKnotPoints(this->_kp);
		}
		//Otherwise load normal default values
		else this->_knotPoints = WCNurbs::LoadDefaultKnotPoints(this->_kp, this->_degree);
	}
	//Bezier Knot Points
	else if (this->_mode == WCNurbsMode::Bezier()) {
		//Make sure not actually default
		if (this->_cp != this->_degree+1) {
			this->_mode = WCNurbsMode::Default();
			CLOGGER_WARN(WCLogManager::RootLogger(), "WCNurbsCurve::LoadKnotPoints - Converting mode to Default.");
			this->_knotPoints = WCNurbs::LoadDefaultKnotPoints(this->_kp, this->_degree);
		}
		//Otherwise load normal Bezier values		
		else this->_knotPoints = WCNurbs::LoadBezierKnotPoints(this->_kp);
	}
	//Custom Knot Points
	else if (this->_mode == WCNurbsMode::Custom()) {
		bool isCustom = true;
		//Check to make sure custom array is valid
		for (WPUInt i=1; i<this->_degree; i++) {
			if (knotPoints.at(i) != knotPoints.at(0)) isCustom = false;
			if (knotPoints.at(this->_kp-i-1) != knotPoints.at(this->_kp-1)) isCustom = false;
		}
		//If still custom, then use it
		if (isCustom)
			this->_knotPoints = WCNurbs::LoadCustomKnotPoints(knotPoints);
		//Otherwise just make curve mode Default
		else {
			this->_mode = WCNurbsMode::Default();
			CLOGGER_WARN(WCLogManager::RootLogger(), "WCNurbsCurve::LoadKnotPoints - Converting mode to Default.");
			this->_knotPoints = WCNurbs::LoadDefaultKnotPoints(this->_kp, this->_degree);
		}
	}
}


GLfloat* WCNurbsCurve::GenerateCurveHigh(const WPUInt &lod, const bool &server, GLuint &buffer) {
//Conditional compilation as long as transform feedback is included
#ifdef GL_EXT_transform_feedback
	/*** Setup programs and bindable uniforms ***/		

	//Now generate the control points VBO
	this->GenerateControlPointsVBO();
	//Create a knotpoint VBO
	this->GenerateKnotPointsVBO();
	//Setup the right program
	if ((this->_mode == WCNurbsMode::Bezier()) && (this->_degree <= 3)) {
		std::cout << "Error in sing Bezier23.\n";
//		glUseProgram(this->_context->CurveBezier23());
//		glUniform4i(WCNurbsCurve::_locations[NURBSCURVE_LOC_PARAMS_BEZIER23], this->_degree, this->_cp, this->_kp, WCNurbsCurve::_vertsPerBatch);
	}
	//Degree > 3, Default and Custom
	else {
//		std::cout << "Using Default.\n";
		glUseProgram(this->_context->CurveDefaultProgram());		
		glUniform4i(this->_context->CurveLocations()[NURBSCURVE_LOC_PARAMS_DEFAULT], 
			this->_degree, this->_cp, this->_kp, this->_context->CurveVerticesPerBatch());
	}
	//Make sure no problems so far
	if (glGetError() != GL_NO_ERROR) 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::GenerateCurveHigh - Setup Problems.");

	/*** Setup Output Buffer ***/
	
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, lod * NURBSCURVE_FLOATS_PER_VERTEX * sizeof(GLfloat), 0, GL_STATIC_DRAW);
	//Bind the buffer for transform feedback	
	glBindBufferBaseEXT(GL_TRANSFORM_FEEDBACK_BUFFER_EXT, 0, buffer);	
//	if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsCurve::GenerateCurveHigh Error - Output buffer setup.\n";

	/*** Prepare the Input Buffer ***/
	
	WPFloat u = this->_knotPoints[0];
	WPFloat range = this->_knotPoints[this->_kp-1] - this->_knotPoints[0];
	WPFloat du = range / ((GLfloat)(lod-1));
	int numBatches = (int)ceil( (float)lod / (float)this->_context->CurveVerticesPerBatch() );
	//Create a temporary array (LOD vertices)
	GLfloat *data = new GLfloat[numBatches * NURBSCURVE_FLOATS_PER_VERTEX];
	for (int i=0; i<numBatches; i++) {
		//Load array with value for each batch
		data[i*4] =   (GLfloat)u;								//Set first position to u
		data[i*4+1] = (GLfloat)du;								//Set second position to du
		data[i*4+2] = (GLfloat)this->_knotPoints[this->_kp-1];	//Set third position to uMax
		data[i*4+3] = (GLfloat)1.0;								//Dummy data
		//Increment u
		u += du * this->_context->CurveVerticesPerBatch();				
	}	
	//Create a temporary input buffer
	GLuint inputBuffer;
	glGenBuffers(1, &inputBuffer);
	//Set up transform feedback	buffer
	glBindBuffer(GL_ARRAY_BUFFER, inputBuffer);
	//Allocate space for the batch data and load data value array
	glBufferData(GL_ARRAY_BUFFER, numBatches * NURBSCURVE_FLOATS_PER_VERTEX * sizeof(GLfloat), data, GL_STATIC_DRAW);
//	if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsCurve::GenerateCurveHigh Error - Input buffer setup.\n";
	//Delete data array
	delete data;
			
	/*** Time to Generate ***/

	//Disable rasterization
	glEnable(GL_RASTERIZER_DISCARD_EXT);		
	//Begin the transform feedback
	glBeginTransformFeedbackEXT(GL_POINTS);
//	if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsCurve::GenerateCurveHigh Error - BeginTransformFeedback.\n";
	
	/*** Draw Data ***/

	glVertexPointer(4, GL_FLOAT, 0, 0);
	//There are LOD vertices
	glDrawArrays(GL_POINTS, 0, numBatches);		
//	if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsCurve::GenerateCurveHigh Error - Draw Arrays.\n";
		
	/*** Wrap Stuff Up ***/
	
	glEndTransformFeedbackEXT();
	//Reenable rasterization
	glDisable(GL_RASTERIZER_DISCARD_EXT);
	//Delete the temporary input buffer
	glDeleteBuffers(1, &inputBuffer);
	//Should check for errors here
	if (glGetError() != GL_NO_ERROR) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::GenerateCurveHigh - Unspecified GL Error.");
		//Need to do more here
	}
/*** Debug ***
	std::cout << "High Feedback Verts: " << this->_lod+1 << std::endl;	
	GLfloat *data2 = (GLfloat*)glMapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER_EXT, GL_READ_ONLY);
	for (int i=0; i<this->_lod+1; i++) printf("\t%d: %f %f %f %f\n", i, data2[i*4], data2[i*4+1], data2[i*4+2], data2[i*4+3]);
	glUnmapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER_EXT);	
/*** Debug ***/
#endif // GL_EXT_transform_feedback
	return NULL;
}


GLfloat* WCNurbsCurve::GenerateCurveMedium(const WPUInt &lod, const bool &server, GLuint &buffer) {
	/*** Setup programs and texture locations ***/		
	//Default and custom case
	switch (this->_degree) {
		case 2: case 3:
			glUseProgram(this->_context->CurveDefault23Program());
			glUniform4i(this->_context->CurveLocations()[NURBSCURVE_LOC_PARAMS_DEFAULT23], this->_degree, this->_cp, this->_kp, lod);
			break;
		default:
			//Degree > 3 Default, Custom, and Bezier
			glUseProgram(this->_context->CurveDefaultProgram());
			glUniform4i(this->_context->CurveLocations()[NURBSCURVE_LOC_PARAMS_DEFAULT], this->_degree, this->_cp, this->_kp, lod);
			break;
	}
	//Now generate the control points texture
	this->GenerateControlPointsTexture();
	//Create a knotpoint texture
	this->GenerateKnotPointsTexture();

	/*** Bind to framebuffer object ***/
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->_context->CurveFramebuffer());
	//Check to make sure the framebuffer is ready
	GLenum retVal = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	//Check the status of the framebuffer object
	if (retVal != GL_FRAMEBUFFER_COMPLETE_EXT) { 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::GenerateCurveMedium - Framebuffer is not complete.");
		return NULL;
	}
	
	/*** Setup input texture ***/
	
	//Allocate space for the input texture (w * h * RGBA)
	GLfloat *data = new GLfloat[lod * NURBSCURVE_FLOATS_PER_VERTEX];
	WPFloat u = this->_knotPoints[0];
	WPFloat range = this->_knotPoints[this->_kp-1] - this->_knotPoints[0];
	WPFloat du = range / ((GLfloat)(lod-1));	
	//Initialize data in the array
	for (WPUInt i=0; i<lod; i++) {
		data[i*4] =   (GLfloat)u;						//Set first position to u
		data[i*4+1] = (GLfloat)du;						//Set second position to du
		data[i*4+2] = (GLfloat)this->_knotPoints[this->_kp-1];	//Set third position to uMax
		data[i*4+3] = (GLfloat)(i);						//Set fourth position to index		
		//Increment u
		u = STDMIN(u+du, this->_knotPoints[this->_kp-1]);
	}
	//Setup and copy the data into the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_context->CurveInTex());
	glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, lod, 1, GL_RGBA, GL_FLOAT, data);
	//Check for errors
	if (glGetError() != GL_NO_ERROR) 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::GenerateCurveMedium - Setup Input Texture.");
	//Delete data array
	delete data;

	/*** Setup Viewport and Render***/

	//Save the viewport setting
	glPushAttrib(GL_VIEWPORT_BIT);
	//Disable some settings
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	//Set the viewport
	glViewport(0, 0, lod, 1);
	//Draw into the framebuffer
	glBegin(GL_QUADS);
		glVertex3f(-1.0, -1.0, 0.0);
		glVertex3f(-1.0, 1.0, 0.0);
		glVertex3f(1.0, 1.0, 0.0);	
		glVertex3f(1.0, -1.0, 0.0);
	glEnd();
	//Restore the viewport setting
	glPopAttrib();
	//Re-enable some settings
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	//Check for errors
	if (glGetError() != GL_NO_ERROR)
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::GenerateCurveMedium - Render.");

	/*** Save output texture into vertex VBO using simple memory read ***/

	GLfloat *vertData = new GLfloat[lod * NURBSCURVE_FLOATS_PER_VERTEX];
	glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
	glReadPixels(0, 0, lod, 1, GL_RGBA, GL_FLOAT, vertData);
/*** Debug ***
	std::cout << "Medium Generation Vertices: " << lod << std::endl;	
	for (int i=0; i<lod; i++) printf("\t%d: %f %f %f %f\n", i, vertData[i*4], vertData[i*4+1], vertData[i*4+2], vertData[i*4+3]);
/*** Debug ***/
	//See if server or client
	if (server) {
		//If the buffer is not already present, gen it
		if (!buffer) glGenBuffers(1, &buffer);
		//Bind the buffer and load it up
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, lod * NURBSCURVE_FLOATS_PER_VERTEX * sizeof(GLfloat), vertData, GL_STATIC_DRAW);
		//No longer need vertData
		delete vertData;
		//Make sure to set to NULL since we return vertData
		vertData = NULL;
		//Clean up and check for errors
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::GenerateCurveMedium - Cleanup.");
	}

	/*** Save output texture into vertex VBO using a PBO ***

	glBindBuffer(GL_PIXEL_PACK_BUFFER, this->_buffer);
	glBufferData(GL_PIXEL_PACK_BUFFER, this->_context->CurveMaxTextureSize() * 4 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
	glFlush();
	glReadPixels(0, 0, this->_context->CurveMaxTextureSize(), 1, GL_RGBA, GL_FLOAT, NULL);
	glFlush();
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::GenerateCurveMedium - FBO -> PBO -> VBO.");	
	
	/*** Restore the framebuffer ***/
		
	//Clean up the framebuffer object and cp/kp textures
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);	
	//Return client buffer
	return vertData;
}
	

GLfloat* WCNurbsCurve::GenerateCurveLow(const WPUInt &lod, const bool &server, GLuint &buffer) {
	//Setup some variables
	WPFloat u = this->_knotPoints[0];
	WPFloat range = this->_knotPoints[this->_kp-1] - this->_knotPoints[0];
	WPFloat du = range / ((GLfloat)(lod-1));
	WCVector4 pt;
	GLuint size = lod * NURBSCURVE_FLOATS_PER_VERTEX;
	//Create a temporary array (LOD vertices)
	GLfloat *data = new GLfloat[size];
	for (WPUInt i=0; i<lod; i++) {
		pt = this->Evaluate(u);
		data[i*4] =   (GLfloat)pt.I();
		data[i*4+1] = (GLfloat)pt.J();
		data[i*4+2] = (GLfloat)pt.K();
		data[i*4+3] = (GLfloat)pt.L();
		//Increment u (up to kp-1)
		u = STDMIN(u+du, this->_knotPoints[this->_kp-1]);				
	}
/*** Debug ***
	 std::cout << "Generate Low Verts: " << lod << std::endl;	
	 for (int i=0; i<lod; i++) printf("\t%d: %f %f %f %f\n", i, data[i*4], data[i*4+1], data[i*4+2], data[i*4+3]);
/*** Debug ***/
	//If server side
	if (server) {
		//Gen buffer if needed
		if (!buffer) glGenBuffers(1, &buffer);
		//Bind to buffer and copy data in
		glBindBuffer(GL_ARRAY_BUFFER, buffer);	
		glBufferData(GL_ARRAY_BUFFER, size * sizeof(GLfloat), data, GL_STATIC_DRAW);
		delete data;
		data = NULL;
		//Clean up and check for errors
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		//Check for errors
		if (glGetError() != GL_NO_ERROR)
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::GenerateCurveLow - GL error at Cleanup.");	
	}
	//Return client buffer
	return data;
}


GLfloat* WCNurbsCurve::GenerateCurveOne(const bool &server, GLuint &buffer) {
	//Determine the needed size of the buffer
	GLuint size = this->_cp * NURBSCURVE_FLOATS_PER_VERTEX;
	GLfloat *data = new GLfloat[size];		
	//Convert all control points to GLfloat arrays
	WCVector4 controlPoint;
	for (WPUInt i=0; i<this->_cp; i++) {
		//Cast each control point to a GLfloat array
		controlPoint = this->_controlPoints.at(i);
		data[i*4]	= (GLfloat)controlPoint.I();
		data[i*4+1] = (GLfloat)controlPoint.J();
		data[i*4+2] = (GLfloat)controlPoint.K();
		data[i*4+3] = 1.0;	
	}
/*** Debug ***
	 std::cout << "Generate One Verts: " << lod << std::endl;	
	 for (int i=0; i<this->_cp; i++) printf("\t%d: %f %f %f %f\n", i, data[i*4], data[i*4+1], data[i*4+2], data[i*4+3]);
/*** Debug ***/
	//Check to see if server or client
	if (server) {
		//Gen buffer if needed
		if (!buffer) glGenBuffers(1, &buffer);
		//Bind to buffer and copy data in
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, size * sizeof(GLfloat), data, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		delete data;
		data = NULL;
		//Clean up and check for errors
		if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::GenerateCurveOne - GL error at Cleanup.");
	}
	//Return client buffer (should be set to NULL if server)
	return data;
}


/***********************************************~***************************************************/


WCNurbsCurve::WCNurbsCurve(WCGeometryContext *context, const WPUInt &degree, const std::vector<WCVector4> &controlPoints, 
	const WCNurbsMode &mode, const std::vector<WPFloat> &knotPoints) : ::WCGeometricCurve(), _context(context),
	_degree(degree), _mode(mode), _cp((WPUInt)controlPoints.size()), _kp(0), _controlPoints(controlPoints), _knotPoints(NULL),
	_length(0.0), _lod(0), _buffer(0), _altBuffer(NULL) {
	//Make sure cpCollection is non-null
	if (this->_cp == 0) { CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::WCNurbsCurve - Empty control points vector."); return;	}
	//Check to make sure there are at least 2 control points
	if (this->_cp < 2) { CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::WCNurbsCurve - Must have at least two control points."); return; }
	//Make sure not too many control points
	if (this->_cp > NURBSCURVE_MAX_CONTROLPOINTS) { 
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCNurbsCurve::WCNurbsCurve - Too many control points(" << this->_cp << ").");
		this->_cp = NURBSCURVE_MAX_CONTROLPOINTS;	
	}
	//Check to make sure that degree is greater than 0
	if (this->_degree == 0) { CLOGGER_WARN(WCLogManager::RootLogger(), "WCNurbsCurve::WCNurbsCurve - Degree must be greater than 0."); this->_degree = 1; }
	//Check to make sure degree is less than the number of control points
	if ((degree >= this->_cp) || (degree > NURBSCURVE_MAX_DEGREE)) {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCNurbsCurve::WCNurbsCurve - Degree must be less than the number of control points.");
		this->_degree = this->_cp - 1;	
	}

	//Define the number of knot points
	this->_kp = this->_degree + this->_cp + 1;
	//Perform some addtional checks if mode is custom
	if ((this->_mode == WCNurbsMode::Custom()) && (knotPoints.size() != this->_kp)) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::WCNurbsCurve - Invalid knot points collection size:" << knotPoints.size()); 
		this->_mode = WCNurbsMode::Default();
	}
	//Load the knot points
	if (this->_degree != 1) this->LoadKnotPoints(knotPoints);
	//Find the rough length of the curve and the number of needed segments
	this->_length = WCNurbs::EstimateLength(this->_controlPoints);
	//Establish oriented bounding box
	this->_bounds = new WCAlignedBoundingBox(this->_controlPoints);		
}


WCNurbsCurve::WCNurbsCurve(const WCNurbsCurve &curve) :
	::WCGeometricCurve(curve), _context(curve._context), _degree(curve._degree), _mode(curve._mode),
	_cp(curve._cp), _kp(curve._kp), _controlPoints(curve._controlPoints), _knotPoints(NULL),
	_length(curve._length), _lod(0), _buffer(0), _altBuffer(NULL) {
	//Do some more setup only if degree is not 1
	if (this->_degree != 1) {
		//Set up the knot vector
		if (this->_mode != WCNurbsMode::Custom())	
			this->LoadKnotPoints();
		else {
			//Copy the kp array
			this->_knotPoints = new WPFloat[this->_kp];
			memcpy(this->_knotPoints, curve._knotPoints, sizeof(WPFloat) * this->_kp);
		}
	}
	//Establish oriented bounding box
	this->_bounds = new WCAlignedBoundingBox(this->_controlPoints);
}


WCNurbsCurve::WCNurbsCurve(xercesc::DOMElement *element, WCSerialDictionary *dictionary) : 
	::WCGeometricCurve( WCSerializeableObject::ElementFromName(element,"GeometricCurve"), dictionary ),
	_context(NULL), _degree(), _mode(WCNurbsMode::Default()), _cp(), _kp(), _controlPoints(), _knotPoints(),
	_length(0.0), _lod(0), _buffer(0), _altBuffer(NULL) {
	//Make sure element if not null
	if (element == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::WCNurbsCurve - NULL Element passed.");
		//throw error
		return;
	}
	//Get GUID and register it
	WCGUID guid = WCSerializeableObject::GetStringAttrib(element, "guid");
	dictionary->InsertGUID(guid, this);

	//Get context
	this->_context = (WCGeometryContext*)WCSerializeableObject::GetGUIDAttrib(element, "context", dictionary); 
	//Get degree
	this->_degree = (WPUInt)WCSerializeableObject::GetFloatAttrib(element, "degree");
	//Setup NURBS mode
	this->_mode.FromElement(WCSerializeableObject::ElementFromName(element,"Mode"));
	//Get number of control points
	this->_cp = (WPUInt)WCSerializeableObject::GetFloatAttrib(element, "numCP");
	//Get number of knot points
	this->_kp = (WPUInt)WCSerializeableObject::GetFloatAttrib(element, "numKP");

	//Find all nodes called ControlPoints
	XMLCh *xmlString = xercesc::XMLString::transcode("ControlPoint");
	xercesc::DOMNodeList *nodeList = element->getElementsByTagName(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Make sure cp and length agree
	if (nodeList->getLength() != this->_cp) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::WCNurbsCurve - CP size mismatch.");
		//throw error
		return;
	}
	//Loop through all cp and load them
	WCVector4 cp;
	xercesc::DOMElement *cpElement;
	for (WPUInt index=0; index< this->_cp; index++) {
		//Cast to an element
		cpElement = (xercesc::DOMElement*)nodeList->item(index);
		//Load vector from element
		cp.FromElement(cpElement);
		//Add vector into control point list
		this->_controlPoints.push_back(cp);
	}

	//Find all nodes called KnotPoint
	xmlString = xercesc::XMLString::transcode("KnotPoint");
	nodeList = element->getElementsByTagName(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Make sure kp and length agree
	if (nodeList->getLength() != this->_kp) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::WCNurbsCurve - KP size mismatch.");
		//throw error
		return;
	}
	//Allocate kp array
	this->_knotPoints = new WPFloat[this->_kp];
	//Loop through all kp and load them
	WPFloat kp;
	xercesc::DOMElement *kpElement;
	for (WPUInt index=0; index< this->_kp; index++) {
		//Cast to an element
		kpElement = (xercesc::DOMElement*)nodeList->item(index);
		//Load float from element
		kp = WCSerializeableObject::GetFloatAttrib(kpElement, "value");
		//Add value into knot point list
		this->_knotPoints[index] = kp;
	}

	//Find the rough length of the curve and the number of needed segments
	this->_length = WCNurbs::EstimateLength(this->_controlPoints);
	//Establish aligned bounding box
	this->_bounds = new WCAlignedBoundingBox(this->_controlPoints);
}


WCNurbsCurve::~WCNurbsCurve() {
	//See if need to Delete the buffers
	this->ReleaseBuffer(this->_buffer);
	this->ReleaseBuffer(this->_altBuffer);
	//Delete the knot point array
	if (this->_knotPoints) delete this->_knotPoints;
}


void WCNurbsCurve::ControlPoints(const std::vector<WCVector4> &controlPoints) {
	//Make sure number of control points is the same
	if (controlPoints.size() != this->_cp) {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCNurbsCurve::ControlPoints - Wrong number of points.");
		return;
	}
	//Update control points
	this->_controlPoints = controlPoints;
	//Mark the object as dirty
	this->IsVisualDirty(true);
	this->IsSerialDirty(true);
}


void WCNurbsCurve::KnotPoints(const std::vector<WPFloat> &knotPoints) {
	//Make sure number of knot points is the same
	if (knotPoints.size() != this->_kp) {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCNurbsCurve::KnotPoints - Wrong number of points.");
		return;
	}
	//Delete old knot points list
	if (this->_knotPoints != NULL) delete this->_knotPoints;
	//Update knot points
	this->_knotPoints = WCNurbs::LoadCustomKnotPoints(knotPoints);
	//Mark the object as dirty
	this->IsVisualDirty(true);
	this->IsSerialDirty(true);
}


WPUInt WCNurbsCurve::Degree(const WPUInt &degree) {
	//Check to make sure degree is valid
	if ((degree < 1) || (degree > NURBSCURVE_MAX_DEGREE)) {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCNurbsCurve::Degree - Degree(" << degree << ") out of bounds.");
		return this->_degree;
	}
	//Don't let degree exceed number of points
	if (degree >= this->_cp) {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCNurbsCurve::Degree - Degree must not equal or exceed number of control points.");
		return this->_degree;	
	}
	//Don't let degree change if mode is Custom
	if (this->_mode == WCNurbsMode::Custom()) {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCNurbsCurve::Degree - Degree change not allowed in custom knot mode.");
		return this->_degree;	
	}
	//Set the new degree
	this->_degree = degree;
	//Check to make sure at least lod = cp
	this->_lod = this->_cp;	
	//Determine the correct number of knot points
	this->_kp = this->_degree + this->_cp + 1;
	//Do some more setup only if degree is not 1
	if (this->_degree != 1) {			
		//Use knot mode to setup knot array
		this->LoadKnotPoints();
	}
	//Mark the object as dirty
	this->IsVisualDirty(true);
	this->IsSerialDirty(true);
	//Return the degree value
	return this->_degree;
}


WCVisualObject* WCNurbsCurve::HitTest(const WCRay &ray, const WPFloat &tolerance) {
	return NULL;
}


void WCNurbsCurve::ApplyTransform(const WCMatrix4 &transform) {
	//Loop through all control points and apply transform
	for (WPUInt i=0; i<this->_cp; i++)
		this->_controlPoints.at(i) = transform * this->_controlPoints.at(i);
	//Make sure curve is regenerated
	this->IsVisualDirty(true);
	this->IsSerialDirty(true);
}


void WCNurbsCurve::ApplyTranslation(const WCVector4 &translation) {
	//Loop through all control points and apply translation
	for (WPUInt i=0; i<this->_cp; i++)
		this->_controlPoints.at(i) = this->_controlPoints.at(i) + translation;
	//Make sure curve is regenerated
	this->IsVisualDirty(true);
	this->IsSerialDirty(true);
}


void WCNurbsCurve::Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom) {
	//Do nothing if not visible
	if (!this->_isVisible) return;

	//Determine best LOD
	WPUInt lod = (WPUInt)STDMAX((WPFloat)this->_cp, this->_length * sqrt(zoom) / NURBSCURVE_RENDER_ACCURACY);
	WPFloat factor = (WPFloat)this->_lod / (WPFloat)lod;
	//See if need to regenerate
	if ((factor < NURBSCURVE_RENDER_LOWER) || (factor > NURBSCURVE_RENDER_UPPER))  {
		//Set the new lod
		this->_lod = lod;
		//Mark as dirty
		this->IsVisualDirty(true);
	}
	//Check to see if curve needs to be generated
	if (this->IsVisualDirty()) {
		this->GenerateServerBuffer(this->_lod, this->_buffer, true);
		//Mark as clean
		this->IsVisualDirty(false);
	}

	//Set the rendering program
	if (this->_renderProg != 0) glUseProgram(this->_renderProg);
	else glUseProgram(defaultProg);
	//Set up rendering environment
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	//Bump z to move lines off of surfaces
	glTranslated(0.0, 0.0, 0.01);
	glLineWidth((GLfloat)this->_thickness);
	glEnable(GL_LINE_SMOOTH);
	glEnableClientState(GL_VERTEX_ARRAY);	
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffer);
	glVertexPointer(NURBSCURVE_FLOATS_PER_VERTEX, GL_FLOAT, 0, 0);
	//Set color appropriately
	if (color == WCColor::Default()) {
		this->_color.Enable();
	}
	else {
		color.Enable();
		glUseProgram(0);
	}
	//Draw the geometry
	glDrawArrays(GL_LINE_STRIP, 0, this->_lod);
	//Clean up the environment
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisable(GL_LINE_SMOOTH);
	//Restore matrix and mode
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	//Report them errors
	if (glGetError() != GL_NO_ERROR) 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::Render - Unspecified GL error at cleanup.");
}


void WCNurbsCurve::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
	//Something has changed in the control points - mark as dirty
	this->IsVisualDirty(true);
	this->IsSerialDirty(true);
	//Update Length estimate in case length changed
	this->_length = WCNurbs::EstimateLength(this->_controlPoints);
	//Make sure all dependent objects know about it
	this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);
}


WPFloat WCNurbsCurve::Length(const WPFloat &tolerance) {
	//Check if serial dirty or length set to 0.0
	if (this->IsSerialDirty() || this->_length == 0.0) {
		//Estimate the length of the curve and needed LOD
		WPFloat length = WCNurbs::EstimateLength(this->_controlPoints);
		WPUInt lod = (WPUInt)(length / tolerance);
		//Get client buffer of vertex data
		GLfloat *verts = this->GenerateClientBuffer(lod, true);
		//Set up points
		WCVector4 p0(verts[0], verts[1], verts[2], verts[3]), p1;
		//Loop through the vertex data
		for (WPUInt i=1; i<this->_lod; i++) {
			p1 = WCVector4(verts[i*4], verts[i*4+1], verts[i*4+2], 1.0);
			length += p1.Distance(p0);
			p0 = p1;
		}
		//Make sure to delete the vert data
		this->ReleaseBuffer(verts);
		//Set the length
		this->_length = length;
	}
	//Return the length
	return this->_length;
}


WCVector4 WCNurbsCurve::Evaluate(const WPFloat &u) {
	WCVector4 c;
	WPFloat eval = u;
	//Bounds check the u value
	if (eval < 0.0) eval = 0.0;
	if (eval > 1.0) eval = 1.0;

	//Bezier case
	if (this->_mode == WCNurbsMode::Bezier()) {
		WCMatrix n(this->_degree+1, this->_degree+1, __bezier_coef[this->_degree]);
		WPFloat *pts = new WPFloat[(this->_degree+1)*4];
		WCVector w(this->_degree+1, 0.0);
		WCVector4 tmpVec;
		for (WPUInt i=0; i<=this->_degree; i++) {
			tmpVec = this->_controlPoints.at(i);
			pts[i*4]	= tmpVec.I(); 
			pts[i*4+1]	= tmpVec.J(); 
			pts[i*4+2]	= tmpVec.K(); 
			pts[i*4+3]	= tmpVec.L();
			w.Set(i, tmpVec.L());
		}
		WCMatrix g(this->_degree+1, 4, pts);
		delete pts;
		WCMatrix ng = n * g;
		//Calculate t vector
		WCVector t(this->_degree+1, 1.0);			
		WPFloat mag = eval;
		//Setup t vector
		for (int j=this->_degree-1; j>=0; j--) {
			t.Set(j, mag);
			mag *= u;
		}
		//Calculate the point
		c = (t * ng).ToVector4();
		//Calculate w
//		WPFloat wFloat = t.DotProduct(n * w);
//		std::cout << w << std::endl;
//		std::cout << wFloat << std::endl;
//		std::cout << c << std::endl;
//		c /= c.L();
//		c.L(1.0);
		return c;
	}
	//Default & custom case
	else if ((this->_mode == WCNurbsMode::Default()) || (this->_mode == WCNurbsMode::Custom())) {
		//Find the span for the u value
		WPUInt span = WCNurbs::FindSpan(this->_cp, this->_degree, eval, this->_knotPoints);
		WCVector4 c, p0;

		WPFloat* basisValues = WCNurbs::BasisValues(span, eval, this->_degree, this->_knotPoints);
		//Return if error
		if (basisValues == NULL) return WCVector4();
		WPFloat w = 0.0;
		//Evaluate the point
		for(WPUInt j=0; j<=this->_degree; j++) {
			p0 = this->_controlPoints.at(span-this->_degree+j);
			w += p0.L() * basisValues[j];
			c += p0 * p0.L() * basisValues[j];
		}
		//Make sure to delete basis values
		delete basisValues;		
		//Do the w-divide
		c = c / w;
		//Set W = 1.0
		c.L(1.0);
		return c;	
	}
	//Error case
	else CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::Evaluate - Invalid mode.");
	//Return the calculated value
	return c;
}


/*** Need to normalize rational derivatives ***/
WCVector4 WCNurbsCurve::Derivative(const WPFloat &u, const WPUInt &der) {
	WPFloat eval = u;
	WPUInt derivative = der;
	//Bounds check the u value
	if (eval < this->_knotPoints[0]) eval = this->_knotPoints[0];
	if (eval > this->_knotPoints[this->_kp-1]) eval = this->_knotPoints[this->_kp-1];
	//Make sure der is not larger than degree
	if (derivative > this->_degree) derivative = this->_degree;

	//Now find the derivative of the curve at the point
	WPUInt span = WCNurbs::FindSpan(this->_cp, this->_degree, eval, this->_knotPoints);
	WPFloat* basisValues = WCNurbs::BasisValues(span, eval, this->_degree, this->_knotPoints, der);
	//Return if error
	if (basisValues == NULL) return WCVector4();	
	
	//Declare some temparary values
	WCVector4 c;
	int index;
/*** Debug ***/
	WPFloat w=1;
/*** Debug ***/	
//	WCGeometricPoint *point;
	WCVector4 point;
	//Calculate the derivative value
	for (WPUInt j=0; j<=this->_degree; j++) {
		point = this->_controlPoints.at(span-this->_degree+j);
		index = span - this->_degree + j;
/*** Debug ***/
//Need to normalize the derivative		
//		w += (point->W() * basisValues[j]);
/*** Debug ***/
//		c = c + point->Data() * point->W() * basisValues[j];		
		c = c + point * basisValues[j];		
	}
	//Do the w-divide
	c = c / w;
	//Set W = 1.0
	c.L(1.0);
	//Make sure to delete the basisValues array
	delete basisValues;
	//Return the derivative vector
	return c;		
}


/*** Need to normalize rational derivatives ***/
WCRay WCNurbsCurve::Tangent(const WPFloat &u) {
	WPFloat eval = u;
	//Check on degree 1
	if (this->_degree == 1) return WCRay(WCVector4(), WCVector4());
	//Check bounds on u
	if (eval < this->_knotPoints[0]) eval = this->_knotPoints[0];
	if (eval > this->_knotPoints[this->_kp-1]) eval = this->_knotPoints[this->_kp-1];

	//Now find the derivative of the curve at the point
	WPUInt span = WCNurbs::FindSpan(this->_cp, this->_degree, eval, this->_knotPoints);
	WPFloat* basisValues = WCNurbs::BasisValues(span, eval, this->_degree, this->_knotPoints, 1);
	if (basisValues == NULL) return WCRay(WCVector4(), WCVector4());
	//Declare some temparary values
	WCVector4 n, nDer;
	WCVector4 pt;
	WPFloat d=0.0, dDer=0.0;
	//Calculate the derivative value
	for (WPUInt i=0; i<=this->_degree; i++) {
		pt = this->_controlPoints.at(span - this->_degree + i);
		d +=	pt.L() * basisValues[i];
		dDer += pt.L() * basisValues[i+this->_degree+1];
		n +=	pt * basisValues[i];// * pt.L();
		nDer += pt * basisValues[i+this->_degree+1];// * pt.L();
	}
	//Make sure to delete basisValues
	delete basisValues;
	//Calculate c and cDer
	WCVector4 c = n / d;
	c.L(1.0);
	WCVector4 cDer = ((nDer * d) - (n * dDer)) / (d * d);
	//Normalize the derivative
	cDer.Normalize(true);
	//Create a ray to display the tangent
	return WCRay(c, cDer);
}


std::pair<WCVector4,WPFloat> WCNurbsCurve::PointInversion(const WCVector4 &point) {
	WPFloat u;
	//Get base vector for point
	WCVector4 refPoint = point;
	refPoint.K(0.0);
	WCVector4 curvePoint;
	int index=-1;
	WPUInt i;
	WPFloat dist, minDist=10000000.0;

	//Calculate a starting u value
	if (this->_degree == 1) {
		WCVector4 x2x1, x1x0, tmp;
		WPFloat t;
		//Loop through all control points to find the closest
		for (i=0; i<this->_cp-1; i++) {
			//Calculate distance from point to line segment
			x2x1 = this->_controlPoints.at(i+1) - this->_controlPoints.at(i);
			x1x0 = this->_controlPoints.at(i) - refPoint;
			//Calculate the distance metric for this segment
			tmp = x2x1.CrossProduct(x1x0);
			dist = tmp.Magnitude() / x2x1.Magnitude();
			//Find the parametric t value
			t = -1.0 * x1x0.DotProduct(x2x1) / pow( x2x1.Magnitude(), 2);			
			//Check to see if this is the smallest so far
			if ((dist < minDist) && (t >= 0.0) && (t <= 1.0)) {
				//Save the distance
				minDist = dist;
				//Save the index
				index = i;
				//Save the t
				u = t;
			}
		}
		//Check to make sure some reasonable index was found
		if (index == -1) return std::make_pair(WCVector4(), -1.0);
		//Now find the point on the line segment and return it
		WCVector4 p = this->_controlPoints.at(index) + (this->_controlPoints.at(index+1) - this->_controlPoints.at(index)) * u;
		return std::make_pair(p, u);
	}

	//Check to see if curve needs to be generated
	if (this->IsVisualDirty()) switch (this->_context->CurvePerformanceLevel()) {
		//Switch on performance level
//		case NURBSCURVE_PERFLEVEL_HIGH:		this->GenerateCurveHigh(); break;
//		case NURBSCURVE_PERFLEVEL_MEDIUM:	this->GenerateCurveMedium(); break;
//		case NURBSCURVE_PERFLEVEL_LOW:		this->GenerateCurveLow(); break;				
		//Mark as clean
			this->IsVisualDirty(true);
	}
	//Otherwise use tessellated vertices
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffer);
	GLfloat *data = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	//Loop through all points and find closest
	for (i=0; i<this->_lod; i++) {
		//Create a point from the data set
		curvePoint.Set(data[i*4], data[i*4+1], data[i*4+2], data[i*4+3]);
		//Calculate distance from the reference point
		dist = refPoint.Distance(curvePoint);
		//Is this closer
		if (dist < minDist) {
			//Set the min distance
			minDist = dist;
			//Save the point index
			index = i;
		}
	}
	//Make sure to unmap the buffer
	glUnmapBuffer(GL_ARRAY_BUFFER);
	//Calculate the u value for the initial minimum point
	u = index * this->_knotPoints[this->_kp-1] / (this->_lod+1);
	
	//Initialize metrics
	WPFloat *basisValues;
	WCVector4 pt, vDist;
	dist = minDist;
	WCVector4 c, cDer, c2Der;
	WCVector4 n, nDer, n2Der;
	WPFloat top, bottom, uNx, w, wDer, w2Der, w2, w3;
	//Loop until condition 1, 2, or 4 is met
	int j = 0;
	while (j <= NURBSCURVE_INVERSION_MAX_ITERATIONS) {
		//Init the point values
		c.Set(0.0, 0.0, 0.0, 0.0);
		w = 0.0;
		wDer = 0.0;
		w2Der = 0.0;
		cDer.Set(0.0, 0.0, 0.0, 0.0);
		c2Der.Set(0.0, 0.0, 0.0, 0.0);
		//Find the span and basis values up to second derivative
		index = WCNurbs::FindSpan(this->_cp, this->_degree, u, this->_knotPoints);
		basisValues = WCNurbs::BasisValues(index, u, this->_degree, this->_knotPoints, 2);
		//Calculate C, C', and C''
		for (i=0; i<=this->_degree; i++) {
			pt = this->_controlPoints.at(index-this->_degree+i);
			//C, C', and C''
			n +=	 pt * basisValues[i];
			nDer +=  pt * basisValues[i+this->_degree+1];
			n2Der += pt * basisValues[i+2*this->_degree+2];
			//w, w', and w''
			w +=	 pt.L() * basisValues[i];
			wDer +=	 pt.L() * basisValues[i+this->_degree+1];
			w2Der += pt.L() * basisValues[i+2*this->_degree+2];
		}
		//Make sure to delete the basis values
		delete basisValues;
		//Calculate C
		c = n / w;
		c.L(1.0);
		//Calculate C'
		w2 = w * w;
		cDer = ((nDer * w) - (n * wDer)) / w2;
		cDer.L(1.0);
		//Calculate C''
		w3 = w2 * w;
		c2Der = ((n2Der * w2) - (nDer * wDer * 2.0) - (n * w2Der * w) + (n * wDer * wDer * 2.0)) / w3;
		c2Der.L(1.0);

		//Calculate metrics
		vDist = c - refPoint;
		dist = vDist.Magnitude();

		/*** Check for condition 1 ***/
		if (dist < NURBSCURVE_EPSILON_ONE) return std::make_pair(c,u);
		
		//Do some interim calculations
		top = cDer.DotProduct(vDist);
		bottom = cDer.Magnitude();
		/*** Check condition 2 ***/
		if (( STDFABS(top) / (bottom*dist) ) <= NURBSCURVE_EPSILON_TWO) return std::make_pair(c, u);
		
		//Calculate next u
		bottom = c2Der.DotProduct(vDist) + (bottom * bottom);
		uNx = u - (top / bottom);
		//Adjust u by rule (3) if necessary (closed and open curves)
		if (this->_isClosed) {
			if (uNx < this->_knotPoints[index]) uNx = this->_knotPoints[index+1] - this->_knotPoints[index] + uNx;
			else if (uNx > this->_knotPoints[index+1]) uNx = this->_knotPoints[index] + uNx - this->_knotPoints[index+1];
		}
		else {
			if (uNx < this->_knotPoints[index]) uNx = this->_knotPoints[index];
			else if (uNx > this->_knotPoints[index+1]) uNx = this->_knotPoints[index+1];
		}
		
		/*** Check rule (4) for halting ***/
		top = (cDer * (uNx - u)).Magnitude();
		if (top <= NURBSCURVE_EPSILON_ONE) return std::make_pair(c, u);

		//Swap the u value to the next u value
		u = uNx;
		//Increment max loop counter
		j++;
	}
	//Return the point on the curve
	return std::make_pair(c, u);		
}


GLfloat* WCNurbsCurve::GenerateClientBuffer(WPUInt &lod, const bool &managed) {
	GLuint dummy;
	GLfloat* buffer;
	//Make sure LOD >= 2
	if (lod < 2) lod = 2;

	//Check for special generation cases - degree 1 curves
	if (this->_degree == 1) {
		//Set the LOD to number of CP
		lod = this->_cp;
		//Generate the buffer of data
		buffer = this->GenerateCurveOne(false, dummy);
	}
	//Low generation only on LOD > MaxTextureSize || PerfLevel == Low
	else if ((lod > (WPUInt)this->_context->CurveMaxTextureSize()) ||
			 (this->_context->CurvePerformanceLevel() == NURBSCURVE_PERFLEVEL_LOW)) {
		//Generate the buffer of data using low method
		buffer = this->GenerateCurveLow(lod, false, dummy);
	}
	//Medium generation only if PerfLevel == Med
	else if (this->_context->CurvePerformanceLevel() == NURBSCURVE_PERFLEVEL_MEDIUM) {
		//Generate the buffer of data using medium method
		buffer = this->GenerateCurveMedium(lod, false, dummy);
	}
	//High generation only if perf level == High
	else if (this->_context->CurvePerformanceLevel() == NURBSCURVE_PERFLEVEL_HIGH) {
		//Generate the buffer of data using high method
		buffer = this->GenerateCurveHigh(lod, false, dummy);
	}
	//Error path
	else {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCNurbsCurve::GenerateServerBuffer - Unknown generation path.");
		//throw error
	}
	//Return the buffer
	return buffer;
}


void WCNurbsCurve::ReleaseBuffer(GLfloat* buffer) {
	//For now just delete the buffer
	if (buffer != NULL) delete buffer;
}


void WCNurbsCurve::GenerateServerBuffer(WPUInt &lod, GLuint &buffer, const bool &managed) {
	//Make sure LOD >= 2
	if (lod < 2) lod = 2;

	//Check for special generation cases - degree 1 curves
	if (this->_degree == 1) {
		//Generate the buffer of data
		this->GenerateCurveOne(true, buffer);
		//Set LOD to numCP
		lod = this->_cp;
	}
	//Low generation only on LOD > MaxTextureSize || PerfLevel == Low
	else if ((lod > (WPUInt)this->_context->CurveMaxTextureSize()) ||
		(this->_context->CurvePerformanceLevel() == NURBSCURVE_PERFLEVEL_LOW)) {
		//Generate the buffer of data using low method
		this->GenerateCurveLow(lod, true, buffer);
	}
	//Medium generation only if PerfLevel == Med
	else if (this->_context->CurvePerformanceLevel() == NURBSCURVE_PERFLEVEL_MEDIUM) {
		//Generate the buffer of data using medium method
		this->GenerateCurveMedium(lod, true, buffer);
	}
	//High generation only if perf level == High
	else if (this->_context->CurvePerformanceLevel() == NURBSCURVE_PERFLEVEL_HIGH) {
		//Generate the buffer of data using high method
		this->GenerateCurveHigh(lod, true, buffer);
	}
	//Error path
	else {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCNurbsCurve::GenerateServerBuffer - Unknown generation path.");
		//throw error
	}
}


void WCNurbsCurve::ReleaseBuffer(GLuint &buffer) {
	//For now just delete the buffer
	if (buffer) glDeleteBuffers(1, &buffer);
}


void WCNurbsCurve::InsertKnot(const WPFloat &u, const WPUInt &multiplicity) {
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::RefineKnot - Not yet implemented.");
}


void WCNurbsCurve::RefineKnot(void) {
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::RefineKnot - Not yet implemented.");
}


void WCNurbsCurve::RemoveKnot(void) {
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::RemoveKnot - Not yet implemented.");
}


void WCNurbsCurve::ElevateDegree(void) {
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::ElevateDegree - Not yet implemented.");
}


void WCNurbsCurve::ReduceDegree(void) {
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::ReduceDegree - Not yet implemented.");
}

	
WCNurbsCurve& WCNurbsCurve::operator=(const WCNurbsCurve &curve) {
	//Check to make sure not copying self
	if (this == &curve) return *this;
	//Copy size of cp and kp
	this->_cp = curve._cp;
	this->_kp = curve._kp;
	//Copy the control points
	this->_controlPoints = curve._controlPoints;
	//Free the current knotpoints and allocate space for new ones
	if (this->_knotPoints != NULL) delete this->_knotPoints;
	this->_knotPoints = new WPFloat[this->_kp];
	//Copy the knot points
	memcpy(this->_knotPoints, curve._knotPoints, this->_kp * sizeof(WPFloat));
	//Clear the LOD
	this->_lod = 0;
	//Copy all of the parameters
	this->_degree = curve._degree;
	this->_mode = curve._mode;
	this->_length = curve._length;
	//Mark as dirty
	this->IsVisualDirty(true);
	this->IsSerialDirty(true);
	//Return this object
	return *this;
}
		

bool WCNurbsCurve::operator==(const WCNurbsCurve &curve) {
	//Check each data member
	if (this->_degree != curve._degree) return false;
	if (this->_mode != curve._mode) return false;
	if (this->_cp != curve._cp) return false;
	if (this->_kp != curve._kp) return false;
	//Check the arrays and collections
	for (WPUInt i=0; i<this->_cp; i++)
		if (this->_controlPoints.at(i) != curve._controlPoints.at(i)) return false;
	for (WPUInt i=0; i<this->_kp; i++)
		if (this->_knotPoints[i] != curve._knotPoints[i]) return false;
/*** Debug ***/
	//May want to check on equivalence of curves due to reparameterization
/*** Debug ***/	
	//If you got here, they are the same
	return true;
}


xercesc::DOMElement* WCNurbsCurve::Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dictionary) {
	//Insert self into dictionary
	WCGUID guid = dictionary->InsertAddress(this);
	//Create primary element for this object
	XMLCh* xmlString = xercesc::XMLString::transcode("NURBSCurve");
	xercesc::DOMElement*  element = document->createElement(xmlString);
	xercesc::XMLString::release(&xmlString);
	//Add GUID attribute
	WCSerializeableObject::AddStringAttrib(element, "guid", guid);
	//Include the parent element
	xercesc::DOMElement* curveElement = this->WCGeometricCurve::Serialize(document, dictionary);
	element->appendChild(curveElement);

	//Add Context attrib
	WCSerializeableObject::AddGUIDAttrib(element, "context", this->_context, dictionary);
	//Degree
	WCSerializeableObject::AddFloatAttrib(element, "degree", this->_degree);
	//Number of control point
	WCSerializeableObject::AddFloatAttrib(element, "numCP", this->_cp);
	//Number of knot points
	WCSerializeableObject::AddFloatAttrib(element, "numKP", this->_kp);

	//NURBS Mode
	xmlString = xercesc::XMLString::transcode("Mode");
	xercesc::DOMElement *modeElement = document->createElement(xmlString);
	this->_mode.ToElement(modeElement);
	element->appendChild(modeElement);
	xercesc::XMLString::release(&xmlString);

	//Add list of control points
	for (WPUInt i=0; i<this->_cp; i++)
		this->_controlPoints.at(i).ToElement(element, "ControlPoint");

	//List of knot points
	xmlString = xercesc::XMLString::transcode("KnotPoint");
	xercesc::DOMElement *kpElement;
	for (WPUInt i=0; i<this->_kp; i++) {
		kpElement = document->createElement(xmlString);
		WCSerializeableObject::AddFloatAttrib(kpElement, "value", this->_knotPoints[i]);
		element->appendChild(kpElement);
	}
	xercesc::XMLString::release(&xmlString);

	//Return the element
	return element;
}


/***********************************************~***************************************************/


WCNurbsCurve* WCNurbsCurve::GlobalInterpolation(const std::vector<WCVector4> &pts, const WPUInt &degree) {
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::GlobalInterpolation - Not yet implemented.");
	return NULL;
}


WCNurbsCurve* WCNurbsCurve::LocalInterpolation(const std::vector<WCVector4> &pts, const WPUInt &degree) {
	return NULL;
}


WCNurbsCurve* WCNurbsCurve::CircularArc(WCGeometryContext *context, const WCVector4 &center, const WCVector4 &xUnit, const WCVector4 &yUnit, 
	const WPFloat &radius, const WPFloat &startAngleDeg, const WPFloat &endAngleDeg) {
	//Create vector of control points
	std::vector<WCVector4> cp;
	//Create vector of knot points
	std::vector<WPFloat> kp;
	//Load control point and knot point vectors
	WCNurbs::CircularPoints(center, xUnit, yUnit, radius, startAngleDeg, endAngleDeg, cp, kp);
	//Create the curve
	WCNurbsCurve *curve = new WCNurbsCurve(context, 2, cp, WCNurbsMode::Custom(), kp);
	//Check to make sure curve is valid
	if (curve == NULL)
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::CircularArc - NULL arc created.");
	return curve;
}


WCNurbsCurve* WCNurbsCurve::Conic(WCGeometryContext *context, const WCVector4 &p0, const WCVector4 &t0, const WCVector4 &p2, 
	const WCVector4 &t2, const WCVector4 &p) {
	WCVector4 p1;
//	std::cout << "***********************************\n";
//	std::cout << "First: " << p0 << std::endl;
//	std::cout << "First Tangent: " << t0 << std::endl;
//	std::cout << "Second: " << p2 << std::endl;
//	std::cout << "Second Tangent: " << t2 << std::endl;
//	std::cout << "Third: " << p << std::endl;
	bool retVal = OneArcConic(p0, t0, p2, t2, p, p1);
	//Check to make sure arc could be created
	if (!retVal) return NULL;
//	std::cout << "P1: " << p1 << std::endl;
	//Check for poorly formed conic
	if (p1.L() <= -1.0) {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCNurbsCurve::Conic - p1.W is <= -1.0.");
		return NULL; 
	}
	//Classify type and number of segments
	WPUInt nsegs;
	if (p1.L() >= 1.0) nsegs = 1;
	else {
		WPFloat angle = Angle(p0, p1, p2);
//		std::cout << "Angle: " << angle << std::endl;
		if ((p1.L() > 0.0) && (angle > 60.0)) nsegs = 1;
		else if ((p1.L() < 0.0) && (angle > 90.0)) nsegs = 4;
		else nsegs = 2;
	}
//	std::cout << "Number of Segments: " << nsegs << std::endl;
	
	WPUInt n = 2 * nsegs;
	WPUInt j = n + 1;
	//Create knot point and control point arrays
	WPFloat *knots = new WPFloat[j+3];
	WCVector4 *controlPoints = new WCVector4[j];
	
	//Load end knots
	for (int i=0; i<3; i++) {
		knots[i] = 0.0;
		knots[i+j] = 1.0;
	}
	//Load first and last control points
	controlPoints[0] = p0;
	controlPoints[n] = p2;
	
	//Single segment case
	if (nsegs == 1) controlPoints[1] = p1;
	//Otherwise
	else {
		WCVector4 q1, s, r1;
		SplitArc(p0, p1, p2, q1, s, r1);
		//Two segment case
		if (nsegs == 2) {
			//Load control points
			controlPoints[1] = q1;
			controlPoints[2] = s;
			controlPoints[3] = r1;
			//Load knot points
			knots[3] = 0.5;
			knots[4] = 0.5;
		}
		//Four segment case
		else {
			controlPoints[4] = s;
			WCVector4 hq1, hs, hr1;
			SplitArc(p0, q1, s, hq1, hs, hr1);
			controlPoints[1] = hq1;
			controlPoints[2] = hs;
			controlPoints[3] = hr1;
			SplitArc(s, r1, p2, hq1, hs, hr1);
			controlPoints[6] = hs;
			controlPoints[5] = hq1;
			controlPoints[7] = hr1;
			//Load the remaining knot points
			for (int i=0; i<2; i++) {
				knots[i+3] = 0.25;
				knots[i+5] = 0.5;
				knots[i+7] = 0.75;
			}
		}
	}
	
	//Create vector of control points
	std::vector<WCVector4> cp;
	for (WPUInt i=0; i<j; i++) cp.push_back(controlPoints[i]);
	//Create vector of knot points
	std::vector<WPFloat> kp;
	for (WPUInt i=0; i<j+3; i++) kp.push_back(knots[i]);
	//Delete the arrays
	delete knots;
	delete controlPoints;
	//Create the curve
	WCNurbsCurve *curve = new WCNurbsCurve(context, 2, cp, WCNurbsMode::Custom(), kp);
	//Return the curve
	return curve;
}


WCNurbsCurve* WCNurbsCurve::Ellipse(WCGeometryContext *context, const WCVector4 &center, const WCVector4 &xUnit, const WCVector4 &yUnit, 
	const WPFloat &major, const WPFloat &minor) {
	//Create vector of control points
	std::vector<WCVector4> cp;
	//Create vector of knot points
	std::vector<WPFloat> kp;
	//Load control and knot point vectors
	WCNurbs::EllipsePoints(center, xUnit, yUnit, major, minor, cp, kp);
	//Create the curve
	WCNurbsCurve *curve = new WCNurbsCurve(context, 2, cp, WCNurbsMode::Custom(), kp);
	return curve;
}


/***********************************************~***************************************************/


std::ostream& __WILDCAT_NAMESPACE__::operator<<(std::ostream& out, const WCNurbsCurve &curve) {
	//Print out basic surface info
	out << "NurbsCurve(" << (void*)&curve << "):\n";
	out << "\t Context: " << curve._context << std::endl;
	out << "\t Vertices:" << curve._lod << std::endl;
	out << "\t Degree(" << curve._degree << ")\n";	
	
	//Only print if collection has items
	if (curve._controlPoints.size() != 0) {
		//Print out control point information
		out << "\t Control Points(" << curve._cp << ")\n";
		for (WPUInt i=0; i<curve._cp; i++) {
			std::cout << "\t\t{" << curve._controlPoints.at(i).I() << ", \t";
			std::cout << curve._controlPoints.at(i).J() << ", \t";
			std::cout << curve._controlPoints.at(i).K() << ", \t";
			std::cout << curve._controlPoints.at(i).L() << "}\n";
		}
	}
	//Only print if knotPoints is not NULL
	if (curve._knotPoints != NULL) {
		//Print out knot point information
		out << "\t Knot Points(" << curve._kp << ")\n\t\t{";
		for (WPUInt i=0; i<curve._kp; i++) {
			std::cout << curve._knotPoints[i];
			if (i != curve._kp-1) std::cout << ", ";
			else std::cout << "}\n";
		}
	}
	return out;
}


/***********************************************~***************************************************/


