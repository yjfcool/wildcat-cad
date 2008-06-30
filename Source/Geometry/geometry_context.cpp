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
#include "Geometry/geometry_context.h"
#include "Geometry/nurbs_curve.h"
#include "Geometry/nurbs_surface.h"
#include "Geometry/trimmed_nurbs_surface.h"


/***********************************************~***************************************************/


void WCGeometryContext::StartCurve(void) {
	GLenum retVal;
	//Determine performance level
//	if (WCAdapter::HasGLEXTTransformFeedback() && WCAdapter::HasGLEXTBindableUniform() && 
//		WCAdapter::HasGLEXTGeometryShader4() && false) {//WCAdapter::HasGLEXTGPUShader4()
//		//All criteria met for high performance
//		this->_ncPerfLevel = NURBSCURVE_PERFLEVEL_HIGH;
//		CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCGeometryContext::StartCurve - Performance set to High.");			
//	}	
//	else
	if (WCAdapter::HasGLARBFragmentShader() && WCAdapter::HasGLARBShadingLanguage100() &&
		WCAdapter::HasGLARBTextureRectangle() && WCAdapter::HasGLEXTTextureFloat() &&
		WCAdapter::HasGLARBPixelBufferObject() && WCAdapter::HasGLEXTFramebufferObject()) {
		//All criteria met for medium performance
		this->_ncPerfLevel = NURBSCURVE_PERFLEVEL_MEDIUM;
//		CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCGeometryContext::StartCurve - Performance set to Medium.");
	}
	//Otherwise, remain at low level
	else {
		this->_ncPerfLevel = NURBSCURVE_PERFLEVEL_LOW;
//		CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCGeometryContext::StartCurve - Performance set to low.");
	}
/*** Debug ***/	
	//Force Performance Level
//	this->_ncPerfLevel = NURBSCURVE_PERFLEVEL_HIGH;
//	this->_ncPerfLevel = NURBSCURVE_PERFLEVEL_MEDIUM;
//	this->_ncPerfLevel = NURBSCURVE_PERFLEVEL_LOW;
/*** Debug ***/
	//Compile Shaders
	switch(this->_ncPerfLevel) {
		//Set up programs for high performance mode
		case NURBSCURVE_PERFLEVEL_HIGH:
#ifdef GL_EXT_transform_feedback
			CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCGeometryContext::StartCurve - Actually Running as High");
			//Get program IDs
			this->_ncDefault = this->_shaderManager->ProgramFromName("nc_default_plH");
			this->_ncDefault23 = 0;
			this->_ncBezier23 = this->_shaderManager->ProgramFromName("nc23_bezier_plH");				
			//Determine number of vertices per batch
			this->_ncVertsPerBatch = STDMIN(NURBSCURVE_MAX_VERT_PER_BATCH, WCAdapter::GetMaxGeometryOutputVertices());
			//Initialize generation buffers
			glGenBuffers(1, &(this->_ncCPBuffer));
			glBindBuffer(GL_ARRAY_BUFFER, this->_ncCPBuffer);
			glBufferData(GL_ARRAY_BUFFER, NURBSCURVE_MAX_CONTROLPOINTS*4*sizeof(GLfloat), NULL, GL_STATIC_DRAW);
			glGenBuffers(1, &(this->_ncKPBuffer));
			glBindBuffer(GL_ARRAY_BUFFER, this->_ncKPBuffer);
			glBufferData(GL_ARRAY_BUFFER, NURBSCURVE_MAX_KNOTPOINTS*4*sizeof(GLfloat), NULL, GL_STATIC_DRAW);
#endif // GL_EXT_transform_feedback
			if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometryContext::StartCurve - GL error at High Creation.");
			break;

		//Set up programs for medium performance mode	
		case NURBSCURVE_PERFLEVEL_MEDIUM:
			CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCNurbsCurve::Start Debug - Actually Running as Medium");
			//Get program IDs
			this->_ncDefault = this->_shaderManager->ProgramFromName("nc_default_plM");
			this->_ncDefault23 = this->_shaderManager->ProgramFromName("nc23_default_plM");
			this->_ncBezier23 = this->_shaderManager->ProgramFromName("nc23_bezier_plM");
			//Set up generation static
			this->_ncMaxTexSize = STDMIN(WCAdapter::GetMax2DTextureSize(), (GLint)NURBSCURVE_MAX_LOD);
			//Set up control point texture
			glGenTextures(1, &(this->_ncCPTex));
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_ncCPTex);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);		
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);					
			glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA32F_ARB, this->_ncMaxTexSize, 1, 0, GL_RGBA, GL_FLOAT, NULL);
			//Setup knot point texture
			glGenTextures(1, &(this->_ncKPTex));	
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_ncKPTex);	
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);		
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);					
			glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA32F_ARB, this->_ncMaxTexSize, 1, 0, GL_RGBA, GL_FLOAT, NULL);
			//Setup input texture
			glGenTextures(1, &(this->_ncInTex));
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_ncInTex);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);		
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);					
			glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA32F_ARB, this->_ncMaxTexSize, 1, 0, GL_RGBA, GL_FLOAT, NULL);
			//Setup output texture				
			glGenTextures(1, &(this->_ncOutTex));
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_ncOutTex);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);		
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);					
			glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA32F_ARB, this->_ncMaxTexSize, 1, 0, GL_RGBA, GL_FLOAT, NULL);
			//Setup the framebuffer object
			glGenFramebuffersEXT(1, &(this->_ncFramebuffer));
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->_ncFramebuffer);
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, this->_ncOutTex, 0);
			retVal = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
			if (retVal != GL_FRAMEBUFFER_COMPLETE_EXT)
				CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometryContext::StartCurve - Framebuffer is not complete.");				
			//Clean up a bit
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
			if (glGetError() != GL_NO_ERROR) 
				CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometryContext::StartCurve - GL error at Medium Creation.");
			break;

		//Set up low performance parameters	
		default:
			CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCGeometryContext::StartCurve Debug - Actually Running as Low.");
			//Nothing to do here for now
			break;
	}

	//Setup locations for all programs
	if (this->_ncPerfLevel != NURBSCURVE_PERFLEVEL_LOW) {		
		//Allocate space for locations
		this->_ncLocations = new GLint[9];
		//Set the locations of each control point variable
		this->_ncLocations[NURBSCURVE_LOC_CP_DEFAULT] = glGetUniformLocation(this->_ncDefault, "controlPoints");
		this->_ncLocations[NURBSCURVE_LOC_CP_DEFAULT23] = glGetUniformLocation(this->_ncDefault23, "controlPoints");
		this->_ncLocations[NURBSCURVE_LOC_CP_BEZIER23] = glGetUniformLocation(this->_ncBezier23, "controlPoints");
		//Set the location of each knot point variable
		this->_ncLocations[NURBSCURVE_LOC_KP_DEFAULT] = glGetUniformLocation(this->_ncDefault, "knotPoints");
		this->_ncLocations[NURBSCURVE_LOC_KP_DEFAULT23] = glGetUniformLocation(this->_ncDefault23, "knotPoints");
		this->_ncLocations[NURBSCURVE_LOC_KP_BEZIER23] = glGetUniformLocation(this->_ncBezier23, "knotPoints");
		//Set the location of each params variable
		this->_ncLocations[NURBSCURVE_LOC_PARAMS_DEFAULT] = glGetUniformLocation(this->_ncDefault, "numParams");
		this->_ncLocations[NURBSCURVE_LOC_PARAMS_DEFAULT23] = glGetUniformLocation(this->_ncDefault23, "numParams");
		this->_ncLocations[NURBSCURVE_LOC_PARAMS_BEZIER23] = glGetUniformLocation(this->_ncBezier23, "numParams");
		if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometryContext::StartCurve - GL error at Location Finding.");
	}
	//Set up remaining HIGH performance parameters
	if (this->_ncPerfLevel == NURBSCURVE_PERFLEVEL_HIGH) {
#ifdef GL_EXT_transform_feedback
		//Determine min buffer sizes for bindable uniform arrays
		this->_ncMinKPBufferSize = glGetUniformBufferSizeEXT(this->_ncDefault, this->_ncLocations[NURBSCURVE_LOC_KP_DEFAULT]);
		this->_ncMinCPBufferSize = glGetUniformBufferSizeEXT(this->_ncDefault, this->_ncLocations[NURBSCURVE_LOC_CP_DEFAULT]);	
		if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometryContext::StartCurve - At High Min Buffer Sizing.");			
		//Bind the bindable uniforms
		glUniformBufferEXT(this->_ncDefault,  this->_ncLocations[NURBSCURVE_LOC_CP_DEFAULT],  this->_ncCPBuffer);	
		glUniformBufferEXT(this->_ncDefault,  this->_ncLocations[NURBSCURVE_LOC_KP_DEFAULT],  this->_ncKPBuffer);				
		glUniformBufferEXT(this->_ncBezier23, this->_ncLocations[NURBSCURVE_LOC_CP_BEZIER23], this->_ncCPBuffer);	
		glUniformBufferEXT(this->_ncBezier23, this->_ncLocations[NURBSCURVE_LOC_KP_BEZIER23], this->_ncKPBuffer);
#endif // GL_EXT_transform_feedback
		if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometryContext::StartCurve - At High Uniform Bindable Binding.");			
	}
	//Set up remaining MEDIUM performance parameters
	else if (this->_ncPerfLevel == NURBSCURVE_PERFLEVEL_MEDIUM) {
		//Set uniform values for Default program
		glUseProgram(this->_ncDefault);
		glUniform1i(glGetUniformLocation(this->_ncDefault, "verts"), 0);
		glUniform1i(this->_ncLocations[NURBSCURVE_LOC_CP_DEFAULT], 1);
		glUniform1i(this->_ncLocations[NURBSCURVE_LOC_KP_DEFAULT], 2);
		//Set uniform values for Default23 program
		glUseProgram(this->_ncDefault23);
		glUniform1i(glGetUniformLocation(this->_ncDefault23, "verts"), 0);
		glUniform1i(this->_ncLocations[NURBSCURVE_LOC_CP_DEFAULT23], 1);
		glUniform1i(this->_ncLocations[NURBSCURVE_LOC_KP_DEFAULT23], 2);
		//Set uniform values for Bezier23 program
		glUseProgram(this->_ncBezier23);
		glUniform1i(glGetUniformLocation(this->_ncBezier23, "verts"), 0);
		glUniform1i(this->_ncLocations[NURBSCURVE_LOC_CP_BEZIER23], 1);
		glUniform1i(this->_ncLocations[NURBSCURVE_LOC_KP_BEZIER23], 2);
		//Stop using any programs
		glUseProgram(0);		
	}
}


