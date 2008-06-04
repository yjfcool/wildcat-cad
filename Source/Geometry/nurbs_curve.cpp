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


/*** Local macros ***/
#define ABS(a)	 (a>0?a:-a)


/*** Extern Variables ***/
extern WPFloat* __bezier_coef[8];


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
//	GLfloat data[4 * this->_kp];
	//Copy knot points into array (cast WPFloat to GLfloat)
	for (WPUInt i=0; i<this->_kp; i++) data[i*4] = (GLfloat)this->_knotPoints[i];
	//Set up some parameters
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//Set up texture
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_context->CurveKPTex());	
	glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, this->_kp, 1, GL_RGBA, GL_FLOAT, data);
	//Delete array and check for errors
	if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsCurve::GenerateKnotPointsTexture Error - Texture Setup.\n";
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
//	GLfloat data[this->_cp * 4];
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
	if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsCurve::GenerateControlPointsTexture Error - Texture Setup.\n";
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


void WCNurbsCurve::GenerateCurveHigh(void) {
	//Check for special generation cases, otherwise set the program
	if (this->_degree == 1) return this->GenerateCurveOne();
			
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
	
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffer);	
	glBufferData(GL_ARRAY_BUFFER, (this->_lod+1)*NURBSCURVE_FLOATS_PER_VERTEX*sizeof(GLfloat), 0, GL_STATIC_DRAW);
	//Bind the buffer for transform feedback	
	glBindBufferBaseEXT(GL_TRANSFORM_FEEDBACK_BUFFER_EXT, 0, this->_buffer);	
//	if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsCurve::GenerateCurveHigh Error - Output buffer setup.\n";

	/*** Prepare the Input Buffer ***/
	
	WPFloat u = this->_knotPoints[0];
	WPFloat range = this->_knotPoints[this->_kp-1] - this->_knotPoints[0];
	WPFloat du = range / ((GLfloat)(this->_lod-1));
	int numBatches = (int)ceil( (float)this->_lod / (float)this->_context->CurveVerticesPerBatch() );
	//Create a temporary array (LOD vertices)
	GLfloat *data = new GLfloat[numBatches * 4];
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
	glBufferData(GL_ARRAY_BUFFER, numBatches*4*sizeof(GLfloat), data, GL_STATIC_DRAW);
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
}


