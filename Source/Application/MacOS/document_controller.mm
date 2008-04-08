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


/*** Imported Header Files ***/
#import "Application/MacOS/document_controller.h"
#import "Application/MacOS/document.h"

#import "Application/MacOS/part_document.h"
#import "Application/MacOS/vis_document.h"


/*** Included Header Files ***/
#include "Application/dialog_manager.h"
#include "Application/dialog.h"


/***********************************************~***************************************************/


class WCDialogDocTypeSelector : public WCDialogController {
public:
	WCDialogDocTypeSelector() : ::WCDialogController() { }
	virtual ~WCDialogDocTypeSelector() {}

	virtual void ReceiveMessage(const std::string &message) {										//!< Receive message from a dialog
		//See what type of message
		if (message == "SelectDocType") {
			std::string docType = this->_dialog->StringFromScript("docType");

			//Lets create a VisDocument
			NSError *outError = [NSError alloc];
			WCDocument_OSX *document;
			if (docType == "visDoc") {
				document = [[WCVisDocument alloc] initWithType:@"Whatever" error:&outError];
			}
			if (docType == "partDoc") {
				document = [[WCPartDocument alloc] initWithType:@"Whatever" error:&outError];
			}
			//Make sure document was created
			if (document == nil) {
				CLOGGER_ERROR(WCLogManager::RootLogger(), "WCDialogDocTypeSelector::ReceiveMessage - Not able to create document of type: " << docType);
				return;
			}
			//Add the document into the controller
			[[NSDocumentController sharedDocumentController] addDocument:document];
			//Make the window controllers
			[document makeWindowControllers];
			//Show the document
			[document showWindows];
			//Close the dialog
			this->_dialog->CloseWindow(false);
		}
		else if (message == "RequestOpenDocument") {
			std::cout << "WCDialogDocTypeSelector switching to OpenDoc\n";
		}
		else {
			CLOGGER_ERROR(WCLogManager::RootLogger(), "WCDialogDocTypeSelector::ReceiveMessage - Unknown message: " << message);
		}
	}
};



/***********************************************~***************************************************/

@implementation WCDocumentController


- (IBAction)newDocument:(id)sender
{
	//Show dialog to ask for document type
	WCDialogManager::DisplayDialog("docTypeSelector", new WCDialogDocTypeSelector());
}


@end


/***********************************************~***************************************************/

