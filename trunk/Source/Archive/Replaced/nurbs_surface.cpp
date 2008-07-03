/*** Included Header Files ***/
#include "matrix.h"
#include "nurbs_surface.h"
#include "shader_manager.h"
#include "adapter.h"
#include "nurbs.h"
#include "bounding_object.h"
#include "geometric_line.h"
#include "nurbs_curve.h"


/*** Optional Header Files ***/
#ifdef __WILDCAT_SCENE_INCLUDED__
#include "scene.h"
#include "visual_layer.h"
#include "camera.h"
#include "render_state.h"
#endif


/*** Initialize static class variables ***/
//General Statics
WPUInt	WCNurbsSurface::_count = 0;
WPUInt	WCNurbsSurface::_perfLevel = NURBSSURFACE_PERFLEVEL_LOW;
GLint*	WCNurbsSurface::_locations = NULL;
//Program Statics
GLuint	WCNurbsSurface::_default = 0;
GLuint	WCNurbsSurface::_default23 = 0;
GLuint	WCNurbsSurface::_bezier23 = 0;
//High Generation Statics
GLint	WCNurbsSurface::_vertsPerBatch = NURBSSURFACE_MAX_VERT_PER_BATCH;
int		WCNurbsSurface::_minCPBufferSize = 0;
int		WCNurbsSurface::_minKPBufferSize = 0;
GLintptr WCNurbsSurface::_kpStride = 4;
GLuint	WCNurbsSurface::_cpBuffer = 0;
GLuint	WCNurbsSurface::_kpUBuffer = 0;
GLuint	WCNurbsSurface::_kpVBuffer = 0;
//Medium Generation Statics
GLuint	WCNurbsSurface::_cpTex = 0;
GLuint	WCNurbsSurface::_kpUTex = 0;
GLuint	WCNurbsSurface::_kpVTex = 0;
GLuint	WCNurbsSurface::_inTex = 0;
GLuint	WCNurbsSurface::_vertTex = 0;
GLuint	WCNurbsSurface::_normTex = 0;
GLint	WCNurbsSurface::_maxTexSize = 0;
GLuint	WCNurbsSurface::_framebuffer = 0;


/*** Local macros ***/
#define MIN(a,b) (a<b?a:b)
#define MAX(a,b) (a>b?a:b)
#define ABS(a)	 (a>0?a:-a)


/*** Extern Variables ***/
extern WPFloat* __bezier_coef[8];


/***********************************************~***************************************************/


WPUInt WCNurbsSurface::Start(void) {
	//Check to make sure class is not already initialized
	if (WCNurbsSurface::_count == 0) {
		//Determine performance level
		if (WCAdapter::HasGLEXTTransformFeedback() && WCAdapter::HasGLEXTBindableUniform() &&
			WCAdapter::HasGLEXTGeometryShader4() ) {//WCAdapter::HasGLEXTGPUShader4()
			//All criteria met for high performance
			WCNurbsSurface::_perfLevel = NURBSSURFACE_PERFLEVEL_HIGH;
			std::cout << "WCNurbsSurface::Start Debug - Performance set to High.\n";
		}
		else if (WCAdapter::HasGLARBFragmentShader() && WCAdapter::HasGLARBShadingLanguage100() &&
				 WCAdapter::HasGLARBTextureRectange() && WCAdapter::HasGLEXTTextureFloat() &&
				 WCAdapter::HasGLARBPixelBufferObject() && WCAdapter::HasGLEXTFramebufferObject()) {
			//All criteria met for medium performance
			WCNurbsSurface::_perfLevel = NURBSSURFACE_PERFLEVEL_MEDIUM;
//			std::cout << "WCNurbsSurface::Start Debug - Performance set to Medium.\n";
		}
		//Otherwise, remain at low level (see static initialization)
		else {
			std::cout << "WCNurbsSurface::Start Debug - Performance set to Low.\n";
		}
/*** Debug ***/
		//Force Performance Level
//		WCNurbsSurface::_perfLevel = NURBSSURFACE_PERFLEVEL_HIGH;
//		WCNurbsSurface::_perfLevel = NURBSSURFACE_PERFLEVEL_MEDIUM;
		WCNurbsSurface::_perfLevel = NURBSSURFACE_PERFLEVEL_LOW;
/*** Debug ***/
		//Compile Shaders
		switch(WCNurbsSurface::_perfLevel) {
			//Set up programs for high performance mode
			case NURBSSURFACE_PERFLEVEL_HIGH:
				std::cout << "WCNurbsSurface::Start Debug - Actually Running as High\n";
				//Setup high generation programs
				WCNurbsSurface::_default = WCShaderManager::ProgramFromName("ns_default_plH");
				WCNurbsSurface::_bezier23 = WCShaderManager::ProgramFromName("ns23_bezier_plH");
				//Determine number of vertices per batch
				WCNurbsSurface::_vertsPerBatch = std::min(WCNurbsSurface::_vertsPerBatch, WCAdapter::GetMaxGeometryOutputVertices());
				//Initialize generation buffers
				glGenBuffers(1, &(WCNurbsSurface::_cpBuffer));
				glBindBuffer(GL_ARRAY_BUFFER, WCNurbsSurface::_cpBuffer);
				glBufferData(GL_ARRAY_BUFFER, NURBSSURFACE_MAX_CONTROLPOINTS*4*sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
				glGenBuffers(1, &(WCNurbsSurface::_kpUBuffer));
				glBindBuffer(GL_ARRAY_BUFFER, WCNurbsSurface::_kpUBuffer);
				glBufferData(GL_ARRAY_BUFFER, NURBSSURFACE_MAX_KNOTPOINTS*4*sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
				glGenBuffers(1, &(WCNurbsSurface::_kpVBuffer));
				glBindBuffer(GL_ARRAY_BUFFER, WCNurbsSurface::_kpVBuffer);
				glBufferData(GL_ARRAY_BUFFER, NURBSSURFACE_MAX_KNOTPOINTS*4*sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
				//Clean up a bit
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsSurface::Start Error - At High Creation.\n";
				break;
			case NURBSSURFACE_PERFLEVEL_MEDIUM:
				std::cout << "WCNurbsSurface::Start Debug - Actually Running as Medium\n";			
				//Setup medium generation programs
				WCNurbsSurface::_default = WCShaderManager::ProgramFromName("ns_default_plM");
				WCNurbsSurface::_default23 = WCShaderManager::ProgramFromName("ns23_default_plM");				
				WCNurbsSurface::_bezier23 = WCShaderManager::ProgramFromName("ns23_bezier_plM");				
				//Determine maximum texture size
				WCNurbsSurface::_maxTexSize = std::min(WCAdapter::GetMax2DTextureSize(), NURBSSURFACE_MAX_TEXSIZE);
				if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsSurface::Start Error - At Medium Program Compilation.\n";
				//Set up cp texture
				glGenTextures(1, &(WCNurbsSurface::_cpTex));
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_RECTANGLE_ARB, WCNurbsSurface::_cpTex);
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA_FLOAT32_APPLE, NURBSSURFACE_MAX_CONTROLPOINTS, NURBSSURFACE_MAX_CONTROLPOINTS, 0, GL_RGBA, GL_FLOAT, NULL);
				//Set up kpU texture
				glGenTextures(1, &(WCNurbsSurface::_kpUTex));
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_RECTANGLE_ARB, WCNurbsSurface::_kpUTex);
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA_FLOAT32_APPLE, NURBSSURFACE_MAX_KNOTPOINTS, 1, 0, GL_RGBA, GL_FLOAT, NULL);
				//Set up kpV texture
				glGenTextures(1, &(WCNurbsSurface::_kpVTex));
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_RECTANGLE_ARB, WCNurbsSurface::_kpVTex);
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA_FLOAT32_APPLE, NURBSSURFACE_MAX_KNOTPOINTS, 1, 0, GL_RGBA, GL_FLOAT, NULL);
				//Set up main input texture
				glGenTextures(1, &(WCNurbsSurface::_inTex));
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_RECTANGLE_ARB, WCNurbsSurface::_inTex);
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA_FLOAT32_APPLE, WCNurbsSurface::_maxTexSize, WCNurbsSurface::_maxTexSize, 0, GL_RGBA, GL_FLOAT, NULL);
				//Set up output vertex texture
				glGenTextures(1, &(WCNurbsSurface::_vertTex));
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_RECTANGLE_ARB, WCNurbsSurface::_vertTex);
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA_FLOAT32_APPLE, WCNurbsSurface::_maxTexSize, WCNurbsSurface::_maxTexSize, 0, GL_RGBA, GL_FLOAT, NULL);
				//Set up output normal texture
				glGenTextures(1, &(WCNurbsSurface::_normTex));
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_RECTANGLE_ARB, WCNurbsSurface::_normTex);
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA_FLOAT32_APPLE, WCNurbsSurface::_maxTexSize, WCNurbsSurface::_maxTexSize, 0, GL_RGBA, GL_FLOAT, NULL);
				if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsSurface::Start Error - At Medium Texture Creation.\n";
				//Generate the framebuffer object
				glGenFramebuffersEXT(1, &(WCNurbsSurface::_framebuffer));
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, WCNurbsSurface::_framebuffer);
				glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, WCNurbsSurface::_vertTex, 0);
				glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT, GL_TEXTURE_RECTANGLE_ARB, WCNurbsSurface::_normTex, 0);
				GLenum retVal = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
				if (retVal != GL_FRAMEBUFFER_COMPLETE_EXT) std::cout << "WCNurbsSurface::Start - Framebuffer is not complete.\n";
				//Clean up a bit
				glBindTexture(GL_TEXTURE_RECTANGLE_ARB, 0);
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
				if (glGetError() != GL_NO_ERROR) 
					CLOGGER_ERROR(WCLogManager::RootLogger(), "WCNurbsSurface::Start Error - At Medium Creation.");
				break;
			default:
