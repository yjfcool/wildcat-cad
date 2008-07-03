#include "Scene/text.h"

/*** DEBUG ***/
//Need #ifdef for OSX
#include "Application/MacOS/text_osx.h"
/*** DEBUG ***/


/***********************************************~***************************************************/


WCText::WCText(WCScene *scene, const std::string &text, const WCColor &color, const WCTextFont &font, 
	const WCTextStyle &style, const float fontSize) : _baseText(NULL) {
	//Determine font name
	std::string fontName;
	switch(font._type) {
		case TEXTFONT_TIMES: fontName = "Times"; break;
		case TEXTFONT_PALATINO: fontName = "Palatino"; break;
		case TEXTFONT_HELVETICA: fontName = "Helvetica"; break;
		case TEXTFONT_TAHOMA: fontName = "Tahoma"; break;
		default: fontName = "Times"; break;
	}
	
	//Determine font style
	std::string fontStyle;
	switch(style._style) {
		case TEXTSTYLE_ROMAN: fontStyle = "Roman"; break;
		case TEXTSTYLE_BOLD: fontStyle = "Bold"; break;
		case TEXTSTYLE_ITALIC: fontStyle = "Italic"; break;
		default: fontStyle = "Roman"; break;
	}
	//Combine font and style
	fontName = fontName + "-" + fontStyle;
	//Create base text object
	this->_baseText = new WCText_OSX(text, color, fontName, fontSize);
}


WCText::~WCText() {
	//Delete baseText if there
	if (this->_baseText != NULL) delete this->_baseText;
}


GLfloat WCText::Width(void) {
	//Get the base text width
	return this->_baseText->Width();
}


GLfloat WCText::Height(void) {
	//Get the base text height
	return this->_baseText->Height();
}


std::string WCText::Text(void) {
	//Get the base text string
	return this->_baseText->Text();
}


void WCText::Text(const std::string &text) {
	//Set the base text string
	this->_baseText->Text(text);
}


WCColor WCText::Color(void) {
	//Get the base text color
	return this->_baseText->Color();
}


void WCText::Color(const WCColor &color) {
	//Set the base text color
	this->_baseText->Color(color);
}


float WCText::FontSize(void) {
	//Get the base text size
	return this->_baseText->FontSize();
}


void WCText::FontSize(const float size) {
	//Set the base text size
	this->_baseText->FontSize(size);
}


bool WCText::IsUnderlined(void) {
	//Get the base text underlined state
	return this->_baseText->IsUnderlined();
}


void WCText::IsUnderlined(const bool &state) {
	//Set the base text underlined state
	this->_baseText->IsUnderlined(state);
}


void WCText::DrawAtPoint(const GLfloat x, const GLfloat y) {
	//Call to baseText draw method
	if (this->_baseText != NULL) this->_baseText->DrawAtPoint(x, y);
}


void WCText::DrawAtPoint(const WCVector4 &pt, const WCVector4 &uUnit, const WCVector4 &vUnit) {
	//Call to baseText draw method
	if (this->_baseText != NULL) this->_baseText->DrawAtPoint(pt, uUnit, vUnit);
}


/***********************************************~***************************************************/

