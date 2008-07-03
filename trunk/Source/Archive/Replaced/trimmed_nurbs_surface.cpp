#include "trimmed_nurbs_surface.h"
#include "shader_manager.h"
#include "adapter.h"
#include "curve_profile.h"
#include "nurbs_curve.h"


WPUInt WCTrimmedNurbsSurface::_count = 0;
GLuint WCTrimmedNurbsSurface::_visProgram = 0;
GLint WCTrimmedNurbsSurface::_texLocation = 0;
GLuint WCTrimmedNurbsSurface::_framebuffer = 0;
WPUInt WCTrimmedNurbsSurface::_perfLevel = TRIMSURFACE_PERFLEVEL_LOW;


/***********************************************~***************************************************/


WPUInt WCTrimmedNurbsSurface::Start(void) {
	//Check to make sure class is not already initialized
	if (WCTrimmedNurbsSurface::_count == 0) {
		//Determine performance level
		if (WCAdapter::HasGLEXTTransformFeedback() && WCAdapter::HasGLEXTBindableUniform() && 
			WCAdapter::HasGLEXTGeometryShader4() ) {//WCAdapter::HasGLEXTGPUShader4()
			//All criteria met for high performance
			WCTrimmedNurbsSurface::_perfLevel = TRIMSURFACE_PERFLEVEL_HIGH;
			std::cout << "WCTrimmedNurbsSurface::Start Debug - Performance set to high.\n";			
		}	
		else if (WCAdapter::HasGLARBFragmentShader() && WCAdapter::HasGLARBShadingLanguage100() &&
				 WCAdapter::HasGLARBTextureRectange() && WCAdapter::HasGLEXTTextureFloat() &&
				 WCAdapter::HasGLARBPixelBufferObject() && WCAdapter::HasGLEXTFramebufferObject()) {
			//All criteria met for medium performance
			WCTrimmedNurbsSurface::_perfLevel = TRIMSURFACE_PERFLEVEL_MEDIUM;
			std::cout << "WCTrimmedNurbsSurface::Start Debug - Performance set to medium.\n";
		}
		//Otherwise, remain at low level (see static initialization)
		else {
			std::cout << "WCTrimmedNurbsSurface::Start Debug - Performance set to low.\n";
		}
/*** Debug ***/		
		WCTrimmedNurbsSurface::_perfLevel = TRIMSURFACE_PERFLEVEL_LOW;
/*** Debug ***/
		//Compile Shaders
		switch(WCTrimmedNurbsSurface::_perfLevel) {
			//Set up programs for high performance mode
			case TRIMSURFACE_PERFLEVEL_HIGH:
				break;
			case TRIMSURFACE_PERFLEVEL_MEDIUM:
				break;
			default:
//				std::cout << "WCTrimmedNurbsSurface::Start Error - Invalid performance mode indicator.\n";
				break;			
		}

		//Compile Shaders for Visualization of Trim Surface
//		GLuint xlight =		CompileGLSLShaderFromFile(GL_VERTEX_SHADER,		"../../shaders/xform_light.vsh");
//		GLuint vis =		CompileGLSLShaderFromFile(GL_FRAGMENT_SHADER,	"../../shaders/trimsurface_vis.fsh");
		//First create the program objects
//		WCTrimmedNurbsSurface::_visProgram = glCreateProgram();
		
		//Attach vertex shader
//		glAttachShader(WCTrimmedNurbsSurface::_visProgram, xlight);
//		glAttachShader(WCTrimmedNurbsSurface::_visProgram, vis);
		//Link the generate program
//		glLinkProgram(WCTrimmedNurbsSurface::_visProgram);
//		GLint linked;
//		glGetProgramiv(WCTrimmedNurbsSurface::_visProgram, GL_OBJECT_LINK_STATUS_ARB, &linked);
//		if (!linked) {
//			char temp[256] = "";
//			//If not, print out compilation errors
//			glGetProgramInfoLog(WCTrimmedNurbsSurface::_visProgram, 256, NULL, temp);
//			std::cout << "\tTrimSurface Visualize Program link failed:\n" << temp << std::endl;
//			return 0;
//		}
//		else std::cout << "\tTrimSurface Visualize Program linked successfully.\n";

		//Set the locations of the uniform variable
		WCTrimmedNurbsSurface::_texLocation = glGetUniformLocation(WCTrimmedNurbsSurface::_visProgram, "alphaTexture");
		if (WCTrimmedNurbsSurface::_texLocation == -1) std::cout << "WCTrimmedNurbsSurface::Start Error - Not able to find uniform for texture.\n";

		//Create the trim texture framebuffer object
		glGenFramebuffersEXT(1, &(WCTrimmedNurbsSurface::_framebuffer));
		//Make sure to switch back to the default framebuffer
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

		//Make sure that vertex and normal arrays are enabled
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_INDEX_ARRAY);
	}
	//Return the count of existing NurbsSurfaces
	return WCTrimmedNurbsSurface::_count++;
}


