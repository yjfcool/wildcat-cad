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
#include "Utility/wutil.h"


/*** Library Support ***/
#pragma comment( lib, "opengl32.lib" )			// Search For OpenGL32.lib While Linking
#pragma comment( lib, "glu32.lib" )				// Search For GLu32.lib While Linking
//#pragma comment( lib, "glaux.lib" )			// Search For GLaux.lib While Linking


/*** Locally Defined Values ***/
//None


/***********************************************~***************************************************/


// GL version 1.3 functions
PFNGLACTIVETEXTUREPROC							glActiveTexture = NULL;
PFNGLCLIENTACTIVETEXTUREPROC					glClientActiveTexture = NULL;
PFNGLMULTITEXCOORD1DPROC						glMultiTexCoord1d = NULL;
PFNGLMULTITEXCOORD1DVPROC						glMultiTexCoord1dv = NULL;
PFNGLMULTITEXCOORD1FPROC						glMultiTexCoord1f = NULL;
PFNGLMULTITEXCOORD1FVPROC						glMultiTexCoord1fv = NULL;
PFNGLMULTITEXCOORD1IPROC						glMultiTexCoord1i = NULL;
PFNGLMULTITEXCOORD1IVPROC						glMultiTexCoord1iv = NULL;
PFNGLMULTITEXCOORD1SPROC						glMultiTexCoord1s = NULL;
PFNGLMULTITEXCOORD1SVPROC						glMultiTexCoord1sv = NULL;
PFNGLMULTITEXCOORD2DPROC						glMultiTexCoord2d = NULL;
PFNGLMULTITEXCOORD2DVPROC						glMultiTexCoord2dv = NULL;
PFNGLMULTITEXCOORD2FPROC						glMultiTexCoord2f = NULL;
PFNGLMULTITEXCOORD2FVPROC						glMultiTexCoord2fv = NULL;
PFNGLMULTITEXCOORD2IPROC						glMultiTexCoord2i = NULL;
PFNGLMULTITEXCOORD2IVPROC						glMultiTexCoord2iv = NULL;
PFNGLMULTITEXCOORD2SPROC						glMultiTexCoord2s = NULL;
PFNGLMULTITEXCOORD2SVPROC						glMultiTexCoord2sv = NULL;
PFNGLMULTITEXCOORD3DPROC						glMultiTexCoord3d = NULL;
PFNGLMULTITEXCOORD3DVPROC						glMultiTexCoord3dv = NULL;
PFNGLMULTITEXCOORD3FPROC						glMultiTexCoord3f = NULL;
PFNGLMULTITEXCOORD3FVPROC						glMultiTexCoord3fv = NULL;
PFNGLMULTITEXCOORD3IPROC						glMultiTexCoord3i = NULL;
PFNGLMULTITEXCOORD3IVPROC						glMultiTexCoord3iv = NULL;
PFNGLMULTITEXCOORD3SPROC						glMultiTexCoord3s = NULL;
PFNGLMULTITEXCOORD3SVPROC						glMultiTexCoord3sv = NULL;
PFNGLMULTITEXCOORD4DPROC						glMultiTexCoord4d = NULL;
PFNGLMULTITEXCOORD4DVPROC						glMultiTexCoord4dv = NULL;
PFNGLMULTITEXCOORD4FPROC						glMultiTexCoord4f = NULL;
PFNGLMULTITEXCOORD4FVPROC						glMultiTexCoord4fv = NULL;
PFNGLMULTITEXCOORD4IPROC						glMultiTexCoord4i = NULL;
PFNGLMULTITEXCOORD4IVPROC						glMultiTexCoord4iv = NULL;
PFNGLMULTITEXCOORD4SPROC						glMultiTexCoord4s = NULL;
PFNGLMULTITEXCOORD4SVPROC						glMultiTexCoord4sv = NULL;
PFNGLLOADTRANSPOSEMATRIXFPROC					glLoadTransposeMatrixf = NULL;
PFNGLLOADTRANSPOSEMATRIXDPROC					glLoadTransposeMatrixd = NULL;
PFNGLMULTTRANSPOSEMATRIXFPROC					glMultTransposeMatrixf = NULL;
PFNGLMULTTRANSPOSEMATRIXDPROC					glMultTransposeMatrixd = NULL;
PFNGLSAMPLECOVERAGEPROC							glSampleCoverage = NULL;
PFNGLCOMPRESSEDTEXIMAGE3DPROC					glCompressedTexImage3d = NULL;
PFNGLCOMPRESSEDTEXIMAGE2DPROC					glCompressedTexImage2d = NULL;
PFNGLCOMPRESSEDTEXIMAGE1DPROC					glCompressedTexImage1d = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC				glCompressedTexSubImage3d = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC				glCompressedTexSubImage2d = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC				glCompressedTexSubImage1d = NULL;
PFNGLGETCOMPRESSEDTEXIMAGEPROC					glCompressedTexImage = NULL;


