#ifndef __MODAL_DIALOG_OSX_H__
#define __MODAL_DIALOG_OSX_H__


/*** Imported Header Files ***/
#import <Cocoa/Cocoa.h>
#import <WebKit/WebView.h>


/*** Objective-C Class Predefines ***/
//None


/***********************************************~***************************************************/


@interface ModalDialog : NSWindowController {
	IBOutlet WebView							*webView;											//!<
	NSURL										*location;											//!<
}

- (id)initWithLocation:(NSURL*)url;																	//!<
- (WebView*)WebView;																				//!<


@end


/***********************************************~***************************************************/


#endif //__MODAL_DIALOG_OSX_H__

