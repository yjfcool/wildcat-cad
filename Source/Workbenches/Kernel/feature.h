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


#ifndef __FEATURE_H__
#define __FEATURE_H__


/*** Included Header Files ***/
#include "Kernel/wftrl.h"


/*** Local Defines ***/
#define FEATURE_DEFAULT_COLOR					1.0, 1.0, 1.0, 1.0
#define FEATURE_SELECTED_COLOR					1.0, 1.0, 0.0, 1.0
#define FEATURE_INPROCESS_COLOR					0.0, 1.0, 0.0, 1.0
#define FEATURE_POINT_SIZE						4.0
#define FEATURE_LINE_THICKNESS					2.0
#define FEATURE_LINESTIPPLE_FACTOR				2.0
#define FEATURE_LINESTIPPLE_PATTERN				0x5555


/*** Class Predefines ***/
class WCDocument;
class WCEventController;
class WCTreeElement;


/***********************************************~***************************************************/


class WCFeature : virtual public WCSerializeableObject, virtual public WCSelectionObject {
protected:
	std::string									_name;												//!< Name of the feature
	WCDocument									*_document;											//!< Parent document
	WCFeature									*_creator;											//!< Creator feature
	WCEventController							*_controller;										//!< Event controller
	WCTreeElement								*_treeElement;										//!< Tree element
	bool										_isFeatureDirty;									//!< Dirty flag
	bool										_isFixed;											//!< Fixed flag
	bool										_isConstruction;									//!< Construction flag
	bool										_isActive;											//!< Active flag
private:
	//Hidden Constructors
	WCFeature();																					//!< Deny access to default constructor
	WCFeature(const WCFeature& feature);															//!< Deny access to copy constructor
	WCFeature& operator=(const WCFeature& feature);													//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCFeature(WCFeature *creator, const std::string &name="");										//!< Primary constructor
	WCFeature(xercesc::DOMElement *element, WCSerialDictionary *dictionary);						//!< Persistance constructor
	virtual ~WCFeature();																			//!< Default destructor
	
	//Member Access Methods
	virtual inline std::string GetName(void) const		{ return this->_name; }						//!< Get the name
	virtual inline WCEventController* Controller(void)	{ return this->_controller; }				//!< Get event controller
	virtual inline WCTreeElement* TreeElement(void)		{ return this->_treeElement; }				//!< Get tree element
	virtual inline WCDocument* Document(void)			{ return this->_document; }					//!< Get the parent document
	virtual void MarkDirty(void);																	//!< Set the dirty flag
	virtual inline bool IsFeatureDirty(void) const		{ return this->_isFeatureDirty; }			//!< Get the dirty flag	
	virtual inline bool IsFixed(void) const				{ return this->_isFixed; }					//!< Get fixed state
	virtual inline void IsFixed(const bool &state)		{ this->_isFixed = state; }					//!< Set fixed state
	virtual inline bool IsConstruction(void) const		{ return this->_isConstruction; }			//!< Get construction state
	virtual inline void IsConstruction(const bool &state){ this->_isConstruction = state; }			//!< Set construction state
	virtual inline bool IsActive(void) const			{ return this->_isActive; }					//!< Get active state
	virtual inline void IsActive(const bool &state)		{ this->_isActive = state; }				//!< Set active state
		
	//Inherited Methods
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
	virtual WCObject* Associate(void)			{ return this; }									//!< Get pointer to self
	virtual void OnSelection(const bool fromManager, std::list<WCVisualObject*> objects) { }		//!< Called on selection
	virtual void OnDeselection(const bool fromManager) { }											//!< Called on deselection

	//Required Methods
	virtual std::string RootName(void) const=0;														//!< Give the root name for the class
	virtual bool Regenerate(void)=0;																//!< Validate and rebuild
	virtual bool Name(const std::string &name)=0;													//!< Handle renaming the part feature
		
	//Feature Tree Methods
	friend std::ostream& operator<<(std::ostream& out, const WCFeature &feature);					//!< Overloaded output operator		
};


/***********************************************~***************************************************/


#endif //__FEATURE_H__