WPUInt WCTrimmedNurbsSurface::Stop(void) {
	//Only close if object has already been initialized	
	if(WCTrimmedNurbsSurface::_count == 1) {
		//Delete programs
		glDeleteProgram(WCTrimmedNurbsSurface::_visProgram);
		//Delete the framebuffer object
		glDeleteFramebuffersEXT(1, &(WCTrimmedNurbsSurface::_framebuffer));		
		//Return the status
		return 0;
	}
	//Return the number of existing NurbsSurface instances
	return WCTrimmedNurbsSurface::_count--;
}


/***********************************************~***************************************************/

/*
void WCTrimmedNurbsSurface::GenerateTexture(void) {
	GLubyte checkImage[64][64][4];
	int c;
	
	for (int i=0; i<64; i++) {
		for (int j=0; j<64; j++) {
			c = ((((i & 0x8)==0)^((j & 0x8))==0))*255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}

	//Set up some parameters
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//Set up texture
	glBindTexture(GL_TEXTURE_2D, this->_trimTexture);
	//Set up texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
}
*/


void WCTrimmedNurbsSurface::GenerateTextureHigh(void) {
}


void WCTrimmedNurbsSurface::GenerateTextureMedium(void) {
}


void WCTrimmedNurbsSurface::GenerateTextureLow(void) {
	//Make sure no programs are active
	glUseProgram(0);	
	//Setup output texture
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_RECTANGLE_EXT, this->_trimTexture);	
	glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, GL_RGBA, this->_texWidth, this->_texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	//Make sure to unbind from the texture
	glBindTexture(GL_TEXTURE_RECTANGLE_EXT, 0);
	
	/*** Setup framebuffer ***/
	
	//Bind to the custom framebuffer
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, WCTrimmedNurbsSurface::_framebuffer);
	//Bind the output texture to the framebuffer object
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, this->_trimTexture, 0);
	//Check to make sure the framebuffer is ready
	GLenum retVal = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	//Check the status of the framebuffer object
	if (retVal != GL_FRAMEBUFFER_COMPLETE_EXT) { std::cout << "WCTrimmedNurbsSurface::GenerateTexture Error - Framebuffer is not complete.\n"; return; }

	/*** Setup viewport and matrices ***/

#ifdef __WILDCAT_SCENE_INCLUDED__
		//...
#else
	//Get the current viewport
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
#endif
	//Set up the viewport
	glViewport(0, 0, this->_texWidth, this->_texHeight);
	
	/*** Determine the points and draw line strips ***/
	
	//Draw into the framebuffer
	glColor4f(1.0, 1.0, 1.0, 1.0);
//	int lod;
//	WCNurbsCurve* curve;
	WCVector4 xyzPos, uvPos;