//				std::cout << "WCNurbsSurface::Start Debug - Actually Running as Low\n";			
				//Nothing to set up for now in low performance mode
				break;
		}
		//Setup locations for all programs
		if (WCNurbsSurface::_perfLevel != NURBSSURFACE_PERFLEVEL_LOW) {
			//Set the locations of each uniform variable
			WCNurbsSurface::_locations = new GLint[15];
			//Set the locations of each control point variable
			WCNurbsSurface::_locations[NURBSSURFACE_LOC_CP_DEFAULT] = glGetUniformLocation(WCNurbsSurface::_default, "controlPoints");
			WCNurbsSurface::_locations[NURBSSURFACE_LOC_CP_BEZIER23] = glGetUniformLocation(WCNurbsSurface::_bezier23, "controlPoints");
			//Set the location of each knot point U variable
			WCNurbsSurface::_locations[NURBSSURFACE_LOC_KPU_DEFAULT] = glGetUniformLocation(WCNurbsSurface::_default, "knotPointsU");
			WCNurbsSurface::_locations[NURBSSURFACE_LOC_KPU_BEZIER23] = glGetUniformLocation(WCNurbsSurface::_bezier23, "knotPointsU");
			//Set the location of each knot point U variable
			WCNurbsSurface::_locations[NURBSSURFACE_LOC_KPV_DEFAULT] = glGetUniformLocation(WCNurbsSurface::_default, "knotPointsV");
			WCNurbsSurface::_locations[NURBSSURFACE_LOC_KPV_BEZIER23] = glGetUniformLocation(WCNurbsSurface::_bezier23, "knotPointsV");
			//Set the location of each params U variable
			WCNurbsSurface::_locations[NURBSSURFACE_LOC_PARAMSU_DEFAULT] = glGetUniformLocation(WCNurbsSurface::_default, "numParamsU");
			WCNurbsSurface::_locations[NURBSSURFACE_LOC_PARAMSU_BEZIER23] = glGetUniformLocation(WCNurbsSurface::_bezier23, "numParamsU");
			//Set the location of each params U variable
			WCNurbsSurface::_locations[NURBSSURFACE_LOC_PARAMSV_DEFAULT] = glGetUniformLocation(WCNurbsSurface::_default, "numParamsV");
			WCNurbsSurface::_locations[NURBSSURFACE_LOC_PARAMSV_BEZIER23] = glGetUniformLocation(WCNurbsSurface::_bezier23, "numParamsV");
			if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsSurface::Start Error - At Uniform Get Locations.\n";
		}
		if (WCNurbsSurface::_perfLevel == NURBSSURFACE_PERFLEVEL_HIGH) {
			//Determine min buffer sizes for bindable uniform arrays
			WCNurbsSurface::_minKPBufferSize = glGetUniformBufferSizeEXT(WCNurbsSurface::_default, WCNurbsSurface::_locations[NURBSSURFACE_LOC_KPU_DEFAULT]);
			WCNurbsSurface::_minCPBufferSize = glGetUniformBufferSizeEXT(WCNurbsSurface::_default, WCNurbsSurface::_locations[NURBSSURFACE_LOC_CP_DEFAULT]);
			if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsSurface::Start Error - At High Buffer Sizing.\n";
			//Bind the bindable uniforms
			glUniformBufferEXT(WCNurbsSurface::_default, WCNurbsSurface::_locations[NURBSSURFACE_LOC_CP_DEFAULT], WCNurbsSurface::_cpBuffer);
			glUniformBufferEXT(WCNurbsSurface::_default, WCNurbsSurface::_locations[NURBSSURFACE_LOC_KPU_DEFAULT], WCNurbsSurface::_kpUBuffer);
			glUniformBufferEXT(WCNurbsSurface::_default, WCNurbsSurface::_locations[NURBSSURFACE_LOC_KPV_DEFAULT], WCNurbsSurface::_kpVBuffer);
			glUniformBufferEXT(WCNurbsSurface::_bezier23, WCNurbsSurface::_locations[NURBSSURFACE_LOC_CP_BEZIER23], WCNurbsSurface::_cpBuffer);
			glUniformBufferEXT(WCNurbsSurface::_bezier23, WCNurbsSurface::_locations[NURBSSURFACE_LOC_KPU_BEZIER23], WCNurbsSurface::_kpUBuffer);
			glUniformBufferEXT(WCNurbsSurface::_bezier23, WCNurbsSurface::_locations[NURBSSURFACE_LOC_KPV_BEZIER23], WCNurbsSurface::_kpVBuffer);
			if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsSurface::Start Error - At High Uniform Bindable Binding.\n";
		}
		if (WCNurbsSurface::_perfLevel == NURBSSURFACE_PERFLEVEL_MEDIUM) {
			//Set up a few more locations
			WCNurbsSurface::_locations[NURBSSURFACE_LOC_CP_DEFAULT23] = glGetUniformLocation(WCNurbsSurface::_default23, "controlPoints");
			WCNurbsSurface::_locations[NURBSSURFACE_LOC_KPU_DEFAULT23] = glGetUniformLocation(WCNurbsSurface::_default23, "knotPointsU");
			WCNurbsSurface::_locations[NURBSSURFACE_LOC_KPV_DEFAULT23] = glGetUniformLocation(WCNurbsSurface::_default23, "knotPointsV");
			WCNurbsSurface::_locations[NURBSSURFACE_LOC_PARAMSU_DEFAULT23] = glGetUniformLocation(WCNurbsSurface::_default23, "numParamsU");
			WCNurbsSurface::_locations[NURBSSURFACE_LOC_PARAMSV_DEFAULT23] = glGetUniformLocation(WCNurbsSurface::_default23, "numParamsV");
			//Set up more stuff
			glUseProgram(WCNurbsSurface::_default);
			glUniform1i(glGetUniformLocation(WCNurbsSurface::_default, "verts"), 0);
			glUniform1i(WCNurbsSurface::_locations[NURBSSURFACE_LOC_CP_DEFAULT], 1);
			glUniform1i(WCNurbsSurface::_locations[NURBSSURFACE_LOC_KPU_DEFAULT], 2);
			glUniform1i(WCNurbsSurface::_locations[NURBSSURFACE_LOC_KPV_DEFAULT], 3);
			glUseProgram(WCNurbsSurface::_default23);
			glUniform1i(glGetUniformLocation(WCNurbsSurface::_default23, "verts"), 0);
			glUniform1i(WCNurbsSurface::_locations[NURBSSURFACE_LOC_CP_DEFAULT23], 1);
			glUniform1i(WCNurbsSurface::_locations[NURBSSURFACE_LOC_KPU_DEFAULT23], 2);
			glUniform1i(WCNurbsSurface::_locations[NURBSSURFACE_LOC_KPV_DEFAULT23], 3);
			glUseProgram(WCNurbsSurface::_bezier23);
			glUniform1i(glGetUniformLocation(WCNurbsSurface::_bezier23, "verts"), 0);
			glUniform1i(WCNurbsSurface::_locations[NURBSSURFACE_LOC_CP_BEZIER23], 1);
			glUniform1i(WCNurbsSurface::_locations[NURBSSURFACE_LOC_KPU_BEZIER23], 2);
			glUniform1i(WCNurbsSurface::_locations[NURBSSURFACE_LOC_KPV_BEZIER23], 3);
			//Stop using any programs
			glUseProgram(0);
		}
	}
	//Return the count of existing NurbsSurfaces
	return WCNurbsSurface::_count++;
}