// GL version 1.5 functions
PFNGLGENQUERIESPROC								glGenQueries = NULL;
PFNGLDELETEQUERIESPROC							glDeleteQueries = NULL;
PFNGLISQUERYPROC								glIsQuery = NULL;
PFNGLBEGINQUERYPROC								glBeginQuery = NULL;
PFNGLENDQUERYPROC								glEndQuery = NULL;
PFNGLGETQUERYIVPROC								glGetQueryiv = NULL;
PFNGLGETQUERYOBJECTIVPROC						glGetQueryObjectiv = NULL;
PFNGLGETQUERYOBJECTUIVPROC						glGetQueryObjectuiv = NULL;
PFNGLBINDBUFFERPROC								glBindBuffer = NULL;
PFNGLDELETEBUFFERSPROC							glDeleteBuffers = NULL;
PFNGLGENBUFFERSPROC								glGenBuffers = NULL;
PFNGLISBUFFERPROC								glIsBuffer = NULL;
PFNGLBUFFERDATAPROC								glBufferData = NULL;
PFNGLBUFFERSUBDATAPROC							glBufferSubData = NULL;
PFNGLGETBUFFERSUBDATAPROC						glGetBufferSubData = NULL;
PFNGLMAPBUFFERPROC								glMapBuffer = NULL;
PFNGLUNMAPBUFFERPROC							glUnmapBuffer = NULL;
PFNGLGETBUFFERPARAMETERIVPROC					glGetBufferParameteriv = NULL;
PFNGLGETBUFFERPOINTERVPROC						glGetBufferPointerv = NULL;


// GL version 2.0 functions
PFNGLBLENDEQUATIONSEPARATEPROC					glBlendEquationSeparate = NULL;
PFNGLDRAWBUFFERSPROC							glDrawBuffers = NULL;
PFNGLSTENCILOPSEPARATEPROC						glStencilOpSeparate = NULL;
PFNGLSTENCILFUNCSEPARATEPROC					glStencilFuncSeparate = NULL;
PFNGLSTENCILMASKSEPARATEPROC					glStencilMaskSeparate = NULL;
PFNGLATTACHSHADERPROC							glAttachShader = NULL;
PFNGLBINDATTRIBLOCATIONPROC						glBindAttribLocation = NULL;
PFNGLCOMPILESHADERPROC							glCompileShader = NULL;
PFNGLCREATEPROGRAMPROC							glCreateProgram = NULL;
PFNGLCREATESHADERPROC							glCreateShader = NULL;
PFNGLDELETEPROGRAMPROC							glDeleteProgram = NULL;
PFNGLDELETESHADERPROC							glDeleteShader = NULL;
PFNGLDETACHSHADERPROC							glDetachShader = NULL;
PFNGLDISABLEVERTEXATTRIBARRAYPROC				glDisableVertexAttribArray = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC				glEnableVertexAttribArray = NULL;
PFNGLGETACTIVEATTRIBPROC						glGetActiveAttrib = NULL;
PFNGLGETACTIVEUNIFORMPROC						glGetActiveUniform = NULL;
PFNGLGETATTACHEDSHADERSPROC						glGetAttachedShaders = NULL;
PFNGLGETATTRIBLOCATIONPROC						glGetAttribLocation = NULL;
PFNGLGETPROGRAMIVPROC							glGetProgramiv = NULL;
PFNGLGETPROGRAMINFOLOGPROC						glGetProgramInfoLog = NULL;
PFNGLGETSHADERIVPROC							glGetShaderiv = NULL;
PFNGLGETSHADERINFOLOGPROC						glGetShaderInfoLog = NULL;
PFNGLGETSHADERSOURCEPROC						glGetShaderSource = NULL;
PFNGLGETUNIFORMLOCATIONPROC						glGetUniformLocation = NULL;
PFNGLGETUNIFORMFVPROC							glGetUniformfv = NULL;
PFNGLGETUNIFORMIVPROC							glGetUniformiv = NULL;
PFNGLGETVERTEXATTRIBDVPROC						glGetVertexAttribdv = NULL;
PFNGLGETVERTEXATTRIBFVPROC						glGetVertexAttribfv = NULL;
PFNGLGETVERTEXATTRIBIVPROC						glGetVertexAttribiv = NULL;
PFNGLGETVERTEXATTRIBPOINTERVPROC				glGetVertexAttribPointerv = NULL;
PFNGLISPROGRAMPROC								glIsProgram = NULL;
PFNGLISSHADERPROC								glIsShader = NULL;
PFNGLLINKPROGRAMPROC							glLinkProgram = NULL;
PFNGLSHADERSOURCEPROC							glShaderSource = NULL;
PFNGLUSEPROGRAMPROC								glUseProgram = NULL;
PFNGLUNIFORM1FPROC								glUniform1f = NULL;
PFNGLUNIFORM2FPROC								glUniform2f = NULL;
PFNGLUNIFORM3FPROC								glUniform3f = NULL;
PFNGLUNIFORM4FPROC								glUniform4f = NULL;
PFNGLUNIFORM1IPROC								glUniform1i = NULL;
PFNGLUNIFORM2IPROC								glUniform2i = NULL;
PFNGLUNIFORM3IPROC								glUniform3i = NULL;
PFNGLUNIFORM4IPROC								glUniform4i = NULL;
PFNGLUNIFORM1FVPROC								glUniform1fv = NULL;
PFNGLUNIFORM2FVPROC								glUniform2fv = NULL;
PFNGLUNIFORM3FVPROC								glUniform3fv = NULL;
PFNGLUNIFORM4FVPROC								glUniform4fv = NULL;
PFNGLUNIFORM1IVPROC								glUniform1iv = NULL;
PFNGLUNIFORM2IVPROC								glUniform2iv = NULL;
PFNGLUNIFORM3IVPROC								glUniform3iv = NULL;
PFNGLUNIFORM4IVPROC								glUniform4iv = NULL;
PFNGLUNIFORMMATRIX2FVPROC						glUniformMatrix2fv = NULL;
PFNGLUNIFORMMATRIX3FVPROC						glUniformMatrix3fv = NULL;
PFNGLUNIFORMMATRIX4FVPROC						glUniformMatrix4fv = NULL;
PFNGLVALIDATEPROGRAMPROC						glValidateProgram = NULL;
PFNGLVERTEXATTRIB1DPROC							glVertexAttrib1d = NULL;
PFNGLVERTEXATTRIB1DVPROC						glVertexAttrib1dv = NULL;
PFNGLVERTEXATTRIB1FPROC							glVertexAttrib1f = NULL;
PFNGLVERTEXATTRIB1FVPROC						glVertexAttrib1fv = NULL;
PFNGLVERTEXATTRIB1SPROC							glVertexAttrib1s = NULL;
PFNGLVERTEXATTRIB1SVPROC						glVertexAttrib1sv = NULL;
PFNGLVERTEXATTRIB2DPROC							glVertexAttrib2d = NULL;
PFNGLVERTEXATTRIB2DVPROC						glVertexAttrib2dv = NULL;
PFNGLVERTEXATTRIB2FPROC							glVertexAttrib2f = NULL;
PFNGLVERTEXATTRIB2FVPROC						glVertexAttrib2fv = NULL;
PFNGLVERTEXATTRIB2SPROC							glVertexAttrib2s = NULL;
PFNGLVERTEXATTRIB2SVPROC						glVertexAttrib2sv = NULL;
PFNGLVERTEXATTRIB3DPROC							glVertexAttrib3d = NULL;
PFNGLVERTEXATTRIB3DVPROC						glVertexAttrib3dv = NULL;
PFNGLVERTEXATTRIB3FPROC							glVertexAttrib3f = NULL;
PFNGLVERTEXATTRIB3FVPROC						glVertexAttrib3fv = NULL;
PFNGLVERTEXATTRIB3SPROC							glVertexAttrib3s = NULL;
PFNGLVERTEXATTRIB3SVPROC						glVertexAttrib3sv = NULL;
PFNGLVERTEXATTRIB4NBVPROC						glVertexAttrib4nbv = NULL;
PFNGLVERTEXATTRIB4NIVPROC						glVertexAttrib4niv = NULL;
PFNGLVERTEXATTRIB4NSVPROC						glVertexAttrib4nsv = NULL;
PFNGLVERTEXATTRIB4NUBPROC						glVertexAttrib4nub = NULL;
PFNGLVERTEXATTRIB4NUBVPROC						glVertexAttrib4nubv = NULL;
PFNGLVERTEXATTRIB4NUIVPROC						glVertexAttrib4nuiv = NULL;
PFNGLVERTEXATTRIB4NUSVPROC						glVertexAttrib4nusv = NULL;
PFNGLVERTEXATTRIB4BVPROC						glVertexAttrib4bv = NULL;
PFNGLVERTEXATTRIB4DPROC							glVertexAttrib4d = NULL;
PFNGLVERTEXATTRIB4DVPROC						glVertexAttrib4dv = NULL;
PFNGLVERTEXATTRIB4FPROC							glVertexAttrib4f = NULL;
PFNGLVERTEXATTRIB4FVPROC						glVertexAttrib4fv = NULL;
PFNGLVERTEXATTRIB4IVPROC						glVertexAttrib4iv = NULL;
PFNGLVERTEXATTRIB4SPROC							glVertexAttrib4s = NULL;
PFNGLVERTEXATTRIB4SVPROC						glVertexAttrib4sv = NULL;
PFNGLVERTEXATTRIB4UBVPROC						glVertexAttrib4ubv = NULL;
PFNGLVERTEXATTRIB4UIVPROC						glVertexAttrib4uiv = NULL;
PFNGLVERTEXATTRIB4USVPROC						glVertexAttrib4usv = NULL;
PFNGLVERTEXATTRIBPOINTERPROC					glVertexAttribPointer = NULL;