void WCGeometryContext::StopCurve(void) {
	//Delete curve generation programs
	glDeleteProgram(this->_ncDefault);
	glDeleteProgram(this->_ncDefault23);		
	glDeleteProgram(this->_ncBezier23);
	//Delete the curve program locations
	if (this->_ncLocations != NULL) delete this->_ncLocations;
	//Depending on PerfMode...
	if (this->_ncPerfLevel == NURBSCURVE_PERFLEVEL_HIGH) {
		//Delete CP and KP uniform buffers
		glDeleteBuffers(1, &(this->_ncCPBuffer));
		glDeleteBuffers(1, &(this->_ncKPBuffer));
	}
	else if(this->_ncPerfLevel == NURBSCURVE_PERFLEVEL_MEDIUM) {
		//Delete CP and KP textures
		glDeleteTextures(1, &(this->_ncCPTex));
		glDeleteTextures(1, &(this->_ncKPTex));
		glDeleteTextures(1, &(this->_ncInTex));
		glDeleteTextures(1, &(this->_ncOutTex));
		//Delete framebuffer
		glDeleteFramebuffersEXT(1, &(this->_ncFramebuffer));
	}
	//Check for errors
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometryContext::StopCurve - Unspecified error.");
}


/***********************************************~***************************************************/


