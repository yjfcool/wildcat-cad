/*** Included header files ***/
#include "nurbs_curve.h"
#include "geometric_algorithms.h"
#include "nurbs.h"
#include "shader_manager.h"
#include "adapter.h"
#include "geometric_line.h"

/*** Optional Header Files ***/
#ifdef __WILDCAT_SCENE_INCLUDED__
#include "scene.h"
#include "visual_layer.h"
#include "camera.h"
#include "render_state.h"
#endif


/*** Initialize static class variables ***/
//General Statics
WPUInt	WCNurbsCurve::_count = 0;
WPUInt	WCNurbsCurve::_perfLevel = NURBSCURVE_PERFLEVEL_LOW;
GLint*	WCNurbsCurve::_locations = NULL;
//Program Statics
GLuint	WCNurbsCurve::_default = 0; 
GLuint	WCNurbsCurve::_default23 = 0; 
GLuint	WCNurbsCurve::_bezier23 = 0;
//High Generation Statics
GLint	WCNurbsCurve::_vertsPerBatch = NURBSCURVE_MAX_VERT_PER_BATCH;
int		WCNurbsCurve::_minCPBufferSize = 0;
int		WCNurbsCurve::_minKPBufferSize = 0;
GLintptr WCNurbsCurve::_kpStride = 8;
GLuint	WCNurbsCurve::_cpBuffer = 0;
GLuint	WCNurbsCurve::_kpBuffer = 0;
//Medium Generation Statics
GLuint	WCNurbsCurve::_cpTex = 0;
GLuint	WCNurbsCurve::_kpTex = 0;
GLuint	WCNurbsCurve::_inTex = 0;
GLuint	WCNurbsCurve::_outTex = 0;
GLint	WCNurbsCurve::_maxTexSize = 0;
GLuint	WCNurbsCurve::_framebuffer = 0;


/*** Local macros ***/
#define ABS(a)	 (a>0?a:-a)


/*** Extern Variables ***/
extern WPFloat* __bezier_coef[8];


/***********************************************~***************************************************/


WPUInt WCNurbsCurve::Start(void) {
	//Check to make sure class is not already initialized
	if (WCNurbsCurve::_count == 0) {
		//Determine performance level
		if (WCAdapter::HasGLEXTTransformFeedback() && WCAdapter::HasGLEXTBindableUniform() && 
			WCAdapter::HasGLEXTGeometryShader4() ) {//WCAdapter::HasGLEXTGPUShader4()
			//All criteria met for high performance
			WCNurbsCurve::_perfLevel = NURBSCURVE_PERFLEVEL_HIGH;
//			CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCNurbsCurve::Start - Performance set to high.");			
		}	
		else if (WCAdapter::HasGLARBFragmentShader() && WCAdapter::HasGLARBShadingLanguage100() &&
				 WCAdapter::HasGLARBTextureRectange() && WCAdapter::HasGLEXTTextureFloat() &&
				 WCAdapter::HasGLARBPixelBufferObject() && WCAdapter::HasGLEXTFramebufferObject()) {
			//All criteria met for medium performance
			WCNurbsCurve::_perfLevel = NURBSCURVE_PERFLEVEL_MEDIUM;
//			CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCNurbsCurve::Start - Performance set to medium.");
		}
		//Otherwise, remain at low level (see static initialization)
		else {
//			CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCNurbsCurve::Start - Performance set to low.");
		}
/*** Debug ***/	
		//Force Performance Level
//		WCNurbsCurve::_perfLevel = NURBSCURVE_PERFLEVEL_HIGH;
//		WCNurbsCurve::_perfLevel = NURBSCURVE_PERFLEVEL_MEDIUM;
		WCNurbsCurve::_perfLevel = NURBSCURVE_PERFLEVEL_LOW;
/*** Debug ***/
		//Compile Shaders
		switch(WCNurbsCurve::_perfLevel) {
			//Set up programs for high performance mode
			case NURBSCURVE_PERFLEVEL_HIGH:
				CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCNurbsCurve::Start - Actually Running as High");
				if(!WCShaderManager::Started()) {
					CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::Start - Must initialize WCShaderManager first.");
					return 0;
				}
				//Get program IDs
				WCNurbsCurve::_default = WCShaderManager::ProgramFromName("nc_default_plH");
				WCNurbsCurve::_default23 = 0;
				WCNurbsCurve::_bezier23 = WCShaderManager::ProgramFromName("nc23_bezier_plH");				
				//Determine number of vertices per batch
				WCNurbsCurve::_vertsPerBatch = std::min(WCNurbsCurve::_vertsPerBatch, WCAdapter::GetMaxGeometryOutputVertices());
				//Initialize generation buffers
				glGenBuffers(1, &(WCNurbsCurve::_cpBuffer));
				glBindBuffer(GL_ARRAY_BUFFER, WCNurbsCurve::_cpBuffer);
				glBufferData(GL_ARRAY_BUFFER, NURBSCURVE_MAX_CONTROLPOINTS*4*sizeof(GLfloat), NULL, GL_STATIC_DRAW);
				glGenBuffers(1, &(WCNurbsCurve::_kpBuffer));
				glBindBuffer(GL_ARRAY_BUFFER, WCNurbsCurve::_kpBuffer);
				glBufferData(GL_ARRAY_BUFFER, NURBSCURVE_MAX_KNOTPOINTS*4*sizeof(GLfloat), NULL, GL_STATIC_DRAW);
				if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::Start - GL error at High Creation.");
				break;
				
			//Set up programs for medium performance mode	
			case NURBSCURVE_PERFLEVEL_MEDIUM:
				CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCNurbsCurve::Start Debug - Actually Running as Medium");
				if(!WCShaderManager::Started()) {
					CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::Start - Must initialize WCShaderManager first.");
					return 0;
				}
				//Get program IDs
				WCNurbsCurve::_default = WCShaderManager::ProgramFromName("nc_default_plM");
				WCNurbsCurve::_default23 = WCShaderManager::ProgramFromName("nc23_default_plM");
				WCNurbsCurve::_bezier23 = WCShaderManager::ProgramFromName("nc23_bezier_plM");
				//Set up generation static
				glGetIntegerv(GL_MAX_TEXTURE_SIZE, &(WCNurbsCurve::_maxTexSize));
				WCNurbsCurve::_maxTexSize = std::min(WCNurbsCurve::_maxTexSize, NURBSCURVE_MAX_LOD);
				//Set up control point texture
				glGenTextures(1, &(WCNurbsCurve::_cpTex));
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_RECTANGLE_ARB, WCNurbsCurve::_cpTex);
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);		
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);					
				glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA_FLOAT32_APPLE, NURBSCURVE_MAX_CONTROLPOINTS, 1, 0, GL_RGBA, GL_FLOAT, NULL);
				//Setup knot point texture
				glGenTextures(1, &(WCNurbsCurve::_kpTex));	
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_RECTANGLE_ARB, WCNurbsCurve::_kpTex);	
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);		
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);					
				glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA_FLOAT32_APPLE, NURBSCURVE_MAX_KNOTPOINTS, 1, 0, GL_RGBA, GL_FLOAT, NULL);
				//Setup input texture
				glGenTextures(1, &(WCNurbsCurve::_inTex));
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_RECTANGLE_ARB, WCNurbsCurve::_inTex);
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);		
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);					
				glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA_FLOAT32_APPLE, WCNurbsCurve::_maxTexSize, 1, 0, GL_RGBA, GL_FLOAT, NULL);
				//Setup output texture				
				glGenTextures(1, &(WCNurbsCurve::_outTex));
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_RECTANGLE_ARB, WCNurbsCurve::_outTex);
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);		
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);					
				glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA_FLOAT32_APPLE, WCNurbsCurve::_maxTexSize, 1, 0, GL_RGBA, GL_FLOAT, NULL);
				//Setup the framebuffer object
				glGenFramebuffersEXT(1, &(WCNurbsCurve::_framebuffer));
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, WCNurbsCurve::_framebuffer);
				glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, WCNurbsCurve::_outTex, 0);
				GLenum retVal = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
				if (retVal != GL_FRAMEBUFFER_COMPLETE_EXT) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::Start - Framebuffer is not complete.");				
				//Clean up a bit
				glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
				if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::Start - GL error at Medium Creation.");
				break;
				
			default:
