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


#ifndef __PART_PAD_H__
#define __PART_PAD_H__


/*** Included Header Files ***/
#include "Kernel/wftrl.h"
#include "PartDesign/part_feature.h"
#include "PartDesign/part_pad_actions.h"
#include "PartDesign/part_pad_controller.h"
#include "PartDesign/part_pad_modes.h"


/*** Local Defines ***/
#define PARTPAD_CLASSNAME						"Pad"


/*** Class Predefines ***/
class WCPartBody;
class WCSketchProfile;


/***********************************************~***************************************************/


class WCPartPad : virtual public WCPartFeature {
protected:
	std::list<std::pair<WCSketchProfile*,bool> >_profiles;											//!< List of included profiles
	WCVector4									_direction;											//!< Extrusion direction
	WPFloat										_posDepth, _negDepth;								//!< Extrusion distance
	std::list<WCGeometricPoint*>				_points;											//!< List of associated points
	std::list<WCGeometricLine*>					_lines;												//!< List of extrusion lines
	std::list<WCNurbsCurve*>					_curves;											//!< List of extrusion curves
	std::list<WCNurbsSurface*>					_surfaces;											//!< List of extrusion surfaces
private:
	//Private Methods
	void GeneratePoints(void);																		//!< Private method to generate all points
	void GenerateCurves(void);																		//!< Private method to generate all curves
	void GenerateSurfaces(void);																	//!< Private method to generate all surfaces
	void GenerateTopBottom(void);																	//!< Private method to generate top and bottom surfaces and profiles
	void GenerateTopology(void);																	//!< Private method to generate topology model
	
	//Deny Access
	WCPartPad();																					//!< Deny access to default constructor
	WCPartPad(const WCPartPad& pad);																//!< Deny access to copy constructor
	WCPartPad& operator=(const WCPartPad &pad);														//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCPartPad(WCPartBody *body, const std::string &name, const std::list<std::pair<WCSketchProfile*,bool> > &profiles,//!< Primary constructor
												const WCVector4 &direction, const WPFloat &posDepth, const WPFloat &negDepth);
	WCPartPad(xercesc::DOMElement *element, WCSerialDictionary *dictionary);						//!< Persistance constructor
	virtual ~WCPartPad();																			//!< Default destructor
	
	//Member Access Methods
	inline std::list<std::pair<WCSketchProfile*,bool> > Profiles(void) const { return this->_profiles; }//!< Get the associated sketch profiles
	inline WCVector4 Direction(void) const		{ return this->_direction; }						//!< Get the extrusion direction
	inline WPFloat PositiveDepth(void) const	{ return this->_posDepth; }							//!< Get the positive extrusion distance
	inline WPFloat NegativeDepth(void) const	{ return this->_negDepth; }							//!< Get the negative extrusion distance
	
	//Required Inherited Methods
	virtual std::string RootName(void) const	{ return PARTPAD_CLASSNAME; }						//!< Get the class name
	void ReceiveNotice(WCObjectMsg msg, WCObject *sender);											//!< Receive notice from point or curve
	bool Regenerate(void)						{ return true; }									//!< Validate and rebuild
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
	void Render(const GLuint defaultProg=0, const WCColor color=WCColor::Default());				//!< Render the object
	void OnSelection(const bool fromManager, std::list<WCVisualObject*> objects);					//!< Called on selection
	void OnDeselection(const bool fromManager);														//!< Called on deselection

	/*** Actions ***/
	static WCDrawingMode* ModeCreate(WCPartWorkbench *wb);											//!< Return create mode controller
	static WCActionPartPadCreate* ActionCreate(WCPartBody *body, const std::string &padName,		//!< Primary creation action
												const std::list<std::pair<WCSketchProfile*,bool> > &profiles,
												const WCVector4 &direction, const WPFloat &posDepth, const WPFloat &negDepth);
	
	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCPartPad &pad);						//!< Overloaded output operator		
};


/***********************************************~***************************************************/


#endif //__PART_PAD_H__