void WCGeometryContext::StartSurface(void) {
	GLenum retVal;
	//Determine performance level
	if (WCAdapter::HasGLEXTTransformFeedback() && WCAdapter::HasGLEXTBindableUniform() &&
		WCAdapter::HasGLEXTGeometryShader4() ) {//WCAdapter::HasGLEXTGPUShader4()
		//All criteria met for high performance
		this->_nsPerfLevel = NURBSSURFACE_PERFLEVEL_HIGH;
//		CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCGeometryContext::StartSurface - Performance set to High.");
	}
	else if (WCAdapter::HasGLARBFragmentShader() && WCAdapter::HasGLARBShadingLanguage100() &&
			 WCAdapter::HasGLARBTextureRectangle() && WCAdapter::HasGLEXTTextureFloat() &&
			 WCAdapter::HasGLARBPixelBufferObject() && WCAdapter::HasGLEXTFramebufferObject()) {
		//All criteria met for medium performance
		this->_nsPerfLevel = NURBSSURFACE_PERFLEVEL_MEDIUM;
//		CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCGeometryContext::StartSurface - Performance set to Medium.");
	}
	//Otherwise, remain at low level
	else {
		this->_nsPerfLevel = NURBSSURFACE_PERFLEVEL_LOW;
//		CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCGeometryContext::StartSurface - Performance set to Low.");
	}
/*** Debug ***/
	//Force Performance Level
//		this->_nsPerfLevel = NURBSSURFACE_PERFLEVEL_HIGH;
//		this->_nsPerfLevel = NURBSSURFACE_PERFLEVEL_MEDIUM;
		this->_nsPerfLevel = NURBSSURFACE_PERFLEVEL_LOW;
/*** Debug ***/
	//Perform initialization based on performance level
	switch(this->_nsPerfLevel) {
		//Set up programs for high performance mode
		case NURBSSURFACE_PERFLEVEL_HIGH:
#ifdef GL_EXT_transform_feedback
//			CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCGeometryContext::StartSurface - Actually Running as High.");
			//Setup high generation programs
			this->_nsDefault = this->_shaderManager->ProgramFromName("ns_default_plH");
			this->_nsBezier23 = this->_shaderManager->ProgramFromName("ns23_bezier_plH");
			//Determine number of vertices per batch
			this->_nsVertsPerBatch = STDMIN(NURBSSURFACE_MAX_VERT_PER_BATCH, WCAdapter::GetMaxGeometryOutputVertices());
			//Initialize generation buffers
			glGenBuffers(1, &(this->_nsCPBuffer));
			glBindBuffer(GL_ARRAY_BUFFER, this->_nsCPBuffer);
			glBufferData(GL_ARRAY_BUFFER, NURBSSURFACE_MAX_CONTROLPOINTS*4*sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
			glGenBuffers(1, &(this->_nsKPUBuffer));
			glBindBuffer(GL_ARRAY_BUFFER, this->_nsKPUBuffer);
			glBufferData(GL_ARRAY_BUFFER, NURBSSURFACE_MAX_KNOTPOINTS*4*sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
			glGenBuffers(1, &(this->_nsKPVBuffer));
			glBindBuffer(GL_ARRAY_BUFFER, this->_nsKPVBuffer);
			glBufferData(GL_ARRAY_BUFFER, NURBSSURFACE_MAX_KNOTPOINTS*4*sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
			//Clean up a bit
			glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif // GL_EXT_transform_feedback
			//Check for errors
			if (glGetError() != GL_NO_ERROR)
				CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometryContext::StartSurface - At High Creation.");
			break;

		case NURBSSURFACE_PERFLEVEL_MEDIUM:
//			CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCGeometryContext::StartSurface - Actually Running as Medium.");
			//Setup medium generation programs
			this->_nsDefault = this->_shaderManager->ProgramFromName("ns_default_plM");
			this->_nsDefault23 = this->_shaderManager->ProgramFromName("ns23_default_plM");				
			this->_nsBezier23 = this->_shaderManager->ProgramFromName("ns23_bezier_plM");				
			//Determine maximum texture size
			this->_nsMaxTexSize = STDMIN(WCAdapter::GetMax2DTextureSize(), (GLint)NURBSSURFACE_MAX_TEXSIZE);
			//Check for errors
			if (glGetError() != GL_NO_ERROR) 
				CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometryContext::StartSurface - At Medium Program Compilation.");
			//Set up cp texture
			glGenTextures(1, &(this->_nsCPTex));
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_nsCPTex);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA32F_ARB, NURBSSURFACE_MAX_CONTROLPOINTS, NURBSSURFACE_MAX_CONTROLPOINTS, 0, GL_RGBA, GL_FLOAT, NULL);
			//Set up kpU texture
			glGenTextures(1, &(this->_nsKPUTex));
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_nsKPUTex);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA32F_ARB, NURBSSURFACE_MAX_KNOTPOINTS, 1, 0, GL_RGBA, GL_FLOAT, NULL);
			//Set up kpV texture
			glGenTextures(1, &(this->_nsKPVTex));
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_nsKPVTex);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA32F_ARB, NURBSSURFACE_MAX_KNOTPOINTS, 1, 0, GL_RGBA, GL_FLOAT, NULL);
			//Set up main input texture
			glGenTextures(1, &(this->_nsInTex));
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_nsInTex);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA32F_ARB, this->_nsMaxTexSize, this->_nsMaxTexSize, 0, GL_RGBA, GL_FLOAT, NULL);
			//Set up output vertex texture
			glGenTextures(1, &(this->_nsVertTex));
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_nsVertTex);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA32F_ARB, this->_nsMaxTexSize, this->_nsMaxTexSize, 0, GL_RGBA, GL_FLOAT, NULL);
			//Set up output normal texture
			glGenTextures(1, &(this->_nsNormTex));
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_nsNormTex);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA32F_ARB, this->_nsMaxTexSize, this->_nsMaxTexSize, 0, GL_RGBA, GL_FLOAT, NULL);
			//Check for errors
			if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometryContext::StartSurface - At Medium Texture Creation.");
			//Generate the framebuffer object
			glGenFramebuffersEXT(1, &(this->_nsFramebuffer));
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->_nsFramebuffer);
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, this->_nsVertTex, 0);
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_RECTANGLE_ARB, this->_nsNormTex, 0);
			retVal = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
			//Check for errors
			if (retVal != GL_FRAMEBUFFER_COMPLETE_EXT) 
				CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometryContext::StartSurface - Framebuffer is not complete.");
			//Clean up a bit
			glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
			//Check for errors
			if (glGetError() != GL_NO_ERROR) 
				CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometryContext::StartSurface - At Medium Creation.");
			break;
			
		default:
