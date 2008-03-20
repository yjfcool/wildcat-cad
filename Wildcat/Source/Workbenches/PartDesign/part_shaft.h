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


#ifndef __PART_SHAFT_H__
#define __PART_SHAFT_H__


/*** Included Header Files ***/
#include "Kernel/wftrl.h"
#include "PartDesign/part_feature.h"
#include "PartDesign/part_shaft_actions.h"
#include "PartDesign/part_shaft_controller.h"
#include "PartDesign/part_shaft_modes.h"


/*** Local Defines ***/
#define PARTSHAFT_CLASSNAME						"Shaft"


/*** Class Predefines ***/
class WCPartBody;
class WCSketchProfile;
class WCSketchAxis;


/***********************************************~***************************************************/


class WCPartShaft : virtual public WCPartFeature {
protected:
	std::list<std::pair<WCSketchProfile*,bool> >_profiles;											//!< List of included profiles
	WCSketchAxis								*_axis;												//!< Revolution axis
	bool										_profilesOnRight;									//!< Are the profiles on the right side
	WPFloat										_cwAngle, _ccwAngle;								//!< Revolution angles
	std::list<WCGeometricPoint*>				_points;											//!< List of revolution points
	std::list<WCGeometricLine*>					_lines;												//!< List of revolution lines
	std::list<WCNurbsCurve*>					_curves;											//!< List of revolution curves
	std::list<WCNurbsSurface*>					_surfaces;											//!< List of revolution surfaces
private:
	std::list<WCTrimProfile> GenerateFrontProfile(const WCRay &ray);								//!< Generate front profile
	void GenerateBackProfile(const WCRay &ray);														//!< Generate back profile
	void GenerateSurfaces(const WCRay &ray, const std::list<WCTrimProfile> &profileList);			//!< Private method to generate all surfaces
	void GenerateCurves(const WCRay &ray, const std::list<WCTrimProfile> &profileList);				//!< Private method to generate all curves
	void GeneratePoints(const WCRay &ray);															//!< Private method to generate all points
	void GenerateTopology(const WCRay &ray);														//!< Private method to generate all topology
	void Initialize(void);																			//!< Initialization method
	//Hidden Constructors
	WCPartShaft();																					//!< Deny access to default constructor
	WCPartShaft(const WCPartShaft& shaft);															//!< Deny access to copy constructor
	WCPartShaft& operator=(const WCPartShaft &shaft);												//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCPartShaft(WCPartBody *body, const std::string &name, const std::list<std::pair<WCSketchProfile*,bool> > &profiles,//!< Primary constructor
												WCSketchAxis *axis, const bool profilesOnRight,
												const WPFloat &cwAngle, const WPFloat &ccwAngle);
	WCPartShaft(xercesc::DOMElement *element, WCSerialDictionary *dictionary);						//!< Persistance constructor
	virtual ~WCPartShaft();																			//!< Default destructor
	
	//Member Access Methods
	inline std::list<std::pair<WCSketchProfile*,bool> > Profiles(void) const { return this->_profiles; }//!< Get the associated sketch profiles
	inline WCSketchAxis* Axis(void) const		{ return this->_axis; }								//!< Get the revolution axis
	inline WPFloat ClockwiseAngle(void) const	{ return this->_cwAngle; }							//!< Get the clockwise revolution angle
	inline WPFloat CounterClockwiseAngle(void) const{ return this->_ccwAngle; }						//!< Get the ccw revolution angle
	
	//Required Inherited Methods
	virtual std::string RootName(void) const	{ return PARTSHAFT_CLASSNAME; }						//!< Get the class name
	void ReceiveNotice(WCObjectMsg msg, WCObject *sender);											//!< Receive notice from point or curve
	bool Regenerate(void);																			//!< Validate and rebuild
	void OnSelection(const bool fromManager, std::list<WCVisualObject*> objects);					//!< Called on selection
	void OnDeselection(const bool fromManager);														//!< Called on deselection
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object

	/*** Static Processing Methods ***/
	static bool QualifyProfiles(const std::list<WCSketchProfile*> &inputProfiles,					//!< Qualify a list of profiles for inclusion
												std::list<std::pair<WCSketchProfile*,bool> > &outputProfiles, const WCSketchAxis *axis);

	/*** Actions ***/
	static WCDrawingMode* ModeCreate(WCPartWorkbench *wb);											//!< Return create mode controller
	static WCActionPartShaftCreate* ActionCreate(WCPartBody *body, const std::string &shaftName,	//!< Primary creation action
												const std::list<std::pair<WCSketchProfile*,bool> > &profiles,
												WCSketchAxis *axis, const bool profilesOnRight,
												const WPFloat &cwAngle, const WPFloat &ccwAngle);
	
	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCPartShaft &shaft);					//!< Overloaded output operator		
};


/***********************************************~***************************************************/


#endif //__PART_SHAFT_H__

