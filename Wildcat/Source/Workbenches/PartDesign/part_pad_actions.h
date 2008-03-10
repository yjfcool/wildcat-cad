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


#ifndef __PART_PAD_ACTIONS_H__
#define __PART_PAD_ACTIONS_H__


/*** Included Header Files ***/
#include "Kernel/wftrl.h"
#include "Kernel/action.h"


/*** Local Defines ***/
//None


/*** Class Predefines ***/
class WCPartBody;
class WCSketchProfile;
class WCPartPad;


/***********************************************~***************************************************/


class WCActionPartPadCreate : public WCAction {
private:
	WCPartBody									*_body;												//!< Associated body
	std::string									_padName;											//!< Name of new pad
	std::list< std::pair<WCSketchProfile*,bool> >_profiles;											//!< Associated profiles
	WCVector4									_direction;											//!< Extrusion direction
	WPFloat										_posDepth, _negDepth;								//!< Extrusion distance
	WCPartPad									*_pad;												//!< Post-creation pad
	//Hidden Constructors
	WCActionPartPadCreate();																		//!< Deny access to default constructor
	WCActionPartPadCreate(const WCActionPartPadCreate& action);										//!< Deny access to copy constructor
	WCActionPartPadCreate(WCPartBody *body, const std::string &padName,								//!< Primary constructor
												const std::list< std::pair<WCSketchProfile*,bool> > &profiles,
												const WCVector4 &direction, const WPFloat &posDepth, const WPFloat &negDepth);
	//Friend Declarations
	friend class WCPartPad;																			//!< Make WCPartPad a friend
public:
	WCActionPartPadCreate(xercesc::DOMElement *element, WCSerialDictionary *dictionary);			//!< Persistance constructor
	~WCActionPartPadCreate()					{ }													//!< Default destructor

	//Member Access Methods
	inline WCPartPad* Pad(void)					{ return this->_pad; }								//!< Get the created pad

	//Inherited Methods
	WCFeature* Execute(void);																		//!< Execute the action
	bool Merge(WCAction *action)				{ return false; }									//!< Try to merge two actions
	bool Rollback(void)							{ return false; }									//!< Try to rollback the action
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
};


/***********************************************~***************************************************/


#endif //__PART_PAD_ACTIONS_H__