WPUInt WCNurbsSurface::Stop(void) {
	//Only close if object has already been initialized
	if(WCNurbsSurface::_count == 1) {
		//Delete programs
		glDeleteProgram(WCNurbsSurface::_default);
		glDeleteProgram(WCNurbsSurface::_default23);
		glDeleteProgram(WCNurbsSurface::_bezier23);
		//Delete locations
		delete WCNurbsSurface::_locations;
		//Depending on PerfMode...
		if (WCNurbsSurface::_perfLevel == NURBSSURFACE_PERFLEVEL_HIGH) {
			//Delete the generation buffers
			glDeleteBuffers(1, &WCNurbsSurface::_cpBuffer);
			glDeleteBuffers(1, &WCNurbsSurface::_kpUBuffer);
			glDeleteBuffers(1, &WCNurbsSurface::_kpVBuffer);
		}
		if (WCNurbsSurface::_perfLevel == NURBSSURFACE_PERFLEVEL_MEDIUM) {
			//Delete the framebuffer object
			glDeleteFramebuffersEXT(1, &WCNurbsSurface::_framebuffer);
			//Delete the generation textures
			glDeleteTextures(1, &WCNurbsSurface::_cpTex);
			glDeleteTextures(1, &WCNurbsSurface::_kpUTex);
			glDeleteTextures(1, &WCNurbsSurface::_kpVTex);
			glDeleteTextures(1, &WCNurbsSurface::_inTex);
			glDeleteTextures(1, &WCNurbsSurface::_vertTex);
			glDeleteTextures(1, &WCNurbsSurface::_normTex);
		}
		if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsSurface::Stop Error - Unspecified error.\n";
	}
	//Return the number of existing NurbsSurface instances
	return WCNurbsSurface::_count--;
}


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
	if (WCNurbsSurface::_minKPBufferSize < size) std::cout << "WCNurbsSurface::GenerateKnotPointsVBO Error - Undersized array in shader.\n";
	//Otherwise, copy the kp array into the vbo (Cast WPFloat to GLfloat)	
	for (int i=0; i<this->_kpU; i++) data[i*4] = (GLfloat)this->_knotPointsU[i];
	//Bind the knot point buffer and load it
	glBindBuffer(GL_ARRAY_BUFFER, WCNurbsSurface::_kpUBuffer);	
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
/*** Debug ***/
	std::cout << "Knot Points U VBO: " << this->_kpU << std::endl;	
	GLfloat *data2 = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	for (int i=0; i<this->_kpU; i++) printf("\t%d: %f\n", i, data2[i*4]);
	glUnmapBuffer(GL_ARRAY_BUFFER);	
/*** Debug ***/	
	//Unbind the buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	//Now load the V buffer
	for (int i=0; i<this->_kpV; i++) data[i*4] = (GLfloat)this->_knotPointsV[i];
	//Bind the knot point buffer and load it
	glBindBuffer(GL_ARRAY_BUFFER, WCNurbsSurface::_kpVBuffer);	
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
/*** Debug ***/
	std::cout << "Knot Points V VBO: " << this->_kpV << std::endl;	
	data2 = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	for (int i=0; i<this->_kpV; i++) printf("\t%d: %f\n", i, data2[i*4]);
	glUnmapBuffer(GL_ARRAY_BUFFER);	
/*** Debug ***/		
	//Unbind the buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Check for errors here
	if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsSurface::GenerateKnotPointsVBO Fatal - Unspecified GL Error.\n";
		
}


void WCNurbsSurface::GenerateKnotPointsTextures(void) {
	//Create temporary array for data
	GLfloat dataU[4 * this->_kpU];
	//Copy knot points into array (cast WPFloat to GLfloat)
	for (int i=0; i<this->_kpU; i++) dataU[i*4] = (GLfloat)this->_knotPointsU[i];
	//Set up some parameters
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//Set up texture
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, WCNurbsSurface::_kpUTex);	
	glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, this->_kpU, 1, GL_RGBA, GL_FLOAT, dataU);
	if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsSurface::GenerateKnotPointsTexture Error - U Texture Setup.\n";

	//Create temporary array for data
	GLfloat dataV[4 * this->_kpU];
	for (int i=0; i<this->_kpV; i++) dataV[i*4] = (GLfloat)this->_knotPointsV[i];
	//Set up some parameters
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//Set up texture
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, WCNurbsSurface::_kpVTex);	
	glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, this->_kpV, 1, GL_RGBA, GL_FLOAT, dataV);
	if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsSurface::GenerateKnotPointsTexture Error - V Texture Setup.\n";
}


