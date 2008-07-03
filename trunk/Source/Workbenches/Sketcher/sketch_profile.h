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


#ifndef __SKETCH_PROFILE_H__
#define __SKETCH_PROFILE_H__


/*** Included Header Files ***/
#include "Kernel/wftrl.h"
#include "Sketcher/sketch_feature.h"
#include "Sketcher/sketch_profile_controller.h"


/*** Locally Defined Values ***/
#define SKETCHPROFILE_CLASSNAME					"Profile"
#define SKETCH_PROFILE_DEFAULT_TOLERANCE		0.00001


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCSketch;
class WCSketchAxis;


/***********************************************~***************************************************/


struct WCProfileTreeNode {
	WCSketchProfile* profile;
	std::list<WCProfileTreeNode*> children;
};


/***********************************************~***************************************************/


//Defines for profile types
enum ProfileType {
	ProfileOutside,
	ProfileInside,
	ProfileDistinct,
	ProfileCoincident,
	ProfileIntersect
};

class WCProfileType {
private:
	ProfileType									_type;												//!< Type id
	WCProfileType();																				//!< Deny access to default constructor 
	WCProfileType(ProfileType type)				{ this->_type = type; }								//!< Hidden primary constructor
	friend class WCScene;
public:
	//Constructors and Destructors
	WCProfileType(const WCProfileType &type) : _type(type._type)	{ }								//!< Copy constructor
	~WCProfileType()							{ }													//!< Default destructor
	static WCProfileType Outside(void)			{ return WCProfileType(ProfileOutside); }			//!< Outside type
	static WCProfileType Inside(void)			{ return WCProfileType(ProfileInside); }			//!< Inside type
	static WCProfileType Distinct(void)			{ return WCProfileType(ProfileDistinct); }			//!< Distinct type
	static WCProfileType Coincident(void)		{ return WCProfileType(ProfileCoincident); }		//!< Coincident type
	static WCProfileType Intersect(void)		{ return WCProfileType(ProfileIntersect); }			//!< Intersection type
	
	//Overridden Operators
	WCProfileType& operator=(const WCProfileType &type)	{ this->_type = type._type; return *this; }	//!< Type equation				
	bool operator==(const WCProfileType &type) const { return this->_type == type._type; }			//!< Equals operator
	bool operator!=(const WCProfileType &type) const { return this->_type != type._type; }			//!< Inequality operator
};


/***********************************************~***************************************************/


class WCSketchProfile : public WCSketchFeature {
protected:
	std::list< std::pair<WCGeometricCurve*,bool> > _curveList;										//!< Ordered list of curves and directions
	bool										_isLinear;											//!< Is the profile linear
	bool										_isClosed;											//!< Is the profile closed
	bool										_isSelfIntersecting;								//!< Does it intersect itself
private:
	//Private Methods
	bool DetermineIsLinear(const std::list<WCGeometricCurve*> &list);								//!< Determine if profile is linear
	bool DetermineIsClosed(void);																	//!< Determine if profile is closed
	bool DetermineIsSelfIntersecting(void);															//!< Determine if profile is self-intersecting
	void ForceClockwise(void);																		//!< Force the profile to be defined clockwise

	//Deny Access
	WCSketchProfile();																				//!< Deny access to default constructor
	WCSketchProfile(const WCSketchProfile& pt);														//!< Deny access to copy constructor
	WCSketchProfile& operator=(const WCSketchProfile &pt);											//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCSketchProfile(WCSketch *sketch, const std::string &name, std::list<WCGeometricCurve*> &list);	//!< Primary constructor
	WCSketchProfile(xercesc::DOMElement *element, WCSerialDictionary *dictionary);					//!< Persistance constructor
	~WCSketchProfile();																				//!< Default destructor
	
	//Member Access Methods
	inline std::list<std::pair<WCGeometricCurve*,bool> > CurveList(void) const { return this->_curveList; }	//!< Get the curve list
	inline bool IsLinear(void) const			{ return this->_isLinear; }							//!< Get if the profile is linear
	inline bool IsClosed(void) const			{ return this->_isClosed; }							//!< Get if the profile is closed
	inline bool IsSelfIntersecting(void) const	{ return this->_isSelfIntersecting; }				//!< Get if the profile is self-intersecting
	
	//Member Methods
	WPInt IsOnRight(const WCSketchAxis *axis);														//!< Test relationship of profile to axis
	WPUInt IsInside(const WCVector4 &point);														//!< Test if a point is inside the profile
	WCProfileType Categorize(WCSketchProfile *profile);												//!< Categorize the profiles against each other
	inline void BoundaryList(const bool &detailed, std::list<WCVector4> &outputList,				//!< Output list of boundary points
												const WPFloat &tol=NURBSCURVE_LENGTH_ACCURACY) { 
												return BuildBoundaryList(this->_curveList, outputList, detailed, tol); }
	GLuint Triangulate(GLuint &vertexBuffer, GLuint &indexBuffer);									//!< Triangulate the profile (if possible)

	//Overloaded Operators
	bool operator==(const WCSketchProfile &profile);												//!< Equality operator
	bool operator==(const std::list<WCGeometricCurve*> &curveList);									//!< Equality operator
	bool operator!=(const WCSketchProfile &profile);												//!< In-equality operator
	
	//Inherited Methods
	inline std::string RootName(void) const		{ return SKETCHPROFILE_CLASSNAME; }					//!< Get the class name
	inline void IsConstruction(const bool state){ }													//!< Set the construction state
	inline WCGeometricObject* Base(void)		{ return NULL; }									//!< Get underlying geometric object
	void InjectConstraints(WCConstraintPlanner *planner) { }										//!< Induce effect onto constraint graph
	void ReceiveNotice(WCObjectMsg msg, WCObject *sender);											//!< Receive notice from curve
	bool Regenerate(void);																			//!< Validate and rebuild
	std::list<WCObject*> DeletionDependencies(void);												//!< Downward dependent objects
	void OnSelection(const bool fromManager, std::list<WCVisualObject*> objects);					//!< Called on selection
	void OnDeselection(const bool fromManager);														//!< Called on deselection	
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
	
	/*** Static Functions ***/
	static std::list<WCProfileTreeNode*> CategorizeIntoTree(const std::list<WCSketchProfile*> &profiles);	//!< Build categorization tree
	static std::list< std::list<WCSketchProfile*> > FlattenCategorizationTree(std::list<WCProfileTreeNode*> &rootList);//!< Flatten tree into list
	
	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCSketchProfile &profile);				//!< Overloaded output operator		
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__SKETCH_PROFILE_H__

