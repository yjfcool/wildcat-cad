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


#ifndef __WILDCAT_GL_UTILITIES_H__
#define __WILDCAT_GL_UTILITIES_H__


/***********************************************~***************************************************/


#include <gl/gl.h>			// Header File For The OpenGL32 Library
#include <gl/glu.h>			// Header File For The GLu32 Library
#include <gl/glaux.h>		// Header File For The Glaux Library
#include <Application/Win32/wglext.h>
#include <Application/Win32/glext.h>


/***********************************************~***************************************************/


// GL version 1.3 functions
extern PFNGLACTIVETEXTUREPROC					glActiveTexture;
extern PFNGLCLIENTACTIVETEXTUREPROC				glClientActiveTexture;
extern PFNGLMULTITEXCOORD1DPROC					glMultiTexCoord1d;
extern PFNGLMULTITEXCOORD1DVPROC				glMultiTexCoord1dv;
extern PFNGLMULTITEXCOORD1FPROC					glMultiTexCoord1f;
extern PFNGLMULTITEXCOORD1FVPROC				glMultiTexCoord1fv;
extern PFNGLMULTITEXCOORD1IPROC					glMultiTexCoord1i;
extern PFNGLMULTITEXCOORD1IVPROC				glMultiTexCoord1iv;
extern PFNGLMULTITEXCOORD1SPROC					glMultiTexCoord1s;
extern PFNGLMULTITEXCOORD1SVPROC				glMultiTexCoord1sv;
extern PFNGLMULTITEXCOORD2DPROC					glMultiTexCoord2d;
extern PFNGLMULTITEXCOORD2DVPROC				glMultiTexCoord2dv;
extern PFNGLMULTITEXCOORD2FPROC					glMultiTexCoord2f;
extern PFNGLMULTITEXCOORD2FVPROC				glMultiTexCoord2fv;
extern PFNGLMULTITEXCOORD2IPROC					glMultiTexCoord2i;
extern PFNGLMULTITEXCOORD2IVPROC				glMultiTexCoord2iv;
extern PFNGLMULTITEXCOORD2SPROC					glMultiTexCoord2s;
extern PFNGLMULTITEXCOORD2SVPROC				glMultiTexCoord2sv;
extern PFNGLMULTITEXCOORD3DPROC					glMultiTexCoord3d;
extern PFNGLMULTITEXCOORD3DVPROC				glMultiTexCoord3dv;
extern PFNGLMULTITEXCOORD3FPROC					glMultiTexCoord3f;
extern PFNGLMULTITEXCOORD3FVPROC				glMultiTexCoord3fv;
extern PFNGLMULTITEXCOORD3IPROC					glMultiTexCoord3i;
extern PFNGLMULTITEXCOORD3IVPROC				glMultiTexCoord3iv;
extern PFNGLMULTITEXCOORD3SPROC					glMultiTexCoord3s;
extern PFNGLMULTITEXCOORD3SVPROC				glMultiTexCoord3sv;
extern PFNGLMULTITEXCOORD4DPROC					glMultiTexCoord4d;
extern PFNGLMULTITEXCOORD4DVPROC				glMultiTexCoord4dv;
extern PFNGLMULTITEXCOORD4FPROC					glMultiTexCoord4f;
extern PFNGLMULTITEXCOORD4FVPROC				glMultiTexCoord4fv;
extern PFNGLMULTITEXCOORD4IPROC					glMultiTexCoord4i;
extern PFNGLMULTITEXCOORD4IVPROC				glMultiTexCoord4iv;
extern PFNGLMULTITEXCOORD4SPROC					glMultiTexCoord4s;
extern PFNGLMULTITEXCOORD4SVPROC				glMultiTexCoord4sv;
extern PFNGLLOADTRANSPOSEMATRIXFPROC			glLoadTransposeMatrixf;
extern PFNGLLOADTRANSPOSEMATRIXDPROC			glLoadTransposeMatrixd;
extern PFNGLMULTTRANSPOSEMATRIXFPROC			glMultTransposeMatrixf;
extern PFNGLMULTTRANSPOSEMATRIXDPROC			glMultTransposeMatrixd;
extern PFNGLSAMPLECOVERAGEPROC					glSampleCoverage;
extern PFNGLCOMPRESSEDTEXIMAGE3DPROC			glCompressedTexImage3d;
extern PFNGLCOMPRESSEDTEXIMAGE2DPROC			glCompressedTexImage2d;
extern PFNGLCOMPRESSEDTEXIMAGE1DPROC			glCompressedTexImage1d;
extern PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC			glCompressedTexSubImage3d;
extern PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC			glCompressedTexSubImage2d;
extern PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC			glCompressedTexSubImage1d;
extern PFNGLGETCOMPRESSEDTEXIMAGEPROC			glCompressedTexImage;