//			CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCGeometryContext::StartSurface - Actually Running as Low.");
			//Nothing to set up for now in low performance mode
			break;
	}
	//Setup locations for all programs
	if (this->_nsPerfLevel != NURBSSURFACE_PERFLEVEL_LOW) {
		//Set the locations of each uniform variable
		this->_nsLocations = new GLint[15];
		//Set the locations of each control point variable
		this->_nsLocations[NURBSSURFACE_LOC_CP_DEFAULT] = glGetUniformLocation(this->_nsDefault, "controlPoints");
		this->_nsLocations[NURBSSURFACE_LOC_CP_BEZIER23] = glGetUniformLocation(this->_nsBezier23, "controlPoints");
		//Set the location of each knot point U variable
		this->_nsLocations[NURBSSURFACE_LOC_KPU_DEFAULT] = glGetUniformLocation(this->_nsDefault, "knotPointsU");
		this->_nsLocations[NURBSSURFACE_LOC_KPU_BEZIER23] = glGetUniformLocation(this->_nsBezier23, "knotPointsU");
		//Set the location of each knot point U variable
		this->_nsLocations[NURBSSURFACE_LOC_KPV_DEFAULT] = glGetUniformLocation(this->_nsDefault, "knotPointsV");
		this->_nsLocations[NURBSSURFACE_LOC_KPV_BEZIER23] = glGetUniformLocation(this->_nsBezier23, "knotPointsV");
		//Set the location of each params U variable
		this->_nsLocations[NURBSSURFACE_LOC_PARAMSU_DEFAULT] = glGetUniformLocation(this->_nsDefault, "numParamsU");
		this->_nsLocations[NURBSSURFACE_LOC_PARAMSU_BEZIER23] = glGetUniformLocation(this->_nsBezier23, "numParamsU");
		//Set the location of each params U variable
		this->_nsLocations[NURBSSURFACE_LOC_PARAMSV_DEFAULT] = glGetUniformLocation(this->_nsDefault, "numParamsV");
		this->_nsLocations[NURBSSURFACE_LOC_PARAMSV_BEZIER23] = glGetUniformLocation(this->_nsBezier23, "numParamsV");
		//Check for errors
		if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometryContext::StartSurface - At Uniform Get Locations.");
	}
	if (this->_nsPerfLevel == NURBSSURFACE_PERFLEVEL_HIGH) {
#ifdef GL_EXT_transform_feedback
		//Determine min buffer sizes for bindable uniform arrays
		this->_nsMinKPBufferSize = glGetUniformBufferSizeEXT(this->_nsDefault, this->_nsLocations[NURBSSURFACE_LOC_KPU_DEFAULT]);
		this->_nsMinCPBufferSize = glGetUniformBufferSizeEXT(this->_nsDefault, this->_nsLocations[NURBSSURFACE_LOC_CP_DEFAULT]);
		if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometryContext::StartSurface - At High Buffer Sizing.");
		//Bind the bindable uniforms
		glUniformBufferEXT(this->_nsDefault, this->_nsLocations[NURBSSURFACE_LOC_CP_DEFAULT], this->_nsCPBuffer);
		glUniformBufferEXT(this->_nsDefault, this->_nsLocations[NURBSSURFACE_LOC_KPU_DEFAULT], this->_nsKPUBuffer);
		glUniformBufferEXT(this->_nsDefault, this->_nsLocations[NURBSSURFACE_LOC_KPV_DEFAULT], this->_nsKPVBuffer);
		glUniformBufferEXT(this->_nsBezier23, this->_nsLocations[NURBSSURFACE_LOC_CP_BEZIER23], this->_nsCPBuffer);
		glUniformBufferEXT(this->_nsBezier23, this->_nsLocations[NURBSSURFACE_LOC_KPU_BEZIER23], this->_nsKPUBuffer);
		glUniformBufferEXT(this->_nsBezier23, this->_nsLocations[NURBSSURFACE_LOC_KPV_BEZIER23], this->_nsKPVBuffer);
#endif // GL_EXT_transform_feedback
		if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometryContext::StartSurface - At High Uniform Bindable Binding.");
	}
	if (this->_nsPerfLevel == NURBSSURFACE_PERFLEVEL_MEDIUM) {
		//Set up a few more locations
		this->_nsLocations[NURBSSURFACE_LOC_CP_DEFAULT23] = glGetUniformLocation(this->_nsDefault23, "controlPoints");
		this->_nsLocations[NURBSSURFACE_LOC_KPU_DEFAULT23] = glGetUniformLocation(this->_nsDefault23, "knotPointsU");
		this->_nsLocations[NURBSSURFACE_LOC_KPV_DEFAULT23] = glGetUniformLocation(this->_nsDefault23, "knotPointsV");
		this->_nsLocations[NURBSSURFACE_LOC_PARAMSU_DEFAULT23] = glGetUniformLocation(this->_nsDefault23, "numParamsU");
		this->_nsLocations[NURBSSURFACE_LOC_PARAMSV_DEFAULT23] = glGetUniformLocation(this->_nsDefault23, "numParamsV");
		//Set up more stuff
		glUseProgram(this->_nsDefault);
		glUniform1i(glGetUniformLocation(this->_nsDefault, "verts"), 0);
		glUniform1i(this->_nsLocations[NURBSSURFACE_LOC_CP_DEFAULT], 1);
		glUniform1i(this->_nsLocations[NURBSSURFACE_LOC_KPU_DEFAULT], 2);
		glUniform1i(this->_nsLocations[NURBSSURFACE_LOC_KPV_DEFAULT], 3);
		glUseProgram(this->_nsDefault23);
		glUniform1i(glGetUniformLocation(this->_nsDefault23, "verts"), 0);
		glUniform1i(this->_nsLocations[NURBSSURFACE_LOC_CP_DEFAULT23], 1);
		glUniform1i(this->_nsLocations[NURBSSURFACE_LOC_KPU_DEFAULT23], 2);
		glUniform1i(this->_nsLocations[NURBSSURFACE_LOC_KPV_DEFAULT23], 3);
		glUseProgram(this->_nsBezier23);
		glUniform1i(glGetUniformLocation(this->_nsBezier23, "verts"), 0);
		glUniform1i(this->_nsLocations[NURBSSURFACE_LOC_CP_BEZIER23], 1);
		glUniform1i(this->_nsLocations[NURBSSURFACE_LOC_KPU_BEZIER23], 2);
		glUniform1i(this->_nsLocations[NURBSSURFACE_LOC_KPV_BEZIER23], 3);
		//Stop using any programs
		glUseProgram(0);
	}
}


