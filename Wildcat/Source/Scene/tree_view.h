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


#ifndef __TREE_VIEW_H__
#define __TREE_VIEW_H__


/*** Included Header Files ***/
#include "Scene/wscnl.h"
#include "Scene/overlay.h"


/*** Local Defines ***/
#define TREEVIEW_DEFAULT_PLACEMENT				WCPlacement::UpperLeft()


/*** Class Predefines ***/
class WCEventController;
class WCIcon;
class WCTreeView;
class WCText;
class WCVerticalScrollbar;


/***********************************************~***************************************************/


class WCTreeElement : public WCObject {
private:
	WCTreeView									*_tree;												//!< TreeView parent
	std::string									_name;												//!< Element name
	WCText										*_label;											//!< Rendering text label
	WCEventController							*_controller;										//!< Element controller
	std::list<WCTreeElement*>					_childList;											//!< Relations
	bool										_isSelected;										//!< Selection flag
	bool										_isMouseOver;										//!< Mouseover flag
	bool										_isOpen;											//!<
	WSTexture									*_icon;												//!< Element icon texture

	//Private Methods
	void IsMouseOver(const bool state)			{ this->_isMouseOver = state; }						//!< Set the mouse over state

	//Deny Access
	WCTreeElement();																				//!< Deny access to default constructor
	WCTreeElement(const WCTreeElement &element);													//!< Deny access to copy constructor
	WCTreeElement& operator=(const WCTreeElement &element);											//!< Deny access to equals operator
	
	//Friend Class
	friend class WCTreeView;
public:
	WCTreeElement(WCTreeView *tree, std::string name, WCEventController *controller, WSTexture *icon);//!< Primary constructor
	~WCTreeElement();																				//!< Default destructor
	
	//Member Access Methods
	inline std::string Name(void) const			{ return this->_name; }								//!< Get the name
	inline WCEventController* Controller(void)	{ return this->_controller; }						//!< Get the related controller
	inline WCTreeElement* FirstChild(void)		{ return this->_childList.front(); }				//!< Get first child
	inline WCTreeElement* LastChild(void)		{ return this->_childList.back(); }					//!< Get last child
	inline bool IsOpen(void) const				{ return this->_isOpen; }							//!< Get the open state
	inline void IsOpen(const bool state)		{ this->_isOpen = state; }							//!< Set the open state
	inline bool IsSelected(void) const			{ return this->_isSelected; }						//!< Get the selection state
	void IsSelected(const bool state);																//!< Set the selection state
	WPUInt CountVisibleChildren(void);																//!< Get count of visible children
	WPFloat FindMaxWidth(const WPUInt depth);														//!< Find the maximum display width
	
	//Child Methods
	bool AddFirstChild(WCTreeElement *element);														//!< Add as first child
	bool AddLastChild(WCTreeElement *element);														//!< Add as last child
	bool AddBefore(WCTreeElement *element, WCTreeElement *sibling);									//!< Add element before sibling
	bool AddAfter(WCTreeElement *element, WCTreeElement *sibling);									//!< Add element after sibling
	bool Remove(WCTreeElement *element);															//!< Remove a child element

	//Mouse Events
	WCTreeElement* OnMouseDown(const WCMouseButton &button, WCVector4 &pos, WPUInt depth);			//!< Mouse button pressed
	WCTreeElement* OnMouseMove(WCVector4 &pos, WPUInt depth);										//!< Mouse being moved

	//Inherited Methods
	void ReceiveNotice(WCObjectMsg msg, WCObject *sender);											//!< Receive notice from dependent	

	//Public Access Methods
	void Render(WPFloat &x, WPFloat &y, WPUInt depth);												//!< Render the element
	void Print(int indent);																			//!< Print the element
};


/***********************************************~***************************************************/


class WCTreeView : public WCOverlay {
protected:
	//Data Objects
	std::map<WCTreeElement*, WCTreeElement*>	_elementMap;										//!< Mapping of all elements
	WCTreeElement								*_root;												//!< Tree root
	std::list<WCTreeElement*>					_selected;											//!< Selected elements
	WCTreeElement								*_mouseOver;										//!< Current mouse over element
	//Rendering Objects
	GLuint										_tex;												//!< Display texture
	GLuint										_texWidth, _texHeight;								//!< Texture width and height
	GLuint										_framebuffer;										//!< Rendering framebuffer
	GLuint										_vertexBuffer, _texCoordBuffer;						//!< Display buffers
	WCVerticalScrollbar							*_scrollbar;										//!< Scrollbar widget
	WPFloat										_scale;												//!< View scale factor
	WPFloat										_virtualWidth, _virtualHeight;						//!< Virtual tree width and height
	
private:
	//Private Methods
	void GenerateTexture(void);																		//!< Texture generation method
	void GenerateBuffers(void);																		//!< Display buffer generation
	WCVector4 FromVirtualWindow(const WCVector4 &pos);
	WCVector4 ToVirtualWindow(const WCVector4 &pos);

	//Deny Access
	WCTreeView();																					//!< Deny access to the default constructor
	WCTreeView(const WCTreeView &view);																//!< Deny access to copy constructor
	WCTreeView& operator=(const WCTreeView &view);													//!< Deny access to equals operator
	
public:
	//Constructors and Destructors
	WCTreeView(WCUserInterfaceLayer *layer);														//!< Primary constructor
	~WCTreeView();																					//!< Default destructor

	//Member Access Methods
	void MarkDirty(void);																			//!< Mark the tree as dirty
	void IsVisible(const bool state);																//!< Set the visibility state

	//Tree Element Methods
	bool RegisterElement(WCTreeElement *element);													//!< Register a new element
	bool UnregisterElement(WCTreeElement *element);													//!< Unregister an element
	WCTreeElement* ElementFromName(const std::string &name);										//!< Get element from name
	
	//Inherited Member Functions
	inline void SetPlacement(const WPFloat lowerLeft, const WPFloat lowerRight) { }					//!< Set the placement of the widget
	inline void SetSize(const WPFloat width, const WPFloat height) { }								//!< Set the size of the widget
	void ReceiveNotice(WCObjectMsg msg, WCObject *sender);											//!< Receive notice from dependent
	void OnReshape(void);																			//!< Modify placement on resize
	void OnMouseDown(const WCMouseButton &button, const WPFloat x, const WPFloat y);				//!< Mouse button pressed
	void OnMouseUp(const WCMouseButton &button, const WPFloat x, const WPFloat y);					//!< Mouse button released
	void OnMouseEnter(const WPFloat x, const WPFloat y);											//!< Modified MouseEnter
	void OnMouseExit(const WPFloat x, const WPFloat y);												//!< Modified MouseExit
	void OnMouseMove(const WPFloat x, const WPFloat y);												//!< Mouse being moved
	void OnScroll(const WPFloat &x, const WPFloat &y);												//!< Scroll-wheel events
	void OnArrowKeyPress(const WCArrowKey &key);													//!< Arrow key presses
	void Render(void);																				//!< Primary render entry point
	
	/*** Friend Functions ***/
	friend std::ostream& operator<<(std::ostream& out, WCTreeView &view);							//!< Overloaded output operator	
};


/***********************************************~***************************************************/


#endif //__TREE_VIEW_H__

