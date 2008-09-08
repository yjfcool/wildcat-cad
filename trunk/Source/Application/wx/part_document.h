// part_document.h

#ifndef WX_WILDCAT_PART_DOCUMENT_HEADER
#define WX_WILDCAT_PART_DOCUMENT_HEADER

#include "wx/docview.h"
#include "PartDesign/part.h"

class WCPartDocument: public wxDocument
{
	DECLARE_DYNAMIC_CLASS(WCPartDocument)
protected:
	WCPart										*_part;												//!< Associated Wildcat part
	__WILDCAT_NAMESPACE__::WCDocument			*_document;											//!< Associated Wildcat document
public:

	//Member Access Methods
	inline WCPart* Part(void)					{ return this->_part; }								//!< Get the associated part
	inline __WILDCAT_NAMESPACE__::WCDocument* Document(void) { return this->_document; }			//!< Get the associated document

	WCPartDocument(void);
	virtual ~WCPartDocument(void);

	// wxDocument's virtual functions
	bool OnCreate(const wxString& path, long flags);
};

#endif