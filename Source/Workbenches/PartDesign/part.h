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


#ifndef __PART_H__
#define __PART_H__


/*** Included Header Files ***/
#include "Kernel/wftrl.h"
#include "Kernel/document.h"
#include "PartDesign/part_actions.h"
#include "PartDesign/part_controller.h"
#include "PartDesign/part_workbench.h"


/*** Locally Defined Values ***/
#define PART_CLASSNAME							"Part"


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCPartFeature;
class WCPartBody;


/***********************************************~***************************************************/


class WCPart : public WCDocument {
protected:
	//Base Objects
	std::map<std::string, WCPartFeature*>		_featureMap;										//!< Map of all feature in the part
	std::list<WCPartFeature*>					_featureList;										//!< List of all features in the part
	WCPartWorkbench								*_workbench;										//!< Associated part workbench
	WCPartBody									*_currentBody;										//!< Current body

	//Scene Objects
	WCPointLayer								*_pointLayer;										//!< Point layer
	WCLineLayer									*_lineLayer;										//!< Line layer
	WCVisualLayer								*_curveLayer;										//!< Curve layer
	WCVisualLayer								*_surfaceLayer;										//!< Surface layer

	//Geometry Objects
	std::map<WCGeometricPoint*,WCEventController*>	_pointMap;										//!< Part points
	std::map<WCGeometricLine*,WCEventController*>	_lineMap;										//!< Part lines
	std::map<WCGeometricCurve*,WCEventController*>	_curveMap;										//!< Part curves
	std::map<WCGeometricSurface*,WCEventController*>_surfaceMap;									//!< Part surfaces

	//Topology Objects
	//...
private:
	void Initialize(void);																			//!< Initialization method
	//Hidden Constructors
	WCPart();																						//!< Deny access to default constructor
	WCPart(const WCPart& part);																		//!< Deny access to copy constructor
	WCPart& operator=(const WCPart& part);															//!< Deny access to equals operator
	//Friend Declarations
	friend class WCPartWorkbench;																	//!< Workbench is a friend
public:
	//Constructors and Destructors
	WCPart(std::string name, std::string filename="");												//!< Primary constructor
	WCPart(xercesc::DOMElement *element, WCSerialDictionary *dictionary);							//!< Persistance constructor
	~WCPart();																						//!< Default destructor

	//Member Access Methods
	inline WCLineLayer *LinesLayer(void)		{ return this->_lineLayer; }						//!< Get the lines layer
	inline WCPointLayer* PointsLayer(void)		{ return this->_pointLayer; }						//!< Get the points layer
	inline WCVisualLayer* CurvesLayer(void)		{ return this->_curveLayer; }						//!< Get the curves layer
	inline WCVisualLayer* SurfacesLayer(void)	{ return this->_surfaceLayer; }						//!< Get the surfaces layer
	inline WCPartBody* Body(void)				{ return this->_currentBody; }						//!< Get the current body
	void Body(WCPartBody* body);																	//!< Set the current body
	inline std::map<WCGeometricPoint*,WCEventController*>& PointMap(void) { return this->_pointMap;}//!< Get the point map
	inline std::map<WCGeometricLine*,WCEventController*>& LineMap(void) { return this->_lineMap; }	//!< Get the line map
	inline std::map<WCGeometricCurve*,WCEventController*>& CurveMap(void) { return this->_curveMap;}//!< Get the curve map

	//Feature Methods
	bool AddFeature(WCPartFeature *feature, const bool &selectable);								//!< Add a part feature
	bool RemoveFeature(WCPartFeature *feature, const bool &selectable);								//!< Remove a part feature
	bool DeleteFeatures(std::list<WCPartFeature*> features);										//!< Completely delete features
	bool ReorderFeature(WCPartFeature *feature);													//!< Reorder a part feature
	inline WCWorkbench* Workbench(void)			{ return this->_workbench; }						//!< Get the part workbench
	WCPartFeature* FeatureFromName(const std::string &name);										//!< Get feature from name
	std::string GenerateFeatureName(WCFeature *feature);											//< Find a good name
	inline bool CheckName(const std::string &name){ return (this->_featureMap.find(name) ==			//!< See if name is used
												  this->_featureMap.end()); }

	//Geometry Methods
	bool AddPoint(WCGeometricPoint *point, WCEventController* controller);							//!< Add a point + controller
	bool RemovePoint(WCGeometricPoint *point);														//!< Remove point + controller
	bool AddLine(WCGeometricLine *line, WCEventController* controller);								//!< Add a curve + controller
	bool RemoveLine(WCGeometricLine *line);															//!< Remove curve + controller
	bool AddCurve(WCGeometricCurve *curve, WCEventController* controller);							//!< Add a curve + controller
	bool RemoveCurve(WCGeometricCurve *curve);														//!< Remove curve + controller
	bool AddSurface(WCGeometricSurface *surface, WCEventController* controller);					//!< Add a surface + controller
	bool RemoveSurface(WCGeometricSurface *surface);												//!< Remove surface + controller
	
	//Topology Methods
	//...

	//Required Inherited Methods
	bool Name(const std::string &name);																//!< Modify name - must check
	std::string RootName(void) const			{ return PART_CLASSNAME; }							//!< Get the class name
	void MarkDirty(void);																			//!< Mark as dirty
	void ReceiveNotice(WCObjectMsg msg, WCObject *sender);											//!< Receive notice from sub-features
	bool Regenerate(void);																			//!< Validate and rebuild
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
	xercesc::DOMDocument* Serialize(void);															//!< Document serialization method
	WCAlignedBoundingBox BoundingBox(void)		{ return WCAlignedBoundingBox(); }					//!< For now
	void Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom) { }			//!< Render the object	

	/*** Actions and Drawing Modes ***/
	static WCActionPartDelete* ActionDelete(WCPart *part, std::list<WCPartFeature*> features);		//!< Primary delete action

	/*** Friend Methods ***/
	friend std::ostream& operator<<(std::ostream& out, const WCPart &part);							//!< Overloaded output operator	
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__PART_H__