//				CLOGGER_DEBUG(WCLogManager::RootLogger(), "WCNurbsCurve::Start Debug - Actually Running as Low.");
				//Nothing to do here for now
				break;
		}
		//Setup locations for all programs
		if (WCNurbsCurve::_perfLevel != NURBSCURVE_PERFLEVEL_LOW) {		
			//Allocate space for locations
			WCNurbsCurve::_locations = new GLint[9];
			//Set the locations of each control point variable
			WCNurbsCurve::_locations[NURBSCURVE_LOC_CP_DEFAULT] = glGetUniformLocation(WCNurbsCurve::_default, "controlPoints");
			WCNurbsCurve::_locations[NURBSCURVE_LOC_CP_DEFAULT23] = glGetUniformLocation(WCNurbsCurve::_default23, "controlPoints");
			WCNurbsCurve::_locations[NURBSCURVE_LOC_CP_BEZIER23] = glGetUniformLocation(WCNurbsCurve::_bezier23, "controlPoints");
			//Set the location of each knot point variable
			WCNurbsCurve::_locations[NURBSCURVE_LOC_KP_DEFAULT] = glGetUniformLocation(WCNurbsCurve::_default, "knotPoints");
			WCNurbsCurve::_locations[NURBSCURVE_LOC_KP_DEFAULT23] = glGetUniformLocation(WCNurbsCurve::_default23, "knotPoints");
			WCNurbsCurve::_locations[NURBSCURVE_LOC_KP_BEZIER23] = glGetUniformLocation(WCNurbsCurve::_bezier23, "knotPoints");
			//Set the location of each params variable
			WCNurbsCurve::_locations[NURBSCURVE_LOC_PARAMS_DEFAULT] = glGetUniformLocation(WCNurbsCurve::_default, "numParams");
			WCNurbsCurve::_locations[NURBSCURVE_LOC_PARAMS_DEFAULT23] = glGetUniformLocation(WCNurbsCurve::_default23, "numParams");
			WCNurbsCurve::_locations[NURBSCURVE_LOC_PARAMS_BEZIER23] = glGetUniformLocation(WCNurbsCurve::_bezier23, "numParams");
			if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::Start - GL error at Location Finding.");
		}
		
		if (WCNurbsCurve::_perfLevel == NURBSCURVE_PERFLEVEL_HIGH) {
			//Determine min buffer sizes for bindable uniform arrays
			WCNurbsCurve::_minKPBufferSize = glGetUniformBufferSizeEXT(WCNurbsCurve::_default, WCNurbsCurve::_locations[NURBSCURVE_LOC_KP_DEFAULT]);
			WCNurbsCurve::_minCPBufferSize = glGetUniformBufferSizeEXT(WCNurbsCurve::_default, WCNurbsCurve::_locations[NURBSCURVE_LOC_CP_DEFAULT]);	
			if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::Start - At High Min Buffer Sizing.");			
			//Bind the bindable uniforms
			glUniformBufferEXT(WCNurbsCurve::_default, WCNurbsCurve::_locations[NURBSCURVE_LOC_CP_DEFAULT], WCNurbsCurve::_cpBuffer);	
			glUniformBufferEXT(WCNurbsCurve::_default, WCNurbsCurve::_locations[NURBSCURVE_LOC_KP_DEFAULT], WCNurbsCurve::_kpBuffer);				
			glUniformBufferEXT(WCNurbsCurve::_default23, WCNurbsCurve::_locations[NURBSCURVE_LOC_CP_DEFAULT23], WCNurbsCurve::_cpBuffer);	
			glUniformBufferEXT(WCNurbsCurve::_default23, WCNurbsCurve::_locations[NURBSCURVE_LOC_KP_DEFAULT23], WCNurbsCurve::_kpBuffer);	
			glUniformBufferEXT(WCNurbsCurve::_bezier23, WCNurbsCurve::_locations[NURBSCURVE_LOC_CP_BEZIER23], WCNurbsCurve::_cpBuffer);	
			glUniformBufferEXT(WCNurbsCurve::_bezier23, WCNurbsCurve::_locations[NURBSCURVE_LOC_KP_BEZIER23], WCNurbsCurve::_kpBuffer);							
			if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::Start - At High Uniform Bindable Binding.");			
		}
		if (WCNurbsCurve::_perfLevel == NURBSCURVE_PERFLEVEL_MEDIUM) {
			glUseProgram(WCNurbsCurve::_default);
			glUniform1i(glGetUniformLocation(WCNurbsCurve::_default, "verts"), 0);		
			glUniform1i(WCNurbsCurve::_locations[NURBSCURVE_LOC_CP_DEFAULT], 1);
			glUniform1i(WCNurbsCurve::_locations[NURBSCURVE_LOC_KP_DEFAULT], 2);
			glUseProgram(WCNurbsCurve::_default23);
			glUniform1i(glGetUniformLocation(WCNurbsCurve::_default23, "verts"), 0);		
			glUniform1i(WCNurbsCurve::_locations[NURBSCURVE_LOC_CP_DEFAULT23], 1);
			glUniform1i(WCNurbsCurve::_locations[NURBSCURVE_LOC_KP_DEFAULT23], 2);
			glUseProgram(WCNurbsCurve::_bezier23);
			glUniform1i(glGetUniformLocation(WCNurbsCurve::_bezier23, "verts"), 0);		
			glUniform1i(WCNurbsCurve::_locations[NURBSCURVE_LOC_CP_BEZIER23], 1);
			glUniform1i(WCNurbsCurve::_locations[NURBSCURVE_LOC_KP_BEZIER23], 2);												
			//Stop using any programs
			glUseProgram(0);		
		}
	}
	//Return the count of existing NurbsCurves
	return WCNurbsCurve::_count++;
}


WPUInt WCNurbsCurve::Stop(void) {
	//Only close if object has already been initialized	
	if(WCNurbsCurve::_count == 1) {
		//Delete programs
		glDeleteProgram(WCNurbsCurve::_default);
		glDeleteProgram(WCNurbsCurve::_default23);		
		glDeleteProgram(WCNurbsCurve::_bezier23);
		//Delete the locations
		if (WCNurbsCurve::_locations != NULL) delete WCNurbsCurve::_locations;
		//Delete generation statics
		if (WCNurbsCurve::_perfLevel == NURBSCURVE_PERFLEVEL_HIGH) {
			glDeleteBuffers(1, &(WCNurbsCurve::_cpBuffer));
			glDeleteBuffers(1, &(WCNurbsCurve::_kpBuffer));
		}
		else if(WCNurbsCurve::_perfLevel == NURBSCURVE_PERFLEVEL_MEDIUM) {
			glDeleteTextures(1, &(WCNurbsCurve::_cpTex));
			glDeleteTextures(1, &(WCNurbsCurve::_kpTex));		
		}
		//Return the status
		return WCNurbsCurve::_count = 0;
	}
	//Return the number of existing NurbsCurve
	return WCNurbsCurve::_count--;
}