void WCGeometryContext::StopSurface(void) {
	//Delete programs
	glDeleteProgram(this->_nsDefault);
	glDeleteProgram(this->_nsDefault23);
	glDeleteProgram(this->_nsBezier23);
	//Delete locations
	if (this->_nsLocations != NULL) delete this->_nsLocations;
	//Depending on PerfMode...
	if (this->_nsPerfLevel == NURBSSURFACE_PERFLEVEL_HIGH) {
		//Delete the generation buffers
		glDeleteBuffers(1, &this->_nsCPBuffer);
		glDeleteBuffers(1, &this->_nsKPUBuffer);
		glDeleteBuffers(1, &this->_nsKPVBuffer);
	}
	else if (this->_nsPerfLevel == NURBSSURFACE_PERFLEVEL_MEDIUM) {
		//Delete the framebuffer object
		glDeleteFramebuffersEXT(1, &this->_nsFramebuffer);
		//Delete the generation textures
		glDeleteTextures(1, &this->_nsCPTex);
		glDeleteTextures(1, &this->_nsKPUTex);
		glDeleteTextures(1, &this->_nsKPVTex);
		glDeleteTextures(1, &this->_nsInTex);
		glDeleteTextures(1, &this->_nsVertTex);
		glDeleteTextures(1, &this->_nsNormTex);
	}
	//Check for errors
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometryContext::StopSurface - Unspecified error.");
}

	
/***********************************************~***************************************************/