//EXT_BINDABLE_UNIFORM
PFNGLUNIFORMBUFFEREXTPROC						glUniformBufferEXT = NULL;
PFNGLGETUNIFORMBUFFERSIZEEXTPROC				glGetUniformBufferSizeEXT = NULL;
PFNGLGETUNIFORMOFFSETEXTPROC					glGetUniformOffsetEXT = NULL;


//EXT_FRAMEBUFFER_OBJECT
PFNGLISRENDERBUFFEREXTPROC						glIsRenderbufferEXT = NULL;
PFNGLBINDRENDERBUFFEREXTPROC					glBindRenderbufferEXT = NULL;
PFNGLDELETERENDERBUFFERSEXTPROC					glDeleteRenderbuffersEXT = NULL;
PFNGLGENRENDERBUFFERSEXTPROC					glGenRenderbuffersEXT = NULL;
PFNGLRENDERBUFFERSTORAGEEXTPROC					glRenderbufferStorageEXT = NULL;
PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC			glGetRenderbufferParameterivEXT = NULL;
PFNGLISFRAMEBUFFEREXTPROC						glIsFramebufferEXT = NULL;
PFNGLBINDFRAMEBUFFEREXTPROC						glBindFramebufferEXT = NULL;
PFNGLDELETEFRAMEBUFFERSEXTPROC					glDeleteFramebuffersEXT = NULL;
PFNGLGENFRAMEBUFFERSEXTPROC						glGenFramebuffersEXT = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC				glCheckFramebufferStatusEXT = NULL;
PFNGLFRAMEBUFFERTEXTURE1DEXTPROC				glFramebufferTexture1DEXT = NULL;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC				glFramebufferTexture2DEXT = NULL;
PFNGLFRAMEBUFFERTEXTURE3DEXTPROC				glFramebufferTexture3DEXT = NULL;
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC				glFramebufferRenderbufferEXT = NULL;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC glGetFramebufferAttachmentParameterivEXT = NULL;
PFNGLGENERATEMIPMAPEXTPROC						glGenerateMipMapEXT = NULL;