/***********************************************~***************************************************/


/*
WPFloat WCNurbsCurve::EstimateLength(void) {
	//Initialize length variable
	WPFloat length = 0.0;
	//Approximate length using control points
	for (int i=0; i<this->_cp-1; i++) {
		//Get vector from control point to the next
		length += (this->_cpCollection.At(i+1)->Data() - this->_cpCollection.At(i)->Data()).Magnitude();
	}
	return length;
}
*/

void WCNurbsCurve::GenerateKnotPointsVBO(void) {
	//Determine actual number of bytes needed in the buffer
	WPUInt size = this->_kp * 4 * sizeof(GLfloat);
	GLfloat data[size / sizeof(GLfloat)];	
	//Check to see if shader array is undersize	
	if (WCNurbsCurve::_minKPBufferSize < size) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::GenerateKnotPointsVBO - Undersized array in shader.");
	}	
	//Otherwise, copy the kp array into the vbo (Cast WPFloat to GLfloat)	
	for (int i=0; i<this->_kp; i++) data[i*4] = (GLfloat)this->_knotPoints[i];
	//Bind the knot point buffer and load it
	glBindBuffer(GL_ARRAY_BUFFER, WCNurbsCurve::_kpBuffer);	
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
}


void WCNurbsCurve::GenerateKnotPointsTexture(void) {
	//Create temporary array for data
	GLfloat data[4 * this->_kp];
	//Copy knot points into array (cast WPFloat to GLfloat)
	for (int i=0; i<this->_kp; i++) data[i*4] = (GLfloat)this->_knotPoints[i];
	//Set up some parameters
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//Set up texture
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, WCNurbsCurve::_kpTex);	
	glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, this->_kp, 1, GL_RGBA, GL_FLOAT, data);
//	if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsCurve::GenerateKnotPointsTexture Error - Texture Setup.\n";		
}


void WCNurbsCurve::GenerateControlPointsVBO(void) {
	//Determine the needed size of the buffer
	GLuint size = this->_cp * 4 * sizeof(GLfloat);
	GLfloat data[size / sizeof(GLfloat)];
	//Check for shader array undersize
	if (WCNurbsCurve::_minCPBufferSize < size) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::GenerateControlPointsVBO - Undersized array in shader.");
	}
	//Otherwise, convert all control points to GLfloat arrays
	for (int i=0; i<this->_cp; i++) {
		//Cast each control point to a GLfloat array
		data[i*4]	= (GLfloat)(this->_cpCollection.At(i)->Data()).I();
		data[i*4+1] = (GLfloat)(this->_cpCollection.At(i)->Data()).J();
		data[i*4+2] = (GLfloat)(this->_cpCollection.At(i)->Data()).K();
		data[i*4+3] = (GLfloat)(this->_cpCollection.At(i)->Data()).L();
	}
	//Bind to buffer and copy data in
	glBindBuffer(GL_ARRAY_BUFFER, WCNurbsCurve::_cpBuffer);
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
}


void WCNurbsCurve::GenerateControlPointsTexture(void) {
	//Create temporary array for data
	GLfloat data[this->_cp * 4];
	//Copy control points into array
	for (int i=0; i<this->_cp; i++) {
		//Cast each control point to a GLfloat array
		data[i*4]	= (GLfloat)(this->_cpCollection.At(i)->Data()).I();
		data[i*4+1] = (GLfloat)(this->_cpCollection.At(i)->Data()).J();
		data[i*4+2] = (GLfloat)(this->_cpCollection.At(i)->Data()).K();
		data[i*4+3] = (GLfloat)(this->_cpCollection.At(i)->Data()).L();	
	}
	//Set up some parameters
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//Set up texture
	glActiveTexture(GL_TEXTURE1);	
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, WCNurbsCurve::_cpTex);	
	glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, this->_cp, 1, GL_RGBA, GL_FLOAT, data);
//	if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsCurve::GenerateControlPointsTexture Error - Texture Setup.\n";		
}


void WCNurbsCurve::LoadKnotPoints(const WCCollection<WPFloat> *collection) {
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
		for (int i=1; i<this->_degree; i++) {
			if (collection->At(i) != collection->At(0)) isCustom = false;
			if (collection->At(this->_kp-i-1) != collection->At(this->_kp-1)) isCustom = false;
		}
		//If still custom, then use it
		if (isCustom)
			this->_knotPoints = WCNurbs::LoadCustomKnotPoints(collection);
		//Otherwise just make curve mode Default
		else {
			this->_mode = WCNurbsMode::Default();
			CLOGGER_WARN(WCLogManager::RootLogger(), "WCNurbsCurve::LoadKnotPoints - Converting mode to Default.");
			this->_knotPoints = WCNurbs::LoadDefaultKnotPoints(this->_kp, this->_degree);
		}
	}
}


void WCNurbsCurve::LoadControlPoints(const WCCollection<WCControlPoint*> *controlPoints) {
	//Empty the current control point collection
	for (int i=0; i<this->_cpCollection.Size(); i++) this->_cpCollection.At(i)->Release(*this);
	//Copy the contents of the control point collection
	this->_cpCollection = (*controlPoints);
	//Loop through all control points and retain points
	for (int i=0; i<this->_cp; i++) this->_cpCollection.At(i)->Retain(*this);
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
//		std::cout << "Using Bezier23.\n";
		glUseProgram(WCNurbsCurve::_bezier23);
		glUniform4i(WCNurbsCurve::_locations[NURBSCURVE_LOC_PARAMS_BEZIER23], this->_degree, this->_cp, this->_kp, WCNurbsCurve::_vertsPerBatch);
	}
	//Degree > 3, Default and Custom
	else {
//		std::cout << "Using Default.\n";
		glUseProgram(WCNurbsCurve::_default);		
		glUniform4i(WCNurbsCurve::_locations[NURBSCURVE_LOC_PARAMS_DEFAULT], this->_degree, this->_cp, this->_kp, WCNurbsCurve::_vertsPerBatch);
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
	int numBatches = ceil( (float)this->_lod / (float)WCNurbsCurve::_vertsPerBatch );
	//Create a temporary array (LOD vertices)
	GLfloat data[numBatches * 4];
	for (int i=0; i<numBatches; i++) {
		//Load array with value for each batch
		data[i*4] = u;									//Set first position to u
		data[i*4+1] = du;								//Set second position to du
		data[i*4+2] = this->_knotPoints[this->_kp-1];	//Set third position to uMax
		data[i*4+3] = 1.0;								//Dummy data
		//Increment u
		u += du * WCNurbsCurve::_vertsPerBatch;				
	}	
	//Create a temporary input buffer
	GLuint inputBuffer;
	glGenBuffers(1, &inputBuffer);
	//Set up transform feedback	buffer
	glBindBuffer(GL_ARRAY_BUFFER, inputBuffer);
	//Allocate space for the batch data and load data value array
	glBufferData(GL_ARRAY_BUFFER, numBatches*4*sizeof(GLfloat), data, GL_STATIC_DRAW);
//	if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsCurve::GenerateCurveHigh Error - Input buffer setup.\n";
			
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
	if (this->_lod > WCNurbsCurve::_maxTexSize) {
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
				glUseProgram(WCNurbsCurve::_default23);
				glUniform4i(WCNurbsCurve::_locations[NURBSCURVE_LOC_PARAMS_DEFAULT23], this->_degree, this->_cp, this->_kp, this->_lod+1);
//			}
			break;
		default:
			//Degree > 3 Default, Custom, and Bezier
			glUseProgram(WCNurbsCurve::_default);
			glUniform4i(WCNurbsCurve::_locations[NURBSCURVE_LOC_PARAMS_DEFAULT], this->_degree, this->_cp, this->_kp, this->_lod+1);
			break;
	}
	//Now generate the control points texture
	this->GenerateControlPointsTexture();
	//Create a knotpoint texture
	this->GenerateKnotPointsTexture();

	/*** Bind to framebuffer object ***/
	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, WCNurbsCurve::_framebuffer);
	//Check to make sure the framebuffer is ready
	GLenum retVal = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	//Check the status of the framebuffer object
	if (retVal != GL_FRAMEBUFFER_COMPLETE_EXT) { 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::GenerateCurveMedium - Framebuffer is not complete."); return; }
	
	/*** Setup input texture ***/
	
	//Allocate space for the texture (w * h * RGBA)
	GLfloat data[this->_lod * 4];
	WPFloat u = this->_knotPoints[0];
	WPFloat range = this->_knotPoints[this->_kp-1] - this->_knotPoints[0];
	WPFloat du = range / ((GLfloat)(this->_lod-1));	
	//Initialize data in the array
	for (int i=0; i<this->_lod; i++) {
		data[i*4] = u;									//Set first position to u
		data[i*4+1] = du;								//Set second position to du
		data[i*4+2] = this->_knotPoints[this->_kp-1];	//Set third position to uMax
		data[i*4+3] = GLfloat(i);						//Set fourth position to index		
		//Increment u
		u = std::min(u+du, this->_knotPoints[this->_kp-1]);
	}
	//Setup and copy the data into the texture
//	for (int i=0; i<this->_lod; i++) printf("%d: %f %f %f %f\n", i, data[i*4], data[i*4+1], data[i*4+2], data[i*4+3]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, WCNurbsCurve::_inTex);
	glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, this->_lod, 1, GL_RGBA, GL_FLOAT, data);
	if (glGetError() != GL_NO_ERROR) 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::GenerateCurveMedium - Setup Input Texture.");
	
	/*** Setup Viewport and Render***/
	
