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


#ifndef __TRIMMED_NURBS_SURFACE_H__
#define __TRIMMED_NURBS_SURFACE_H__


/*** Included Header Files ***/
#include "Geometry/wgeol.h"
#include "Geometry/nurbs_surface.h"


/*** Locally Defined Values ***/
//Size Constants
#define TRIMSURFACE_MAX_TEX_SIZE				512
#define TRIMSURFACE_PI_TEX_SIZE					32
//Shader Location Constants
#define TRIMSURFACE_LOC_PI_PARAMS				0
#define TRIMSURFACE_LOC_PI_SURFDATA				1


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


typedef std::list<std::pair<WCGeometricCurve*,bool> > WCTrimProfile;


/***********************************************~***************************************************/


class WCTrimmedNurbsSurface : public WCNurbsSurface {
protected:
	std::list<WCTrimProfile>					_profileList;										//!< List of profiles
	bool										_isTextureDirty;									//!< Texture dirty flag
	GLuint										_trimTexture;										//!< Trim texture
	GLuint										_texWidth, _texHeight;								//!< Trim texture width and height

private:
	//Private Methods
	void GeneratePISurfaceTexture(const GLfloat* buffer, const WPUInt &lodU, const WPUInt &lodV);	//!< Setup PI surface texture
	GLuint PointInversionHigh(std::list<WCVector4> &boundaryList);									//!< Invert list of points - GPU-based method
	GLuint PointInversionLow(std::list<WCVector4> &boundaryList);									//!< Invert list of points - CPU-based method
	GLuint GenerateTriangulations(std::list<GLuint> &triList);										//!< Generate vertex list
	//Hidden Constructors
	WCTrimmedNurbsSurface();																		//!< Deny access to default constructor
public:
	//Constructors and Destructors
	WCTrimmedNurbsSurface(WCGeometryContext *context, const std::list<WCTrimProfile> &profileList,	//!< Primary constructor
					const WPUInt &degreeU, const WPUInt &degreeV,
					const WPUInt &cpU, const WPUInt &cpV, const std::vector<WCVector4> &controlPoints,
					const WCNurbsMode &modeU, const WCNurbsMode &modeV,
					const std::vector<WPFloat> &kpU=std::vector<WPFloat>(), const std::vector<WPFloat> &kpV=std::vector<WPFloat>());
	WCTrimmedNurbsSurface(const WCTrimmedNurbsSurface &surf);										//!< Copy constructor
	WCTrimmedNurbsSurface(xercesc::DOMElement *element, WCSerialDictionary *dictionary);			//!< Persistance constructor
	virtual ~WCTrimmedNurbsSurface();																//!< Default destructor

	//General Access Methods
	inline bool IsTextureDirty(void)			{ return this->_isTextureDirty; }					//!< Get the texture dirty flag
	inline void MarkTextureDirty(void)			{ this->_isTextureDirty = true; }					//!< Mark the texture as dirty

	//Inherited Member Methods
	virtual WPFloat Area(const WPFloat &tolerance=GEOMETRICOBJECT_DEFAULT_EPSILON);					//!< Return the area of the surface
	virtual WCVector4 Evaluate(const WPFloat &u, const WPFloat &v);									//!< Evaluate a specific point on the surface
	virtual WCVector4 Derivative(const WPFloat &u, const WPUInt &uDer,								//!< Get the surface derivative
								 const WPFloat &v, const WPUInt &vDer);
	virtual WCRay Tangent(const WPFloat &u, const WPFloat &v);										//!< Get a tangential ray from the surface at u,v
	virtual std::pair<WCVector4,WCVector4> PointInversion(const WCVector4 &point);					//!< Project from point to closest location on surface
	virtual WCVisualObject* HitTest(const WCRay &ray, const WPFloat &tolerance);					//!< Hit test with a ray	
	virtual void Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom);		//!< Render the object
	virtual void ReceiveNotice(WCObjectMsg msg, WCObject *sender);									//!< Receive messages from other objects

	//Original Member Functions
	void GenerateTrimTexture(GLuint &texWidth, GLuint &texHeight, GLuint &texture, const bool &managed);//!< Generate trim texture
	void ReleaseTrimTexture(GLuint &texture);														//!< Release the trim texture
	void GenerateTessellation(WPUInt &lodU, WPUInt &lodV, std::vector<GLfloat*> &buffers);			//!< Generate tessellation of surface

	//Operator Overloads
	WCTrimmedNurbsSurface& operator=(const WCTrimmedNurbsSurface &surface);							//!< Equals operator
	bool operator==(const WCTrimmedNurbsSurface &surface);											//!< Equality operator	

	//Serialization and Object Methods
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
	
	/*** Non-Member Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCTrimmedNurbsSurface &surface);		//!< Overloaded output operator
};
 

/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__TRIMMED_NURBS_SURFACE_H__

