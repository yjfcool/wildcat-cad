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


#ifndef __VIS_MOTIONTRACKER_H__
#define __VIS_MOTIONTRACKER_H__


/*** Included Header Files ***/
#include <RTVisualization/wrtv.h>
#include <RTVisualization/vis_feature.h>
#include <RTVisualization/vis_motiontracker_controller.h>


/*** Locally Defined Values ***/
#define VISMOTIONTRACKER_CLASSNAME				"Tracker"


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCVisualization;
class WCVisMesh;


/***********************************************~***************************************************/


class WCVisMotionTracker : public WCVisFeature, public WCVisualObject {
protected:
	WPFloat										_time;												//!< Time value
	WCVector4									_position, _rotation;								//!< First-order values
	WCVector4									_initPosition, _initRotation;						//!< Initial first-order values
	WPFloat										_scale;												//!< Scale for the tracker
	WCVisMesh									*_mesh;												//!< Visualization mesh
	GLuint										_vertBuffer, _normBuffer;							//!< Data buffers
private:
	void GenerateBuffers(void);																		//!< Generate vert and norm buffer
	void Initialize(void);																			//!< Private initializer
	//Hidden Constructors
	WCVisMotionTracker();																			//!< Deny access to default constructor
	WCVisMotionTracker(const WCVisMotionTracker&);													//!< Deny access to copy constructor
	WCVisMotionTracker& operator=(const WCVisMotionTracker&);										//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCVisMotionTracker(WCVisualization *vis, const std::string &name, const unsigned int &id, 		//!< Primary constructor
												WCVisMesh *mesh, const WPFloat &scale);
	WCVisMotionTracker(xercesc::DOMElement *element, WCSerialDictionary *dictionary);				//!< Persistance constructor
	~WCVisMotionTracker();																			//!< Default destructor

	//Member Access Methods
	//None

	//Visualization Methods
	virtual void OnReceiveData(const unsigned int &type, void* data);								//!< Receive data from a tracker
	virtual void StartVisualization(void);															//!< Signal start of the visualization
	virtual void PauseVisualization(void);															//!< Signal a pause in the visualization
	virtual void StopVisualization(void);															//!< Signal end of the visualization
	virtual void ResetVisualization(void);															//!< Signal reset of the visualization

	//Inherited Methods
	inline std::string RootName(void) const		{ return VISMOTIONTRACKER_CLASSNAME; }				//!< Get the class name
	virtual void ReceiveNotice(WCObjectMsg msg, WCObject *sender);									//!< Receive notice from point or curve
	virtual bool Regenerate(void);																	//!< Validate and rebuild
	virtual void OnSelection(const bool fromManager, std::list<WCVisualObject*> objects);			//!< Called on selection
	virtual void OnDeselection(const bool fromManager);												//!< Called on deselection	
	virtual void Render(const GLuint &defaultProg, const WCColor &color, const WPFloat &zoom);		//!< Render the object
	
	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, const WCVisMotionTracker &tracker);			//!< Overloaded output operator		
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__VIS_MOTIONTRACKER_H__

