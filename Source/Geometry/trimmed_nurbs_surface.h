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
#define TRIMSURFACE_TEX_LOCATION				0
#define TRIMSURFACE_DEFAULT_TEX_WIDTH			256
#define TRIMSURFACE_DEFAULT_TEX_HEIGHT			256
//Performance Levels
#define TRIMSURFACE_PERFLEVEL_HIGH				0
#define TRIMSURFACE_PERFLEVEL_MEDIUM			1
#define TRIMSURFACE_PERFLEVEL_LOW				2


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
//None


/***********************************************~***************************************************/


struct WCTrimTriangulation {
	GLuint numTriangles;
	GLuint vertexBuffer;
	GLuint indexBuffer;
	bool outside;
};


typedef std::pair<std::list<std::pair<WCGeometricCurve*,bool> >,bool> WCTrimProfile;
std::list<WCVector4> TrimProfileBoundaryList(std::list<std::pair<WCGeometricCurve*,bool> > &curveList, const bool detailed);


/***********************************************~***************************************************/


class WCTrimmedNurbsSurface : public WCNurbsSurface {
protected:
	WCGeometryContext							*_context;											//!< Geometry context object
	std::list<WCTrimTriangulation>				_triList;											//!< List of trim triangulations
	std::list<WCTrimProfile>					_profileList;										//!< List of profiles
	WCMatrix4									_trimMatrix, _invTrimMatrix;						//!< Orientation matrices
	GLuint										_trimTexture;										//!< Trim texture
	WPFloat										_width, _height;									//!< Width and height of the surface
	WPUInt										_texWidth, _texHeight;								//!< Trim texture width and height

private:
	//Private Methods
	void ClearTriangulations(void);																	//!< Clear all triangulations (GL buffers)
	std::list<WCVector4> BoundaryList(std::list<std::pair<WCGeometricCurve*,bool> > curveList);		//!< Get boundary points for a profile
	void GenerateTrimList(void);																	//!< Generate trim list
	void GenerateTexture(void);																		//!< Generate texture for display
	
	//Access Denied
	WCTrimmedNurbsSurface();																		//!< Deny access to default constructor
	WCTrimmedNurbsSurface(const WCTrimmedNurbsSurface &surf);										//!< For now deny access to copy constructor
	WCTrimmedNurbsSurface& operator=(const WCTrimmedNurbsSurface &surf);							//!< For now deny access to equals operator
public:
	//Constructors and Destructors
	WCTrimmedNurbsSurface(WCGeometryContext *context, const std::list<WCTrimProfile> &profileList,	//!< Primary constructor
					const WPUInt &degreeU, const WPUInt &degreeV,
					const WPUInt &cpU, const WPUInt &cpV, const std::vector<WCVector4> &controlPoints,
					const WCNurbsMode &modeU, const WCNurbsMode &modeV,
					const std::vector<WPFloat> &kpU=std::vector<WPFloat>(), const std::vector<WPFloat> &kpV=std::vector<WPFloat>());
	WCTrimmedNurbsSurface(xercesc::DOMElement *element, WCSerialDictionary *dictionary);			//!< Persistance constructor
	~WCTrimmedNurbsSurface();																		//!< Default destructor
	
	//Member Access Functions
	inline GLuint TrimTexture(void) const		{ return this->_trimTexture; }						//!< Get the ID for the texture
	inline WPUInt TextureSize(void) const		{ return this->_texWidth; }							//!< Get the texture size
	void TextureSize(const WPUInt &size);															//!< Set the texture size
	
	//Inherited Member Functions
	bool Intersect(const WCGeometricPoint &point)		{ return false; }							//!< Check for intersection with point
	bool Intersect(const WCGeometricCurve &curve)		{ return false; }							//!< Check for intersection with curve	
	bool Intersect(const WCGeometricSurface &surface)	{ return false; }							//!< Check for intersection with surface		
	WPFloat Area(void)							{ return 1.0; }										//!< Return the area of the surface
	void Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom);				//!< Render the object
	void ReceiveNotice(WCObjectMsg msg, WCObject *sender);											//!< Receive messages from other objects

	//Serialization and Object Methods
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
	
	/*** Non-Member Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCTrimmedNurbsSurface &surface);		//!< Overloaded output operator
};
 

/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__TRIMMED_NURBS_SURFACE_H__

