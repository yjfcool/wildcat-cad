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


#ifndef __SERIALIZEBALE_OBJECT_H__
#define __SERIALIZEBALE_OBJECT_H__


/*** Included Header Files ***/
#include <Utility/wutil.h>
#include <Utility/object.h>


/*** Locally Defined Values ***/
#define	SERIALIZATION_UNKNOWN_GUID				"UNKNOWN"


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
typedef std::string								WCGUID;												//!< Typedef for a global unique ID
class WCSerialDictionary;


/***********************************************~***************************************************/


class WCSerializeableObject : virtual public WCObject {
private:
	//Private Members
	bool										_isSerialDirty;										//!< File dirty flag
	
	//Deny Access
	WCSerializeableObject(const WCSerializeableObject& obj);										//!< Deny access to copy constructor
	WCSerializeableObject& operator=(const WCSerializeableObject& obj);								//!< Deny access to equals operator
	
public:
	//Constructors and Destructors
	WCSerializeableObject() : ::WCObject(), _isSerialDirty(false) { }								//!< Primary constructor
	virtual ~WCSerializeableObject()			{ }													//!< Primary constructor

	//Member Access Methods
	inline void IsSerialDirty(const bool status){ this->_isSerialDirty = status; }					//!< Set the dirty flag
	inline bool IsSerialDirty(void) const		{ return this->_isSerialDirty; }					//!< Get the dirty flag	

	/*** Static Helper Methods ***/
	static void AddFloatAttrib(xercesc::DOMElement *element, const std::string &name,				//!< Static helper method for attributes
												const WPFloat &value);
	static void AddStringAttrib(xercesc::DOMElement *element, const std::string &name,				//!< Static helper method for attributes
												const std::string &value);
	static void AddBoolAttrib(xercesc::DOMElement *element, const std::string &name,				//!< Static helper method for attributes
												const bool &value);
	static void AddGUIDAttrib(xercesc::DOMElement *element, const std::string &name,				//!< Static helper method for attributes
												const void* address, WCSerialDictionary *dictionary);

	static float GetFloatAttrib(xercesc::DOMElement *element, const std::string &name);				//!< Static helper method for attributes
	static std::string GetStringAttrib(xercesc::DOMElement *element, const std::string &name);		//!< Static helper method for attributes
	static bool GetBoolAttrib(xercesc::DOMElement *element, const std::string &name);				//!< Static helper method for attributes
	static void* GetGUIDAttrib(xercesc::DOMElement *element, const std::string &name,				//!< Static helper method for attributes
												WCSerialDictionary *dictionary);
	static xercesc::DOMElement* ElementFromName(xercesc::DOMElement* element, const std::string &name);	//!< Static helper function for initialization

	//Required Virtual Methods
	virtual xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);//!< Serialize the object
};


/***********************************************~***************************************************/


class WCSerialDictionary : public WCSerializeableObject {
private:
	std::map<void*,WCGUID>						_addressMap;										//!< Map of addresses to GUIDs
	std::map<WCGUID,void*>						_guidMap;											//!< Map of GUIDs to addresses
public:
	//Constructors and Destructors
	WCSerialDictionary() : ::WCSerializeableObject(), _addressMap(), _guidMap() { }					//!< Default constructor
	WCSerialDictionary(xercesc::DOMElement* element);												//!< Persistance constructor
	~WCSerialDictionary()						{ }													//!< Default destructor
	
	//Insertion and Lookup Methods
	WCGUID InsertAddress(const void *address);														//!< Insert an address into the map
	WCGUID GUIDFromAddress(const void *address);													//!< Get a GUID from the map
	void InsertGUID(const WCGUID &guid, const void* address);										//!< Insert a GUID and address into the map
	void* AddressFromGUID(const WCGUID &guid);														//!< Get an address from the map
	void UpdateAddress(const WCGUID &guid, const void* address);									//!< Update GUID with new address
	void Clear(void);																				//!< Clear the dictionary

	//Serialization Methods
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__SERIALIZEBALE_OBJECT_H__