void WCNurbsSurface::GenerateControlPointsVBO(void) {
	//Determine the needed size of the buffer
	GLuint size = this->_cpU * this->_cpV * 4 * sizeof(GLfloat);
	GLfloat data[size / sizeof(GLfloat)];
	//Check for shader array undersize
	if (WCNurbsSurface::_minCPBufferSize < size) std::cout << "WCNurbsSurface::GenerateControlPointsVBO Error - Undersized array in shader.\n";
	//Otherwise, convert all control points to GLfloat arrays
	for (int i=0; i<(this->_cpU * this->_cpV); i++) {
		//Cast each control point to a GLfloat array
		data[i*4]	= (GLfloat)(this->_cpCollection.At(i)->Data()).I();
		data[i*4+1] = (GLfloat)(this->_cpCollection.At(i)->Data()).J();
		data[i*4+2] = (GLfloat)(this->_cpCollection.At(i)->Data()).K();
		data[i*4+3] = (GLfloat)(this->_cpCollection.At(i)->Data()).L();
	}
	//Bind to buffer and copy data in
	glBindBuffer(GL_ARRAY_BUFFER, WCNurbsSurface::_cpBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
/*** Debug ***
	std::cout << "Control Point VBO: " << this->_cpU * this->_cpV << std::endl;
	GLfloat *data2 = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	for (int i=0; i<(this->_cpU * this->_cpV); i++) printf("\t%d: %f %f %f %f\n", i, data2[i*4], data2[i*4+1], data2[i*4+2], data2[i*4+3]);
	glUnmapBuffer(GL_ARRAY_BUFFER);
/*** Debug ***/
	//Clean up and check for errors
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsSurface::GenerateControlPointsVBO Fatal - Unspecified GL Error.\n";
}


void WCNurbsSurface::GenerateControlPointsTexture(void) {
	//Create temporary array for data
	GLfloat data[4*NURBSSURFACE_MAX_CONTROLPOINTS*NURBSSURFACE_MAX_CONTROLPOINTS];
	WCVector4 point;
	int index = 0;
	//Copy control points into array
	for (int i=0; i<this->_cpV; i++) {
		for (int j=0; j<this->_cpU; j++) {
			//Cast each control point to a GLfloat array		
			point = this->_cpCollection.At(i*this->_cpU + j)->Data();
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
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, WCNurbsSurface::_cpTex);
	glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, this->_cpU, this->_cpV, GL_RGBA, GL_FLOAT, data);
	if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsSurface::GenerateControlPointsTexture Error - Texture Setup.\n";	
}


void WCNurbsSurface::LoadKnotPoints(const WCCollection<WPFloat> *uColl, const WCCollection<WPFloat> *vColl) {
	//Load the U knot point array
	if (this->_modeU == WCNurbsMode::Default())
		this->_knotPointsU = WCNurbs::LoadDefaultKnotPoints(this->_kpU, this->_degreeU);
	else if (this->_modeU == WCNurbsMode::Bezier())
		this->_knotPointsU = WCNurbs::LoadBezierKnotPoints(this->_kpU);
	else if (this->_modeU == WCNurbsMode::Custom())
		this->_knotPointsU = WCNurbs::LoadCustomKnotPoints(uColl);	

	//Load the V knot point array
	if (this->_modeV == WCNurbsMode::Default())
		this->_knotPointsV = WCNurbs::LoadDefaultKnotPoints(this->_kpV, this->_degreeV);
	else if (this->_modeV == WCNurbsMode::Bezier())
		this->_knotPointsV = WCNurbs::LoadBezierKnotPoints(this->_kpV);
	else if (this->_modeV == WCNurbsMode::Custom())
		this->_knotPointsV = WCNurbs::LoadCustomKnotPoints(vColl);		
}
	

void WCNurbsSurface::LoadControlPoints(const WCCollection<WCControlPoint*> *controlPoints) {
	//Empty the current control point collection
	while(this->_cpCollection.Size() > 0) {
		this->_cpCollection.At(0)->Release(*this);
		this->_cpCollection.Remove(0);
	}
	//Clear the old collection
	this->_cpCollection.Clear();
	//Copy the contents of the control point collection
	this->_cpCollection = (*controlPoints);
	//Loop through all control points
	for (int i=0; i<(this->_cpU*this->_cpV); i++) {
		//Retain each control point
		this->_cpCollection.At(i)->Retain(*this);
	}	
}


void WCNurbsSurface::GenerateSurfaceHigh(void) {
	//Check for special generation cases, otherwise set the program
	if ((this->_cpCollection.Size() == 4) && (this->_numVerts <= 225)) return this->GenerateSurfaceSize4();
//	if ((this->_degreeU == 1) && (this->_degreeV = 1)) return this->GenerateSurfaceOne();

	/*** Setup programs and bindable uniforms ***/

	//Now generate the control points VBO
	this->GenerateControlPointsVBO();
	//Create a knotpoint VBO
	this->GenerateKnotPointsVBOs();
	
	//Set the program and some uniforms
	if ((this->_degreeU <= 3) && (this->_degreeV <= 3) && (this->_modeU == WCNurbsMode::Bezier()) && (this->_modeV == WCNurbsMode::Bezier())) {
		std::cout << "Using Bezier23\n";
		glUseProgram(WCNurbsSurface::_bezier23);
		glUniform4i(WCNurbsSurface::_locations[NURBSSURFACE_LOC_PARAMSU_BEZIER23], this->_degreeU, this->_cpU, this->_kpU, 0.0);
		glUniform4i(WCNurbsSurface::_locations[NURBSSURFACE_LOC_PARAMSV_BEZIER23], this->_degreeV, this->_cpV, this->_kpV, 0.0);
	//Otherwise use the default case
	} else {
		std::cout << "Using Default\n";
		glUseProgram(WCNurbsSurface::_default);	
		glUniform4i(WCNurbsSurface::_locations[NURBSSURFACE_LOC_PARAMSU_DEFAULT], this->_degreeU, this->_cpU, this->_kpU, 0.0);		
		glUniform4i(WCNurbsSurface::_locations[NURBSSURFACE_LOC_PARAMSV_DEFAULT], this->_degreeV, this->_cpV, this->_kpV, 0.0);								
	}	
	//Make sure no problems so far
	if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsSurface::GenerateSurfaceHigh Error - Setup Problems.\n";
	
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
	if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsSurface::GenerateSurfaceeHigh Error - Output buffers setup.\n";

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
	for (int r=0; r <this->_lodV; r++) {
		//Reset u each loop
		u = this->_knotPointsU[0];
		//Loop through each batch on each line
		for (int i=0; i<this->_lodU; i++) {
			//Load array with value for each batch
			data[index++]	= u;							//Set first position to u
			data[index++]	= v;							//Set second position to du
			data[index++]	= 0.0;							//Set third position to nothing
			data[index++]	= 0.0;							//Set fourth position to nothing
			//Increment u
			u += du;					
		}
		//Increment dv
		v += dv;
	}
	for (int i=0; i<this->_numVerts; i++) printf("%d: %f %f %f %f\n", i, data[i*4], data[i*4+1], data[i*4+2], data[i*4+3]);
	//Create a temporary input buffer
	GLuint inputBuffer;
	glGenBuffers(1, &inputBuffer);
	//Set up transform feedback	buffer
	glBindBuffer(GL_ARRAY_BUFFER, inputBuffer);
	//Allocate space for the batch data and load data value array
	glBufferData(GL_ARRAY_BUFFER, this->_numVerts * 4 * sizeof(GLfloat), data, GL_STATIC_DRAW);
	if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsSurface::GenerateSurfaceHigh Error - Input buffer setup.\n";

	/*** Time to Generate ***/
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnable(GL_RASTERIZER_DISCARD_EXT);
	glBeginTransformFeedbackEXT(GL_POINTS);
	if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsSurface::GenerateSurfaceHigh Error - Begin Transform Feedback.\n";
	
	glVertexPointer(4, GL_FLOAT, 4 * sizeof(GLfloat), 0);
	glDrawArrays(GL_POINTS, 0, this->_numVerts);
	if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsSurface::GenerateSurfaceHigh Error - Draw Arrays.\n";
	
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
	if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsSurface::GenerateSurfaceHigh Error - Unspecified GL Error.\n";
}


void WCNurbsSurface::GenerateSurfaceMedium(void) {
	//Check for special generation cases, otherwise set the program
	if ((this->_cpCollection.Size() == 4) && (this->_numVerts <= NURBSSURFACE_SIZE4_CUTOFF)) return this->GenerateSurfaceSize4();
//	if ((this->_degreeU == 1) && (this->_degreeV = 1)) return this->GenerateSurfaceOne();
	if ((this->_lodU > WCNurbsSurface::_maxTexSize) || (this->_lodV > WCNurbsSurface::_maxTexSize)) {
		std::cout << "WCNurbsSurface::GenerateSurfaceMedium Warning - LOD exceeds hardware Maximum Texture Size.\n";
		return this->GenerateSurfaceLow();
	}

	/*** Setup programs and texture locations ***/
	
	if ((this->_degreeU <= 3) && (this->_degreeV <= 3)) {
		//Degree 2 & 3 Bezier
		if ((this->_modeU == WCNurbsMode::Bezier()) && (this->_modeV == WCNurbsMode::Bezier())) {
//			std::cout << "Using Bezier23\n";
			glUseProgram(WCNurbsSurface::_bezier23);
			glUniform4i(WCNurbsSurface::_locations[NURBSSURFACE_LOC_PARAMSU_BEZIER23], this->_degreeU, this->_cpU, this->_kpU, 0);
			glUniform4i(WCNurbsSurface::_locations[NURBSSURFACE_LOC_PARAMSV_BEZIER23], this->_degreeV, this->_cpV, this->_kpV, 0);			
		}
		//Degree 2 & 3 Default and Custom
		else {
//			std::cout << "Using Default23\n";
			glUseProgram(WCNurbsSurface::_default23);
			glUniform4i(WCNurbsSurface::_locations[NURBSSURFACE_LOC_PARAMSU_DEFAULT23], this->_degreeU, this->_cpU, this->_kpU, 0);
			glUniform4i(WCNurbsSurface::_locations[NURBSSURFACE_LOC_PARAMSV_DEFAULT23], this->_degreeV, this->_cpV, this->_kpV, 0);
		}
	//Otherwise use the default case
	} else {
//		std::cout << "Using Default\n";
		glUseProgram(WCNurbsSurface::_default);	
		glUniform4i(WCNurbsSurface::_locations[NURBSSURFACE_LOC_PARAMSU_DEFAULT], this->_degreeU, this->_cpU, this->_kpU, 0);		
		glUniform4i(WCNurbsSurface::_locations[NURBSSURFACE_LOC_PARAMSV_DEFAULT], this->_degreeV, this->_cpV, this->_kpV, 0);								
	}
	if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsSurface::GenerateCurveMedium Error - Programs did not initialize.\n";
	//Now generate the control points texture
	this->GenerateControlPointsTexture();
	//Create a knotpoint texture
	this->GenerateKnotPointsTextures();

	/*** Setup framebuffer object ***/

	//Bind to framebuffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, WCNurbsSurface::_framebuffer);
	//Setup draw buffers
	GLenum buffers[] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT };
	glDrawBuffers(2, buffers);
	//Check to make sure the framebuffer is ready
//	GLenum retVal = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	//Check the status of the framebuffer object
//	if (retVal != GL_FRAMEBUFFER_COMPLETE_EXT) { std::cout << "WCNurbsSurface::GenerateCurveMedium Error - Framebuffer is not complete.\n"; return; }	
	
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
	for (int i=0; i<this->_lodV; i++) {
		//Make sure to reset u
		u = 0;
		for (int j=0; j<this->_lodU; j++) {
			data[index++] = u;				//Set first position to u
			data[index++] = v;				//Set second position to v
			data[index++] = 0.0;			//Set third position to 0.0
			data[index++] = 0.0;			//Set fourth position to 0.0
			uvData[uvIndex++] = u;			//Set the u coordinate
			uvData[uvIndex++] = v;			//Set the v coordinate
			//Increment u
			u = std::min(u+du, this->_knotPointsU[this->_kpU-1]);
		}
		//Selectively Increment v
		if (i < this->_lodV) v = std::min(v+dv, this->_knotPointsV[this->_kpV-1]);
	}
//	for (int i=0; i<this->_numVerts; i++) printf("%d: %f %f %f %f\n", i, data[i*4], data[i*4+1], data[i*4+2], data[i*4+3]);
	//Setup and copy the data into the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_RECTANGLE_ARB, WCNurbsSurface::_inTex);
	glTexSubImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, 0, 0, this->_lodU, this->_lodV, GL_RGBA, GL_FLOAT, data);
	//Copy data into TexCoord Buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_TEXCOORD_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, this->_numVerts * 2 * sizeof(GLfloat), uvData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsSurface::GenerateSurfaceMedium Error - Setup Input Texture.\n";	
	
	/*** Setup and render ***/

