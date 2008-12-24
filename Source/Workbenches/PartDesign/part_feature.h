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


#ifndef __PART_FEATURE_H__
#define __PART_FEATURE_H__


/*** Included Header Files ***/
#include <Kernel/wftrl.h>
#include <Kernel/feature.h>


/*** Locally Defined Values ***/
#define PARTFEATURE_POINT_SIZE					3.0
#define PARTFEATURE_LINE_THICKNESS				2.0
#define PARTFEATURE_LINESTIPPLE_FACTOR			2.0
#define PARTFEATURE_LINESTIPPLE_PATTERN			0x5555
#define PARTFEATURE_DEFAULT_CONSTRUCTION		false


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCPart;


/***********************************************~***************************************************/


class WCPartFeature : public WCFeature {
public:
	//Static Members
	static WCColor								DefaultPointColor;									//!< Default point color
	static WCColor								DefaultCurveColor;									//!< Default curve color
	static WCColor								DefaultSurfaceColor;								//!< Default surface color
	static WCColor								ConstraintColor;									//!< Default constraint color
	static WCColor								SelectedColor;										//!< Selected feature color
	static WCColor								ConstructionColor;									//!< Construction feature color
	static WCColor								InprocessColor;										//!< Inprocess feature color
	static GLuint								DefaultPointRenderer;								//!< Default point render program
	static GLuint								DefaultCurveRenderer;								//!< Default curve render program
	static GLuint								DefaultSurfaceRenderer;								//!< Default surface render program
	
protected:
	WCPart										*_part;												//!< Parent part

private:
	//Deny Access
	WCPartFeature();																				//!< Deny access to default constructor
	WCPartFeature(const WCPartFeature& feature);													//!< Deny access to copy constructor
	WCPartFeature& operator=(const WCPartFeature& feature);											//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCPartFeature(WCFeature *creator, const std::string &name="");									//!< Primary constructor
	WCPartFeature(xercesc::DOMElement *element, WCSerialDictionary *dictionary);					//!< Persistance constructor
	virtual ~WCPartFeature();																		//!< Default destructor
	
	//Member Access Methods
	inline WCPart* Part(void)					{ return this->_part; }								//!< Get the parent part
	virtual bool Name(const std::string &name);														//!< Handle renaming the part feature
	virtual void MarkDirty(void);																	//!< Mark as dirty
	
	//Inherited Methods
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object

	/*** Static Serialization Methods ***/
	static bool Deserialize(xercesc::DOMElement* featureElement, WCSerialDictionary *dictionary);	//!< Master deserialization list

	/*** Friend Methods ***/
	friend std::ostream& operator<<(std::ostream& out, const WCPartFeature &feature);				//!< Overloaded output operator	
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__PART_FEATURE_H__

