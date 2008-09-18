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


/*** Included Header Files ***/
#include "Application/wx/toolbar_bridge.h"
#include "Application/wx/wildcat_app.h"
#include "Application/wx/main_frame.h"
#include "Utility/log_manager.h"
#include "Application/toolbar_manager.h"
#include "Kernel/document.h"
#include "Kernel/workbench.h"


/***********************************************~***************************************************/


WCToolbarButton_Bridge::WCToolbarButton_Bridge(const WCUserMessage &message, const std::string &tooltip,
	const unsigned int &type, const std::string &stdIcon, const std::string &activeIcon, const bool &active,
	const bool &enabled) : _button(NULL), _message(message), _tooltip(tooltip), _type(type), _stdIcon(stdIcon),
	_activeIcon(activeIcon), _isActive(active), _isEnabled(enabled) {
	//Nothing else for now
}



WCToolbarButton_Bridge::~WCToolbarButton_Bridge() {
	//Delete the wx button base
	//...
}


//void WCToolbarButton_Bridge::AddInstance(WCToolbarButton_Win32* instance) {
//	//Do nothing
//}


void WCToolbarButton_Bridge::IsActive(const bool &state) {
	//Set the active flag (if needed)
	if (this->_isActive != state) {
		this->_isActive = state;
	}
}


void WCToolbarButton_Bridge::IsEnabled(const bool &state) {
	//Set the enabled flag (if needed)
	if (this->_isEnabled != state) {
		this->_isEnabled = state;
	}
}


/***********************************************~***************************************************/

// static member variable
int WCToolbar_Bridge::_nextToolID(FIRST_TOOLBAR_BUTTON_ID);


WCToolbar_Bridge::WCToolbar_Bridge(WCMainFrame *frame, WCToolbar* toolbar, const std::string &title, const WCVector4 &position) : 
	_frame(frame),	_isVisible(false), _title(title), _core(toolbar), _platform(NULL) {
	//Create platform toolbar
	this->_platform = this->_frame->AddToolBar(wxConvertMB2WX(title.c_str()));
	//Make sure is not nil
	if (this->_platform == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCToolbar_Bridge::WCToolbar_Bridge - Not able to create platform toolbar");
	}
	//Add the toolbar to wx
	this->_platform->Realize();
	this->_frame->AuiManager()->AddPane(this->_platform, wxAuiPaneInfo().Name(wxConvertMB2WX(title.c_str())).Caption(wxConvertMB2WX(title.c_str())).ToolbarPane().Top().Left());
	this->_frame->AuiManager()->GetPane(this->_platform).Show();
}


WCToolbar_Bridge::~WCToolbar_Bridge() {
	//Release platform
	if (this->_platform != NULL) {
		//Close the toolbar
		delete this->_platform;
	}
}


void WCToolbar_Bridge::OnButtonPress(WCToolbarButton_Bridge *button) {
	std::cout << "What, I'm here?\n";
	//Get bridge button
//	WCToolbarButton_Bridge* bridge = button->Bridge();
//	//If button is enabled, pass along
//	if (bridge->IsEnabled()) {
//		//Get user message and pass to active workbench
//		this->_document->ActiveWorkbench()->OnUserMessage( bridge->Message() );
//	}
}


void WCToolbar_Bridge::OnCloseButtonPress(void) {
	//Pass to the platform
	this->_platform->Show(false);
	//Update the flag
	this->_isVisible = false;
}


void WCToolbar_Bridge::AddButton(WCToolbarButton_Bridge *button) {
	//Create wx Button object with appropriate parameters
	wxString msg = wxConvertMB2WX(button->Message().c_str());
	wxString tooltip = wxConvertMB2WX(button->Tooltip().c_str());
	//m_type = type;
	wxString stdIcon = wxConvertMB2WX(button->StandardIcon().c_str());
	wxString activeIcon = wxConvertMB2WX(button->ActiveIcon().c_str());
	//Call to platform to add the toolbar button
	wxString imagePath = wxGetApp().GetExeFolder() + _T("/Resources/") + stdIcon + _T(".tiff");
	wxToolBarToolBase* tool = this->_frame->AddToolBarTool(this->_platform, this->_nextToolID, msg, wxBitmap(imagePath, wxBITMAP_TYPE_TIF), tooltip);
	//Update the toolbar
	this->_platform->Realize();
	this->_nextToolID++;
}


WCToolbarButton* WCToolbar_Bridge::ButtonFromName(const std::string &name) {
	std::cout << "What am I doing here?\n";
	//Get the toolbar manager from the document
//	WCToolbarManager *manager = this->_document->ToolbarManager();
	//Request button from manager
//	return manager->ButtonFromName(name);
	return NULL;
}


void WCToolbar_Bridge::IsVisible(const bool &state) {
	//Work with visCount
	if (state != this->_isVisible) {
		//Pass to the platform
		this->_platform->Show(state);
		//Update the flag
		this->_isVisible = state;
	}
}


/***********************************************~***************************************************/

