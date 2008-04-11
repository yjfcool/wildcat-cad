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


#ifndef __VISUALIZATION_H__
#define __VISUALIZATION_H__


/*** Included Header Files ***/
#include "RTVisualization/wrtv.h"
#include "Kernel/document.h"
#include "RTVisualization/vis_actions.h"
#include "RTVisualization/vis_controller.h"
#include "RTVisualization/vis_workbench.h"


/*** Locally Defined Values ***/
#define VISUALIZATION_CLASSNAME					"Visualzation"


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCVisFeature;
class WCVisListener;
class WCSignalDescriptor;


/***********************************************~***************************************************/


//Values for visualization state
#define VISSTATE_STOPPED						0
#define VISSTATE_RUNNING						1
#define VISSTATE_PAUSED							2
#define VISSTATE_RESET							3
#define VISSTATE_ERROR							99

class WCVisualizationState {
private:
	unsigned int								_state;												//!< Visualization state
	//Hidden Constructors
	WCVisualizationState();																			//!< Deny access to default constructor 
	WCVisualizationState(const int &state)		{ this->_state = state; }							//!< Hidden primary constructor
public:
	//Constructors and Destructors
	WCVisualizationState(const WCVisualizationState &state) : _state(state._state)	{ }				//!< Copy constructor
	static WCVisualizationState Stopped(void)	{ return WCVisualizationState(VISSTATE_STOPPED); }	//!< Stopped state
	static WCVisualizationState Running(void)	{ return WCVisualizationState(VISSTATE_RUNNING); }	//!< Running state
	static WCVisualizationState Paused(void)	{ return WCVisualizationState(VISSTATE_PAUSED); }	//!< Paused state
	static WCVisualizationState Reset(void)		{ return WCVisualizationState(VISSTATE_RESET); }	//!< Reset state
	static WCVisualizationState Error(void)		{ return WCVisualizationState(VISSTATE_ERROR); }	//!< Error state
	
	//Overridder Operators
	WCVisualizationState& operator=(const WCVisualizationState &state)								//!< State equation				
												{ this->_state = state._state; return *this; }
	bool operator==(const WCVisualizationState &state) const { return this->_state == state._state; }//!< Equals operator
	bool operator!=(const WCVisualizationState &state) const { return this->_state != state._state; }//!< Inequality operator
};


/***********************************************~***************************************************/


class WCVisualization : public WCDocument {
protected:
	std::map<std::string, WCVisFeature*>		_featureMap;										//!< Map of all feature in the vis
	std::map<unsigned int, WCVisFeature*>		_idMap;												//!< Map of features with ID
	std::list<WCVisFeature*>					_featureList;										//!< List of all features in the vis
	std::map<WPUInt,WCSignalDescriptor*>		_descriptorSizeMap;									//!< Map of sizes per signal descriptor
	WCVisWorkbench								*_workbench;										//!< Associated visualization workbench
	WCVisualLayer								*_visualLayer;										//!< Visualization layer
	WCTreeElement								*_featureTreeElement;								//!< Holder for all feature elements
	WCTreeElement								*_listenerTreeElement;								//!< Holder for all listener elements
	WCTreeElement								*_recorderTreeElement;								//!< Holder for all recorder elements
	WCVisualizationState						_state;												//!< State of the visualization
private:
	void Initialize(void);																			//!< Initialize the visualization
	//Hidden Constructors
	WCVisualization();																				//!< Deny access to default constructor
	WCVisualization(const WCVisualization&);														//!< Deny access to copy constructor
	WCVisualization& operator=(const WCVisualization&);												//!< Deny access to equals operator
	//Friend Declarations
	friend class WCVisWorkbench;																	//!< Workbench is a friend
public:
	//Constructors and Destructors
	WCVisualization(const std::string &configFile, const std::string &outputFile);					//!< Primary constructor
	WCVisualization(xercesc::DOMElement *element, WCSerialDictionary *dictionary);					//!< Persistance constructor
	~WCVisualization();																				//!< Default destructor

	//Member Access Methods
	inline WCVisualLayer* VisualizationLayer(void)	{ return this->_visualLayer; }					//!< Get the visualization layer
	inline WCTreeElement* FeaturesTreeElement(void)	{ return this->_featureTreeElement; }			//!< Get feature tree element
	inline WCTreeElement* ListenersTreeElement(void){ return this->_listenerTreeElement; }			//!< Get listener tree element
	inline WCTreeElement* RecordersTreeElement(void){ return this->_recorderTreeElement; }			//!< Get recorder tree element
	inline WCVisualizationState State(void) const	{ return this->_state; }						//!< Get visualization state

	//Feature Methods
	bool AddFeature(WCVisFeature *feature, const bool &selectable);									//!< Add a feature
	bool RemoveFeature(WCVisFeature *feature, const bool &selectable);								//!< Remove a feature
	bool DeleteFeatures(std::list<WCVisFeature*> features);											//!< Completely delete features
	inline WCWorkbench* Workbench(void)			{ return this->_workbench; }						//!< Get the workbench
	WCVisFeature* FeatureFromName(const std::string &name);											//!< Get feature from name
	WCVisFeature* FeatureFromID(const unsigned int &id);											//!< Get feature from ID
	std::string GenerateFeatureName(WCFeature *feature);											//< Find a good name
	inline bool CheckName(const std::string &name){ return (this->_featureMap.find(name) ==			//!< See if name is used
												  this->_featureMap.end()); }

	//Visualization Methods
	void ResetVisualization(void);																	//!< Signal a reset of the visualization
	void StartVisualization(void);																	//!< Start the visualization
	void PauseVisualization(void);																	//!< Pause the visualization
	void StopVisualization(void);																	//!< Stop the visualization

	//Required Inherited Methods
	virtual bool Name(const std::string &name);														//!< Modify name - must check
	virtual std::string RootName(void) const		{ return VISUALIZATION_CLASSNAME; }				//!< Get the class name
	virtual void MarkDirty(void);																	//!< Mark as dirty
	virtual void ReceiveNotice(WCObjectMsg msg, WCObject *sender);									//!< Receive notice from sub-features
	virtual bool Regenerate(void);																	//!< Validate and rebuild
	xercesc::DOMElement* Serialize(xercesc::DOMDocument *document, WCSerialDictionary *dict);		//!< Serialize the object
	xercesc::DOMDocument* Serialize(void);															//!< Document serialization method
	virtual WCAlignedBoundingBox BoundingBox(void)	{ return WCAlignedBoundingBox(); }				//!< For now
	virtual void Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom) { }	//!< Render the object	

	/*** Actions and Drawing Modes ***/
//	static WCActionPartDelete* ActionDelete(WCPart *part, std::list<WCPartFeature*> features);		//!< Primary delete action

	/*** Friend Methods ***/
	friend std::ostream& operator<<(std::ostream& out, const WCVisualization &vis);					//!< Overloaded output operator	
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__VISUALIZATION_H__

