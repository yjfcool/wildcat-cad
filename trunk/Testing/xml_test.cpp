#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#if defined(XERCES_NEW_IOSTREAMS)
#include <iostream>
#else
#include <iostream.h>
#endif


int main (int argc, char* args[]) {

	try {
		xercesc::XMLPlatformUtils::Initialize();
	}
	catch (const xercesc::XMLException& toCatch) {
		char* message = xercesc::XMLString::transcode(toCatch.getMessage());
		std::cout << "Error during initialization! :\n"
			 << message << "\n";
		xercesc::XMLString::release(&message);
		return 1;
	}

	xercesc::XercesDOMParser* parser = new xercesc::XercesDOMParser();
	parser->setValidationScheme(xercesc::XercesDOMParser::Val_Always);    
	parser->setDoNamespaces(true);    // optional

	xercesc::ErrorHandler* errHandler = (xercesc::ErrorHandler*) new xercesc::HandlerBase();
	parser->setErrorHandler(errHandler);
	char* xmlFile = "shader_manifest.xml";

	try {
		parser->parse(xmlFile);
	}
	catch (const xercesc::XMLException& toCatch) {
		char* message = xercesc::XMLString::transcode(toCatch.getMessage());
		std::cout << "Exception message is: \n"
			 << message << "\n";
		xercesc::XMLString::release(&message);
		return -1;
	}
	catch (const xercesc::DOMException& toCatch) {
		char* message = xercesc::XMLString::transcode(toCatch.msg);
		std::cout << "Exception message is: \n"
			 << message << "\n";
		xercesc::XMLString::release(&message);
		return -1;
	}
	catch (...) {
		std::cout << "Unexpected Exception \n" ;
		return -1;
	}

	delete parser;
	delete errHandler;
	
	/***************************************************/
	
	XMLCh tempStr[100];

	xercesc::XMLString::transcode("Range", tempStr, 99);
	xercesc::DOMImplementation* impl = xercesc::DOMImplementationRegistry::getDOMImplementation(tempStr);

	xercesc::XMLString::transcode("root", tempStr, 99);
	xercesc::xercesc::DOMDocument*   doc = impl->createDocument(0, tempStr, 0);
	xercesc::xercesc::DOMElement*   root = doc->getDocumentElement();

	xercesc::XMLString::transcode("FirstElement", tempStr, 99);
	xercesc::xercesc::DOMElement*   e1 = doc->createElement(tempStr);
	root->appendChild(e1);

	xercesc::XMLString::transcode("SecondElement", tempStr, 99);
	xercesc::xercesc::DOMElement*   e2 = doc->createElement(tempStr);
	root->appendChild(e2);

	xercesc::XMLString::transcode("aTextNode", tempStr, 99);
	xercesc::DOMText*       textNode = doc->createTextNode(tempStr);
	e1->appendChild(textNode);

	// optionally, call release() to release the resource associated with the range after done
	xercesc::DOMRange* range = doc->createRange();
	range->release();

	// removedElement is an orphaned node, optionally call release() to release associated resource
	xercesc::xercesc::DOMElement* removedElement = root->removeChild(e2);
	removedElement->release();

	// no need to release this returned object which is owned by implementation
	xercesc::XMLString::transcode("*", tempStr, 99);
	xercesc::DOMNodeList*    nodeList = doc->getElementsByTagName(tempStr);

	// done with the document, must call release() to release the entire document resources
	doc->release();
	
	return 0;
}