#ifndef __WILDCAT_SCENE_INCLUDED__
	//Get the current viewport
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
#endif
	//Disable some settings
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	//Set the viewport
	glViewport(0, 0, WCNurbsCurve::_maxTexSize, 1);
	//Draw into the framebuffer
	glBegin(GL_QUADS);
		glVertex3f(-1.0, -1.0, 0.0);
		glVertex3f(-1.0, 1.0, 0.0);
		glVertex3f(1.0, 1.0, 0.0);	
		glVertex3f(1.0, -1.0, 0.0);
	glEnd();
#ifdef __WILDCAT_SCENE_INCLUDED__
	//Reset viewport
	glViewport(0, 0, this->_layer->Scene()->WindowWidth(), this->_layer->Scene()->WindowHeight());
#else	
	//Restore viewport and polygon mode
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
#endif
	//Re-enable some settings
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	//Check for errors
	if (glGetError() != GL_NO_ERROR)CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::GenerateCurveMedium - Render.");

	/*** Save output texture into vertex VBO ***/

	glBindBuffer(GL_PIXEL_PACK_BUFFER, this->_buffer);
	glBufferData(GL_PIXEL_PACK_BUFFER, WCNurbsCurve::_maxTexSize * 4 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
	glReadPixels(0, 0, WCNurbsCurve::_maxTexSize, 1, GL_RGBA, GL_FLOAT, NULL);
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
	GLfloat data[(this->_lod+1)*NURBSCURVE_FLOATS_PER_VERTEX];
	for (int i=0; i<this->_lod; i++) {
		pt = this->Evaluate(u);
		data[i*4] = pt.I();
		data[i*4+1] = pt.J();
		data[i*4+2] = pt.K();
		data[i*4+3] = pt.L();
		//Increment u (up to kp-1)
		u = std::min(u+du, this->_knotPoints[this->_kp-1]);				
	}
	//Allocate space for the output vertex data
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffer);	
	glBufferData(GL_ARRAY_BUFFER, this->_lod * NURBSCURVE_FLOATS_PER_VERTEX * sizeof(GLfloat), data, GL_STATIC_DRAW);		
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
	GLfloat data[size / sizeof(GLfloat)];		
	//Convert all control points to GLfloat arrays
	//Load data vector
	for (int i=0; i<this->_cp; i++) {
		//Cast each control point to a GLfloat array
		data[i*4]	= (GLfloat)(this->_cpCollection.At(i)->Data()).I();
		data[i*4+1] = (GLfloat)(this->_cpCollection.At(i)->Data()).J();
		data[i*4+2] = (GLfloat)(this->_cpCollection.At(i)->Data()).K();
		data[i*4+3] = 1.0;	
	}
	//Bind to buffer and copy data in
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffer);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
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