//	GLfloat *uvData;
	GLuint buffer;
	glGenBuffers(1, &buffer);
	//Go through all of the exterior curves
	for (int profileID=0; profileID < this->_exteriorCurves.Size(); profileID++) {
		//Go through all of the curves in the profile
//		for (int curveID=0; curveID < this->_exteriorCurves.At(profileID)->Size(); curveID++) {
//			//Set the current curve
//			curve = (WCNurbsCurve*)this->_exteriorCurves.At(curveID);
//			lod = curve->LevelOfDetail() + 1;
//			//Allocate space to hold the inverted points
//			uvData = new GLfloat[lod];
//			//Bind to the curve's points
//			glBindBuffer(GL_ARRAY_BUFFER, curve->VertexBuffer());
//			GLfloat* xyzData = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
//			//Invert the points and copy the results into uvData
//			for (int i=0; i<lod; i++) {
//				xyzPos.Set(xyzData[i*4], xyzData[i*4+1], xyzData[i*4+2]);
//				uvPos = this->PointInversion(xyzPos);
//				uvData[i*4]	  = (GLfloat)uvPos.I();
//				uvData[i*4+1] = (GLfloat)uvPos.J();
//				uvData[i*4+2] = (GLfloat)uvPos.K();
//				uvData[i*4+3] = 1.0;
//			}
//			//Unmap from the buffer
//			glUnmapBuffer(GL_ARRAY_BUFFER);
///*** Debug ***/			
//			//Set the color of the lines
//			glColor4f(1.0, 1.0, 1.0, 1.0);
///*** Debug ***/
//			//Draw the curve onto the texture
//			glBindBuffer(GL_ARRAY_BUFFER, buffer);
//			glBufferData(GL_ARRAY_BUFFER, lod*4*sizeof(GLfloat), uvData, GL_STATIC_DRAW);
//			//Make sure to delete the unneeded uvData array
//			delete uvData;
//			//Set up the vertex pointer info
//			glVertexPointer(4, GL_FLOAT, 0, 0);
//			//There are LOD vertices - draw them
//			glDrawArrays(GL_LINE_STRIP, 0, lod);
//		}
	}
	//Go through all of the interior curves now
	//...
	
	//Check for errors
	glutReportErrors();
		
	/*** Restore the viewport ***/	
		
#ifdef __WILDCAT_SCENE_INCLUDED__
		//...
#else	
	//Restore viewport and polygon mode
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
#endif
	
	/*** Clean up framebuffer and textures ***/
	
	//Unbind the framebuffer texture
	glBindTexture(GL_TEXTURE_2D, 0);	
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}


/***********************************************~***************************************************/

WCTrimmedNurbsSurface::WCTrimmedNurbsSurface(const WPUInt degreeU, const WPUInt degreeV, const WPUInt cpU, const WPUInt cpV, 
												const WCCollection<WCControlPoint*> *controlPoints,
												const WCNurbsMode &modeU, const WCNurbsMode &modeV,
												const WCCollection<WPFloat> *kpU, const WCCollection<WPFloat> *kpV) : 
	:: WCNurbsSurface(degreeU, degreeV, cpU, cpV, controlPoints, modeU, modeV, kpU, kpV),
	_trimTexture(0),  _texWidth(TRIMSURFACE_DEFAULT_TEX_WIDTH), _texHeight(TRIMSURFACE_DEFAULT_TEX_HEIGHT) {
	//Make sure static function is called
	WCTrimmedNurbsSurface::Start();	

	//Set up trim texture object
	glGenTextures(1, &(this->_trimTexture));
}


WCTrimmedNurbsSurface::~WCTrimmedNurbsSurface() {
	//Call to the static class deinitializer
	WCTrimmedNurbsSurface::Stop();
	//Delete texture
	glDeleteTextures(1, &(this->_trimTexture));
}