#ifndef __WILDCAT_SCENE_INCLUDED__
	//Get the current polygon mode
	GLint polyMode[2];
	glGetIntegerv(GL_POLYGON_MODE, polyMode);
	//Get the current viewport
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
#endif
	//Disable some settings
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	//Set up the viewport and polygon mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glViewport(0, 0, WCNurbsSurface::_maxTexSize, WCNurbsSurface::_maxTexSize);
	GLfloat w = this->_lodU * 2.0 / WCNurbsSurface::_maxTexSize - 1.0;
	GLfloat h = this->_lodV * 2.0 / WCNurbsSurface::_maxTexSize - 1.0;	
	GLfloat quad[] = { -1.0, -1.0, -1.0, h, w, h, w, -1.0 };
	//Turn on vertex arrays
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, quad);
	glDrawArrays(GL_QUADS, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
	//Restore the viewport and polygon mode
#ifdef __WILDCAT_SCENE_INCLUDED__
	//Reset viewport
	glViewport(0, 0, this->_layer->Scene()->WindowWidth(), this->_layer->Scene()->WindowHeight());
	//Reset polygon mode
	if (this->_layer->Scene()->ActiveCamera()->RenderState()->PolygonMode() == GL_LINE)
		this->_layer->Scene()->ActiveCamera()->RenderState()->PolygonMode(GL_LINE);
#else
	//Reset the viewport
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	//Reset the polygon mode
	if (polyMode[0] == GL_LINE)	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
	//Re-enable some settings
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);	
//	if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsSurface::GenerateSurfaceMedium Error - Render.\n";		
			
	/*** Save output textures into vertex and normal VBOs ***/
		
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
	if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsSurface::GenerateSurfaceMedium Error - Cleanup .\n";
}