//EXT_GEOMETRY_SHADER4
PFNGLPROGRAMPARAMETERIEXTPROC					glProgramParameteriEXT = NULL;


// EXT_TRANSFORM_FEEDBACK
PFNGLBEGINTRANSFORMFEEDBACKEXTPROC				glBeginTransformFeedbackEXT = NULL;
PFNGLENDTRANSFORMFEEDBACKEXTPROC				glEndTransformFeedbackEXT = NULL;
PFNGLTRANSFORMFEEDBACKATTRIBSEXTPROC			glTransforFeedbackAttribsEXT = NULL;
PFNGLBINDBUFFERRANGEEXTPROC						glBindBufferRangeEXT = NULL;
PFNGLBINDBUFFEROFFSETEXTPROC					glBindBufferOffsetEXT = NULL;
PFNGLBINDBUFFERBASEEXTPROC						glBindBufferBaseEXT = NULL;
PFNGLTRANSFORMFEEDBACKVARYINGSEXTPROC			glTransformFeedbackVaryingsEXT = NULL;
PFNGLACTIVEVARYINGEXTPROC						glActiveVaryingEXT = NULL;
PFNGLGETVARYINGLOCATIONEXTPROC					glGetVaryingLocationEXT = NULL;
PFNGLGETACTIVEVARYINGEXTPROC					glGetActiveVaryingEXT = NULL;
PFNGLGETTRANSFORMFEEDBACKVARYINGEXTPROC			glGetTransformFeedbackVaryingEXT = NULL;


/***********************************************~***************************************************/