void WCGeometryContext::StartTrimSurface(void) {
	//Make sure there is support for needed extensions
	if (WCAdapter::HasGLEXTTextureFloat() && WCAdapter::HasGLARBTextureRectangle() && WCAdapter::HasGLEXTFramebufferObject()) {
		//Get program IDs
		this->_tsPointInversion = this->_shaderManager->ProgramFromName("trimsurface_inversion");
		this->_tsTriangulate = this->_shaderManager->ProgramFromName("trimsurface_triangulate");
		//Set some program values
		this->_tsLocations = new GLint[1];
		this->_tsLocations[TRIMSURFACE_LOC_PI_PARAMS] = glGetUniformLocation(this->_tsPointInversion, "params");
		this->_tsLocations[TRIMSURFACE_LOC_PI_SURFDATA] = glGetUniformLocation(this->_tsPointInversion, "surfData");
		glUseProgram(this->_tsPointInversion);
		glUniform1i(glGetUniformLocation(this->_tsPointInversion, "verts"), 0);
		glUniform1i(this->_tsLocations[TRIMSURFACE_LOC_PI_SURFDATA], 1);
		glUseProgram(0);
		//Determine maximum texture size
		this->_tsMaxTexSize = STDMIN(WCAdapter::GetMax2DTextureSize(), (GLint)TRIMSURFACE_MAX_TEX_SIZE);
		//Set up trim texture generation texture
		glGenTextures(1, &(this->_tsInTex));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_tsInTex);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA32F_ARB, this->_tsMaxTexSize, this->_tsMaxTexSize, 0, GL_RGBA, GL_FLOAT, NULL);
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometryContext::StartTrimSurface - Create Input Texture: " << err);
		//Set up trim texture generation texture
		glGenTextures(1, &(this->_tsSurfTex));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_tsSurfTex);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA32F_ARB, TRIMSURFACE_PI_TEX_SIZE, TRIMSURFACE_PI_TEX_SIZE, 0, GL_RGBA, GL_FLOAT, NULL);
		err = glGetError();
		if (err != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometryContext::StartTrimSurface - Create Surface Texture: " << err);
		//Set up trim texture generation texture
		glGenTextures(1, &(this->_tsOutTex));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_tsOutTex);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA32F_ARB, this->_tsMaxTexSize, this->_tsMaxTexSize, 0, GL_RGBA, GL_FLOAT, NULL);
		err = glGetError();
		if (err != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometryContext::StartTrimSurface - Create Output Texture: " << err);
		//Generate the framebuffer object
		glGenFramebuffersEXT(1, &(this->_tsFramebuffer));
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->_tsFramebuffer);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, this->_tsOutTex, 0);
		err = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		//Check for errors
		if (err != GL_FRAMEBUFFER_COMPLETE_EXT) 
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometryContext::StartTrimSurface - Framebuffer is not complete: " << err);		
	}
	//No texture float support, this could be tough
	else {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometryContext::StartTrimSurface - No support for needed extensions.");
	}
}


