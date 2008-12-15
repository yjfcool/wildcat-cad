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


#ifndef __GEOMETRY_CONTEXT_H__
#define __GEOMETRY_CONTEXT_H__


/*** Included Headers ***/
#include "Geometry/wgeol.h"


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


class WCGeometryContext {
private:
	//General Context Objects
	WCGLContext									*_glContext;										//!< Pointer to GL context
	WCShaderManager								*_shaderManager;									//!< Pointer to shader manager for GL context
		
	//NURBS Curve Objects
	int											_ncPerfLevel;										//!< Static performance level indicator
	GLint										*_ncLocations;										//!< Values for locations of uniforms
	GLuint										_ncDefault, _ncDefault23, _ncBezier23;				//!< Static shader programs for vertex generation
	int											_ncMinCPBufferSize, _ncMinKPBufferSize;				//!< Values for the buffer sizes of CP and KP in shaders
	GLint										_ncVertsPerBatch;									//!< Number of vertices per batch
	GLuint										_ncCPBuffer, _ncKPBuffer;							//!< High uniform buffers for generation
	GLuint										_ncCPTex, _ncKPTex, _ncOutTex;						//!< Medium textures for generation
	GLint										_ncMaxTexSize;										//!< Medium maxium texture size
	GLuint										_ncFramebuffer;										//!< Framebuffer for medium generate use

	//NURBS Surface Objects
	WPUInt										_nsPerfLevel;										//!< Static performance level indicator
	GLint										*_nsLocations;										//!< Values for locations of uniforms	
	GLuint										_nsDefault, _nsDefault23, _nsBezier23;				//!< Static shader programs for vertex generation	
	int											_nsMinCPBufferSize, _nsMinKPBufferSize;				//!< Values for the buffer sizes of CP and KP in shaders
	GLint										_nsVertsPerBatch;									//!< Number of vertices per batch
	GLuint										_nsCPBuffer, _nsKPUBuffer, _nsKPVBuffer;			//!< High uniform buffers for generation
	GLuint										_nsCPTex, _nsKPUTex, _nsKPVTex, _nsVertTex, _nsNormTex, _nsTexTex;//!< Texture objects for use in medium generate
	GLint										_nsMaxTexSize;										//!< Medium maxium texture size	
	GLuint										_nsFramebuffer;										//!< Framebuffer for generate use

	//Trim NURBS Surface Objects
	GLint										*_tsLocations;										//!< Values for locations of uniforms
	GLint										_tsMaxTexSize;										//!< Maxium texture size	
	GLuint										_tsPointInversion, _tsTriangulate;					//!< Static shader programs for trims
	GLuint										_tsInTex, _tsSurfTex, _tsOutTex;					//!< Textures for trim generation
	GLuint										_tsFramebuffer;										//!< Framebuffer for trim surface use

	//Intersection Objects
	GLint										*_iLocations;										//!< Values for locations of uniforms
	GLuint										_cliM, _cciM, _sliM, _sciM, _ssiM, _tliM, _tciM, _tsiM, _ttiM;	//!< Static shader programs for intersection
	GLuint										_cciLeftTex, _cciRightTex, _ssiLeftTex, _ssiRightTex;//!< Textures for intersection
	GLuint										_cciOutTex, _ssiOutTex;								//!< Output textures for framebuffers
	GLuint										_cciFramebuffer, _ssiFramebuffer;					//!< Framebuffers for intersection
	
	//Private Initalization Methods
	void StartCurve(void);																			//!< Set up all NURBS curve global objects
	void StopCurve(void);																			//!< Destroy all NURBS curve global objects
	void StartSurface(void);																		//!< Set up all NURBS surface global objects
	void StopSurface(void);																			//!< Destroy all NURBS surface global objects
	void StartTrimSurface(void);																	//!< Set up all Trimmed NURBS surface global objects
	void StopTrimSurface(void);																		//!< Destroy all Trimmed NURBS curve global objects
	void StartIntersection(void);																	//!< Set up all Intersection global objects
	void StopIntersection(void);																	//!< Destroy all Intersection global objects
	
