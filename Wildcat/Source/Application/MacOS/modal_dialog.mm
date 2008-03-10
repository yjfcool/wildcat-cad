//Imported Header Files
#import "Application/MacOS/modal_dialog.h"


/***********************************************~***************************************************/


@implementation ModalDialog


- (id)init
{
    // Do the regular Cocoa thing, specifying a particular nib.
    self = [super initWithWindowNibName:@"ModalDialog"];
	location = [NSURL URLWithString:@"http://www.skyscrapercity.com"];
    return self;
}


- (id)initWithLocation:(NSURL*)url
{
    self = [super initWithWindowNibName:@"ModalDialog"];
	location = url;
	return self;
}


- (void)dealloc
{
    // Do the regular Cocoa thing.
    [super dealloc];
}


- (void)windowDidLoad
{
	//Try loading location into webview
	[[webView mainFrame] loadRequest:[NSURLRequest requestWithURL:location]];
}


- (WebView*)WebView
{
	return webView;
}


@end


/***********************************************~***************************************************/