WCNurbsCurve::WCNurbsCurve(const WPUInt degree, const WCCollection<WCControlPoint*> *cpCollection, const WCNurbsMode &mode, const WCCollection<WPFloat> *kpCollection) :
	::WCGeometricCurve(), _degree(degree), _mode(mode), _drawCP(false), _cp(0), _kp(0), _knotPoints(NULL) {
	//Make sure static function is called
	WCNurbsCurve::Start();
	
	//Make sure cpCollection is non-null
	if (cpCollection == NULL) { CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::WCNurbsCurve - Invalid control points collection."); return;	}
	//Set the number of control points
	this->_cp = cpCollection->Size();
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
		//See if the collection is null
		if (kpCollection == NULL) { CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::WCNurbsCurve - Invalid knot points collection."); return; }
		//Make sure the collection is the right size
		if (kpCollection->Size() != this->_kp) {
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::WCNurbsCurve - Invalid knot points collection size."); return; }
	}

	//Set up buffers
	glGenBuffers(1, &(this->_buffer));
	//Set up control point collection
	this->LoadControlPoints(cpCollection);	
	//Do some more setup only if degree is 1
	if (this->_degree == 1) {
		//Copy the control points into a vbo
		this->GenerateCurveOne();
	//Otherwise, load the knot points
	} else {			
		//Find the rough length of the curve and the number of needed segments
		WPFloat length = WCNurbs::EstimateLength(this->_cpCollection);
		this->_lod = (length / NURBSCURVE_GENERATE_ACCURACY) + 1;	
		//Use knot mode to setup knot array
		this->LoadKnotPoints(kpCollection);
	}		
	//Establish oriented bounding box
	this->_bounds = new WCAlignedBoundingBox(this->_cpCollection);
}


WCNurbsCurve::WCNurbsCurve(const WCNurbsCurve &curve) :
	::WCGeometricCurve(curve), _degree(curve._degree), _mode(curve._mode), _drawCP(curve._drawCP), _cp(curve._cp), _kp(curve._kp), _knotPoints(NULL) {
	//Make sure static function is called
	WCNurbsCurve::Start();
	//Check to make sure at least lod = cp-1
	this->_lod = this->_cp-1;	
	//Set up buffers
	glGenBuffers(1, &(this->_buffer));
	//Set up control point collection
	this->LoadControlPoints(&(curve._cpCollection));
	//Do some more setup only if degree is 1
	if (this->_degree == 1) {
		//Copy the control points into a vbo
		this->GenerateCurveOne();
	//Otherwise, load the knot points
	} else {			
		//Find the rough length of the curve and the number of needed segments
		WPFloat length = WCNurbs::EstimateLength(this->_cpCollection);
		this->_lod = (length / NURBSCURVE_GENERATE_ACCURACY) + 1;
		//Set up the knot vector
		if (this->_mode != WCNurbsMode::Custom())	
			this->LoadKnotPoints(NULL);
		//Custom setup requires copying the array
		else {
			//Allocate space for the arary
			this->_knotPoints = new WPFloat[this->_kp];
			//Copy the data into the array
			for (int i=0; i<curve._kp; i++)
				this->_knotPoints[i] = curve._knotPoints[i];
		}
	}		
}


WCNurbsCurve::~WCNurbsCurve() {
	//Call to the static class deinitializer
	WCNurbsCurve::Stop();
	//Delete the three buffers
	glDeleteBuffers(1, &(this->_buffer));
	//Release all control points
	for (int i=0; i<this->_cp; i++) this->_cpCollection.At(i)->Release(*this);
	//Clear the collection of control points
	this->_cpCollection.Clear();	
	//Delete the knot point array
	if (this->_knotPoints != NULL) delete this->_knotPoints;
}


WPUInt WCNurbsCurve::Degree(const WPUInt degree) {
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
		WPFloat length = WCNurbs::EstimateLength(this->_cpCollection);
		this->_lod = (length / NURBSCURVE_GENERATE_ACCURACY) + 1;	
		//Use knot mode to setup knot array
		this->LoadKnotPoints(NULL);
	}
	//Mark the object as dirty
	this->_isVisualDirty = true;
	//Return the degree value
	return this->_degree;
}


WPUInt WCNurbsCurve::LevelOfDetail(WPUInt lod) {
	//Only adjust LOD for degree > 1
	if (this->_degree == 1) return this->_lod;
	//Check to make sure LOD is valid
	if (lod < this->_cp) {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCNurbsCurve::LevelOfDetail - LOD(" << lod << ") must be at least #CP.");
		lod = this->_cp;	
	 }
	 if (lod > NURBSCURVE_MAX_LOD) {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCNurbsCurve::LevelOfDetail - LOD(" << lod << ") out of bounds.");
		lod = NURBSCURVE_MAX_LOD;
	}
	//Set the new lod
	this->_lod = lod;	
	//Mark the object as dirty
	this->_isVisualDirty = true;
	//Return the level of detail
	return this->_lod;
}


WCVisualObject* WCNurbsCurve::HitTest(const WCRay &ray, const WPFloat tolerance) {
	int i;
	//Only do hit check on cp if they are visible
	if (this->_drawCP) {
		//Clear selection for all points
//		for (i=0; i<this->_cpCollection.Size(); i++) this->_cpCollection.At(i)->IsSelected(false);
		//Check all control points
		for (i=0; i<this->_cpCollection.Size(); i++) {
			if (this->_cpCollection.At(i)->HitTest(ray, tolerance)) return this->_cpCollection.At(i);
		}
	}
	return NULL;
}


void WCNurbsCurve::ApplyTransform(const WCMatrix4 &transform) {
	//Loop through all control points and apply transform
	for (int i=0; i<this->_cpCollection.Size(); i++)
		this->_cpCollection.At(i)->ApplyTransform(transform);
	this->_isVisualDirty = true;
}


void WCNurbsCurve::ApplyTranslation(const WCVector4 &translation) {
	//Loop through all control points and apply translation
	for (int i=0; i<this->_cpCollection.Size(); i++)
		this->_cpCollection.At(i)->ApplyTranslation(translation);
	this->_isVisualDirty = true;
}


void WCNurbsCurve::Render(const GLuint defaultProg, const WCColor color) {
	//Do nothing if not visible
	if (!this->_isVisible) return;
	//Check to see if curve needs to be generated
	if (this->_isVisualDirty) {
		switch(WCNurbsCurve::_perfLevel) {
			//Switch on performance level
			case NURBSCURVE_PERFLEVEL_HIGH:		this->GenerateCurveHigh();		break;
			case NURBSCURVE_PERFLEVEL_MEDIUM:	this->GenerateCurveMedium();	break;
			case NURBSCURVE_PERFLEVEL_LOW:		this->GenerateCurveLow();		break;
		}
		//Update the bounding box
		this->_bounds->Set(this->_buffer, this->_lod);		
		//Mark as clean
		this->_isVisualDirty = false;		
	}
	//Set the rendering program
	if (this->_renderProg != 0) glUseProgram(this->_renderProg);
	else glUseProgram(defaultProg);
	//Set up rendering environment
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
	//Restore the default prog
//	if (this->_renderProg != 0) glUseProgram(defaultProg);
	//Report them errors
	if (glGetError() != GL_NO_ERROR) CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::Render - GL error at Cleanup.");
}


void WCNurbsCurve::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
	//Something has changed in the control points - mark as dirty
	this->_isVisualDirty = true;
	//Check on degree of curve
	if (this->_degree != 1) {
		//Update LOD estimate incase length changed
		WPFloat length = WCNurbs::EstimateLength(this->_cpCollection);
		this->_lod = (length / NURBSCURVE_GENERATE_ACCURACY) + 1;
	}
	//Make sure all dependent objects know about it
	this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);
}


std::list<WPFloat> WCNurbsCurve::Intersect(WCGeometricPoint *point, const WPFloat &tolerance) {
	std::cout << "Curve-Point Intersect\n";
	return std::list<WPFloat>();
}


