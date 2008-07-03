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


#ifndef __SKETCH_CONIC_ACTIONS_H__
#define __SKETCH_CONIC_ACTIONS_H__


/*** Included Header Files ***/
#include "Kernel/wftrl.h"
#include "Kernel/action.h"


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCSketch;
class WCSketchConicTwoPoint;


/***********************************************~***************************************************/


class WCActionSketchConicTwoPointCreate : public WCAction {
private:
	WCSketchConicTwoPoint						*_conic;											//!< Associated sketch conic
	std::string									_conicName;											//!< Name of new point
	WCSketch									*_sketch;											//!< Creating sketch
	WCVector4									_start, _startTangent, _end, _endTangent, _fifthPoint;//!< Point vectors
	//Constructors
	WCActionSketchConicTwoPointCreate(WCSketch *sketch, const std::string &conicName,				//!< Hidden primay constructor
												const WCVector4 &start, const WCVector4 &startTangent, 
												const WCVector4 &end, const WCVector4 &endTangent, const WCVector4 &fifthPoint);
	//Make SketchLine a friend
	friend class WCSketchConicTwoPoint;
public:
	//Constructors and Destructors
	~WCActionSketchConicTwoPointCreate()					{ }										//!< Default destructor
	
	//Inherited Methods
	WCFeature* Execute(void);																		//!< Execute the action
	bool Merge(WCAction *action);																	//!< Try to merge two actions
	bool Rollback(void);																			//!< Try to rollback the action
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document);																	//!< Serialize the object
	static WCActionSketchConicTwoPointCreate* Deserialize(xercesc::DOMElement* obj);						//!< Deserialize the object
	bool Validate(xercesc::DOMElement* obj);																//!< Validate an object
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__SKETCH_CONIC_ACTIONS_H_