void WCGeometryContext::StopTrimSurface(void) {
	//Delete programs
	glDeleteProgram(this->_tsPointInversion);
	glDeleteProgram(this->_tsTriangulate);
	//Delete locations
	if (this->_tsLocations != NULL) delete this->_tsLocations;
	//Delete framebuffers
	glDeleteFramebuffersEXT(1, &this->_tsFramebuffer);
	//Delete the generation textures
	glDeleteTextures(1, &this->_tsInTex);
	glDeleteTextures(1, &this->_tsOutTex);
	//Check for errors
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometryContext::StopTrimSurface - Unspecified error.");
}


/***********************************************~***************************************************/


void WCGeometryContext::StartIntersection(void) {
	//Make sure there is support for needed extensions
	if (WCAdapter::HasGLEXTTextureFloat() && WCAdapter::HasGLARBTextureRectangle() && WCAdapter::HasGLEXTFramebufferObject()) {
		//Get program IDs
		this->_cciM = this->_shaderManager->ProgramFromName("cci_plM");
//		this->_sciM = this->_shaderManager->ProgramFromName("sci_plM");
//		this->_ssiM = this->_shaderManager->ProgramFromName("ssi_plM");
//		this->_tciM = this->_shaderManager->ProgramFromName("tci_plM");
//		this->_tsiM = this->_shaderManager->ProgramFromName("tsi_plM");
//		this->_ttiM = this->_shaderManager->ProgramFromName("tti_plM");

		//Set the locations of each uniform variable
		this->_iLocations = new GLint[15];
		this->_iLocations[INTERSECTION_CCI_PARAMS] = glGetUniformLocation(this->_cciM, "params");
		if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometryContext::StartIntersection - Locations.");

		//Set up CCI left texture
		glGenTextures(1, &(this->_cciLeftTex));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_cciLeftTex);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA32F_ARB, WCAdapter::GetMax2DTextureSize(), 1, 0, GL_RGBA, GL_FLOAT, NULL);
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometryContext::StartIntersection - Left Texture: " << err);
		//Setup CCI right texture
		glGenTextures(1, &(this->_cciRightTex));	
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_cciRightTex);	
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);		
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);					
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA32F_ARB, WCAdapter::GetMax2DTextureSize(), 1, 0, GL_RGBA, GL_FLOAT, NULL);
		if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometryContext::StartIntersection - Right Texture.");
		//Setup output texture				
		glGenTextures(1, &(this->_cciOutTex));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, this->_cciOutTex);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);		
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);					
		glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA32F_ARB, WCAdapter::GetMax2DTextureSize(), 1, 0, GL_RGBA, GL_FLOAT, NULL);
		//Setup the framebuffer object
		glGenFramebuffersEXT(1, &(this->_cciFramebuffer));
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, this->_cciFramebuffer);
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, this->_cciOutTex, 0);
		GLenum retVal = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		if (retVal != GL_FRAMEBUFFER_COMPLETE_EXT)
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometryContext::StartIntersection - Framebuffer is not complete.");				
		//Clean up a bit
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		if (glGetError() != GL_NO_ERROR) 
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometryContext::StartIntersection - GL error at Framebuffer Creation.");
		//Setup the programs
		glUseProgram(this->_cciM);
		glUniform1i(glGetUniformLocation(this->_cciM, "leftVerts"), 0);
		glUniform1i(glGetUniformLocation(this->_cciM, "rightVerts"), 1);
		//Stop using any programs
		glUseProgram(0);
		//Check for errors
		if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometryContext::StartIntersection - Conclusion.");
	}
	//No texture float support, this could be tough
	else {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometryContext::StartIntersection - No support for needed extensions");
	}
}