void WCNurbsCurve::GenerateCurveMedium(void) {
	//Check for special generation cases, otherwise set the program
	if (this->_degree == 1) return this->GenerateCurveOne();
	if (this->_lod > (WPUInt)this->_context->CurveMaxTextureSize()) {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCNurbsCurve::GenerateCurveMedium - LOD exceeds hardware Maximum Texture Size.");
		return this->GenerateCurveLow();
	}
	
	/*** Setup programs and texture locations ***/		

	//Default and custom case
	switch (this->_degree) {
		case 2: case 3:
			//Degree 2 & 3 Bezier
//			if (this->_mode == WCNurbsMode::Bezier()) {
//				glUseProgram(WCNurbsCurve::_bezier23);
//				glUseProgram(WCNurbsCurve::_default23);
//				glUniform4i(WCNurbsCurve::_locations[NURBSCURVE_LOC_PARAMS_BEZIER23], this->_degree, this->_cp, this->_kp, this->_lod+1);					
//			}
//			//Degree 2 & 3 Default and Custom
//			else {
				glUseProgram(this->_context->CurveDefault23Program());
				glUniform4i(this->_context->CurveLocations()[NURBSCURVE_LOC_PARAMS_DEFAULT23], this->_degree, this->_cp, this->_kp, this->_lod+1);
//			}
			break;
		default:
			//Degree > 3 Default, Custom, and Bezier
			glUseProgram(this->_context->CurveDefaultProgram());
			glUniform4i(this->_context->CurveLocations()[NURBSCURVE_LOC_PARAMS_DEFAULT], this->_degree, this->_cp, this->_kp, this->_lod+1);
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
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::GenerateCurveMedium - Framebuffer is not complete."); return; }
	
	/*** Setup input texture ***/
	
	//Allocate space for the texture (w * h * RGBA)
	GLfloat *data = new GLfloat[this->_lod * 4];
//	GLfloat data[this->_lod * 4];
	WPFloat u = this->_knotPoints[0];
	WPFloat range = this->_knotPoints[this->_kp-1] - this->_knotPoints[0];
	WPFloat du = range / ((GLfloat)(this->_lod-1));	
	//Initialize data in the array
	for (WPUInt i=0; i<this->_lod; i++) {
		data[i*4] =   (GLfloat)u;						//Set first position to u
		data[i*4+1] = (GLfloat)du;						//Set second position to du
		data[i*4+2] = (GLfloat)this->_knotPoints[this->_kp-1];	//Set third position to uMax
		data[i*4+3] = (GLfloat)(i);						//Set fourth position to index		
		//Increment u
		u = STDMIN(u+du, this->_knotPoints[this->_kp-1]);
	}
	//Setup and copy the data into the texture
//	for (int i=0; i<this->_lod; i++) printf("%d: %f %f %f %f\n", i, data[i*4], data[i*4+1], data[i*4+2], data[i*4+3]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_context->CurveInTex());
	glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, this->_lod, 1, GL_RGBA, GL_FLOAT, data);
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
	glViewport(0, 0, this->_context->CurveMaxTextureSize(), 1);
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
	if (glGetError() != GL_NO_ERROR)CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::GenerateCurveMedium - Render.");

	/*** Save output texture into vertex VBO using simple memory read ***/

	GLfloat *vertData = new GLfloat[this->_context->CurveMaxTextureSize() * 4];
//	GLfloat vertData[this->_context->CurveMaxTextureSize() * 4];
	glReadPixels(0, 0, this->_context->CurveMaxTextureSize(), 1, GL_RGBA, GL_FLOAT, vertData);
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffer);
	glBufferData(GL_ARRAY_BUFFER, this->_context->CurveMaxTextureSize() * 4 * sizeof(GLfloat), vertData, GL_STATIC_DRAW);
	delete vertData;

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
/*** Debug ***
	std::cout << "Medium Generation Vertices: " << this->_lod+1 << std::endl;	
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffer);	
	GLfloat *data2 = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	for (int i=0; i<this->_lod; i++) printf("\t%d: %f %f %f %f\n", i, data2[i*4], data2[i*4+1], data2[i*4+2], data2[i*4+3]);
	glUnmapBuffer(GL_ARRAY_BUFFER);	
/*** Debug ***/
	//Clean up and check for errors
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::GenerateCurveMedium - Cleanup.");
}
	

void WCNurbsCurve::GenerateCurveLow(void) {
	//Check for special generation cases, otherwise set the program
	if (this->_degree == 1) return this->GenerateCurveOne();
	//Setup some variables
	WPFloat u = this->_knotPoints[0];
	WPFloat range = this->_knotPoints[this->_kp-1] - this->_knotPoints[0];
	WPFloat du = range / ((GLfloat)(this->_lod-1));
	WCVector4 pt;
	//Create a temporary array (LOD+1 vertices)
	GLfloat *data = new GLfloat[(this->_lod+1)*NURBSCURVE_FLOATS_PER_VERTEX];
	for (WPUInt i=0; i<this->_lod; i++) {
		pt = this->Evaluate(u);
		data[i*4] =   (GLfloat)pt.I();
		data[i*4+1] = (GLfloat)pt.J();
		data[i*4+2] = (GLfloat)pt.K();
		data[i*4+3] = (GLfloat)pt.L();
		//Increment u (up to kp-1)
		u = STDMIN(u+du, this->_knotPoints[this->_kp-1]);				
	}
	//Allocate space for the output vertex data
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffer);	
	glBufferData(GL_ARRAY_BUFFER, this->_lod * NURBSCURVE_FLOATS_PER_VERTEX * sizeof(GLfloat), data, GL_STATIC_DRAW);
	delete data;
/*** Debug ***
	std::cout << "Generate Low Verts: " << this->_lod+1 << std::endl;	
	GLfloat *data2 = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	for (int i=0; i<this->_lod; i++) printf("\t%d: %f %f %f %f\n", i, data2[i*4], data2[i*4+1], data2[i*4+2], data2[i*4+3]);
	glUnmapBuffer(GL_ARRAY_BUFFER);	
/*** Debug ***/
	//Clean up and check for errors
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::GenerateCurveLow - GL error at Cleanup.");	
}