std::list<WPIntersectRec> WCNurbsCurve::Intersect(WCGeometricCurve *curve, const WPFloat &tol) {
	WPUInt retVal;
	WPFloat uValue, vValue;
	WCVector4 value;
	WPIntersectRec rec;
	std::list<WPIntersectRec> hitList;
	
	//Try casting to a line
	WCGeometricLine *line = dynamic_cast<WCGeometricLine*>(curve);
	//Check for line-line intersection
	if (line != NULL) {
		std::cout << "Curve-Line Intersect\n";
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
		for (int i=1; i<this->_lod; i++) {
			//Determine secVec
			secVec.Set( (WPFloat)data[index], (WPFloat)data[index+1], (WPFloat)data[index+2], (WPFloat)data[index+3] );	
			//See if there is an intersection between line and curve segment
			retVal = LineLineIntersection(firstVec, secVec, p3, p4, tol, value, uValue, vValue);
			//Intersection point found
			if (retVal == INTERSECTION_INTERSECTION) {
				std::cout << "Curve-Line Intersect\n";
				std::cout << "Hit found at: " << value << std::endl;
				std::cout << "U-Value: " << uValue << ", V-Value: " << vValue << std::endl;
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
		for (int i1=1; i1<this->_lod; i1++) {
			//Set c1 second vector
			c1Second.Set((WPFloat)dataA[indexA], (WPFloat)dataA[indexA+1], (WPFloat)dataA[indexA+2], (WPFloat)dataA[indexA+3]);
			//Set the first vector for curve 2
			c2First.Set((WPFloat)dataB[0], (WPFloat)dataB[1], (WPFloat)dataB[2], (WPFloat)dataB[3]);
			indexB = 4;
			//Loop through second curve segments
			for (int i2=1; i2<numVerts; i2++) {
				//Set c2 second vector
				c2Second.Set((WPFloat)dataB[indexB], (WPFloat)dataB[indexB+1], (WPFloat)dataB[indexB+2], (WPFloat)dataB[indexB+3]);
				//See if there is an intersection between c1 and c2 segment
				retVal = LineLineIntersection(c1First, c1Second, c2First, c2Second, tol, value, uValue, vValue);
				//Intersection point found
				if (retVal == INTERSECTION_INTERSECTION) {
					std::cout << "Curve-Curve Intersect\n";
					std::cout << "Hit found at: " << value << std::endl;
					std::cout << "U-Value: " << uValue << ", V-Value: " << vValue << std::endl;
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


WPFloat WCNurbsCurve::Length(WPFloat tolerance) {
	//Initialize length variable
	WPFloat length = 0;
	//Degree = 1 special case
	if (this->_degree == 1) {
		length = WCNurbs::EstimateLength(this->_cpCollection);
		return length;
	}
	//Check to see if curve needs to be generated
	if (this->_isVisualDirty) {
		//Generate the curve - only if degree is not 1
		switch(WCNurbsCurve::_perfLevel) {
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
	for (int i=1; i<this->_lod; i++) {
		p1 = WCVector4(verts[i*4], verts[i*4+1], verts[i*4+2], verts[i*4+3]);
		length += (p1 - p0).Magnitude();
		p0 = p1;
	}
	//Make sure to unmap the vertex data
	glUnmapBuffer(GL_ARRAY_BUFFER);
	return length;
}


WCVector4 WCNurbsCurve::Evaluate(WPFloat u) {
	WCVector4 c;
	//Bounds check the u value
	if (u < 0.0) u = 0.0;
	if (u > 1.0) u = 1.0;
	
	//Degree 1 case
	if (this->_degree == 1) {
		WCVector4 c = (this->_cpCollection.At(0)->Data() * u) + (this->_cpCollection.At(1)->Data() * (1.0 - u));
		return c;
	}
	
	//Bezier case
	if (this->_mode == WCNurbsMode::Bezier()) {
		WCMatrix n(this->_degree+1, this->_degree+1, __bezier_coef[this->_degree]);
		WPFloat pts[(this->_degree+1)*4];
		WCVector w(this->_degree+1, 0.0);
		for (int i=0; i<=this->_degree; i++) {
			pts[i*4]	= this->_cpCollection.At(i)->X(); 
			pts[i*4+1]	= this->_cpCollection.At(i)->Y(); 
			pts[i*4+2]	= this->_cpCollection.At(i)->Z(); 
			pts[i*4+3]	= this->_cpCollection.At(i)->W();
			w.Set(i, this->_cpCollection.At(i)->W());
		}
		WCMatrix g(this->_degree+1, 4, pts);
		WCMatrix ng = n * g;
		//Calculate t vector
		WCVector t(this->_degree+1, 1.0);			
		WPFloat mag = u;
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
		WPUInt span = WCNurbs::FindSpan(this->_cp, this->_degree, u, this->_knotPoints);
		WCVector4 c;
		WCGeometricPoint *p0;

		WPFloat* basisValues = WCNurbs::BasisValues(span, u, this->_degree, this->_knotPoints);
		//Return if error
		if (basisValues == NULL) return WCVector4();
		WPFloat w=0;
		//Evaluate the point
		for(int j=0; j<=this->_degree; j++) {
			p0 = this->_cpCollection.At(span-this->_degree+j);
			w += p0->W() * basisValues[j];
			c += p0->Data() * p0->W() * basisValues[j];
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


WCVector4 WCNurbsCurve::Derivative(WPFloat u, WPUInt der) {
	//Bounds check the u value
	if (u < this->_knotPoints[0]) u = this->_knotPoints[0];
	if (u > this->_knotPoints[this->_kp-1]) u = this->_knotPoints[this->_kp-1];
	//Make sure der is not larger than degree
	if (der > this->_degree) der = this->_degree;

	//Now find the derivative of the curve at the point
	WPUInt span = WCNurbs::FindSpan(this->_cp, this->_degree, u, this->_knotPoints);
	WPFloat* basisValues = WCNurbs::BasisValues(span, u, this->_degree, this->_knotPoints, der);
	//Return if error
	if (basisValues == NULL) return WCVector4();	
	
	//Declare some temparary values
	WCVector4 c;
	int index;
/*** Debug ***/
	WPFloat w=1;
/*** Debug ***/	
	WCGeometricPoint *point;
	//Calculate the derivative value
	for (int j=0; j<=this->_degree; j++) {
		point = this->_cpCollection.At(span-this->_degree+j);
		index = span - this->_degree + j;
/*** Debug ***/
//Need to normalize the derivative		
//		w += (point->W() * basisValues[j]);
/*** Debug ***/
//		c = c + point->Data() * point->W() * basisValues[j];		
		c = c + point->Data() * basisValues[j];		
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


WCRay WCNurbsCurve::Tangent(WPFloat u) {
	//Check on degree 1
	if (this->_degree == 1) return WCRay(WCVector4(), WCVector4());
	//Check bounds on u
	if (u < this->_knotPoints[0]) u = this->_knotPoints[0];
	if (u > this->_knotPoints[this->_kp-1]) u = this->_knotPoints[this->_kp-1];

	//Now find the derivative of the curve at the point
	WPUInt span = WCNurbs::FindSpan(this->_cp, this->_degree, u, this->_knotPoints);
	WPFloat* basisValues = WCNurbs::BasisValues(span, u, this->_degree, this->_knotPoints, 1);
	if (basisValues == NULL) return WCRay(WCVector4(), WCVector4());	
	//Declare some temparary values
	WCVector4 n, nPrime;
	WCVector4 point;
	WPFloat d=0, dPrime=0;
	//Calculate the derivative value
	for (int j=0; j<=this->_degree; j++) {
		point = this->_cpCollection.At(span-this->_degree+j)->Data();
		d = d + (point.L() * basisValues[j]);
		dPrime = dPrime + (point.L() * basisValues[j+this->_degree+1]);
		n = n + (point * basisValues[j] * point.L());	
		nPrime = nPrime + (point * basisValues[j+this->_degree+1] * point.L());	
	}
	//Make sure to delete basisValues
	delete basisValues;
	//Calculate c and cDer	
	WCVector4 c = n / d; 
	c.L(1.0);
	WCVector4 cDer = ((nPrime * d) - (n * dPrime)) / (d * d);
	cDer.Normalize(true);
//	std::cout << "c: " << c << std::endl;
//	std::cout << "\tcDer: " << cDer << std::endl;	
	//Create a ray to display the tangent
	return WCRay(c, cDer);
}


WCVector4 WCNurbsCurve::PointInversion(const WCGeometricPoint &point) {
	//Get base vector for point
	WCVector4 refPoint = point.Data();
	refPoint.K(0.0);
	WCVector4 curvePoint;
	int i, index=-1;
	WPFloat u, dist, minDist=10000000.0;

	//Calculate a starting u value
	if (this->_degree == 1) {
		WCVector4 x2x1, x1x0, tmp;
		WPFloat t;
		//Loop through all control points to find the closest
		for (i=0; i<this->_cp-1; i++) {
			//Calculate distance from point to line segment
			x2x1 = this->_cpCollection.At(i+1)->Data() - this->_cpCollection.At(i)->Data();
			x1x0 = this->_cpCollection.At(i)->Data() - refPoint;
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
		if (index == -1) return WCVector4();
		//Now find the point on the line segment and return it
		WCVector4 p = this->_cpCollection.At(index)->Data() + (this->_cpCollection.At(index+1)->Data() - this->_cpCollection.At(index)->Data()) * u;
		return p;
	}

	//Check to see if curve needs to be generated
	if (this->_isVisualDirty) switch (WCNurbsCurve::_perfLevel) {
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
		dist = (refPoint - curvePoint).Magnitude();
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
	WCGeometricPoint* pt;
	dist = minDist;
	WCVector4 c, cDer, c2Der, vDist;
	WPFloat top, bottom, uNx;
	//Loop until condition 1, 2, or 4 is met
	int j = 0;
	while (j <= NURBSCURVE_INVERSION_MAX_ITERATIONS) {
		//Init the point values
		c.Set(0.0, 0.0, 0.0, 0.0);
		cDer.Set(0.0, 0.0, 0.0, 0.0);
		c2Der.Set(0.0, 0.0, 0.0, 0.0);
		//Find the span and basis values up to second derivative
		index = WCNurbs::FindSpan(this->_cp, this->_degree, u, this->_knotPoints);
		basisValues = WCNurbs::BasisValues(index, u, this->_degree, this->_knotPoints, 2);
		//Calculate C, C', and C''
		for (i=0; i<=this->_degree; i++) {
			pt = this->_cpCollection.At(index-this->_degree+i);
			c =		c	+ (pt->Data() * basisValues[i]);		
			cDer = cDer + (pt->Data() * basisValues[i+this->_degree+1]);
			c2Der = c2Der + (pt->Data() * basisValues[i+2*(this->_degree+1)]);
		}
		//Make sure to delete the basis values
		delete basisValues;		
		//Calculate metrics
		vDist = c - refPoint;
		dist = vDist.Magnitude();
		//Check for condition 1
		if (dist < NURBSCURVE_EPSILON_ONE) return c;	
		
		//Do some interim calculations
		top = cDer.DotProduct(vDist);
		bottom = cDer.Magnitude();
		//Check condition 2
		if (( ABS(top) / (bottom*dist) ) <= NURBSCURVE_EPSILON_TWO) return c;
		
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
		
		//Check rule (4) for halting
		top = (cDer * (uNx - u)).Magnitude();
		if (top <= NURBSCURVE_EPSILON_ONE) return c;
		//Swap the u value to the next u value
		u = uNx;
/*** Debug ***/	
//		std::cout << "Dist: " << dist << std::endl;
//		std::cout << "cosDist: " << cosDist << std::endl;
//		std::cout << "uDist: " << uDist << std::endl;
//		std::cout << "u: " << u << std::endl;
//		std::cout << "i: " << i << std::endl;
/*** Debug ***/	
		//Increment max loop counter
		j++;
	}
	//Return the point on the curve
	return c;		
}


void WCNurbsCurve::InsertKnot(WPFloat u, const WPUInt multiplicity) {
	//Check u values
	if (u < 0.0) { CLOGGER_WARN(WCLogManager::RootLogger(), "WCNurbsCurve::InsertKnot - U out of bounds."); u = 0.0; }
	//New number of knot points and control points
	WPUInt kp = this->_kp + multiplicity;
	WPUInt cp = this->_cp + multiplicity;
	//Allocate space for the new knot array
	WPFloat alpha, *UQ = new WPFloat[kp];
	WCVector4* Qw = new WCVector4[cp];
	WCVector4* Rw = new WCVector4[this->_degree+1];
	if ((UQ == NULL) || (Qw == NULL) || (Rw == NULL)) { 
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::InsertKnot - No memory for new vectors."); return; }	
	WPUInt span, s, L;
	//Find the span index of the new knot	
	WCNurbs::FindSpanMultiplicity(this->_cp, this->_degree, u, this->_knotPoints, span, s);
	
	//Load new knot vector
	for (int i=0; i<=span; i++) UQ[i] = this->_knotPoints[i];
	for (int i=1; i<=multiplicity; i++)	UQ[span+i] = u;
	for (int i=span+1; i<this->_kp; i++) UQ[i+multiplicity] = this->_knotPoints[i];
	//Load unaltered control points
	for (int i=0; i<=span-this->_degree; i++) Qw[i] = this->_cpCollection.At(i)->Data();
	for (int i=span-s; i<this->_cp; i++) Qw[i+multiplicity] = this->_cpCollection.At(i)->Data();
	for (int i=0; i<=this->_degree-s; i++) Rw[i] = this->_cpCollection.At(span-this->_degree+i)->Data();
	for (int j=1; j<=multiplicity; j++) {
		L = span - this->_degree + j;
		for (int i=0; i<this->_degree-j-s; i++) {
			alpha = (u - this->_knotPoints[L+i]) / (this->_knotPoints[i+span+1] - this->_knotPoints[L-i]);
			Rw[i] = Rw[i+1] * alpha + Rw[i] * (1.0 - alpha);
		}
		Qw[L] = Rw[0];
		Qw[span+multiplicity-j-s] = Rw[this->_degree-j-s];
	}
	for (int i=L+1; i<span-s; i++) Qw[i] = Rw[i-L];

	//Save new knot points
	delete this->_knotPoints;
	this->_knotPoints = UQ;
	WCCollection<WCControlPoint*> cpCollection;
	//Release unused old control points
	for (int i=0; i<cp; i++) {
		//...
	}
	this->_cpCollection = cpCollection;
	
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
		WPFloat length = WCNurbs::EstimateLength(this->_cpCollection);
		this->_lod = (length / NURBSCURVE_GENERATE_ACCURACY) + 1;	
	}	
	this->_isVisualDirty = true;
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
	for (int i=0; i<this->_cpCollection.Size(); i++) this->_cpCollection.At(i)->Release(*this);
	this->_cpCollection = curve._cpCollection;
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
	for (int i=0; i<this->_cpCollection.Size(); i++)
		if (this->_cpCollection.At(i) != curve._cpCollection.At(i)) return false;
	for (int i=0; i<this->_kp; i++)
		if (this->_knotPoints[i] != curve._knotPoints[i]) return false;
/*** Debug ***/
	//May want to check on equivalence of curves due to reparameterization
/*** Debug ***/	
	//If you got here, they are the same
	return true;
}


DOMElement* WCNurbsCurve::Serialize(DOMDocument *document) {
	//Create the base element for the object
	DOMElement*  curveElem = document->createElement(XMLString::transcode("NURBSCurve"));
	return curveElem;
}


WCNurbsCurve* WCNurbsCurve::Deserialize(DOMElement* obj) {
	return NULL;
}


bool WCNurbsCurve::Validate(DOMElement* obj) {
	return false;
}


/***********************************************~***************************************************/


WCNurbsCurve* WCNurbsCurve::CircularArc(const WCVector4 &center, const WCVector4 &xUnit, const WCVector4 &yUnit, 
	const WPFloat radius, const WPFloat startAngleDeg, const WPFloat endAngleDeg) {
	WPFloat startAngle = (2.0 * M_PI *startAngleDeg) / 360.0;
	WPFloat endAngle = (2.0 * M_PI * endAngleDeg) / 360.0;

	//Determine some angle stuff
	if (endAngle < startAngle) endAngle = (2.0 * M_PI) + endAngle;
	WPFloat theta = endAngle - startAngle;
	WPUInt numArcs;
	if (theta <= M_PI_2) numArcs = 1;
	else if (theta <= M_PI) numArcs = 2;
	else if (theta <= (M_PI_2 + M_PI)) numArcs = 3;
	else numArcs = 4;
	WPFloat deltaTheta = theta / numArcs;
	WPUInt numControlPoints = 2 * numArcs + 1;
	WPUInt numKnotPoints = 2 * numArcs + 4;
	
	WCVector4 points[numControlPoints+1];
	WPFloat knots[numKnotPoints];
	
	WPFloat w1 = cos(deltaTheta / 2.0);
	WCVector4 p0 = center + (xUnit * radius * cos(startAngle)) + (yUnit * radius * sin(startAngle));
	WCVector4 t0 = (yUnit * cos(startAngle)) - (xUnit * sin(startAngle));
	points[0] = p0;
	WPUInt index = 0;
	WPFloat angle = startAngle;
	
	//Create all the control points
	WCVector4 p1, p2, t2;
	WPFloat dummy;
	for (int i=0; i<=numArcs; i++) {
		angle = angle + deltaTheta;
		p2 = center + (xUnit * radius * cos(angle)) + (yUnit * radius * sin(angle));
		points[index+2] = p2;
		t2 = (yUnit * cos(angle)) - (xUnit * sin(angle));
		RayRayIntersection(p0, t0, p2, t2, dummy, dummy, p1);
		p1.L(w1);
		points[index+1] = p1;
		index += 2;
		if (i < numArcs) {
			p0 = p2;
			t0 = t2;
		}
	}
	
	//Load knot vector
	WPUInt tmpInt = 2 * numArcs + 1;
	for (int i=0; i<3; i++) {
		knots[i] = 0.0;
		knots[i + tmpInt] = 1.0;		
	}
	switch(numArcs) {
		case 1: break;
		case 2: knots[3] = knots[4] = 0.5; break;
		case 3: knots[3] = knots[4] = 1.0 / 3.0;
				knots[5] = knots[6] = 2.0 / 3.0; break;
		case 4: knots[3] = knots[4] = 0.25;
				knots[5] = knots[6] = 0.5;
				knots[7] = knots[8] = 0.75; break;
	}

	//Create collection of control points
	WCCollection<WCControlPoint*> cp;
	WCControlPoint *point;
	for (int i=0; i<numControlPoints; i++) {
		point = new WCControlPoint(points[i]);
		cp.Add(point);
	}
	//Create collection of knot points
	WCCollection<WPFloat> *kp = WCCollection<WPFloat>::CollectionFromArray(numKnotPoints, knots);
	//Create the curve
	WCNurbsCurve *curve = new WCNurbsCurve(2, &cp, WCNurbsMode::Custom(), kp);
	//Check to make sure curve is valid
	if (curve == NULL)
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsCurve::CircularArc - NULL arc created.");
	return curve;
}


WCNurbsCurve* WCNurbsCurve::Conic(const WCVector4 &p0, const WCVector4 &t0, const WCVector4 &p2, const WCVector4 &t2, const WCVector4 &p) {
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
	std::cout << "P1: " << p1 << std::endl;
	//Check for poorly formed conic
	if (p1.L() <= -1.0) {
		CLOGGER_WARN(WCLogManager::RootLogger(), "WCNurbsCurve::Conic - p1.W is <= -1.0.");
		return NULL; 
	}
	//Classify type and number of segments
	WPUInt nsegs;
	if (p1.L() >= 1.0) nsegs = 1.0;
	else {
		WPFloat angle = Angle(p0, p1, p2);
//		std::cout << "Angle: " << angle << std::endl;
		if ((p1.L() > 0.0) && (angle > 60.0)) nsegs = 1.0;
		else if ((p1.L() < 0.0) && (angle > 90.0)) nsegs = 4.0;
		else nsegs = 2.0;
	}
//	std::cout << "Number of Segments: " << nsegs << std::endl;
	
	WPUInt n = 2 * nsegs;
	WPUInt j = n + 1;
	//Create knot point and control point arrays
	WPFloat knots[j+3];
	WCVector4 controlPoints[j];
	
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
	
	//Create collection of control points
	WCCollection<WCControlPoint*> cp;
	for (int i=0; i<j; i++) cp.Add( new WCControlPoint(controlPoints[i]) );
	//Create collection of knot points
	WCCollection<WPFloat> *kp = WCCollection<WPFloat>::CollectionFromArray(j+3, knots);
	//Create the curve
	WCNurbsCurve *curve = new WCNurbsCurve(2, &cp, WCNurbsMode::Custom(), kp);
	return curve;
}


WCNurbsCurve* WCNurbsCurve::Ellipse(const WCVector4 &center, const WCVector4 &xUnit, const WCVector4 &yUnit, 
	const WPFloat major, const WPFloat minor) {
	//Set up arrays for the control points and knots
	WCVector4 controlPoints[9];
	WPFloat knots[12] = { 0.0, 0.0, 0.0, 0.25, 0.25, 0.5, 0.5, 0.75, 0.75, 1.0, 1.0, 1.0 };
	//Calculate the control point values
	WCVector4 x = xUnit * major;
	WCVector4 y = yUnit * minor;
	controlPoints[0] = center + x;
	controlPoints[1] = center + x + y; 
	controlPoints[2] = center + y;
	controlPoints[3] = center - x + y;
	controlPoints[4] = center - x;
	controlPoints[5] = center - x - y;
	controlPoints[6] = center - y;
	controlPoints[7] = center + x - y;
	controlPoints[8] = controlPoints[0];
	//Set the control point weights
	WPFloat w = M_SQRT2 / 2.0;
	controlPoints[0].L( 1.0 );
	controlPoints[1].L( w );
	controlPoints[2].L( 1.0 );
	controlPoints[3].L( w );
	controlPoints[4].L( 1.0 );
	controlPoints[5].L( w );
	controlPoints[6].L( 1.0 );
	controlPoints[7].L( w );
	controlPoints[8].L( 1.0 );
	//Create collection of control points
	WCCollection<WCControlPoint*> cp;
	for (int i=0; i<9; i++) cp.Add( new WCControlPoint(controlPoints[i]) );
	//Create collection of knot points
	WCCollection<WPFloat> *kp = WCCollection<WPFloat>::CollectionFromArray(12, knots);
	//Create the curve
	WCNurbsCurve *curve = new WCNurbsCurve(2, &cp, WCNurbsMode::Custom(), kp);
	return curve;
	return NULL;
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCNurbsCurve &curve) {
	//Print out basic surface info
	out << "NurbsCurve(" << (void*)&curve << "):\n";
	out << "\t Vertices:" << curve._lod << std::endl;
	out << "\t Degree(" << curve._degree << ")\n";	
	
	//Only print if collection has items
	if (curve._cpCollection.Size() != 0) {
		//Print out control point information
		out << "\t Control Points(" << curve._cp << ")\n";
		for (int i=0; i<curve._cp; i++) {
			std::cout << "\t\t{" << curve._cpCollection.At(i)->X() << ", \t";
			std::cout << curve._cpCollection.At(i)->Y() << ", \t";
			std::cout << curve._cpCollection.At(i)->Z() << ", \t";
			std::cout << curve._cpCollection.At(i)->W() << "}\n";
		}
	}
	//Only print if knotPoints is not NULL
	if (curve._knotPoints != NULL) {
		//Print out knot point information
		out << "\t Knot Points(" << curve._kp << ")\n\t\t{";
		for (int i=0; i<curve._kp; i++) {
			std::cout << curve._knotPoints[i];
			if (i != curve._kp-1) std::cout << ", ";
			else std::cout << "}\n";
		}
	}
	return out;
}


/***********************************************~***************************************************/


