// part_document.cpp

#include "stdafx.h"
#include "part_document.h"
#include "part_view.h"
#include "part_canvas.h"
#include "PartDesign/part.h"
#include "PartDesign/part_feature.h"
#include "Application/keymap.h"

IMPLEMENT_DYNAMIC_CLASS(WCPartDocument, wxDocument)

WCPartDocument::WCPartDocument(void):_part(NULL) {
}

WCPartDocument::~WCPartDocument(void) {
	//Delete part if it exists
	if (this->_part != NULL) delete this->_part;
}

bool WCPartDocument::OnCreate(const wxString& path, long flags) {
	wxDocument::OnCreate(path, flags);
	InitOpenGL();
	this->_part = new WCPart("", "");
	this->_document = this->_part;
	//Set the default surface renderer
	WCPartFeature::DefaultSurfaceRenderer = this->_part->Scene()->ShaderManager() ? this->_part->Scene()->ShaderManager()->ProgramFromName("scn_basiclight") : 0;

	return true;
}