void WCNurbsCurve::GenerateCurveOne(void) {
	//Determine the needed size of the buffer
	GLuint size = this->_cp * 4 * sizeof(GLfloat);
	GLfloat *data = new GLfloat[size / sizeof(GLfloat)];		
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
	//Bind to buffer and copy data in
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffer);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	delete data;
/*** Debug ***
	std::cout << "Degree One Curve Vertices: " << this->_lod << std::endl;	
	GLfloat *data2 = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	for (int i=0; i<this->_lod; i++) printf("\t%d: %f %f %f %f\n", i, data2[i*4], data2[i*4+1], data2[i*4+2], data2[i*4+3]);
	glUnmapBuffer(GL_ARRAY_BUFFER);	
/*** Debug ***/
	//Clean up and check for errors
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::GenerateCurveOne - GL error at Cleanup.");
}


/***********************************************~***************************************************/


WCNurbsCurve::WCNurbsCurve(WCGeometryContext *context, const WPUInt &degree, const std::vector<WCVector4> &controlPoints, 
	const WCNurbsMode &mode, const std::vector<WPFloat> &knotPoints) : ::WCGeometricCurve(), _context(context),
	_degree(degree), _mode(mode), _cp((WPUInt)controlPoints.size()), _kp(0), _controlPoints(controlPoints), _knotPoints(NULL), _buffer(0) {
	
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
	//Check to make sure at least lod = cp
	this->_lod = this->_cp;
	//Define the number of knot points
	this->_kp = this->_degree + this->_cp + 1;
	//Perform some addtional checks if mode is custom
	if (this->_mode == WCNurbsMode::Custom()) {
		//See if the vector is null
		if (knotPoints.size() == 0) { CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::WCNurbsCurve - Invalid knot points collection."); return; }
		//Make sure the collection is the right size
		if (knotPoints.size() != this->_kp) {
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::WCNurbsCurve - Invalid knot points collection size."); return; }
	}

	//Set up buffers
	glGenBuffers(1, &(this->_buffer));
	//Do some more setup only if degree is 1
	if (this->_degree == 1) {
		//Copy the control points into a vbo
		this->GenerateCurveOne();
	//Otherwise, load the knot points
	} else {			
		//Find the rough length of the curve and the number of needed segments
		WPFloat length = WCNurbs::EstimateLength(this->_controlPoints);
		this->_lod = (WPUInt)(length / NURBSCURVE_GENERATE_ACCURACY) + 1;	
		//Use knot mode to setup knot array
		this->LoadKnotPoints(knotPoints);
	}		
	//Establish oriented bounding box
	this->_bounds = new WCAlignedBoundingBox(this->_controlPoints);
}


WCNurbsCurve::WCNurbsCurve(const WCNurbsCurve &curve) :
	::WCGeometricCurve(curve), _context(curve._context), _degree(curve._degree), _mode(curve._mode), _cp(curve._cp), _kp(curve._kp), 
	_controlPoints(curve._controlPoints), _knotPoints(NULL), _buffer(0) {

	//Check to make sure at least lod = cp-1
	this->_lod = curve._lod;	
	//Set up buffers
	glGenBuffers(1, &(this->_buffer));
	//Do some more setup only if degree is 1
	if (this->_degree == 1) {
		//Copy the control points into a vbo
		this->GenerateCurveOne();
	//Otherwise, load the knot points
	} else {			
		//Find the rough length of the curve and the number of needed segments
		WPFloat length = WCNurbs::EstimateLength(this->_controlPoints);
		this->_lod = (WPUInt)(length / NURBSCURVE_GENERATE_ACCURACY) + 1;
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
	_context(NULL), _degree(), _mode(WCNurbsMode::Default()), _cp(), _kp(), _controlPoints(), _knotPoints(), _buffer() {
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
	//Get number of control points
	this->_cp = (WPUInt)WCSerializeableObject::GetFloatAttrib(element, "numCP");
	//Get number of knot points
	this->_kp = (WPUInt)WCSerializeableObject::GetFloatAttrib(element, "numKP");

	//Setup NURBS mode
	this->_mode.FromElement(WCSerializeableObject::ElementFromName(element,"Mode"));

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

	//Set up buffers
	glGenBuffers(1, &(this->_buffer));
	//Do some more setup only if degree is 1
	if (this->_degree == 1) {
		//Copy the control points into a vbo
		this->GenerateCurveOne();
	//Otherwise, estimate length and get lod
	} else {			
		//Find the rough length of the curve and the number of needed segments
		WPFloat length = WCNurbs::EstimateLength(this->_controlPoints);
		this->_lod = (WPUInt)(length / NURBSCURVE_GENERATE_ACCURACY) + 1;	
	}	
	//Establish aligned bounding box
	this->_bounds = new WCAlignedBoundingBox(this->_controlPoints);
	//Mark as dirty
	this->_isVisualDirty = true;
}


WCNurbsCurve::~WCNurbsCurve() {
	//Delete the three buffers
	glDeleteBuffers(1, &(this->_buffer));
	//Delete the knot point array
	if (this->_knotPoints != NULL) delete this->_knotPoints;
}


GLuint WCNurbsCurve::VertexBuffer(void) {
	//See if dirty
	if (this->_isVisualDirty) {
		switch(this->_context->CurvePerformanceLevel()) {
			//Switch on performance level
			case NURBSCURVE_PERFLEVEL_HIGH:		this->GenerateCurveHigh();		break;
			case NURBSCURVE_PERFLEVEL_MEDIUM:	this->GenerateCurveMedium();	break;
			case NURBSCURVE_PERFLEVEL_LOW:		this->GenerateCurveLow();		break;
		}
		//Mark as clean
		this->_isVisualDirty = false;		
	}
	//Return the buffer
	return this->_buffer;
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
	this->_isVisualDirty = true;
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
	this->_isVisualDirty = true;
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
		//Find the rough length of the curve and the number of needed segments
		WPFloat length = WCNurbs::EstimateLength(this->_controlPoints);
		this->_lod = (WPUInt)(length / NURBSCURVE_GENERATE_ACCURACY) + 1;	
		//Use knot mode to setup knot array
		this->LoadKnotPoints();
	}
	//Mark the object as dirty
	this->_isVisualDirty = true;
	//Return the degree value
	return this->_degree;
}


WPUInt WCNurbsCurve::LevelOfDetail(const WPUInt &lod) {
	WPUInt levelOfDetail = lod;
	//Only adjust LOD for degree > 1
	if (this->_degree == 1) return this->_lod;
	//Check to make sure LOD is valid
	if (levelOfDetail < this->_cp) {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCNurbsCurve::LevelOfDetail - LOD(" << lod << ") must be at least #CP.");
		levelOfDetail = this->_cp;	
	 }
	 if (levelOfDetail > NURBSCURVE_MAX_LOD) {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCNurbsCurve::LevelOfDetail - LOD(" << lod << ") out of bounds.");
		levelOfDetail = NURBSCURVE_MAX_LOD;
	}
	//Set the new lod
	this->_lod = levelOfDetail;	
	//Mark the object as dirty
	this->_isVisualDirty = true;
	//Return the level of detail
	return this->_lod;
}


WCVisualObject* WCNurbsCurve::HitTest(const WCRay &ray, const WPFloat &tolerance) {
//	int i;
	//Only do hit check on cp if they are visible
//	if (this->_drawCP) {
		//Clear selection for all points
//		for (i=0; i<this->_cpCollection.Size(); i++) this->_cpCollection.At(i)->IsSelected(false);
		//Check all control points
//		for (i=0; i<this->_cpCollection.Size(); i++) {
//			if (this->_cpCollection.At(i)->HitTest(ray, tolerance)) return this->_cpCollection.At(i);
//		}
//	}
	return NULL;
}


void WCNurbsCurve::ApplyTransform(const WCMatrix4 &transform) {
	//Loop through all control points and apply transform
	for (WPUInt i=0; i<this->_cp; i++)
		this->_controlPoints.at(i) = transform * this->_controlPoints.at(i);
	//Make sure curve is regenerated
	this->_isVisualDirty = true;
}


void WCNurbsCurve::ApplyTranslation(const WCVector4 &translation) {
	//Loop through all control points and apply translation
	for (WPUInt i=0; i<this->_cp; i++)
		this->_controlPoints.at(i) = this->_controlPoints.at(i) + translation;
	//Make sure curve is regenerated
	this->_isVisualDirty = true;
}


void WCNurbsCurve::Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom) {
	//Do nothing if not visible
	if (!this->_isVisible) return;
	//Check to see if curve needs to be generated
	if (this->_isVisualDirty) {
		switch(this->_context->CurvePerformanceLevel()) {
			//Switch on performance level
			case NURBSCURVE_PERFLEVEL_HIGH:		this->GenerateCurveHigh();		break;
			case NURBSCURVE_PERFLEVEL_MEDIUM:	this->GenerateCurveMedium();	break;
			case NURBSCURVE_PERFLEVEL_LOW:		this->GenerateCurveLow();		break;
		}
		//Mark as clean
		this->_isVisualDirty = false;	
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
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::Render - Unspecified GL error at cleanup.");
}


void WCNurbsCurve::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
	//Something has changed in the control points - mark as dirty
	this->_isVisualDirty = true;
	//Check on degree of curve
	if (this->_degree != 1) {
		//Update LOD estimate incase length changed
		WPFloat length = WCNurbs::EstimateLength(this->_controlPoints);
		this->_lod = (WPUInt)(length / NURBSCURVE_GENERATE_ACCURACY) + 1;
	}
	//Make sure all dependent objects know about it
	this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);
}


std::list<WPFloat> WCNurbsCurve::Intersect(WCGeometricPoint *point, const WPFloat &tolerance) {
	//Return list
	std::list<WPFloat> hitList;
	//Map the vertex buffer
	WPUInt index=4;
	WPFloat paraFactor = 1.0 / (this->_lod - 1);
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffer);
	GLfloat *data = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	if (data == NULL) {
		//Unbind and map buffer
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::Intersect Point - Curve has NULL buffer.");
		return std::list<WPFloat>();
	}

	//Initialize the p0 vector
	WCVector4 p0((WPFloat)data[0], (WPFloat)data[1], (WPFloat)data[2], (WPFloat)data[3]), p1, direction, tmpVec;
	WPFloat u, dist;
	//Loop through remaining vertices and test each
	for (WPUInt i=1; i<this->_lod; i++) {
		//Determine p1
		p1.Set( (WPFloat)data[index], (WPFloat)data[index+1], (WPFloat)data[index+2], (WPFloat)data[index+3] );
		//See if there is an intersection between line and point segment
		direction = p1 - p0;
		dist = p0.Distance(p1);
		//Get point vector
		tmpVec = point->Data() - p0;
		u = direction.DotProduct(tmpVec) / (dist * dist);
		//Bounds check u
		if ((u > -tolerance) && (u < (1.0 + tolerance))) {
			//Bound u
			u = STDMAX( STDMIN(u, 1.0), 0.0);
			//Get point on segment
			tmpVec = p0 + direction * u;
			//Get distance from point on segment to point
			 dist = tmpVec.Distance(point->Data());
			 //See if within tolerance
			 if (dist <= tolerance) {
				 //Convert u from segment to curve
				u = (i - 1 + u) * paraFactor;
//				std::cout << "Final U: " << u << std::endl;
				//Insert into list
				hitList.push_back(u);
			}
		}
		//Move p1 to p0
		p0 = p1;
		index += 4;
	}
	//Unbind and map buffer
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Check for GL errors
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::Intersect Point - GL error at Cleanup.");
	//Return list of hits
	return hitList;
}