void InitOpenGL(void) {
	//Map OpenGL 1.3 Functions
	glActiveTexture				= (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
	glClientActiveTexture		= (PFNGLCLIENTACTIVETEXTUREPROC)wglGetProcAddress("glClientActiveTexture");
	glMultiTexCoord1d			= (PFNGLMULTITEXCOORD1DPROC)wglGetProcAddress("glMultiTexCoord1d");
	glMultiTexCoord1dv			= (PFNGLMULTITEXCOORD1DVPROC)wglGetProcAddress("glMultiTexCoord1dv");
	glMultiTexCoord1f			= (PFNGLMULTITEXCOORD1FPROC)wglGetProcAddress("glMultiTexCoord1f");
	glMultiTexCoord1fv			= (PFNGLMULTITEXCOORD1FVPROC)wglGetProcAddress("glMultiTexCoord1fv");
	glMultiTexCoord1i			= (PFNGLMULTITEXCOORD1IPROC)wglGetProcAddress("glMultiTexCoord1i");
	glMultiTexCoord1iv			= (PFNGLMULTITEXCOORD1IVPROC)wglGetProcAddress("glMultiTexCoord1iv");
	glMultiTexCoord1s			= (PFNGLMULTITEXCOORD1SPROC)wglGetProcAddress("glMultiTexCoord1s");
	glMultiTexCoord1sv			= (PFNGLMULTITEXCOORD1SVPROC)wglGetProcAddress("glMultiTexCoord1sv");
	glMultiTexCoord2d			= (PFNGLMULTITEXCOORD2DPROC)wglGetProcAddress("glMultiTexCoord2d");
	glMultiTexCoord2dv			= (PFNGLMULTITEXCOORD2DVPROC)wglGetProcAddress("glMultiTexCoord2dv");
	glMultiTexCoord2f			= (PFNGLMULTITEXCOORD2FPROC)wglGetProcAddress("glMultiTexCoord2f");
	glMultiTexCoord2fv			= (PFNGLMULTITEXCOORD2FVPROC)wglGetProcAddress("glMultiTexCoord2fv");
	glMultiTexCoord2i			= (PFNGLMULTITEXCOORD2IPROC)wglGetProcAddress("glMultiTexCoord2i");
	glMultiTexCoord2iv			= (PFNGLMULTITEXCOORD2IVPROC)wglGetProcAddress("glMultiTexCoord2iv");
	glMultiTexCoord2s			= (PFNGLMULTITEXCOORD2SPROC)wglGetProcAddress("glMultiTexCoord2s");
	glMultiTexCoord2sv			= (PFNGLMULTITEXCOORD2SVPROC)wglGetProcAddress("glMultiTexCoord2sv");
	glMultiTexCoord3d			= (PFNGLMULTITEXCOORD3DPROC)wglGetProcAddress("glMultiTexCoord3d");
	glMultiTexCoord3dv			= (PFNGLMULTITEXCOORD3DVPROC)wglGetProcAddress("glMultiTexCoord3dv");
	glMultiTexCoord3f			= (PFNGLMULTITEXCOORD3FPROC)wglGetProcAddress("glMultiTexCoord3f");
	glMultiTexCoord3fv			= (PFNGLMULTITEXCOORD3FVPROC)wglGetProcAddress("glMultiTexCoord3fv");
	glMultiTexCoord3i			= (PFNGLMULTITEXCOORD3IPROC)wglGetProcAddress("glMultiTexCoord3i");
	glMultiTexCoord3iv			= (PFNGLMULTITEXCOORD3IVPROC)wglGetProcAddress("glMultiTexCoord3iv");
	glMultiTexCoord3s			= (PFNGLMULTITEXCOORD3SPROC)wglGetProcAddress("glMultiTexCoord3s");
	glMultiTexCoord3sv			= (PFNGLMULTITEXCOORD3SVPROC)wglGetProcAddress("glMultiTexCoord3sv");
	glMultiTexCoord4d			= (PFNGLMULTITEXCOORD4DPROC)wglGetProcAddress("glMultiTexCoord4d");
	glMultiTexCoord4dv			= (PFNGLMULTITEXCOORD4DVPROC)wglGetProcAddress("glMultiTexCoord4dv");
	glMultiTexCoord4f			= (PFNGLMULTITEXCOORD4FPROC)wglGetProcAddress("glMultiTexCoord4f");
	glMultiTexCoord4fv			= (PFNGLMULTITEXCOORD4FVPROC)wglGetProcAddress("glMultiTexCoord4fv");
	glMultiTexCoord4i			= (PFNGLMULTITEXCOORD4IPROC)wglGetProcAddress("glMultiTexCoord4i");
	glMultiTexCoord4iv			= (PFNGLMULTITEXCOORD4IVPROC)wglGetProcAddress("glMultiTexCoord4iv");
	glMultiTexCoord4s			= (PFNGLMULTITEXCOORD4SPROC)wglGetProcAddress("glMultiTexCoord4s");
	glMultiTexCoord4sv			= (PFNGLMULTITEXCOORD4SVPROC)wglGetProcAddress("glMultiTexCoord4sv");
	glLoadTransposeMatrixf		= (PFNGLLOADTRANSPOSEMATRIXFPROC)wglGetProcAddress("glLoadTransposeMatrixf");
	glLoadTransposeMatrixd		= (PFNGLLOADTRANSPOSEMATRIXDPROC)wglGetProcAddress("glLoadTransposeMatrixd");
	glMultTransposeMatrixf		= (PFNGLMULTTRANSPOSEMATRIXFPROC)wglGetProcAddress("glMultTransposeMatrixf");
	glMultTransposeMatrixd		= (PFNGLMULTTRANSPOSEMATRIXDPROC)wglGetProcAddress("glMultTransposeMatrixd");
	glSampleCoverage			= (PFNGLSAMPLECOVERAGEPROC)wglGetProcAddress("glSampleCoverage");
	glCompressedTexImage3d		= (PFNGLCOMPRESSEDTEXIMAGE3DPROC)wglGetProcAddress("glCompressedTexImage3d");
	glCompressedTexImage2d		= (PFNGLCOMPRESSEDTEXIMAGE2DPROC)wglGetProcAddress("glCompressedTexImage2d");
	glCompressedTexImage1d		= (PFNGLCOMPRESSEDTEXIMAGE1DPROC)wglGetProcAddress("glCompressedTexImage1d");
	glCompressedTexSubImage3d	= (PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC)wglGetProcAddress("glCompressedTexSubImage3d");
	glCompressedTexSubImage2d	= (PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC)wglGetProcAddress("glCompressedTexSubImage2d");
	glCompressedTexSubImage1d	= (PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC)wglGetProcAddress("glCompressedTexSubImage1d");
	glCompressedTexImage		= (PFNGLGETCOMPRESSEDTEXIMAGEPROC)wglGetProcAddress("glCompressedTexImage");

	//Map OpenGL 1.5 Functions
	glGenQueries				= (PFNGLGENQUERIESPROC)wglGetProcAddress("glGenQueries");
	glDeleteQueries				= (PFNGLDELETEQUERIESPROC)wglGetProcAddress("glDeleteQueries");
	glIsQuery					= (PFNGLISQUERYPROC)wglGetProcAddress("glIsQuery");
	glBeginQuery				= (PFNGLBEGINQUERYPROC)wglGetProcAddress("glBeginQuery");
	glEndQuery					= (PFNGLENDQUERYPROC)wglGetProcAddress("glEndQuery");
	glGetQueryiv				= (PFNGLGETQUERYIVPROC)wglGetProcAddress("glGetQueryiv");
	glGetQueryObjectiv			= (PFNGLGETQUERYOBJECTIVPROC)wglGetProcAddress("glGetQueryObjectiv");
	glGetQueryObjectuiv			= (PFNGLGETQUERYOBJECTUIVPROC)wglGetProcAddress("glGetQueryObjectuiv");
	glBindBuffer				= (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
	glDeleteBuffers				= (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
	glGenBuffers				= (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
	glIsBuffer					= (PFNGLISBUFFERPROC)wglGetProcAddress("glIsBuffer");
	glBufferData				= (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
	glBufferSubData				= (PFNGLBUFFERSUBDATAPROC)wglGetProcAddress("glBufferSubData");
	glGetBufferSubData			= (PFNGLGETBUFFERSUBDATAPROC)wglGetProcAddress("glGetBufferSubData");
	glMapBuffer					= (PFNGLMAPBUFFERPROC)wglGetProcAddress("glMapBuffer");
	glUnmapBuffer				= (PFNGLUNMAPBUFFERPROC)wglGetProcAddress("glUnmapBuffer");
	glGetBufferParameteriv		= (PFNGLGETBUFFERPARAMETERIVPROC)wglGetProcAddress("glGetBufferParameteriv");
	glGetBufferPointerv			= (PFNGLGETBUFFERPOINTERVPROC)wglGetProcAddress("glGetBufferPointerv");

	//Map OpenGL 2.0 Functions
	glBlendEquationSeparate		= (PFNGLBLENDEQUATIONSEPARATEPROC)wglGetProcAddress("glBlendEquationSeparate");
	glDrawBuffers				= (PFNGLDRAWBUFFERSPROC)wglGetProcAddress("glDrawBuffers");
	glStencilOpSeparate			= (PFNGLSTENCILOPSEPARATEPROC)wglGetProcAddress("glStencilOpSeparate");
	glStencilFuncSeparate		= (PFNGLSTENCILFUNCSEPARATEPROC)wglGetProcAddress("glStencilFuncSeparate");
	glStencilMaskSeparate		= (PFNGLSTENCILMASKSEPARATEPROC)wglGetProcAddress("glStencilMaskSeparate");
	glAttachShader				= (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
	glBindAttribLocation		= (PFNGLBINDATTRIBLOCATIONPROC)wglGetProcAddress("glBindAttribLocation");
	glCompileShader				= (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
	glCreateProgram				= (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
	glCreateShader				= (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
	glDeleteProgram				= (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
	glDeleteShader				= (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
	glDetachShader				= (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");
	glDisableVertexAttribArray	= (PFNGLDISABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glDisableVertexAttribArray");
	glEnableVertexAttribArray	= (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
	glGetActiveAttrib			= (PFNGLGETACTIVEATTRIBPROC)wglGetProcAddress("glGetActiveAttrib");
	glGetActiveUniform			= (PFNGLGETACTIVEUNIFORMPROC)wglGetProcAddress("glGetActiveUniform");
	glGetAttachedShaders		= (PFNGLGETATTACHEDSHADERSPROC)wglGetProcAddress("glGetAttachedShaders");
	glGetAttribLocation			= (PFNGLGETATTRIBLOCATIONPROC)wglGetProcAddress("glGetAttribLocation");
	glGetProgramiv				= (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
	glGetProgramInfoLog			= (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");
	glGetShaderiv				= (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
	glGetShaderInfoLog			= (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
	glGetShaderSource			= (PFNGLGETSHADERSOURCEPROC)wglGetProcAddress("glGetShaderSource");
	glGetUniformLocation		= (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
	glGetUniformfv				= (PFNGLGETUNIFORMFVPROC)wglGetProcAddress("glGetUniformfv");
	glGetUniformiv				= (PFNGLGETUNIFORMIVPROC)wglGetProcAddress("glGetUniformiv");
	glGetVertexAttribdv			= (PFNGLGETVERTEXATTRIBDVPROC)wglGetProcAddress("glGetVertexAttribdv");
	glGetVertexAttribfv			= (PFNGLGETVERTEXATTRIBFVPROC)wglGetProcAddress("glGetVertexAttribfv");
	glGetVertexAttribiv			= (PFNGLGETVERTEXATTRIBIVPROC)wglGetProcAddress("glGetVertexAttribiv");
	glGetVertexAttribPointerv	= (PFNGLGETVERTEXATTRIBPOINTERVPROC)wglGetProcAddress("glGetVertexAttribPointerv");
	glIsProgram					= (PFNGLISPROGRAMPROC)wglGetProcAddress("glIsProgram");
	glIsShader					= (PFNGLISSHADERPROC)wglGetProcAddress("glIsShader");
	glLinkProgram				= (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
	glShaderSource				= (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
	glUseProgram				= (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
	glUniform1f					= (PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f");
	glUniform2f					= (PFNGLUNIFORM2FPROC)wglGetProcAddress("glUniform2f");
	glUniform3f					= (PFNGLUNIFORM3FPROC)wglGetProcAddress("glUniform3f");
	glUniform4f					= (PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f");
	glUniform1i					= (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
	glUniform2i					= (PFNGLUNIFORM2IPROC)wglGetProcAddress("glUniform2i");
	glUniform3i					= (PFNGLUNIFORM3IPROC)wglGetProcAddress("glUniform3i");
	glUniform4i					= (PFNGLUNIFORM4IPROC)wglGetProcAddress("glUniform4i");
	glUniform1fv				= (PFNGLUNIFORM1FVPROC)wglGetProcAddress("glUniform1fv");
	glUniform2fv				= (PFNGLUNIFORM2FVPROC)wglGetProcAddress("glUniform2fv");
	glUniform3fv				= (PFNGLUNIFORM3FVPROC)wglGetProcAddress("glUniform3fv");
	glUniform4fv				= (PFNGLUNIFORM4FVPROC)wglGetProcAddress("glUniform4fv");
	glUniform1iv				= (PFNGLUNIFORM1IVPROC)wglGetProcAddress("glUniform1iv");
	glUniform2iv				= (PFNGLUNIFORM2IVPROC)wglGetProcAddress("glUniform2iv");
	glUniform3iv				= (PFNGLUNIFORM3IVPROC)wglGetProcAddress("glUniform3iv");
	glUniform4iv				= (PFNGLUNIFORM4IVPROC)wglGetProcAddress("glUniform4iv");
	glUniformMatrix2fv			= (PFNGLUNIFORMMATRIX2FVPROC)wglGetProcAddress("glUniformMatrix2fv");
	glUniformMatrix3fv			= (PFNGLUNIFORMMATRIX3FVPROC)wglGetProcAddress("glUniformMatrix3fv");
	glUniformMatrix4fv			= (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
	glValidateProgram			= (PFNGLVALIDATEPROGRAMPROC)wglGetProcAddress("glValidateProgram");
	glVertexAttrib1d			= (PFNGLVERTEXATTRIB1DPROC)wglGetProcAddress("glVertexAttrib1d");
	glVertexAttrib1dv			= (PFNGLVERTEXATTRIB1DVPROC)wglGetProcAddress("glVertexAttrib1dv");
	glVertexAttrib1f			= (PFNGLVERTEXATTRIB1FPROC)wglGetProcAddress("glVertexAttrib1f");
	glVertexAttrib1fv			= (PFNGLVERTEXATTRIB1FVPROC)wglGetProcAddress("glVertexAttrib1fv");
	glVertexAttrib1s			= (PFNGLVERTEXATTRIB1SPROC)wglGetProcAddress("glVertexAttrib1s");
	glVertexAttrib1sv			= (PFNGLVERTEXATTRIB1SVPROC)wglGetProcAddress("glVertexAttrib1sv");
	glVertexAttrib2d			= (PFNGLVERTEXATTRIB2DPROC)wglGetProcAddress("glVertexAttrib2d");
	glVertexAttrib2dv			= (PFNGLVERTEXATTRIB2DVPROC)wglGetProcAddress("glVertexAttrib2dv");
	glVertexAttrib2f			= (PFNGLVERTEXATTRIB2FPROC)wglGetProcAddress("glVertexAttrib2f");
	glVertexAttrib2fv			= (PFNGLVERTEXATTRIB2FVPROC)wglGetProcAddress("glVertexAttrib2fv");
	glVertexAttrib2s			= (PFNGLVERTEXATTRIB2SPROC)wglGetProcAddress("glVertexAttrib2s");
	glVertexAttrib2sv			= (PFNGLVERTEXATTRIB2SVPROC)wglGetProcAddress("glVertexAttrib2sv");
	glVertexAttrib3d			= (PFNGLVERTEXATTRIB3DPROC)wglGetProcAddress("glVertexAttrib3d");
	glVertexAttrib3dv			= (PFNGLVERTEXATTRIB3DVPROC)wglGetProcAddress("glVertexAttrib3dv");
	glVertexAttrib3f			= (PFNGLVERTEXATTRIB3FPROC)wglGetProcAddress("glVertexAttrib3f");
	glVertexAttrib3fv			= (PFNGLVERTEXATTRIB3FVPROC)wglGetProcAddress("glVertexAttrib3fv");
	glVertexAttrib3s			= (PFNGLVERTEXATTRIB3SPROC)wglGetProcAddress("glVertexAttrib3s");
	glVertexAttrib3sv			= (PFNGLVERTEXATTRIB3SVPROC)wglGetProcAddress("glVertexAttrib3sv");
	glVertexAttrib4nbv			= (PFNGLVERTEXATTRIB4NBVPROC)wglGetProcAddress("glVertexAttrib4nbv");
	glVertexAttrib4niv			= (PFNGLVERTEXATTRIB4NIVPROC)wglGetProcAddress("glVertexAttrib4niv");
	glVertexAttrib4nsv			= (PFNGLVERTEXATTRIB4NSVPROC)wglGetProcAddress("glVertexAttrib4nsv");
	glVertexAttrib4nub			= (PFNGLVERTEXATTRIB4NUBPROC)wglGetProcAddress("glVertexAttrib4nub");
	glVertexAttrib4nubv			= (PFNGLVERTEXATTRIB4NUBVPROC)wglGetProcAddress("glVertexAttrib4nubv");
	glVertexAttrib4nuiv			= (PFNGLVERTEXATTRIB4NUIVPROC)wglGetProcAddress("glVertexAttrib4nuiv");
	glVertexAttrib4nusv			= (PFNGLVERTEXATTRIB4NUSVPROC)wglGetProcAddress("glVertexAttrib4nusv");
	glVertexAttrib4bv			= (PFNGLVERTEXATTRIB4BVPROC)wglGetProcAddress("glVertexAttrib4bv");
	glVertexAttrib4d			= (PFNGLVERTEXATTRIB4DPROC)wglGetProcAddress("glVertexAttrib4d");
	glVertexAttrib4dv			= (PFNGLVERTEXATTRIB4DVPROC)wglGetProcAddress("glVertexAttrib4dv");
	glVertexAttrib4f			= (PFNGLVERTEXATTRIB4FPROC)wglGetProcAddress("glVertexAttrib4f");
	glVertexAttrib4fv			= (PFNGLVERTEXATTRIB4FVPROC)wglGetProcAddress("glVertexAttrib4fv");
	glVertexAttrib4iv			= (PFNGLVERTEXATTRIB4IVPROC)wglGetProcAddress("glVertexAttrib4iv");
	glVertexAttrib4s			= (PFNGLVERTEXATTRIB4SPROC)wglGetProcAddress("glVertexAttrib4s");
	glVertexAttrib4sv			= (PFNGLVERTEXATTRIB4SVPROC)wglGetProcAddress("glVertexAttrib4sv");
	glVertexAttrib4ubv			= (PFNGLVERTEXATTRIB4UBVPROC)wglGetProcAddress("glVertexAttrib4ubv");
	glVertexAttrib4uiv			= (PFNGLVERTEXATTRIB4UIVPROC)wglGetProcAddress("glVertexAttrib4uiv");
	glVertexAttrib4usv			= (PFNGLVERTEXATTRIB4USVPROC)wglGetProcAddress("glVertexAttrib4usv");
	glVertexAttribPointer		= (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");

	//Map EXT_BINDABLE_UNIFORM Functions
	glUniformBufferEXT			= (PFNGLUNIFORMBUFFEREXTPROC)wglGetProcAddress("glUniformBufferEXT");
	glGetUniformBufferSizeEXT	= (PFNGLGETUNIFORMBUFFERSIZEEXTPROC)wglGetProcAddress("glGetUniformBufferSizeEXT");
	glGetUniformOffsetEXT		= (PFNGLGETUNIFORMOFFSETEXTPROC)wglGetProcAddress("glGetUniformOffsetEXT");

	//Map EXT_FRAMEBUFFER_OBJECT Functions
	glIsRenderbufferEXT			= (PFNGLISRENDERBUFFEREXTPROC)wglGetProcAddress("glIsRenderbufferEXT");
	glBindRenderbufferEXT		= (PFNGLBINDRENDERBUFFEREXTPROC)wglGetProcAddress("glBindRenderbufferEXT");
	glDeleteRenderbuffersEXT	= (PFNGLDELETERENDERBUFFERSEXTPROC)wglGetProcAddress("glDeleteRenderbuffersEXT");
	glGenRenderbuffersEXT		= (PFNGLGENRENDERBUFFERSEXTPROC)wglGetProcAddress("glGenRenderbuffersEXT");
	glRenderbufferStorageEXT	= (PFNGLRENDERBUFFERSTORAGEEXTPROC)wglGetProcAddress("glRenderbufferStorageEXT");
	glGetRenderbufferParameterivEXT = (PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC)wglGetProcAddress("glGetRenderbufferParameterivEXT");
	glIsFramebufferEXT			= (PFNGLISFRAMEBUFFEREXTPROC)wglGetProcAddress("glIsFramebufferEXT");
	glBindFramebufferEXT		= (PFNGLBINDFRAMEBUFFEREXTPROC)wglGetProcAddress("glBindFramebufferEXT");
	glDeleteFramebuffersEXT		= (PFNGLDELETEFRAMEBUFFERSEXTPROC)wglGetProcAddress("glDeleteFramebuffersEXT");
	glGenFramebuffersEXT		= (PFNGLGENFRAMEBUFFERSEXTPROC)wglGetProcAddress("glGenFramebuffersEXT");
	glCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)wglGetProcAddress("glCheckFramebufferStatusEXT");
	glFramebufferTexture1DEXT	= (PFNGLFRAMEBUFFERTEXTURE1DEXTPROC)wglGetProcAddress("glFramebufferTexture1DEXT");
	glFramebufferTexture2DEXT	= (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)wglGetProcAddress("glFramebufferTexture2DEXT");
	glFramebufferTexture3DEXT	= (PFNGLFRAMEBUFFERTEXTURE3DEXTPROC)wglGetProcAddress("glFramebufferTexture3DEXT");
	glFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)wglGetProcAddress("glFramebufferRenderbufferEXT");
	glGetFramebufferAttachmentParameterivEXT = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC)wglGetProcAddress("glGetFramebufferAttachmentParameterivEXT");
	glGenerateMipMapEXT			= (PFNGLGENERATEMIPMAPEXTPROC)wglGetProcAddress("glGenerateMipMapEXT");

	//Map EXT_GEOMETRY_SHADER4 Functions
	glProgramParameteriEXT		= (PFNGLPROGRAMPARAMETERIEXTPROC)wglGetProcAddress("glProgramParameteriEXT");

	//Map EXT_TRANSFORM_FEEDBACK Functions
	glBeginTransformFeedbackEXT	= (PFNGLBEGINTRANSFORMFEEDBACKEXTPROC)wglGetProcAddress("glBeginTransformFeedbackEXT");
	glEndTransformFeedbackEXT	= (PFNGLENDTRANSFORMFEEDBACKEXTPROC)wglGetProcAddress("glEndTransformFeedbackEXT");
	glTransforFeedbackAttribsEXT = (PFNGLTRANSFORMFEEDBACKATTRIBSEXTPROC)wglGetProcAddress("glTransforFeedbackAttribsEXT");
	glBindBufferRangeEXT		= (PFNGLBINDBUFFERRANGEEXTPROC)wglGetProcAddress("glBindBufferRangeEXT");
	glBindBufferOffsetEXT		= (PFNGLBINDBUFFEROFFSETEXTPROC)wglGetProcAddress("glBindBufferOffsetEXT");
	glBindBufferBaseEXT			= (PFNGLBINDBUFFERBASEEXTPROC)wglGetProcAddress("glBindBufferBaseEXT");
	glTransformFeedbackVaryingsEXT = (PFNGLTRANSFORMFEEDBACKVARYINGSEXTPROC)wglGetProcAddress("glTransformFeedbackVaryingsEXT");
	glActiveVaryingEXT			= (PFNGLACTIVEVARYINGEXTPROC)wglGetProcAddress("glActiveVaryingEXT");
	glGetVaryingLocationEXT		= (PFNGLGETVARYINGLOCATIONEXTPROC)wglGetProcAddress("glGetVaryingLocationEXT");
	glGetActiveVaryingEXT		= (PFNGLGETACTIVEVARYINGEXTPROC)wglGetProcAddress("glGetActiveVaryingEXT");
	glGetTransformFeedbackVaryingEXT = (PFNGLGETTRANSFORMFEEDBACKVARYINGEXTPROC)wglGetProcAddress("glGetTransformFeedbackVaryingEXT");
	
	//Nothing else for now...
}


/***********************************************~***************************************************/