// GL version 1.5 functions
extern PFNGLGENQUERIESPROC						glGenQueries;
extern PFNGLDELETEQUERIESPROC					glDeleteQueries;
extern PFNGLISQUERYPROC							glIsQuery;
extern PFNGLBEGINQUERYPROC						glBeginQuery;
extern PFNGLENDQUERYPROC						glEndQuery;
extern PFNGLGETQUERYIVPROC						glGetQueryiv;
extern PFNGLGETQUERYOBJECTIVPROC				glGetQueryObjectiv;
extern PFNGLGETQUERYOBJECTUIVPROC				glGetQueryObjectuiv;
extern PFNGLBINDBUFFERPROC						glBindBuffer;
extern PFNGLDELETEBUFFERSPROC					glDeleteBuffers;
extern PFNGLGENBUFFERSPROC						glGenBuffers;
extern PFNGLISBUFFERPROC						glIsBuffer;
extern PFNGLBUFFERDATAPROC						glBufferData;
extern PFNGLBUFFERSUBDATAPROC					glBufferSubData;
extern PFNGLGETBUFFERSUBDATAPROC				glGetBufferSubData;
extern PFNGLMAPBUFFERPROC						glMapBuffer;
extern PFNGLUNMAPBUFFERPROC						glUnmapBuffer;
extern PFNGLGETBUFFERPARAMETERIVPROC			glGetBufferParameteriv;
extern PFNGLGETBUFFERPOINTERVPROC				glGetBufferPointerv;


