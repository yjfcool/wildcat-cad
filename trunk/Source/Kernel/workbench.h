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


#ifndef __WORKBENCH_H__
#define __WORKBENCH_H__


/*** Included Header Files ***/
#include "Kernel/wftrl.h"
#include "Kernel/drawing_mode.h"


/*** Locally Defined Values ***/
#define WORKBENCH_ANIMATE_DURATION		0.3


/*** Namespace Declaration ***/
namespace __WILDCAT_NAMESPACE__ {


/*** Class Predefines ***/
class WCFeature;
class WCWorkbenchLayer;
class WCSelectionManager;
class WCToolbar;
class WCKeyMap;


/***********************************************~***************************************************/


class WCWorkbench : virtual public WCObject {
protected:
	std::string									_name;												//!< Workbench name
	WCFeature									*_feature;											//!< Feature associated with the workbench
	WCDrawingMode								*_drawMode;											//!< Workbench drawing mode controller
	WCWorkbenchLayer							*_layer;											//!< Part layer
	WCSelectionManager							*_selectionManager;									//!< Selection manager
	WCNamedView									_defaultView, _restoreView;							//!< Preset views
	bool										_isPan, _isZoom, _isRotate, _isMultiSelect;			//!< State keys
	std::map<std::string, WCToolbar*>			_toolbarMap;										//!< Map of toolbars
	WCKeyMap									*_keyMap;											//!< Keymap for the workbench
private:
	//Hidden Constructors
	WCWorkbench();																					//!< Deny access to default constructor
	WCWorkbench(const WCWorkbench& wb);																//!< Deny access to copy constructor
	WCWorkbench& operator=(const WCWorkbench& wb);													//!< Deny access to equals operator
public:
	//Constructors and Destructors
	WCWorkbench(WCFeature *feature, const std::string &name);										//!< Primary constructor
	virtual ~WCWorkbench();																			//!< Default destructor

	//Member Access Methods
	inline std::string Name(void) const			{ return this->_name; }								//!< Get the workbench name
	inline WCFeature *Feature(void)				{ return this->_feature; }							//!< Get the associated feature
	inline WCSelectionManager* SelectionManager(void){ return this->_selectionManager; }			//!< Get the selection manager
	inline WCWorkbenchLayer* Layer(void)		{ return this->_layer; }							//!< Get the workbench layer
	inline bool IsPan(void) const				{ return this->_isPan; }							//!< Get the pan state
	inline bool IsZoom(void) const				{ return this->_isZoom; }							//!< Get the zoom state
	inline bool IsRotate(void) const			{ return this->_isRotate; }							//!< Get the rotate state
	inline bool IsMultiSelect(void) const		{ return this->_isMultiSelect; }					//!< Get the multi-select state
	bool IsActive(void) const;																		//!< Is this workbench the active one?
	inline WCKeyMap* KeyMap(void)				{ return this->_keyMap; }							//!< Get the associated keymap

	//Workbench Methods	
	virtual bool OnEnter(void)=0;																	//!< Entrance method
	virtual bool OnExit(void)=0;																	//!< Forced exit method
	virtual bool DrawingMode(WCDrawingMode *drawMode);												//!< Enter a drawing mode
	virtual WCDrawingMode* DrawingMode(void)	{ return this->_drawMode; }							//!< Get the drawing mode controller

	//Event Response
	virtual void OnPanPress(void);																	//!< React to pan press event
	virtual void OnPanRelease(void);																//!< React to pan release event
	virtual void OnZoomPress(void);																	//!< React to zoom press event
	virtual void OnZoomRelease(void);																//!< React to zoom release event
	virtual void OnRotatePress(void);																//!< React to rotate press event
	virtual void OnRotateRelease(void);																//!< React to rotate release event
	virtual void OnMultiSelectPress(void);															//!< React to multi-select press event
	virtual void OnMultiSelectRelease(void);														//!< React to multi-select release event
	virtual void OnIdle(void);																		//!< React to idle event
	virtual void OnMouseMove(const int x, const int y);												//!< React to mouse movement even
	virtual void OnMouseDown(const WCMouseButton &button);											//!< React to mouse down event
	virtual void OnMouseUp(const WCMouseButton &button);											//!< React to mouse up event
	virtual void OnScroll(const float &x, const float &y);											//!< React to scroll wheel events
	virtual void OnArrowKeyPress(const WCArrowKey &key);											//!< React to arrow key event
	virtual void OnReshape(const int width, const int height);										//!< React to window reshape event
	virtual bool OnUserMessage(const WCUserMessage &message);										//!< React to user message
	virtual void Render(void);																		//!< Render the workbench

