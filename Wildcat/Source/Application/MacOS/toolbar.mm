/*** Included Header Files ***/
#include "Application/toolbar.h"
#include "Application/MacOS/toolbar_bridge.h"


/***********************************************~***************************************************/


WCToolbarButton::WCToolbarButton(const WCUserMessage &message, const std::string &tooltip, const unsigned int &type,
	const std::string &stdIcon, const std::string &activeIcon, const bool &active, const bool &enabled) : _bridge(NULL) {

	//Create toolbar button bridge
	this->_bridge = new WCToolbarButton_Bridge(this, message, tooltip, type, stdIcon, activeIcon, active, enabled);
	//Make sure is non-null
	if (this->_bridge == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCToolbarButton::WCToolbarButton - Not able to create toolbar button bridge");
	}
}


WCToolbarButton::~WCToolbarButton() {
	//Try to delete the bridge
	if (this->_bridge != NULL) delete this->_bridge;
	this->_bridge = NULL;
}


bool WCToolbarButton::IsActive(void) const {
	//Just call to the underlying bridge
	if (this->_bridge != NULL) return this->_bridge->IsActive();
	//Otehrwise, return false
	return false;
}


void WCToolbarButton::IsActive(const bool &state) {
	//Just call to the underlying bridge
	if (this->_bridge != NULL) return this->_bridge->IsActive(state);
}


bool WCToolbarButton::IsEnabled(void) const {
	//Just call to the underlying bridge
	if (this->_bridge != NULL) return this->_bridge->IsEnabled();
	//Otehrwise, return false
	return false;

}


void WCToolbarButton::IsEnabled(const bool &state) {
	//Just call to the underlying bridge
	if (this->_bridge != NULL) this->_bridge->IsEnabled(state);
}


/***********************************************~***************************************************/


WCToolbar::WCToolbar(WCDocument *doc, const std::string &name, const WCVector4 &position) : _bridge(NULL) {
	//Create toolbar bridge
	this->_bridge = new WCToolbar_Bridge(doc, this, name, position);
	//Make sure is non-null
	if (this->_bridge == NULL) {
		CLOGGER_ERROR(WCLogManager::RootLogger(), "WCToolbar::WCToolbar - Not able to create toolbar bridge");
	}	
}


WCToolbar::~WCToolbar() {
	//Try to delete the bridge
	if (this->_bridge != NULL) delete this->_bridge;
	this->_bridge = NULL;
}


void WCToolbar::AddButton(WCToolbarButton *button) {
	//Just call to the underlying bridge
	if (this->_bridge != NULL) return this->_bridge->AddButton(button->_bridge);
}


WCToolbarButton* WCToolbar::ButtonFromName(const std::string &name) {
	//Just call to the underlying bridge
	if (this->_bridge != NULL) return this->_bridge->ButtonFromName(name);
	//Otherwise, just return NULL
	return NULL;
}


bool WCToolbar::IsVisible(void) const {
	//Just call to the underlying bridge
	if (this->_bridge != NULL) return this->_bridge->IsVisible();
	//Otehrwise, return false
	return false;
}


void WCToolbar::IsVisible(const bool &state) {
	//Just call to the underlying bridge
	if (this->_bridge != NULL) return this->_bridge->IsVisible(state);
}


/***********************************************~***************************************************/

