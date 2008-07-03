#ifndef __TOOLBAR_BUTTON_OSX_H__
#define __TOOLBAR_BUTTON_OSX_H__


/*** Imported Header Files ***/
#import <Cocoa/Cocoa.h>


/*** Included Header Files ***/
#include <iostream>


/*** C++ Class Predefines ***/
class WCToolbarButton;


/*** Objective-C Class Predefines ***/
@class WCToolbarButton_OSX;


/***********************************************~***************************************************/

class WCToolbarButton_Bridge {
protected:
	std::string									_message;											//!< Message that is passed on element press
	NSString									*_tooltip;											//!< Tooltip for element
	NSImage										*_stdIcon, *_activeIcon;							//!< Icon images for element
	WCToolbarButton								*_button;											//!< Toolbar button
	int											_type;												//!< Button type
	bool										_isActive;											//!< Is the element active
	bool										_isEnabled;											//!< Is the element selected
	//Platform pointers
	WCToolbarButton								*_core;												//!< Pointer to core button
	WCToolbarButton_OSX							*_platform;											//!< Pointer to platform button
public:
	//Constructors and Destructors
	WCToolbarButton_Bridge(const WCUserMessage &message, const std::string &tooltip, unsigned int &type,	//!< Primary constructor
												const std::string &stdIcon, const std::string &activeIcon);
	~WCToolbarButton_Bridge();
};


/***********************************************~***************************************************/


@interface WCToolbarButton_OSX : NSButton {
	WCToolbarButton_Bridge						*_bridgeButton;
}
@end


/***********************************************~***************************************************/


@interface WCToolbarButtonCell_OSX : NSButtonCell {
}
@end


/***********************************************~***************************************************/


#endif //__TOOLBAR_BUTTON_OSX_H__