// GL version 2.0 functions
extern PFNGLBLENDEQUATIONSEPARATEPROC			glBlendEquationSeparate;
extern PFNGLDRAWBUFFERSPROC						glDrawBuffers;
extern PFNGLSTENCILOPSEPARATEPROC				glStencilOpSeparate;
extern PFNGLSTENCILFUNCSEPARATEPROC				glStencilFuncSeparate;
extern PFNGLSTENCILMASKSEPARATEPROC				glStencilMaskSeparate;
extern PFNGLATTACHSHADERPROC					glAttachShader;
extern PFNGLBINDATTRIBLOCATIONPROC				glBindAttribLocation;
extern PFNGLCOMPILESHADERPROC					glCompileShader;
extern PFNGLCREATEPROGRAMPROC					glCreateProgram;
extern PFNGLCREATESHADERPROC					glCreateShader;
extern PFNGLDELETEPROGRAMPROC					glDeleteProgram;
extern PFNGLDELETESHADERPROC					glDeleteShader;
extern PFNGLDETACHSHADERPROC					glDetachShader;
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC		glDisableVertexAttribArray;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC			glEnableVertexAttribArray;
extern PFNGLGETACTIVEATTRIBPROC					glGetActiveAttrib;
extern PFNGLGETACTIVEUNIFORMPROC				glGetActiveUniform;
extern PFNGLGETATTACHEDSHADERSPROC				glGetAttachedShaders;
extern PFNGLGETATTRIBLOCATIONPROC				glGetAttribLocation;
extern PFNGLGETPROGRAMIVPROC					glGetProgramiv;
extern PFNGLGETPROGRAMINFOLOGPROC				glGetProgramInfoLog;
extern PFNGLGETSHADERIVPROC						glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC				glGetShaderInfoLog;
extern PFNGLGETSHADERSOURCEPROC					glGetShaderSource;
extern PFNGLGETUNIFORMLOCATIONPROC				glGetUniformLocation;
extern PFNGLGETUNIFORMFVPROC					glGetUniformfv;
extern PFNGLGETUNIFORMIVPROC					glGetUniformiv;
extern PFNGLGETVERTEXATTRIBDVPROC				glGetVertexAttribdv;
extern PFNGLGETVERTEXATTRIBFVPROC				glGetVertexAttribfv;
extern PFNGLGETVERTEXATTRIBIVPROC				glGetVertexAttribiv;
extern PFNGLGETVERTEXATTRIBPOINTERVPROC			glGetVertexAttribPointerv;
extern PFNGLISPROGRAMPROC						glIsProgram;
extern PFNGLISSHADERPROC						glIsShader;
extern PFNGLLINKPROGRAMPROC						glLinkProgram;
extern PFNGLSHADERSOURCEPROC					glShaderSource;
extern PFNGLUSEPROGRAMPROC						glUseProgram;
extern PFNGLUNIFORM1FPROC						glUniform1f;
extern PFNGLUNIFORM2FPROC						glUniform2f;
extern PFNGLUNIFORM3FPROC						glUniform3f;
extern PFNGLUNIFORM4FPROC						glUniform4f;
extern PFNGLUNIFORM1IPROC						glUniform1i;
extern PFNGLUNIFORM2IPROC						glUniform2i;
extern PFNGLUNIFORM3IPROC						glUniform3i;
extern PFNGLUNIFORM4IPROC						glUniform4i;
extern PFNGLUNIFORM1FVPROC						glUniform1fv;
extern PFNGLUNIFORM2FVPROC						glUniform2fv;
extern PFNGLUNIFORM3FVPROC						glUniform3fv;
extern PFNGLUNIFORM4FVPROC						glUniform4fv;
extern PFNGLUNIFORM1IVPROC						glUniform1iv;
extern PFNGLUNIFORM2IVPROC						glUniform2iv;
extern PFNGLUNIFORM3IVPROC						glUniform3iv;
extern PFNGLUNIFORM4IVPROC						glUniform4iv;
extern PFNGLUNIFORMMATRIX2FVPROC				glUniformMatrix2fv;
extern PFNGLUNIFORMMATRIX3FVPROC				glUniformMatrix3fv;
extern PFNGLUNIFORMMATRIX4FVPROC				glUniformMatrix4fv;
extern PFNGLVALIDATEPROGRAMPROC					glValidateProgram;
extern PFNGLVERTEXATTRIB1DPROC					glVertexAttrib1d;
extern PFNGLVERTEXATTRIB1DVPROC					glVertexAttrib1dv;
extern PFNGLVERTEXATTRIB1FPROC					glVertexAttrib1f;
extern PFNGLVERTEXATTRIB1FVPROC					glVertexAttrib1fv;
extern PFNGLVERTEXATTRIB1SPROC					glVertexAttrib1s;
extern PFNGLVERTEXATTRIB1SVPROC					glVertexAttrib1sv;
extern PFNGLVERTEXATTRIB2DPROC					glVertexAttrib2d;
extern PFNGLVERTEXATTRIB2DVPROC					glVertexAttrib2dv;
extern PFNGLVERTEXATTRIB2FPROC					glVertexAttrib2f;
extern PFNGLVERTEXATTRIB2FVPROC					glVertexAttrib2fv;
extern PFNGLVERTEXATTRIB2SPROC					glVertexAttrib2s;
extern PFNGLVERTEXATTRIB2SVPROC					glVertexAttrib2sv;
extern PFNGLVERTEXATTRIB3DPROC					glVertexAttrib3d;
extern PFNGLVERTEXATTRIB3DVPROC					glVertexAttrib3dv;
extern PFNGLVERTEXATTRIB3FPROC					glVertexAttrib3f;
extern PFNGLVERTEXATTRIB3FVPROC					glVertexAttrib3fv;
extern PFNGLVERTEXATTRIB3SPROC					glVertexAttrib3s;
extern PFNGLVERTEXATTRIB3SVPROC					glVertexAttrib3sv;
extern PFNGLVERTEXATTRIB4NBVPROC				glVertexAttrib4nbv;
extern PFNGLVERTEXATTRIB4NIVPROC				glVertexAttrib4niv;
extern PFNGLVERTEXATTRIB4NSVPROC				glVertexAttrib4nsv;
extern PFNGLVERTEXATTRIB4NUBPROC				glVertexAttrib4nub;
extern PFNGLVERTEXATTRIB4NUBVPROC				glVertexAttrib4nubv;
extern PFNGLVERTEXATTRIB4NUIVPROC				glVertexAttrib4nuiv;
extern PFNGLVERTEXATTRIB4NUSVPROC				glVertexAttrib4nusv;
extern PFNGLVERTEXATTRIB4BVPROC					glVertexAttrib4bv;
extern PFNGLVERTEXATTRIB4DPROC					glVertexAttrib4d;
extern PFNGLVERTEXATTRIB4DVPROC					glVertexAttrib4dv;
extern PFNGLVERTEXATTRIB4FPROC					glVertexAttrib4f;
extern PFNGLVERTEXATTRIB4FVPROC					glVertexAttrib4fv;
extern PFNGLVERTEXATTRIB4IVPROC					glVertexAttrib4iv;
extern PFNGLVERTEXATTRIB4SPROC					glVertexAttrib4s;
extern PFNGLVERTEXATTRIB4SVPROC					glVertexAttrib4sv;
extern PFNGLVERTEXATTRIB4UBVPROC				glVertexAttrib4ubv;
extern PFNGLVERTEXATTRIB4UIVPROC				glVertexAttrib4uiv;
extern PFNGLVERTEXATTRIB4USVPROC				glVertexAttrib4usv;
extern PFNGLVERTEXATTRIBPOINTERPROC				glVertexAttribPointer;