void WCGeometryContext::StopIntersection(void) {
	//Delete programs
	glDeleteProgram(this->_cciM);
	//...
	//Delete the program locations
	if (this->_iLocations != NULL) delete this->_iLocations;
	//Check for errors
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometryContext::StopIntersection - Unspecified error.");
}


/***********************************************~***************************************************/


WCGeometryContext::WCGeometryContext(const WPGLContext &context, WCShaderManager *shaderManager) : _glContext(context), _shaderManager(shaderManager) {
	//Check to make sure context and shader manager are valid
	if (this->_shaderManager == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCGeometryContext::WCGeometryContext - NULL Shader Manager passed."); 
		return;
	}
	//Initialize all NURBS curve parameters
	this->StartCurve();
	//Initialize all NURBS surface parameters
	this->StartSurface();
	//Initialize all Trimmed NURBS surface parameters
	this->StartTrimSurface();
	//Initialze all Intersection parameters
	this->StartIntersection();
}


WCGeometryContext::~WCGeometryContext() {
	//Conclude all NURBS curve parameters
	this->StopCurve();
	//Conclude all NURBS surface parameters
	this->StopSurface();
	//Conclude all Trimmed NURBS surface parameters
	this->StopTrimSurface();
	//Conclude all Intersection parameters
	this->StopIntersection();
}


/***********************************************~***************************************************/


std::ostream& __WILDCAT_NAMESPACE__::operator<<(std::ostream& out, const WCGeometryContext &context) {
	out << "GeometryContext(" << &context << ")\n";
	out << "\t GLContext: " << context._glContext << std::endl;
	out << "\t Shader Manager: " << context._shaderManager << std::endl;
	out << "*** Curve Parameters ***\n";
	out << "\t Perf Level: " << context._ncPerfLevel << std::endl;
	out << "\t Locations:\n";
	out << "\t\t CP Default: " << context._ncLocations[NURBSCURVE_LOC_CP_DEFAULT] << std::endl;
	out << "\t\t CP Default23: " << context._ncLocations[NURBSCURVE_LOC_CP_DEFAULT23] << std::endl;
	out << "\t\t CP Bezier23: " << context._ncLocations[NURBSCURVE_LOC_CP_BEZIER23] << std::endl;
	out << "\t\t KP Default: " << context._ncLocations[NURBSCURVE_LOC_KP_DEFAULT] << std::endl;
	out << "\t\t KP Default23: " << context._ncLocations[NURBSCURVE_LOC_KP_DEFAULT23] << std::endl;
	out << "\t\t KP Bezier23: " << context._ncLocations[NURBSCURVE_LOC_KP_BEZIER23] << std::endl;
	out << "\t\t Params Default: " << context._ncLocations[NURBSCURVE_LOC_PARAMS_DEFAULT] << std::endl;
	out << "\t\t Params Default23: " << context._ncLocations[NURBSCURVE_LOC_PARAMS_DEFAULT23] << std::endl;
	out << "\t\t Params Bezier23: " << context._ncLocations[NURBSCURVE_LOC_PARAMS_BEZIER23] << std::endl;
	out << "\t Programs: \n";
	out << "\t\t Default: " << context._ncDefault << std::endl;
	out << "\t\t Default23: " << context._ncDefault23 << std::endl;
	out << "\t\t Bezier23: " << context._ncBezier23 << std::endl;
	out << "\tMin CP Buffer Size: " << context._ncMinCPBufferSize << std::endl;
	out << "\tMin KP Buffer Size: " << context._ncMinKPBufferSize << std::endl;
	out << "\tVertices Per Batch: " << context._ncVertsPerBatch << std::endl;
//	out << "\tKP Stride: " << context._ncKPStride << std::endl;
	out << "\tCP Buffer: " << context._ncCPBuffer << std::endl;
	out << "\tKP Buffer: " << context._ncKPBuffer << std::endl;
	out << "\tCP Texture: " << context._ncCPTex << std::endl;
	out << "\tKP Texture: " << context._ncKPTex << std::endl;
	out << "\tInput Texture: " << context._ncInTex << std::endl;
	out << "\tOutput Texture: " << context._ncOutTex << std::endl;
	out << "\tMaximum Texture Size: " << context._ncMaxTexSize << std::endl;
	out << "\tFramebuffer: " << context._ncFramebuffer << std::endl;
	
	return out;
}


/***********************************************~***************************************************/

