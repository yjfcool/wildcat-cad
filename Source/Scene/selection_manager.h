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


#ifndef __SELECTION_MANAGER_H__
#define __SELECTION_MANAGER_H__


/*** Included Header Files ***/
#include "Scene/wscnl.h"


/*** Locally Defined Values ***/
//None


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCVisualObject;


/***********************************************~***************************************************/


class WCSelectionType {
public:
	WCSelectionType()							{ }
	~WCSelectionType()							{ }
};


/***********************************************~***************************************************/


class WCSelectionObject : virtual public WCObject {
protected:
	bool										_isSelected;										//!< Selection state
	
public:
	//Constructors and Destructors
	WCSelectionObject() : _isSelected(false)	{ }													//!< Default constructor
	virtual ~WCSelectionObject()				{ }													//!< Default destructor
	
	//Virtual Methods
	virtual WCObject* Associate(void)=0;
	virtual void OnSelection(const bool fromManager, std::list<WCVisualObject*> objects)=0;			//!< Called on selection
	virtual void OnDeselection(const bool fromManager)=0;											//!< Called on deselection
	virtual inline bool IsSelected(void) const	{ return this->_isSelected; }						//!< Current selection state
};


/***********************************************~***************************************************/


class WCSelectionManager : public WCObject {
private:
	WCPerformanceLevel							_perfLevel;											//!< Performance level indicator
	WCScene										*_scene;											//!< Related scene
	GLuint										_tex, _depthRenderBuffer;							//!< Selection texture and render buffer
	GLuint										_framebuffer;										//!< Selection draw framebuffer
	std::map<WCVisualObject*,WCSelectionObject*>_objects;											//!< Map of objects to be considered
	std::list<WCSelectionObject*>				_selected;											//!< List of selected items
	std::list< std::pair<WCVisualObject*,WCSelectionObject*> > _selectedVisuals;					//!< List of selected visual items
	std::stack< std::map<WCVisualObject*,WCSelectionObject*> > _stack;								//!< Push/pop stack
	
	//Private Methods
	void ReadyFramebuffer(void);																	//!< Prepare the framebuffer
	WCColor EncodeColor(const WPInt index);															//!< Encode the index value
	WPInt DecodeColor(const GLubyte r, const GLubyte g, const GLubyte b);							//!< Decode the color values

	WPUInt ProcessSingleSelect(std::vector<std::pair<WCVisualObject*,WCSelectionObject*> > &sa,		//!< Process a single selection
												 GLubyte *pixels, bool notify);
	WPUInt ProcessRectangleSelect(std::vector<std::pair<WCVisualObject*,WCSelectionObject*> > &sa,	//!< Process a rectangular selection
												GLubyte *pixels,const WPUInt width, const WPUInt height, bool notify);	

	WPUInt ProcessSingleSelect(WCSelectionObject **selectionArray, GLubyte *pixels, bool notify);	//!< Process a single selection
	WPUInt ProcessRectangleSelect(WCSelectionObject **selectionArray, GLubyte *pixels,				//!< Process a rectangular selection
												const WPUInt width, const WPUInt height, bool notify);	
	//Deny Access
	WCSelectionManager();																			//!< Deny access to default constructor
	WCSelectionManager(const WCSelectionManager &mgr);												//!< Deny access to copy constructor
	WCSelectionManager& operator=(const WCSelectionManager &mgr);									//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCSelectionManager(WCScene *scene);																//!< Primary constructor
	~WCSelectionManager();																			//!< Default destructor
	
	//Inherited Methods
	void ReceiveNotice(WCObjectMsg msg, WCObject *sender);											//!< Receive notice
	
	//Selection Methods
	bool AddObject(WCVisualObject* object, WCSelectionObject *selector);							//!< Add a selectable object
	bool RemoveObject(WCVisualObject* object);														//!< Remove a selectable object
	WPUInt Select(const WPUInt &xMin, const WPUInt &xMax, const WPUInt &yMin, const WPUInt &yMax, 	//!< Query for selection
												const bool &notify=true);
	bool ForceSelection(WCSelectionObject *object, const bool &notify=false);						//!< Make an object selected
	bool ForceDeselection(WCSelectionObject *object, const bool &notify=false);						//!< Make an object not selected
	void PushObjects(void);																			//!< Push the list of selected items
	void PopObjects(void);																			//!< Pop the list of selected items
	void Clear(const bool &notify=true);															//!< Clear the list of selected items
	inline WPUInt Count(void) const				{ return (WPUInt)this->_selected.size(); }			//!< Get number of selected items
	inline WPUInt CountVisuals(void) const		{ return (WPUInt)this->_selectedVisuals.size(); }	//!< Get number of selected visual items
	std::list<WCSelectionObject*> Selected(void){ return this->_selected; }							//!< Get the list of selected items
	std::list< std::pair<WCVisualObject*,WCSelectionObject*> > SelectedVisuals(void) { 				//!< Get the list of selected visual items
												return this->_selectedVisuals; }
	WCSelectionObject* Top(void)				{ if (this->_selected.size() > 0) 					//!< Return the first selected item
													return this->_selected.front(); 
													else return NULL; }
	WCVisualObject* TopVisual(void)				{ if (this->_selectedVisuals.size() > 0)
													return this->_selectedVisuals.front().first;
													else return NULL; }

	template <class T>
	std::list<T*> FilterSelected(const bool &useAssociate) { std::list<T*> filtered; T* ptr;		//!< Filter selected list by certain type
												  std::list<WCSelectionObject*>::iterator iter = this->_selected.begin();
												  for (; iter!=this->_selected.end(); iter++) {
													if (useAssociate) ptr = dynamic_cast<T*>( (*iter)->Associate() );
													else ptr = dynamic_cast<T*>(*iter);
													if (ptr != NULL) filtered.push_back(ptr); }
												  return filtered; }
												  
	template <class T>
	std::list< std::pair<T*,WCSelectionObject*> > FilterSelectedVisuals(void) {						//!< Filter selected visuals by type
												  std::list<std::pair<T*,WCSelectionObject*> > filtered; T* ptr;
												  std::list< std::pair<WCVisualObject*,WCSelectionObject*> >::iterator iter;
												  for (iter = this->_selectedVisuals.begin(); iter!=this->_selectedVisuals.end(); iter++) {
													ptr = dynamic_cast<T*>((*iter).first);
													if (ptr != NULL) filtered.push_back( std::make_pair(ptr, (*iter).second) ); }
												  return filtered; }

	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, WCSelectionManager &sel);					//!< Overloaded output operator	
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__SELECTION_MANAGER_H__

