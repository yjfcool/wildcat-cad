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


#ifndef __SKETCH_H__
#define __SKETCH_H__


/*** Included Header Files ***/
#include <Kernel/wftrl.h>
#include <Sketcher/sketch_actions.h>
#include <Sketcher/sketch_workbench.h>
#include <PartDesign/part_feature.h>


/*** Locally Defined Values ***/
#define SKETCH_CLASSNAME						"Sketch"
#define SKETCH_AXIS_COLOR						1.0f, 0.5f, 0.5f, 0.8f
#define SKETCH_AXIS_SCALE						1.0
#define SKETCH_ANALYSIS_TOLERANCE				0.01


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCPartPlane;
class WCSketchFeature;
class WCSketchConstraint;
class WCConstraintPlanner;
class WCSketchPoint;
class WCSketchProfile;


/***********************************************~***************************************************/


class WCSketch : public WCPartFeature, public virtual WCVisualObject {
protected:
	WCSketchWorkbench							*_workbench;										//!< Sketch workbench
	WCPartPlane									*_refPlane;											//!< Reference plane
	WCConstraintPlanner							*_planner;											//!< Constraint planner
	std::map<std::string, WCSketchFeature*>		_featureMap;										//!< Sketch features
	std::list<WCSketchFeature*>					_featureList;										//!< List of features
	std::map<WCGeometricPoint*,WCEventController*>	_pointMap;										//!< Sketch points
	std::map<WCGeometricLine*,WCEventController*>	_lineMap;										//!< Sketch lines
	std::map<WCNurbsCurve*,WCEventController*>	_curveMap;											//!< Sketch curves
	std::list<WCSketchConstraint*>				_constraintList;									//!< Sketch constraint list
	std::list<WCSketchProfile*>					_profileList;										//!< Sketch output profile list
	WCTreeElement								*_refTreeElement;									//!< Holder for all reference elements
	WCTreeElement								*_featureTreeElement;								//!< Holder for all feature elements
	WCTreeElement								*_constraintTreeElement;							//!< Holder for all constraint elements
	WCTreeElement								*_profileTreeElement;								//!< Holder for all profile elements
	bool										_drawConstraints;									//!< Should constraints be drawn
	WCText										*_hText, *_vText;									//!< Text labels
private:
	void Initialize(void);																			//!< Private initializer
	void GenerateFeatureGeometry(void);																//!< Generate geometry to publish
	void GenerateTopology(void);																	//!< Generate topology
	bool IsConnectedToProfile(std::list<WCGeometricCurve*> &list, WCGeometricCurve *curve);			//!< Sketch analysis processing
	//Deny Access
	WCSketch();																						//!< Deny access to default constructor
	WCSketch(const WCSketch& sketch);																//!< Deny access to copy constructor
	WCSketch& operator=(const WCSketch& sketch);													//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCSketch(WCFeature *creator, const std::string &name, WCPartPlane* plane);						//!< Primary constructor
	WCSketch(xercesc::DOMElement *element, WCSerialDictionary *dictionary);							//!< Persistance constructor
	~WCSketch();																					//!< Default destructor
	
	//Member Access Methods
	inline WCPartPlane* ReferencePlane(void)	{ return this->_refPlane; }							//!< Get reference plane
	inline WCConstraintPlanner* ConstraintPlanner(void)	{ return this->_planner; }					//!< Get the constraint planner
	inline std::map<std::string, WCSketchFeature*>& FeatureMap(void)								//!< Get the feature map
												{ return  this->_featureMap; }
	inline std::map<WCGeometricPoint*,WCEventController*>& PointMap(void)							//!< Get the point map
												{ return this->_pointMap;}
	inline std::map<WCGeometricLine*,WCEventController*>& LineMap(void)								//!< Get the line map
												{ return this->_lineMap; }
	inline std::map<WCNurbsCurve*,WCEventController*>& CurveMap(void)								//!< Get the curve map
												{ return this->_curveMap;}
	inline WCTreeElement* ReferencesTreeElement(void)	{ return this->_refTreeElement; }			//!< Get references tree element
	inline WCTreeElement* FeaturesTreeElement(void)		{ return this->_featureTreeElement; }		//!< Get features tree element
	inline WCTreeElement* ConstraintsTreeElement(void)	{ return this->_constraintTreeElement; }	//!< Get constraints tree element
	inline WCTreeElement* ProfilesTreeElement(void)		{ return this->_profileTreeElement; }		//!< Get profiles tree element
	inline bool DrawConstraints(void) const		{ return this->_drawConstraints; }					//!< Get constraint drawing state
	inline void DrawConstraints(const bool &state)		{ this->_drawConstraints = state; }			//!< Set constraint drawing state
	inline WCSketchWorkbench* Workbench(void)	{ return this->_workbench; }						//!< Get the sketch workbench