std::list<WPIntersectRec> WCNurbsCurve::Intersect(WCGeometricCurve *curve, const WPFloat &tolerance) {
	WPUInt retVal;
	WPFloat uValue, vValue;
	WCVector4 value;
	WPIntersectRec rec;
	std::list<WPIntersectRec> hitList;
	
	//Try casting to a line
	WCGeometricLine *line = dynamic_cast<WCGeometricLine*>(curve);
	//Check for line-line intersection
	if (line != NULL) {
//		std::cout << "Curve-Line Intersect\n";
		//Evaluate both lines at both ends
		WCVector4 p3 = line->Evaluate(0.0);
		WCVector4 p4 = line->Evaluate(1.0);
		//Check to see if line intersects nurbs bounding box
		//...
		
		//Map the curve buffer and get number of vertices
		WPUInt index=4;
		WPFloat paraFactor = 1.0 / (this->_lod - 1);
		glBindBuffer(GL_ARRAY_BUFFER, this->_buffer);
		GLfloat *data = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
		if (data == NULL) {
			//Unbind and map buffer
			glUnmapBuffer(GL_ARRAY_BUFFER);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::Intersect - Curve has NULL buffer.");
			return std::list<WPIntersectRec>();
		}

		//Initialize the first vector
		WCVector4 firstVec((WPFloat)data[0], (WPFloat)data[1], (WPFloat)data[2], (WPFloat)data[3]), secVec;
		//Loop through remaining vertices and test each
		for (WPUInt i=1; i<this->_lod; i++) {
			//Determine secVec
			secVec.Set( (WPFloat)data[index], (WPFloat)data[index+1], (WPFloat)data[index+2], (WPFloat)data[index+3] );	
			//See if there is an intersection between line and curve segment
			retVal = LineLineIntersection(firstVec, secVec, p3, p4, false, tolerance, value, uValue, vValue);
			//Intersection point found
			if (retVal == INTERSECTION_INTERSECTION) {
//				std::cout << "Curve-Line Intersect\n";
//				std::cout << "Hit found at: " << value << std::endl;
//				std::cout << "U-Value: " << uValue << ", V-Value: " << vValue << std::endl;
				//Modify uValue depending on i
				uValue = (i - 1 + uValue) * paraFactor;
				//Create hit record
				rec = std::make_pair( value, std::make_pair(uValue, vValue) );
				//Add to hit list
				hitList.push_back(rec);
			}
			//Move secVec to firstVec
			firstVec = secVec;
			index += 4;
		}
		//Unbind and map buffer
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		//Check for GL errors
		if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::Intersect - GL error at Cleanup.");
		//Return list of hits
		return hitList;
	}
	else {
		//Try casting to a nurb
		WCNurbsCurve *nurbs = dynamic_cast<WCNurbsCurve*>(curve);
		//Double check that curve is NURBS
		if (nurbs == NULL) {
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::Intersect - Curve is of unknown type.");
			return std::list<WPIntersectRec>();
		}
		//Map curve A buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_buffer);
		GLfloat *dataA = (GLfloat*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_ONLY);
		//Map curve B buffer and get number of vertices
		GLuint buffer = nurbs->VertexBuffer();
		WPUInt numVerts = nurbs->LevelOfDetail();
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		GLfloat *dataB = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
		if ((dataA == NULL) || (dataB == NULL)) {
		//Unbind and map buffers
		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometricLine::Intersect - Curve has NULL buffer.");
			return std::list<WPIntersectRec>();
		}

		WPFloat paraFactorA = 1.0 / (this->_lod - 1);
		WPFloat paraFactorB = 1.0 / (numVerts - 1);
		WCVector4 c1First, c1Second, c2First, c2Second;
		WPUInt indexA=4, indexB;
		c1First.Set((WPFloat)dataA[0], (WPFloat)dataA[1], (WPFloat)dataA[2], (WPFloat)dataA[3]);
		//Loop through first curve segments
		for (WPUInt i1=1; i1<this->_lod; i1++) {
			//Set c1 second vector
			c1Second.Set((WPFloat)dataA[indexA], (WPFloat)dataA[indexA+1], (WPFloat)dataA[indexA+2], (WPFloat)dataA[indexA+3]);
			//Set the first vector for curve 2
			c2First.Set((WPFloat)dataB[0], (WPFloat)dataB[1], (WPFloat)dataB[2], (WPFloat)dataB[3]);
			indexB = 4;
			//Loop through second curve segments
			for (WPUInt i2=1; i2<numVerts; i2++) {
				//Set c2 second vector
				c2Second.Set((WPFloat)dataB[indexB], (WPFloat)dataB[indexB+1], (WPFloat)dataB[indexB+2], (WPFloat)dataB[indexB+3]);
				//See if there is an intersection between c1 and c2 segment
				retVal = LineLineIntersection(c1First, c1Second, c2First, c2Second, false, tolerance, value, uValue, vValue);
				//Intersection point found
				if (retVal == INTERSECTION_INTERSECTION) {
//					std::cout << "Curve-Curve Intersect\n";
//					std::cout << "Hit found at: " << value << std::endl;
//					std::cout << "U-Value: " << uValue << ", V-Value: " << vValue << std::endl;
					//Modify uValue and vValue depending on i1 and i2
					uValue = (i1 - 1 + uValue) * paraFactorA;
					vValue = (i2 - 1 + vValue) * paraFactorB;
					//Create hit record
					rec = std::make_pair( value, std::make_pair(uValue, vValue) );
					//Add to hit list
					hitList.push_back(rec);
				}
				//Move c2 second to first
				c2First = c2Second;
				indexB += 4;			
			}
			//Move c1 second to first
			c1First = c1Second;
			indexA += 4;
		}
		
		//Unbind and map buffers
		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		//Check for GL errors
		if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::Intersect - GL error at Cleanup.");
		//Return list of hits
		return hitList;
	}	
}


