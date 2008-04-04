#ifndef __MODAL_DIALOG_OSX_H__
#define __MODAL_DIALOG_OSX_H__


/*** Imported Header Files ***/
#import <Cocoa/Cocoa.h>
#import <WebKit/WebView.h>


/*** Included Header Files ***/
#include "Application/dialog_manager.h"


/*** Objective-C Class Predefines ***/
//None


/***********************************************~***************************************************/


@interface WCModalDialog : NSWindowController {
	IBOutlet WebView							*webView;											//!< Associated webview object
	NSURL										*location;											//!< Current location
	WCDialog									*_dialog;											//!< Associated dialog object
}

- (id)initWithLocation:(NSURL*)url;																	//!< Initialize with a specific web location
- (id)initWithDialog:(WCDialog*)dialog;																//!< Initialize with a specific dialog
- (WebView*)WebView;																				//!< Get the embedded webview


@end


/***********************************************~***************************************************/


#endif //__MODAL_DIALOG_OSX_H__