void WCNurbsSurface::GenerateSurfaceLow(void) {
	//Check for special generation cases, otherwise set the program
	if (this->_cpCollection.Size() == 4) return this->GenerateSurfaceSize4();
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
	for (int i=0; i<this->_lodV; i++) {
		//Reset u each loop
		u = this->_knotPointsU[0];
		//Loop through u
		for (int j=0; j<this->_lodU; j++) {
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
			for(int k=0; k<=this->_degreeV; k++) {
				//Determine the index of the control point
				index = (this->_cpU * (spanV - this->_degreeV + k)) + spanU - this->_degreeU;
				for (int l=0; l<=this->_degreeU; l++) {
					pt = this->_cpCollection.At(index)->Data();
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
			u = std::min(u+du, this->_knotPointsU[this->_kpU-1]);				
		}
		v = std::min(v+dv, this->_knotPointsV[this->_kpV-1]);
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
	if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsSurface::GenerateSurfaceLow Error - Unspecified error.\n";
}


void WCNurbsSurface::GenerateSurfaceOne(void) {
	std::cout << "WCNurbsSurface::GenerateSurfaceOne Error - Not yet implemented.\n";
}


void WCNurbsSurface::GenerateSurfaceSize4(void) {
	WCVector4 p0 = this->_cpCollection.At(0)->Data();
	WCVector4 p1 = this->_cpCollection.At(1)->Data();
	WCVector4 p2 = this->_cpCollection.At(2)->Data();
	WCVector4 p3 = this->_cpCollection.At(3)->Data();
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
	for(int i=0; i<this->_lodV; i++) {
		//Reset u each loop
		u = this->_knotPointsU[0];
		for (int j=0; j<this->_lodU; j++) {
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
			u = std::min(u+du, this->_knotPointsU[this->_kpU-1]);				
		}
		v = std::min(v+dv, this->_knotPointsV[this->_kpV-1]);
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
	if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsSurface::GenerateSurfaceSize4 Error - Unspecified error.\n";	
}


void WCNurbsSurface::GenerateIndex(void) {
	//Allocate space for local array of data
	GLuint size = (this->_lodU-1) * (this->_lodV-1) * 2 * 3;  //Two triangles per lod, 3 verts per triangle
	GLuint data[size];			
	//Check to make sure data was allocated
	if (data == NULL) {	std::cout << "WCNurbsSurface::GenerateIndex Error - Unable to allocate space for index data.\n"; return; }
	int ll, index = 0;
	//Loop through each line
	for(int v=0; v<this->_lodV-1; v++) {
		//Loop through each element in the line
		for (int u=0; u<this->_lodU-1; u++) {
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
	if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsSurface::GenerateIndex Error - Unspecified error.\n";
}


/***********************************************~***************************************************/


WCNurbsSurface::WCNurbsSurface(const WPUInt degreeU, const WPUInt degreeV, const WPUInt cpU, const WPUInt cpV, 
					const WCCollection<WCControlPoint*> *cpCollection, const WCNurbsMode &modeU, const WCNurbsMode &modeV,
					const WCCollection<WPFloat> *kpU, const WCCollection<WPFloat> *kpV) : 
	::WCGeometricSurface(), _lodU(0), _lodV(0), _numVerts(0), _degreeU(degreeU), _degreeV(degreeV), _modeU(modeU), _modeV(modeV), 
	_cpU(cpU), _cpV(cpV), _kpU(0), _kpV(0), _knotPointsU(NULL), _knotPointsV(NULL) {
	//Make sure static function is called
	WCNurbsSurface::Start();
	
	//Check to make sure a CP collection was passed
	if (cpCollection == NULL) { std::cout << "WCNurbsSurface::WCNurbsSurface Error - Invalid control points collection.\n"; return;	}
	//Check to make sure there are at least 2 control points
	if ((this->_cpU < 2) || (this->_cpV < 2)) {
		std::cout << "WCNurbsSurface::WCNurbsSurface Error - Must have at least two control points in each direction.\n";
		return; }
	//Make sure not too many control points
	if ((this->_cpU * this->_cpV) > NURBSSURFACE_MAX_CONTROLPOINTS) {
		std::cout << "WCNurbsSurface::WCNurbsSurface Error - Too many control points.\n";
		return; }
	//Check to make sure that degree is greater than 0
	if ((this->_degreeU == 0) || (this->_degreeV == 0)) {
		std::cout << "WCNurbsSurface::WCNurbsSurface Error - Degree must be greater than 0.\n";
		if (this->_degreeU == 0) this->_degreeU = 1;
		if (this->_degreeV == 0) this->_degreeU = 1;
	}
	//Check to make sure degree is less than the number of control points
	if ((this->_degreeU >= this->_cpU) || (this->_degreeV >= this->_cpV)) {
		std::cout << "WCNurbsSurface::WCNurbsSurface Error - Degree must be less than the number of control points.\n";
		this->_degreeU = std::min(this->_cpU - 1, this->_degreeU);
		this->_degreeV = std::min(this->_cpV - 1, this->_degreeV);	
	}
	//Define the number of knot points
	this->_kpU = this->_degreeU + this->_cpU + 1;
	this->_kpV = this->_degreeV + this->_cpV + 1;
	//Perform some addtional checks if mode is custom
	if (this->_modeU == WCNurbsMode::Custom()) {
		//See if the collection is null
		if (kpU == NULL) { std::cout << "WCNurbsSurface::WCNurbsSurface Error - Invalid U knot points collection.\n"; return; }
		//Make sure the collection is the right size
		if (kpU->Size() != this->_kpU) { std::cout << "WCNurbsSurface::WCNurbsSurface Error - Invalid U knot points collection size.\n"; return; }
	}
	if (this->_modeV == WCNurbsMode::Custom()) {
		//See if the collection is null
		if (kpV == NULL) { std::cout << "WCNurbsSurface::WCNurbsSurface Error - Invalid V knot points collection.\n"; return; }
		//Make sure the collection is the right size
		if (kpV->Size() != this->_kpV) { std::cout << "WCNurbsSurface::WCNurbsSurface Error - Invalid V knot points collection size.\n"; return; }
	}

	//Set up buffers
	glGenBuffers(4, this->_buffers);
	//Set up control point collection
	this->LoadControlPoints(cpCollection);
	//Use knot mode to setup knot array
	this->LoadKnotPoints(kpU, kpV);
	//Estimate LOD values
	WPFloat length = WCNurbs::EstimateLengthU(this->_cpCollection, this->_cpU);
	this->_lodU = std::max( (int)(length / NURBSSURFACE_GENERATE_ACCURACY) + 1, 2);
	length = WCNurbs::EstimateLengthV(this->_cpCollection, this->_cpV);
	this->_lodV = std::max( (int)(length / NURBSSURFACE_GENERATE_ACCURACY) + 1, 2);
	this->_numVerts = this->_lodU * this->_lodV;
	//Validate surface flags
	this->ValidateClosure();
	this->ValidateSelfIntersection();
	//Establish aligned bounding box
	this->_bounds = new WCAlignedBoundingBox(this->_cpCollection);
	//Doe some error checking
	if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsSurface::WCNurbsSurface Error - Unspecified error.\n";
}


WCNurbsSurface::~WCNurbsSurface() {
	//Call to the static class deinitializer
	WCNurbsSurface::Stop();
	//Delete the buffers
	glDeleteBuffers(5, this->_buffers);
	//Release all control points
	for (int i=0; i<this->_cpU * this->_cpV; i++) this->_cpCollection.At(i)->Release(*this);
	//Clear the collection of control points
	this->_cpCollection.Clear();
	//Delete the knot point array
	if (this->_knotPointsU != NULL) delete this->_knotPointsU;
	if (this->_knotPointsV != NULL) delete this->_knotPointsV;
	//Check for errors
	if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsSurface::~WCNurbsSurface Error - Unspecified error.\n";
}


void WCNurbsSurface::Degree(WPUInt degreeU, WPUInt degreeV) {
	if ((degreeU > NURBSSURFACE_MAX_DEGREE) || (degreeV > NURBSSURFACE_MAX_DEGREE)) {
		std::cout << "WCNurbsSurface::Degree Error - Degree must be less than NURBSSURFACE_MAX_DEGREE.\n";
		degreeU = MIN(NURBSSURFACE_MAX_DEGREE, degreeU);
		degreeV = MIN(NURBSSURFACE_MAX_DEGREE, degreeV);
	}
	//Check to make sure degree is less than the number of control points
	if ((degreeU >= this->_cpU) || (degreeV >= this->_cpV)) {
		std::cout << "WCNurbsSurface::Degree Error - Degree must be less than the number of control points.\n";
		degreeU = std::min(this->_cpU - 1, degreeU);
		degreeV = std::min(this->_cpV - 1, degreeV);
	}
	if ((degreeU < 1) || (degreeV < 1)) {
		std::cout << "WCNurbsSurface::Degree Error - Degree must be greater than 1.\n";
		degreeU = MAX(degreeU, 1);
		degreeV = MAX(degreeV, 1);
	}
	//Don't let degree change if mode is Custom
	if ( (this->_modeU == WCNurbsMode::Custom()) || (this->_modeU == WCNurbsMode::Custom())) {
		std::cout << "WCNurbsSurface::Degree Warning - Degree change not allowed in custom knot mode.\n";
		return;
	}
	//Set the new degree
	this->_degreeU = degreeU;
	this->_degreeV = degreeV;
	//Determine the correct number of knot points
	this->_kpU = this->_degreeU + this->_cpU + 1;
	this->_kpV = this->_degreeV + this->_cpV + 1;
	//Use knot mode to setup knot array
	this->LoadKnotPoints();
/*** Debug ***/
	std::cout << "Degrees: " << this->_degreeU << " " << this->_degreeV << std::endl;
/*** Debug ***/
	//Mark the object as dirty
	this->_isVisualDirty = true;
}


void WCNurbsSurface::LevelOfDetail(WPUInt lodU, WPUInt lodV) {
	//Check to make sure lodU is valid
	if ((lodU < 2) || (lodU > NURBSSURFACE_MAX_LOD)) {
		std::cout << "WCNurbsSurface::LevelOfDetail Error - LOD U(" << lodU << ") out of bounds.\n";
		//throw error
		return;
	}
	//Check to make sure lodV is valid
	if ((lodV < 2) || (lodV > NURBSSURFACE_MAX_LOD)) {
		std::cout << "WCNurbsSurface::LevelOfDetail Error - LOD V(" << lodV << ") out of bounds.\n";
		//throw error
		return;
	}
	//Set the new lod
	this->_lodU = lodU;
	this->_lodV = lodV;
	this->_numVerts = this->_lodU * this->_lodV;
/*** Debug ***/
//	std::cout << "LOD: " << this->_lodU << " " << this->_lodV << std::endl;	
/*** Debug ***/
	//Mark the object as dirty
	this->_isVisualDirty = true;
}


void WCNurbsSurface::IsSelected(const bool status) {
	//Set the selected status
//	this->_isSelected = status;
	//If the surface is selected set
	if (status)	this->_color.Set(0.75, 0.75, 0.75, 1.0);
	//Otherwise set color depending on mouse over
//	else this->IsMouseOver(this->_isMouseOver);
}


void WCNurbsSurface::IsMouseOver(const bool status) {
	//Set the mouse over status
//	this->_isMouseOver = status;
	//Do nothing if surface is already selected
//	if (this->_isSelected) return;
	//If the mouse is over, set
	if (status)	this->_color.Set(0.75, 1.0, 0.75, 1.0);
	//Otherwise set color to default white
	else this->_color.Set(1.0, 1.0, 1.0, 1.0);
}


bool WCNurbsSurface::Intersect(const WCGeometricPoint &point, const WPFloat tolerance) {
	return false;
}


bool WCNurbsSurface::Intersect(const WCGeometricCurve &curve, const WPFloat tolerance) {
	return false;
}


bool WCNurbsSurface::Intersect(const WCGeometricSurface &surface, const WPFloat tolerance) {
	return false;
}


WPFloat WCNurbsSurface::Area(void) {
	return 0.0;
}


WCVisualObject* WCNurbsSurface::HitTest(const WCRay &ray, const WPFloat tolerance) {
	std::cout << "WCNurbsSurface::HitTest Error - Not yet implemented.\n";
	return NULL;
}


void WCNurbsSurface::ApplyTransform(const WCMatrix4 &transform) {
	std::cout << "WCNurbsSurface::ApplyTransform Error - Not yet implemented.\n";
}


void WCNurbsSurface::ApplyTranslation(const WCVector4 &translation) {
	std::cout << "WCNurbsSurface::ApplyTranslation Error - Not yet implemented.\n";
}


void WCNurbsSurface::Render(const GLuint defaultProg, const WCColor color) {
	//Make sure surface is visible
	if (!this->_isVisible) return;
	//Check to see if curve needs to be generated
	if (this->_isVisualDirty) {
		//Generate the curve - switch on performance level
		switch(WCNurbsSurface::_perfLevel) {
			case NURBSSURFACE_PERFLEVEL_HIGH:	this->GenerateSurfaceHigh(); break;
			case NURBSSURFACE_PERFLEVEL_MEDIUM: this->GenerateSurfaceMedium(); break;
			case NURBSSURFACE_PERFLEVEL_LOW:	this->GenerateSurfaceLow(); break;
		}
		//Generate the index for the curve
		this->GenerateIndex();
		//Update the bounding box
		this->_bounds->Set(this->_buffers[NURBSSURFACE_VERTEX_BUFFER], this->_numVerts);		
		//Mark as clean
		this->_isVisualDirty = false;
	}
	//Set the rendering program
	if (this->_renderProg != 0) {
		glUseProgram(this->_renderProg);
#ifdef __WILDCAT_SCENE_INCLUDED__
		this->_layer->Scene()->ActiveCamera()->EnableLighting(this->_renderProg);
#endif
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
	if (glGetError() != GL_NO_ERROR) std::cout << "WCNurbsSurface::Render Error - Unspecified error.\n";
}


void WCNurbsSurface::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
	//Mark the surface as dirty
	this->_isVisualDirty = true;
	//Estimate LOD values
	WPFloat length = WCNurbs::EstimateLengthU(this->_cpCollection, this->_cpU);
	this->_lodU = (length / NURBSSURFACE_GENERATE_ACCURACY) + 1;
	length = WCNurbs::EstimateLengthV(this->_cpCollection, this->_cpV);
	this->_lodV = (length / NURBSSURFACE_GENERATE_ACCURACY) + 1;
	this->_numVerts = this->_lodU * this->_lodV;	
	//Make sure all dependent objects know about it
	this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);	
}

	
WCVector4 WCNurbsSurface::Evaluate(const WPFloat u, const WPFloat v) {
	//Bounds check the u and v values
	if ((u < this->_knotPointsU[0]) || (u > this->_knotPointsU[this->_kpU-1]) || (u < this->_knotPointsU[0]) || (u > this->_knotPointsU[this->_kpU-1])) {
		std::cout << "WCNurbsSurface::Evaluate Error - U or V out of bounds.\n";
		return WCVector4();
	}
	//Otherwise, find the span for the u and v values
	WPUInt spanU = WCNurbs::FindSpan(this->_cpU, this->_degreeU, u, this->_knotPointsU);
	WPUInt spanV = WCNurbs::FindSpan(this->_cpV, this->_degreeV, v, this->_knotPointsV);
	//Calculate basis values for U and V
	WPFloat* basisValuesU = WCNurbs::BasisValues(spanU, u, this->_degreeU, this->_knotPointsU);
	if (basisValuesU == NULL) return WCVector4();		
	WPFloat* basisValuesV = WCNurbs::BasisValues(spanV, v, this->_degreeV, this->_knotPointsV);	
	if (basisValuesV == NULL) { delete basisValuesU; return WCVector4(); }
	//Declare some temparary values
	WCVector4 c;
	int index;
	WPFloat w=0;
	WCGeometricPoint *point;
	//Evaluate the point
	for(int i=0; i<=this->_degreeV; i++) {
		//Determine the index of the control point
		index = (this->_cpU * (spanV - this->_degreeV + i)) + spanU - this->_degreeU;
		for (int j=0; j<=this->_degreeU; j++) {
			point = this->_cpCollection.At(index);			
			w = w + (point->W() * basisValuesU[j] * basisValuesV[i]);
			c = c + point->Data() * point->W() * basisValuesU[j] * basisValuesV[i];
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



WCVector4 WCNurbsSurface::Derivative(const WPFloat u, const WPUInt uDer, const WPFloat v, const WPUInt vDer) {
	std::cout << "WCNUrbsSurface::Derivative Error - Not yet implemented.\n";
	return WCVector4();
}


WCRay WCNurbsSurface::Tangent(const WPFloat u, const WPFloat v) {
	std::cout << "WCNUrbsSurface::Tangent Error - Not yet implemented.\n";	
	return WCRay(WCVector4(), WCVector4());
}


WCVector4 WCNurbsSurface::PointInversion(const WCGeometricPoint &point) {
	//Special case for degree=1 curves
	if ((this->_degreeU == 1) && (this->_degreeV == 1)) {
		std::cout << "WCNurbsSurface::PointInversion Error - Not yet implemented for degree=1 surfaces.\n";
		return WCVector4();
	}
	//Check to see if surface needs to be generated
	if (this->_isVisualDirty) {
		//Generate the curve - switch on performance level
		switch(WCNurbsSurface::_perfLevel) {
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
	WCVector4 refPoint = point.Data();
	refPoint.K(0.0);
	WCVector4 curvePoint;
	int i, j, index=-1;
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
	WCGeometricPoint* pt;
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
				pt = this->_cpCollection.At(index);			
				s = s + (pt->Data() * bvU[j] * bvV[i]);
				sU = sU + (pt->Data() * bvU[j+this->_degreeU+1] * bvV[i]);
				sV = sV + (pt->Data() * bvU[j] * bvV[i+this->_degreeV+1]);
				sUU = sUU + (pt->Data() * bvU[j+2*(this->_degreeU+1)] * bvV[i]);
				sVV = sVV + (pt->Data() * bvU[j] * bvV[i+2*(this->_degreeV+1)]);
				sUV = sUV + (pt->Data() * bvU[j+this->_degreeU+1] * bvV[i+this->_degreeV+1]);
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
			std::cout << "SCNurbsSurface::PointInverse Error - |A| is zero.\n";
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
	
	
void WCNurbsSurface::InsertKnotU(const WPFloat u, const WPUInt multiplicity) {
}


void WCNurbsSurface::InsertKnotV(const WPFloat v, const WPUInt multiplicity) {
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


DOMElement* WCNurbsSurface::Serialize(DOMDocument *document) {
	//Create the base element for the object
	DOMElement*  surfaceElem = document->createElement(XMLString::transcode("NURBSSurface"));
	return surfaceElem;
}


WCNurbsSurface* WCNurbsSurface::Deserialize(DOMElement* obj) {
	return NULL;
}


bool WCNurbsSurface::Validate(DOMElement* obj) {
	return false;
}


/***********************************************~***************************************************/


WCNurbsSurface* WCNurbsSurface::ExtrudeCurve(WCGeometricCurve* curve, const WCVector4 &direction, const WPFloat &posDepth, 
	const WPFloat &negDepth, const bool &aligned) {
//		std::cout << "Aligned: " << aligned << std::endl;
		//Make sure curve is not null
		if (curve == NULL) return NULL;
		WCGeometricLine* line = dynamic_cast<WCGeometricLine*>(curve);
		//See if curve is a simple line
		if (line != NULL) {
			//Create collection of control points
			WCCollection<WCControlPoint*> cp;
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
			WCControlPoint* cp0 = new WCControlPoint(pt0);
			WCControlPoint* cp1 = new WCControlPoint(pt1);
			WCControlPoint* cp2 = new WCControlPoint(pt2);
			WCControlPoint* cp3 = new WCControlPoint(pt3);
			cp.Add(cp0);
			cp.Add(cp1);
			cp.Add(cp2);
			cp.Add(cp3);
			//Create the curve
			WCNurbsSurface *surface = new WCNurbsSurface(1, 1, 2, 2, &cp, WCNurbsMode::Default(), WCNurbsMode::Default());
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
			WCCollection<WCControlPoint*> oldCP = nurbs->ControlPoints();
			WCCollection<WCControlPoint*> cp;
			WCCollection<WPFloat> *kp = WCCollection<WPFloat>::CollectionFromArray(nurbs->NumberKnotPoints(), nurbs->KnotPoints());
			WCControlPoint* controlPoint;
			WCVector4 data;
			//Check alignment
			if (aligned) {
				//First loop
				for (int j=0; j<curveNumCP; j++) {
					data = oldCP.At(j)->Data() + (direction * negDepth);
					controlPoint = new WCControlPoint( data );
					cp.Add(controlPoint);
				}
				//Second loop
				for (int j=0; j<curveNumCP; j++) {
					data = oldCP.At(j)->Data() + ((direction * negDepth * 0.5) + (direction * posDepth * 0.5));
					controlPoint = new WCControlPoint( data );
					cp.Add(controlPoint);
				}
				//Third loop
				for (int j=0; j<curveNumCP; j++) {
					data = oldCP.At(j)->Data() + (direction * posDepth);
					controlPoint = new WCControlPoint( data );
					cp.Add(controlPoint);
				}
			}
			//Reverse alignment
			else {
				//First loop
				for (int j=curveNumCP-1; j>=0; j--) {
					data = oldCP.At(j)->Data() + (direction * negDepth);
					controlPoint = new WCControlPoint( data );
					cp.Add(controlPoint);
				}
				//Second loop
				for (int j=curveNumCP-1; j>=0; j--) {
					data = oldCP.At(j)->Data() + ((direction * negDepth * 0.5) + (direction * posDepth * 0.5));
					controlPoint = new WCControlPoint( data );
					cp.Add(controlPoint);
				}
				//Third loop
				for (int j=curveNumCP-1; j>=0; j--) {
					data = oldCP.At(j)->Data() + (direction * posDepth);
					controlPoint = new WCControlPoint( data );
					cp.Add(controlPoint);
				}
			}

			WCNurbsMode curveMode = nurbs->Mode();
			WCNurbsSurface *surface = new WCNurbsSurface(curveDegree, 2, curveNumCP, 3, &cp, 
				curveMode, WCNurbsMode::Default(), kp, NULL);
			return surface;
		}
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCNurbsSurface &surface) {
	//Print out basic surface info
	out << "NurbsSurface(" << (void*)&surface << "):\n";
	out << "\t Verts: " << surface._numVerts << std::endl;
	out << "\t LOD U:" << surface._lodU << " V: " << surface._lodV << std::endl;
	out << "\t Degree(" << surface._degreeU << " & " << surface._degreeV << ")\n";	

	//Only print if collection has items
	if (surface._cpCollection.Size() != 0) {
		//Print out control point information
		out << "\t Control Points(" << surface._cpU << " x " <<  surface._cpV << ")\n";
		for (int i=0; i<surface._cpCollection.Size(); i++) {
			std::cout << "\t\t{" << surface._cpCollection.At(i)->X() << ", \t";
			std::cout << surface._cpCollection.At(i)->Y() << ", \t";
			std::cout << surface._cpCollection.At(i)->Z() << ", \t";
			std::cout << surface._cpCollection.At(i)->W() << "}\n";
		}
	}
	
	//Print out knot point information
	out << "\t Knot Points(" << surface._kpU << " & " << surface._kpV << ")\n";
	if (surface._knotPointsU != NULL)
		out << "\t\t U: "; for (int i=0; i < surface._kpU; i++) out << surface._knotPointsU[i] << " "; out << std::endl;
	if (surface._knotPointsV != NULL)
		out << "\t\t V: "; for (int i=0; i < surface._kpV; i++) out << surface._knotPointsV[i] << " "; out << std::endl;
	return out;
}


/***********************************************~***************************************************/