WPFloat WCNurbsCurve::Length(const WPFloat &tolerance) {
	//Initialize length variable
	WPFloat length = 0;
	//Degree = 1 special case
	if (this->_degree == 1) {
		length = WCNurbs::EstimateLength(this->_controlPoints);
		return length;
	}
	//Check to see if curve needs to be generated
	if (this->_isVisualDirty) {
		//Generate the curve - only if degree is not 1
		switch(this->_context->CurvePerformanceLevel()) {
			//Switch on performance level
			case NURBSCURVE_PERFLEVEL_HIGH:		this->GenerateCurveHigh();		break;
			case NURBSCURVE_PERFLEVEL_MEDIUM:	this->GenerateCurveMedium();	break;
			case NURBSCURVE_PERFLEVEL_LOW:		this->GenerateCurveLow();		break;						
		}
		//Mark as clean
		this->_isVisualDirty = false;		
	}
	//Bind and map the vertex array
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffer);
	GLfloat *verts = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	//Set up points
	WCVector4 p0(verts[0], verts[1], verts[2], verts[3]), p1;
	//Loop through the vertex data
	for (WPUInt i=1; i<this->_lod; i++) {
		p1 = WCVector4(verts[i*4], verts[i*4+1], verts[i*4+2], verts[i*4+3]);
		length += (p1 - p0).Magnitude();
		p0 = p1;
	}
	//Make sure to unmap the vertex data
	glUnmapBuffer(GL_ARRAY_BUFFER);
	return length;
}