//EXT_FRAMEBUFFER_OBJECT
extern PFNGLISRENDERBUFFEREXTPROC				glIsRenderbufferEXT;
extern PFNGLBINDRENDERBUFFEREXTPROC				glBindRenderbufferEXT;
extern PFNGLDELETERENDERBUFFERSEXTPROC			glDeleteRenderbufferEXT;
extern PFNGLGENRENDERBUFFERSEXTPROC				glGenRenderbufferEXT;
extern PFNGLRENDERBUFFERSTORAGEEXTPROC			glRenderbufferStorageEXT;
extern PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC	glGetRenderbufferParameterivEXT;
extern PFNGLISFRAMEBUFFEREXTPROC				glIsFramebufferEXT;
extern PFNGLBINDFRAMEBUFFEREXTPROC				glBindFramebufferEXT;
extern PFNGLDELETEFRAMEBUFFERSEXTPROC			glDeleteFramebuffersEXT;
extern PFNGLGENFRAMEBUFFERSEXTPROC				glGenFramebuffersEXT;
extern PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC		glCheckFramebufferStatusEXT;
extern PFNGLFRAMEBUFFERTEXTURE1DEXTPROC			glFramebufferTexture1DEXT;
extern PFNGLFRAMEBUFFERTEXTURE2DEXTPROC			glFramebufferTexture2DEXT;
extern PFNGLFRAMEBUFFERTEXTURE3DEXTPROC			glFramebufferTexture3DEXT;
extern PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC		glFramebufferRenderbufferEXT;
extern PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC glGetFramebufferAttachmentParameterivEXT;
extern PFNGLGENERATEMIPMAPEXTPROC				glGenerateMipMapEXT;