	//File Commands
	virtual bool Open(const std::string &filename);													//!< Open Command
	virtual bool Close(void);																		//!< Close Command
	virtual bool New(void);																			//!< New Command
	virtual bool NewFrom(void);																		//!< NewFrom Command
	virtual bool Save(void);																		//!< Save Command
	virtual bool SaveAs(const std::string &filename);												//!< SaveAs Command
	virtual bool SaveAll(void);																		//!< SaveAll Command
	virtual bool Print(void);																		//!< Print Command
	virtual bool DocumentProperties(void);															//!< DocumentProperties Command

	//Edit Commands
	virtual bool Cut(void);																			//!< Cut Command
	virtual bool Copy(void);																		//!< Copy Command
	virtual bool Paste(void);																		//!< Paste Command
	virtual bool PasteSpecial(void);																//!< PasteSpecial Command
	virtual bool Undo(void);																		//!< Undo Command
	virtual bool Redo(void);																		//!< Redo Command
	virtual bool Delete(void);																		//!< Delete Command
	virtual bool Properties(void);																	//!< Properties Command
	virtual bool DocumentSearch(void);																//!< DocumentSearch Command
	virtual bool SelectionSet(void);																//!< SelectionSet Command
	
	//View Commands
	virtual bool Toolbars(void);																	//!< Toolbars Command
	virtual bool CommandsList(void);																//!< CommandList Command
	virtual bool Geometry(void);																	//!< Geometry Command
	virtual bool Specifications(void);																//!< Specification Command
	virtual bool Compass(void);																		//!< Compass Command
	virtual bool Tree(void);																		//!< Tree Command
	virtual bool ZoomToFit(void);																	//!< ZoomToFit Command
	virtual bool ZoomIn(const WPFloat &factor);														//!< ZoomIn Command
	virtual bool ZoomOut(const WPFloat &factor);													//!< ZoomOut Command
	virtual bool Pan(const WPFloat &x, const WPFloat &y);											//!< Pan Command
	virtual bool Rotate(void);																		//!< Rotate Command
	virtual bool LookAt(void);																		//!< LookAt Command
	virtual bool NamedView(const std::string &name);												//!< NamedViews Command
	virtual bool RenderStyle(void);																	//!< RenderMode Command
	virtual bool NavigationMode(void);																//!< NavigationMode Command
	virtual bool Lighting(void);																	//!< Lighting Command
	virtual bool DepthEffect(void);																	//!< DepthEffect Command
	virtual bool Ground(void);																		//!< Ground Command
	virtual bool Magnifier(void);																	//!< Magnifier Command
	virtual bool HideShow(void);																	//!< HideShow Command

	//Tools Commands
	//...

	//Windows Commands
	virtual bool Minimize(void)					{ return false; }									//!< Minimize Command
	virtual bool Maximize(void)					{ return false; }									//!< Maximize Command
	virtual bool Restore(void)					{ return false; }									//!< Restore Command
	
	//Help Commands
	virtual bool HelpSearch(void)				{ return false; }									//!< HelpSearch Command
	virtual bool HelpContents(void)				{ return false; }									//!< HelpContents Command
	virtual bool ToolTips(void)					{ return false; }									//!< ToolTips Command
};


/***********************************************~***************************************************/


}	   // End Wildcat Namespace
#endif //__WORKBENCH_H__