WCVector4 WCNurbsCurve::Evaluate(const WPFloat &u) {
	WCVector4 c;
	WPFloat eval = u;
	//Bounds check the u value
	if (eval < 0.0) eval = 0.0;
	if (eval > 1.0) eval = 1.0;
	
	//Degree 1 case
	if (this->_degree == 1) {
		WCVector4 c = (this->_controlPoints.at(0) * eval) + (this->_controlPoints.at(1) * (1.0 - u));
		return c;
	}
	
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


WCVector4 WCNurbsCurve::PointInversion(const WCVector4 &point, const WPFloat &u) {
	WPFloat localU = u;
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
				localU = t;
			}
		}
		//Check to make sure some reasonable index was found
		if (index == -1) return WCVector4();
		//Now find the point on the line segment and return it
		WCVector4 p = this->_controlPoints.at(index) + (this->_controlPoints.at(index+1) - this->_controlPoints.at(index)) * localU;
		return p;
	}

	//Check to see if curve needs to be generated
	if (this->_isVisualDirty) switch (this->_context->CurvePerformanceLevel()) {
		//Switch on performance level
		case NURBSCURVE_PERFLEVEL_HIGH:		this->GenerateCurveHigh(); break;
		case NURBSCURVE_PERFLEVEL_MEDIUM:	this->GenerateCurveMedium(); break;
		case NURBSCURVE_PERFLEVEL_LOW:		this->GenerateCurveLow(); break;				
		//Mark as clean
		this->_isVisualDirty = false;		
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
	localU = index * this->_knotPoints[this->_kp-1] / (this->_lod+1);
	
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
		index = WCNurbs::FindSpan(this->_cp, this->_degree, localU, this->_knotPoints);
		basisValues = WCNurbs::BasisValues(index, localU, this->_degree, this->_knotPoints, 2);
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
		if (dist < NURBSCURVE_EPSILON_ONE) return c;
		
		//Do some interim calculations
		top = cDer.DotProduct(vDist);
		bottom = cDer.Magnitude();
		/*** Check condition 2 ***/
		if (( ABS(top) / (bottom*dist) ) <= NURBSCURVE_EPSILON_TWO) return c;
		
		//Calculate next u
		bottom = c2Der.DotProduct(vDist) + (bottom * bottom);
		uNx = localU - (top / bottom);
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
		top = (cDer * (uNx - localU)).Magnitude();
		if (top <= NURBSCURVE_EPSILON_ONE) return c;

		//Swap the u value to the next u value
		localU = uNx;
		//Increment max loop counter
		j++;
	}
	//Return the point on the curve
	return c;		
}