	//Deny Access
	WCGeometryContext();																			//!< Deny access to default constructor
	WCGeometryContext(const WCGeometryContext &context);											//!< Deny access to copy constructor
	WCGeometryContext& operator=(const WCGeometryContext &context);									//!< Deny access to equals operator
	
public:
	//Constructors and Destructors
	WCGeometryContext(WCGLContext *context, WCShaderManager *shaderManager);						//!< Primary constructor
	~WCGeometryContext();																			//!< Default destructor
	
	//General Access Methods
	inline WCGLContext* Context(void)			{ return this->_glContext; }						//!< Get the GL context
	inline WCShaderManager* ShaderManager(void)	{ return this->_shaderManager; }					//!< Get the shader manager
	
	//NURBS Curve Access Methods
	inline WPUInt CurvePerformanceLevel(void) const		{ return this->_ncPerfLevel; }				//!< Get curve generation performance level
	inline GLint* CurveLocations(void)					{ return this->_ncLocations; }				//!< Get curve locations
	inline GLuint CurveDefaultProgram(void) const		{ return this->_ncDefault; }				//!< Get curve default program
	inline GLuint CurveDefault23Program(void) const		{ return this->_ncDefault23; }				//!< Get curve default23 program
	inline GLuint CurveBezier23Program(void) const		{ return this->_ncBezier23; }				//!< Get curve bezier23 program
	inline int CurveMinCPBufferSize(void) const			{ return this->_ncMinCPBufferSize; }		//!< Get curve min CP buffer size
	inline int CurveMinKPBufferSize(void) const			{ return this->_ncMinKPBufferSize; }		//!< Get curve min KP buffer size
	inline GLint CurveVerticesPerBatch(void) const		{ return this->_ncVertsPerBatch; }			//!< Get curve vertices per batch
	inline GLuint CurveCPBuffer(void) const				{ return this->_ncCPBuffer; }				//!< Get curve cp buffer
	inline GLuint CurveKPBuffer(void) const				{ return this->_ncKPBuffer; }				//!< Get curve kp buffer
	inline GLuint CurveCPTex(void) const				{ return this->_ncCPTex; }					//!< Get curve cp texture
	inline GLuint CurveKPTex(void) const				{ return this->_ncKPTex; }					//!< Get curve kp texture
	inline GLuint CurveOutTex(void) const				{ return this->_ncOutTex; }					//!< Get curve output texture
	inline GLint CurveMaxTextureSize(void) const		{ return this->_ncMaxTexSize; }				//!< Get curve maximum texture size
	inline GLuint CurveFramebuffer(void) const			{ return this->_ncFramebuffer; }			//!< Get curve framebuffer
	
	//NURBS Surface Access Methods
	inline WPUInt SurfacePerformanceLevel(void) const	{ return this->_nsPerfLevel; }				//!< Get surface generation performance level
	inline GLint* SurfaceLocations(void)				{ return this->_nsLocations; }				//!< Get surface locations
	inline GLuint SurfaceDefaultProgram(void) const		{ return this->_nsDefault; }				//!< Get surface default program
	inline GLuint SurfaceDefault23Program(void) const	{ return this->_nsDefault23; }				//!< Get surface default23 program
	inline GLuint SurfaceBezier23Program(void) const	{ return this->_nsBezier23; }				//!< Get surface bezier23 program
	inline int SurfaceMinCPBufferSize(void) const		{ return this->_nsMinCPBufferSize; }		//!< Get surface minimum cp buffer size
	inline int SurfaceMinKPBufferSize(void) const		{ return this->_nsMinKPBufferSize; }		//!< Get surface mimimum kp buffer size
	inline GLint SurfaceVerticesPerBatch(void) const	{ return this->_nsVertsPerBatch; }			//!< Get surface vertices per batch
	inline GLuint SurfaceCPBuffer(void) const			{ return this->_nsCPBuffer; }				//!< Get surface cp buffer
	inline GLuint SurfaceKPUBuffer(void) const			{ return this->_nsKPUBuffer; }				//!< Get surface kp-u buffer
	inline GLuint SurfaceKPVBuffer(void) const			{ return this->_nsKPVBuffer; }				//!< Get surface kp-v buffer
	inline GLuint SurfaceCPTex(void) const				{ return this->_nsCPTex; }					//!< Get surface cp texture
	inline GLuint SurfaceKPUTex(void) const				{ return this->_nsKPUTex; }					//!< Get surface kp-u texture
	inline GLuint SurfaceKPVTex(void) const				{ return this->_nsKPVTex; }					//!< Get surface kp-v texture
	inline GLuint SurfaceVerticesTex(void) const		{ return this->_nsVertTex; }				//!< Get surface vertex output texture
	inline GLuint SurfaceNormTex(void) const			{ return this->_nsNormTex; }				//!< Get surface normals output texture
	inline GLuint SurfaceTexCoorTex(void) const			{ return this->_nsTexTex; }					//!< Get surface texcoords output texture
	inline GLint SurfaceMaxTextureSize(void) const		{ return this->_nsMaxTexSize; }				//!< Get surface maximum texture size
	inline GLuint SurfaceFramebuffer(void) const		{ return this->_nsFramebuffer; }			//!< Get surface framebuffer