	//Sketch-Part Interaction Methods
	void PublishGeometry(void);																		//!< Put sketch geometry into the part
	void UnpublishGeometry(void);																	//!< Remove geometry from the part
	inline WPUInt ProfileCount(void)			{ return (WPUInt)this->_profileList.size(); }		//!< Get the number of profiles
	inline std::list<WCSketchProfile*> Profiles(void) const { return this->_profileList; }			//!< Get the list of profiles
	
	//Sketch Feature Methods
	bool AddFeature(WCSketchFeature *feature);														//!< Add a sketch feature
	bool RemoveFeature(WCSketchFeature *feature);													//!< Remove a sketch feature
	bool DeleteFeatures(std::list<WCSketchFeature*> features);										//!< Completely delete sketch features
	bool AddConstraint(WCSketchConstraint *constraint);												//!< Add a sketch constraint
	bool RemoveConstraint(WCSketchConstraint *constraint);											//!< Remove constraint from sketch
	std::string GenerateFeatureName(WCSketchFeature *feature);										//!< Find a good name
	WCSketchFeature* FeatureFromName(const std::string &name);										//!< Get feature from name
	inline bool CheckName(const std::string &name){ return (this->_featureMap.find(name) ==			//!< See if name is used
												  this->_featureMap.end()); }

	//Sketch Analysis Methods
	void Analyze(void);																				//!< Analyze geometry, constraint, and projection
	void AnalyzeConstraints(void);																	//!< Analyze sketch constraints
	void AnalyzeProjections(void);																	//!< Analyze 3D projections
	void AnalyzeGeometry(void);																		//!< Analyze points, lines, curves, profiles
	void AnalyzePoints(void);																		//!< Analyze sketch points
	void AnalyzeLines(void);																		//!< Analyze sketch lines
	void AnalyzeCurves(void);																		//!< Analyze sketch curves
	void AnalyzeProfiles(void);																		//!< Analyze sketch profiles

	//Sketch Geometry Methods
	bool AddPoint(WCGeometricPoint *point, WCEventController* controller);							//!< Add a point + controller
	bool RemovePoint(WCGeometricPoint *point);														//!< Remove point + controller
	bool AddLine(WCGeometricLine *line, WCEventController* controller);								//!< Add a curve + controller
	bool RemoveLine(WCGeometricLine *line);															//!< Remove curve + controller
	bool AddCurve(WCNurbsCurve *curve, WCEventController* controller);								//!< Add a curve + controller
	bool RemoveCurve(WCNurbsCurve *curve);															//!< Remove curve + controller

	//Inherited Methods
	inline std::string RootName(void) const		{ return SKETCH_CLASSNAME; }						//!< Get the class name
	void ReceiveNotice(WCObjectMsg msg, WCObject *sender);											//!< Receive notice from sub-features
	bool Regenerate(void);																			//!< Validate and rebuild
	WCAlignedBoundingBox BoundingBox(void);															//!< Get bounding box
	void OnSelection(const bool fromManager, std::list<WCVisualObject*> objects);					//!< Called on selection
	void OnDeselection(const bool fromManager);														//!< Called on deselection	
	void Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom);				//!< Render the object
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object

	/*** Actions ***/
	static WCAction* ActionCreate(WCFeature *creator, const std::string sketchName, WCPartPlane *refPlane);//!< Primary create action
	static WCAction* ActionMarkConstruction(WCSketch *sketch, std::list<WCSketchFeature*> features);	//!< Mark as construction elements
	static WCAction* ActionMarkNonConstruction(WCSketch *sketch, std::list<WCSketchFeature*> features);	//!< Mark as not construction elements
	static WCAction* ActionDelete(WCSketch *sketch, std::list<WCSketchFeature*> features);			//!< Primary delete action
	
	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCSketch &sketch);						//!< Overloaded output operator	
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__SKETCH_H__