//EXT_BINDABLE_UNIFORM
extern PFNGLUNIFORMBUFFEREXTPROC				glUniformBufferEXT;
extern PFNGLGETUNIFORMBUFFERSIZEEXTPROC			glGetUniformBufferSizeEXT;
extern PFNGLGETUNIFORMOFFSETEXTPROC				glGetUniformOffsetEXT;


//EXT_GEOMETRY_SHADER4
extern PFNGLPROGRAMPARAMETERIEXTPROC glProgramParameteriEXT;


// EXT_TRANSFORM_FEEDBACK (not currently supported)
#define GL_INTERLEAVED_ATTRIBS_EXT				0
#define GL_SEPARATE_ATTRIBS_EXT					0
#define GL_TRANSFORM_FEEDBACK_BUFFER_EXT		0
#define GL_RASTERIZER_DISCARD_EXT				0
typedef void (APIENTRYP PFNGLBEGINTRANSFORMFEEDBACKEXTPROC) (GLenum primitiveMode);
typedef void (APIENTRYP PFNGLENDTRANSFORMFEEDBACKEXTPROC) (void);
typedef void (APIENTRYP PFNGLTRANSFORMFEEDBACKATTRIBSEXTPROC) (GLuint count, const GLint *attribs, GLenum bufferMode);
typedef void (APIENTRYP PFNGLBINDBUFFERRANGEEXTPROC) (GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
typedef void (APIENTRYP PFNGLBINDBUFFEROFFSETEXTPROC) (GLenum target, GLuint index, GLuint buffer, GLintptr offset);
typedef void (APIENTRYP PFNGLBINDBUFFERBASEEXTPROC) (GLenum target, GLuint index, GLuint buffer);
typedef void (APIENTRYP PFNGLTRANSFORMFEEDBACKVARYINGSEXTPROC) (GLuint program, GLsizei count, const GLchar **locations, GLenum bufferMode);
typedef void (APIENTRYP PFNGLACTIVEVARYINGEXTPROC) (GLuint program, const GLchar *name);
typedef GLint (APIENTRYP PFNGLGETVARYINGLOCATIONEXTPROC) (GLuint program, const GLchar *name);
typedef void (APIENTRYP PFNGLGETACTIVEVARYINGEXTPROC) (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name);
typedef void (APIENTRYP PFNGLGETTRANSFORMFEEDBACKVARYINGEXTPROC) (GLuint program, GLuint index, GLint *location);
extern  PFNGLBEGINTRANSFORMFEEDBACKEXTPROC		glBeginTransformFeedbackEXT;
extern  PFNGLENDTRANSFORMFEEDBACKEXTPROC		glEndTransformFeedbackEXT;
extern  PFNGLTRANSFORMFEEDBACKATTRIBSEXTPROC	glTransforFeedbackAttribsEXT;
extern  PFNGLBINDBUFFERRANGEEXTPROC				glBindBufferRangeEXT;
extern  PFNGLBINDBUFFEROFFSETEXTPROC			glBindBufferOffsetEXT;
extern  PFNGLBINDBUFFERBASEEXTPROC				glBindBufferBaseEXT;
extern  PFNGLTRANSFORMFEEDBACKVARYINGSEXTPROC	glTransformFeedbackVaryingsEXT;
extern  PFNGLACTIVEVARYINGEXTPROC				glActiveVaryingEXT;
extern  PFNGLGETVARYINGLOCATIONEXTPROC			glGetVaryingLocationEXT;
extern  PFNGLGETACTIVEVARYINGEXTPROC			glGetActiveVaryingEXT;
extern  PFNGLGETTRANSFORMFEEDBACKVARYINGEXTPROC	glGetTransformFeedbackVaryingEXT;


/***********************************************~***************************************************/


//Initialization Method
void InitOpenGL(void);


/***********************************************~***************************************************/


#endif //__WILDCAT_GL_UTILITIES_H__

