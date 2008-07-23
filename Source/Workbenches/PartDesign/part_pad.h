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
#include "PartDesign/part_pad_types.h"


/*** Locally Defined Values ***/
#define PARTPAD_CLASSNAME						"Pad"


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCPartBody;
class WCSketchProfile;


/***********************************************~***************************************************/


class WCPartPad : public WCPartFeature {
protected:
	std::list<std::list<WCSketchProfile*> >		_profiles;											//!< List of lists of profiles <<ext,int>,<ext,int,int>,...)
	bool										_isReversed;										//!< Direction is reversed aligned with direction vector
	WCPartPadType								_firstType, _secondType;							//!< Types of pad
	WPFloat										_firstOffset, _secondOffset;						//!< Extrusion distances
	WCVector4									_direction;											//!< Extrusion direction
	std::list<WCGeometricPoint*>				_points;											//!< List of associated points
	std::list<WCGeometricLine*>					_lines;												//!< List of extrusion lines
	std::list<WCNurbsCurve*>					_curves;											//!< List of extrusion curves
	std::list<WCNurbsSurface*>					_surfaces;											//!< List of extrusion surfaces
	WCTopologyModel								*_topologyModel;									//!< Primary topology model
private:
	void GeneratePoints(std::list<std::list<WSVertexUse*> > &topoBottomPoints,						//!< Generate all points
												std::list<std::list<WSVertexUse*> > &topoTopPoints);
	void GenerateCurves(std::list<std::list<WCTrimProfile> > &topTrims,								//!< Generate all curves
												std::list<std::list<WCTrimProfile> > &bottomTrims,
												std::list<std::vector<WSEdgeUse*> > &topoBottomEUs,
												std::list<std::vector<WSEdgeUse*> > &topoSideEUs, 
												std::list<std::vector<WSEdgeUse*> > &topoTopEUs);
	void GenerateSideSurfaces(std::list<std::list<WSFaceUse*> > &topoFaceUses);						//!< Generate all surfaces
	void GenerateTopBottom(std::list<std::list<WCTrimProfile> > &topTrims,							//!< Generate top and bottom surfaces
												std::list<std::list<WCTrimProfile> > &bottomTrims,
												std::list<std::list<WSFaceUse*> > &topoFaceUses);
	WCTopologyModel* GenerateTopology(std::list<std::list<WSVertexUse*> > &topoBottomPoints,		//!< Generate topology model for the extrusion
												std::list<std::list<WSVertexUse*> > &topoTopPoints,
												std::list<std::vector<WSEdgeUse*> > &topoBottomEUs, 
												std::list<std::vector<WSEdgeUse*> > &topoSideEUs, 
												std::list<std::vector<WSEdgeUse*> > &topoTopEUs,
												std::list<std::list<WSFaceUse*> > &topoFaceUses);
	void Initialize(void);																			//!< Initialization method
	void DetermineDirection(void);																	//!< Determine direction and offsets
	//Hidden Constructors
	WCPartPad();																					//!< Deny access to default constructor
	WCPartPad& operator=(const WCPartPad &pad);														//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCPartPad(WCPartBody *body, const std::string &name, std::list<std::list<WCSketchProfile*> > &profiles,//!< Primary constructor
												const bool &reverseDir,	const WCPartPadType &firstType, const WCPartPadType &secondType,
												const WPFloat &firstOffset=0.0, const WPFloat &secondOffset=0.0);
	WCPartPad(const WCPartPad& pad);																//!< Deny access to copy constructor	
	WCPartPad(xercesc::DOMElement *element, WCSerialDictionary *dictionary);						//!< Persistance constructor
	~WCPartPad();																					//!< Default destructor
	
	//Member Access Methods
	inline std::list<std::list<WCSketchProfile*> > Profiles(void) const { return this->_profiles; }	//!< Get the associated sketch profiles
	inline bool IsReversed(void) const			{ return this->_isReversed; }						//!< Get if direction is reversed
	inline WCPartPadType FirstType(void) const	{ return this->_firstType; }						//!< Get the first pad type
	inline WCPartPadType SecondType(void) const	{ return this->_secondType; }						//!< Get the second pad type
	inline WPFloat FirstOffset(void) const		{ return this->_firstOffset; }						//!< Get the positive extrusion distance
	inline WPFloat SecondOffset(void) const		{ return this->_secondOffset; }						//!< Get the negative extrusion distance
	
	//Required Inherited Methods
	virtual std::string RootName(void) const	{ return PARTPAD_CLASSNAME; }						//!< Get the class name
	virtual void ReceiveNotice(WCObjectMsg msg, WCObject *sender);									//!< Receive notice from point or curve
	virtual bool Regenerate(void);																	//!< Validate and rebuild
	virtual void OnSelection(const bool fromManager, std::list<WCVisualObject*> objects);			//!< Called on selection
	virtual void OnDeselection(const bool fromManager);												//!< Called on deselection
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object

	/*** Actions ***/
	static WCDrawingMode* ModeCreate(WCPartWorkbench *wb);											//!< Return create mode controller
	static WCActionPartPadCreate* ActionCreate(WCPartBody *body, const std::string &padName,		//!< Primary creation action
											   std::list<std::list<WCSketchProfile*> > &profiles, const bool &reversed,
											   const WCPartPadType &firstType, const WCPartPadType &secondType,
											   const WPFloat &firstOffset=0.0, const WPFloat &secondOffset=0.0);
	
	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCPartPad &pad);						//!< Overloaded output operator		
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__PART_PAD_H__

