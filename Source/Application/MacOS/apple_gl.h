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


#ifndef __APPLE_GL_H__
#define __APPLE_GL_H__


/*** Included Apple Header Files ***/
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <OpenGL/OpenGL.h>


/***********************************************~***************************************************/


//GL_TRANSFORM_FEEDBACK_EXT
#ifndef GL_EXT_transform_feedback
#define GL_INTERLEAVED_ATTRIBS_EXT				0
#define GL_SEPARATE_ATTRIBS_EXT					1
void glTransformFeedbackVaryingsEXT(GLuint id, GLuint count, const GLchar** varyings, GLuint type) { }
#endif //GL_EXT_transform_feedback

//GL_GEOMETRY_SHADER_EXT
#ifndef GL_EXT_geometry_shader4
#define GL_GEOMETRY_SHADER_EXT					0
#define GL_GEOMETRY_INPUT_TYPE_EXT				1
#define GL_GEOMETRY_VERTICES_OUT_EXT			2
#define GL_GEOMETRY_OUTPUT_TYPE_EXT				3
void glProgramParameteriEXT(GLuint id, GLenum type, GLenum value) { }
#endif //GL_EXT_geometry_shader


/***********************************************~***************************************************/


#endif //__APPLE_GL_H__

