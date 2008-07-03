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


#ifndef __VISUALIZATION_FEATURE_H__
#define __VISUALIZATION_FEATURE_H__


/*** Included Header Files ***/
#include "RTVisualization/wrtv.h"
#include "RTVisualization/vis_packet.h"
#include "Kernel/feature.h"


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCVisualization;


/***********************************************~***************************************************/


class WCVisFeature : public WCFeature {
protected:
	WCVisualization								*_visualization;									//!< Parent visualization
	unsigned int								_id;												//!< Feature ID
	pthread_mutex_t								_mutex;												//!< Thread mutex
private:
	//Hidden Constructors
	WCVisFeature();																					//!< Deny access to default constructor
	WCVisFeature(const WCVisFeature&);																//!< Deny access to copy constructor
	WCVisFeature& operator=(const WCVisFeature&);													//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCVisFeature(WCVisualization *vis, const std::string &name, const unsigned int &id);			//!< Primary constructor
	WCVisFeature(xercesc::DOMElement *element, WCSerialDictionary *dictionary);						//!< Persistance constructor
	virtual ~WCVisFeature();																		//!< Default destructor
	
	//Member Access Methods
	inline WCVisualization* Visualization(void)	{ return this->_visualization; }					//!< Get the parent
	inline unsigned int ID(void) const			{ return this->_id; }								//!< Get object ID
	virtual bool Name(const std::string &name);														//!< Handle renaming the feature
	virtual void MarkDirty(void);																	//!< Mark as dirty

	//Threading Methods
	virtual void Lock(void);																		//!< Lock the object
	virtual void Unlock(void);																		//!< Unlock the object

	//Inherited Methods
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
	
	//Required Methods
	virtual void OnReceiveData(const unsigned int &type, void* data)=0;								//!< Receive data from a listener
	virtual void ResetVisualization(void)=0;														//!< Signal visualization to reset
	virtual void StartVisualization(void)=0;														//!< Signal start of the visualization
	virtual void PauseVisualization(void)=0;														//!< Signal a pause in the visualization
	virtual void StopVisualization(void)=0;															//!< Signal end of the visualization

	/*** Static Serialization Methods ***/
	static bool Deserialize(xercesc::DOMElement* featureElement, WCSerialDictionary *dictionary);	//!< Master deserialization list

	/*** Friend Methods ***/
	friend std::ostream& operator<<(std::ostream& out, const WCVisFeature &feature);				//!< Overloaded output operator	
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__VISUALIZATION_FEATURE_H__