	//Trimmed NURBS Surface Access Methods
	inline GLint* TrimLocations(void)					{ return this->_tsLocations; }				//!< Get trim surface locations
	inline GLint TrimMaxTextureSize(void) const			{ return this->_tsMaxTexSize; }				//!< Get trim maximum texture size
	inline GLuint TrimInversionProgram(void) const		{ return this->_tsPointInversion; }			//!< Get point inversion program
	inline GLuint TrimTriangulateProgram(void) const	{ return this->_tsTriangulate; }			//!< Get triangulation program
	inline GLuint TrimInTex(void) const					{ return this->_tsInTex; }					//!< Get trim surface input texture
	inline GLuint TrimSurfTex(void) const				{ return this->_tsSurfTex; }				//!< Get trim surface surface data texture
	inline GLuint TrimOutTex(void) const				{ return this->_tsOutTex; }					//!< Get trim surface output texture
	inline GLuint TrimFramebuffer(void) const			{ return this->_tsFramebuffer; }			//!< Get trim surface framebuffer
	
	//Intersection Values Access Methods
	inline GLint* IntersectionLocations(void)			{ return this->_iLocations; }				//!< Get intersection locations
	inline GLuint CurveLineProgram(void) const			{ return this->_cliM; }						//!< Get curve-line program
	inline GLuint CurveCurveProgram(void) const			{ return this->_cciM; }						//!< Get curve-curve program
	inline GLuint SurfaceLineProgram(void) const		{ return this->_sliM; }						//!< Get surface-line program
	inline GLuint SurfaceCurveProgram(void) const		{ return this->_sciM; }						//!< Get surface-curve program
	inline GLuint SurfaceSurfaceProgram(void) const		{ return this->_ssiM; }						//!< Get surface-surface program
	inline GLuint TrimLineProgram(void) const			{ return this->_tliM; }						//!< Get trimSurface-line program
	inline GLuint TrimCurveProgram(void) const			{ return this->_tciM; }						//!< Get trimSurface-curve program
	inline GLuint TrimSurfaceProgram(void) const		{ return this->_tsiM; }						//!< Get trimSurface-surface program
	inline GLuint TrimTrimProgram(void) const			{ return this->_ttiM; }						//!< Get trimSurface-trimSurface program
	inline GLuint CCILeftTex(void) const				{ return this->_cciLeftTex; }				//!< Get CCI left texture
	inline GLuint CCIRightTex(void) const				{ return this->_cciRightTex; }				//!< Get CCI right texture
	inline GLuint CCIOutTex(void) const					{ return this->_cciOutTex; }				//!< Get CCI output texture
	inline GLuint SSIOutTex(void) const					{ return this->_ssiOutTex; }				//!< Get SSI output texture
	inline GLuint CurveCurveFramebuffer(void) const		{ return this->_cciFramebuffer; }			//!< Get CCI framebuffer
	inline GLuint SurfaceSurfaceFramebuffer(void) const	{ return this->_ssiFramebuffer; }			//!< Get SSI framebuffer

	/*** Non-Member Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCGeometryContext &context);			//!< Overloaded output operator
	
	//Shader Access Methods
	GLuint ProgramFromName(const std::string &name);												//!< Get a program from a name
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__GEOMETRY_CONTEXT_H__

