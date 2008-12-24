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


#ifndef __CONSTRAINT_HORIZONTAL_ACTIONS_H__
#define __CONSTRAINT_HORIZONTAL_ACTIONS_H__


/*** Included Header Files ***/
#include <Constraint/wgcsl.h>
#include <Kernel/action.h>


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCSketch;
class WCSketchLine;
class WCConstraintHorizontal;


/***********************************************~***************************************************/


class WCActionConstraintHorizontalCreate : public WCAction {
private:
	WCGUID										_lineGUID, _sketchGUID;								//!< GUID for associated objects
	WCSketchLine								*_line;												//!< Associated line
	std::string									_constraintName;									//!< Name of new object
	WCSketch									*_sketch;											//!< Associated sketch
	WCConstraintHorizontal						*_constraint;										//!< Post-creation pointer
	//Hidden Constructors
	WCActionConstraintHorizontalCreate();															//!< Deny access to default constructor
	WCActionConstraintHorizontalCreate(WCSketch *sketch, const std::string &constraintName,			//!< Primary constructor
												WCSketchLine *line);
	//Friend Declarations
	friend class WCConstraintHorizontal;															//!< Make ConstraintHorizontal a friend
public:
	//Constructors and Destructors
	WCActionConstraintHorizontalCreate(xercesc::DOMDocument *document, WCSerialDictionary *dict);	//!< Persistance constructor
	~WCActionConstraintHorizontalCreate()		{ }													//!< Default destructor
	
	//Inherited Methods
	WCFeature* Execute(void);																		//!< Execute the action
	bool Merge(WCAction *action)				{ return false; }									//!< Try to merge two actions
	bool Rollback(void);																			//!< Try to rollback the action
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__CONSTRAINT_HORIZONTAL_ACTIONS_H__