void WCNurbsCurve::InsertKnot(const WPFloat &u, const WPUInt &multiplicity) {
	WPFloat eval = u;
	//Check u values
	if (eval < 0.0) { CLOGGER_WARN(WCLogManager::RootLogger(), "WCNurbsCurve::InsertKnot - U out of bounds."); eval = 0.0; }
	//New number of knot points and control points
	WPUInt kp = this->_kp + multiplicity;
	WPUInt cp = this->_cp + multiplicity;
	//Allocate space for the new knot array
	WPFloat alpha, *UQ = new WPFloat[kp];
	WCVector4 *Qw = new WCVector4[cp];
	WCVector4 *Rw = new WCVector4[this->_degree+1];
	if ((UQ == NULL) || (Qw == NULL) || (Rw == NULL)) { 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::InsertKnot - No memory for new vectors."); return; }	
	WPUInt span, s, L;
	//Find the span index of the new knot	
	WCNurbs::FindSpanMultiplicity(this->_cp, this->_degree, eval, this->_knotPoints, span, s);
	
	//Load new knot vector
	for (WPUInt i=0; i<=span; i++) UQ[i] = this->_knotPoints[i];
	for (WPUInt i=1; i<=multiplicity; i++)	UQ[span+i] = eval;
	for (WPUInt i=span+1; i<this->_kp; i++) UQ[i+multiplicity] = this->_knotPoints[i];
	//Load unaltered control points
	for (WPUInt i=0; i<=span-this->_degree; i++) Qw[i] = this->_controlPoints.at(i);
	for (WPUInt i=span-s; i<this->_cp; i++) Qw[i+multiplicity] = this->_controlPoints.at(i);
	for (WPUInt i=0; i<=this->_degree-s; i++) Rw[i] = this->_controlPoints.at(span-this->_degree+i);
	for (WPUInt j=1; j<=multiplicity; j++) {
		L = span - this->_degree + j;
		for (WPUInt i=0; i<this->_degree-j-s; i++) {
			alpha = (u - this->_knotPoints[L+i]) / (this->_knotPoints[i+span+1] - this->_knotPoints[L-i]);
			Rw[i] = Rw[i+1] * alpha + Rw[i] * (1.0 - alpha);
		}
		Qw[L] = Rw[0];
		Qw[span+multiplicity-j-s] = Rw[this->_degree-j-s];
	}
	for (WPUInt i=L+1; i<span-s; i++) Qw[i] = Rw[i-L];

	//Save new knot points
	delete this->_knotPoints;
	this->_knotPoints = UQ;
//	WCCollection<WCControlPoint*> cpCollection;
	std::vector<WCVector4> controlPoints;
	//Release unused old control points
	for (WPUInt i=0; i<cp; i++) {
		//...
	}
	this->_controlPoints = controlPoints;
	
	//Set the remaining parameters
	this->_cp = cp;
	this->_kp = kp;
	this->_mode = WCNurbsMode::Custom();
	//Do some more setup only if degree is 1
	if (this->_degree == 1) {
		//Copy the control points into a vbo
		this->GenerateCurveOne();
	//Otherwise, load the knot points
	} else {			
		//Find the rough length of the curve and the number of needed segments
		WPFloat length = WCNurbs::EstimateLength(this->_controlPoints);
		this->_lod = (WPUInt)(length / NURBSCURVE_GENERATE_ACCURACY) + 1;	
	}	
	this->_isVisualDirty = true;

	//Delete the arrays
	delete UQ;
	delete Qw;
	delete Rw;
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
	CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::operator= - Not yet implemented.");
	//Clear the existing control point collection
//	for (int i=0; i<this->_cp; i++) this->_cpCollection.At(i)->Release(*this);
	this->_controlPoints = curve._controlPoints;
	//Free the current knotpoints and allocate space for new ones
	if (this->_knotPoints != NULL) delete this->_knotPoints;
	this->_knotPoints = new WPFloat[this->_kp];
	//Copy the knot points
	memcpy(this->_knotPoints, curve._knotPoints, this->_kp * sizeof(WPFloat));
	//Copy all of the parameters
	this->_degree = curve._degree;
	this->_mode = curve._mode;
	this->_cp = curve._cp;
	this->_kp = curve._kp;
	this->_lod = curve._lod;
	//Copy the control points into a vbo
	if (this->_degree == 1)
		this->GenerateCurveOne();
	//Mark as dirty
	this->_isVisualDirty = true;
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