void WCTrimmedNurbsSurface::Render(GLuint defaultProg) {
	//Check to see if the surface is visible
	if (!this->_isVisible) return;
	//Check to see if curve needs to be generated
	if (this->_isDirty) {
		//Generate the surface and trim texture - switch on performance level
		switch(WCTrimmedNurbsSurface::_perfLevel) {
			case TRIMSURFACE_PERFLEVEL_HIGH:	
				this->GenerateSurfaceHigh(); 
				this->GenerateTextureHigh();
				break;
			case TRIMSURFACE_PERFLEVEL_MEDIUM:	
				this->GenerateSurfaceMedium(); 
				this->GenerateTextureMedium();
				break;
			case TRIMSURFACE_PERFLEVEL_LOW:		
				this->GenerateSurfaceLow(); 
				this->GenerateTextureLow();
				break;
		}
		//Generate the index for the curve
		this->GenerateIndex();
		//Mark as clean
		this->_isDirty = false;
	}
/*** Debug ***/	
	//Make sure to use the visualization program
	glUseProgram(this->_visProgram);
	//Set up the texture
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, this->_trimTexture);
	//Draw the geometry
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_TEXTURE_2D);	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_INDEX_BUFFER]);
	glBindBuffer(GL_ARRAY_BUFFER, this->_buffers[NURBSSURFACE_VERTEX_BUFFER]);
	glVertexPointer(4, GL_FLOAT, NURBSSURFACE_FLOATS_PER_VERTEX * sizeof(GLfloat), 0);
	glNormalPointer(GL_FLOAT, NURBSSURFACE_FLOATS_PER_VERTEX * sizeof(GLfloat), (GLvoid*)(4*sizeof(GLfloat)));
	glTexCoordPointer(2, GL_FLOAT, NURBSSURFACE_FLOATS_PER_VERTEX * sizeof(GLfloat), (GLvoid*)(7*sizeof(GLfloat)));
	//Draw elements (*2 for each triangle in a lod box, *3 for each vertex in a triangle)
	glDrawElements(GL_TRIANGLES, this->_lod*this->_lod*2*3, GL_UNSIGNED_INT, 0);	
	glDisable(GL_TEXTURE_2D);
	glutReportErrors();
/*** Debug ***/	
}
	

void WCTrimmedNurbsSurface::ReceiveNotice(WCObjectMsg msg, WCObject *sender) {
	//Mark the surface as dirty
	this->_isDirty = true;
	//Update any parents about dirtyness
	this->SendBroadcastNotice(OBJECT_NOTIFY_UPDATE);
}


void WCTrimmedNurbsSurface::AddTrimCurve(WCGeometricCurve *curve, const bool inside) {
	//Check to make sure curve is valid
	if (curve == NULL) {
		std::cout << "WCTrimmedNurbsSurface::AddTrimCurve Error - NULL curve refence passed.\n";
		return;
	}
	//Add curve into either inside or outside vectors
	if (inside) {
		//Add to the interior vector
		this->_interiorCurves.Add(curve);
	}
	else {
		//Add to the exterior vector
		this->_exteriorCurves.Add(curve);
	}
	//Retain the curve
	curve->Retain(*this);	
}


void WCTrimmedNurbsSurface::RemoveTrimCurve(const WCGeometricCurve *curve) {
}


DOMElement* WCTrimmedNurbsSurface::Serialize(DOMDocument *document) {
	//Create the base element for the object
	DOMElement*  trimmedElem = document->createElement(XMLString::transcode("TrimmedSurface"));
	return trimmedElem;
}


WCTrimmedNurbsSurface* WCTrimmedNurbsSurface::Deserialize(DOMElement* obj) {
	return NULL;
}


bool WCTrimmedNurbsSurface::Validate(DOMElement* obj) {
	return false;
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCTrimmedNurbsSurface &surface) {
	out << "Trimmed Nurbs Surface\n";
	return out;
}	


/***********************************************~***************************************************/

