/*** Included Header Files ***/
#include "Constraint/constraint_tag.h"


/*** Class Order ***/
//---CPointTag
//---CLineTag
//---CVariableCircleTag
//---CConstraintTag


/*****************************************************************************/


CPointTag2D::CPointTag2D(float x, float y) : 
	_x(x), _y(y) {
	this->_type = CONSTRAINTTAG_POINT_2D;
}


void CPointTag2D::Print(int indent) {
	for (int i=0; i < indent; i++) cout << "\t";
	cout << "PointTag2D(X:" << this->_x << ", Y:" << this->_y << ")\n";
}


/*****************************************************************************/


CLineTag2D::CLineTag2D(float x, float y, float z) :
	_x(x), _y(y), _z(z) {
	this->_type = CONSTRAINTTAG_LINE_2D;
}


void CLineTag2D::Print(int indent) {
	for (int i=0; i < indent; i++) cout << "\t";
	cout << "LineTag2D(X:" << this->_x << ", Y:" << this->_y << ", Z:" << this->_z << ")\n";
}


/*****************************************************************************/


CVariableCircleTag2D::CVariableCircleTag2D(float x, float y, float z) :
	_x(x), _y(y), _z(z) {
	this->_type = CONSTRAINTTAG_VARIABLECIRCLE_2D;
}


void CVariableCircleTag2D::Print(int indent) {
	for (int i=0; i < indent; i++) cout << "\t";
	cout << "VariableCircleTag2D(X:" << this->_x << ", Y:" << this->_y << ", Z:" << this->_z << ")\n";
}


/*****************************************************************************/


CConstraintTag2D::CConstraintTag2D(int type, float value) :
	_value(value) {
	this->_type = type;
}


void CConstraintTag2D::Print(int indent) {
	for (int i=0; i < indent; i++) cout << "\t";
	cout << "ConstraintTag2D(T:";
	switch(this->_type) {
		case CONSTRAINTTYPE_DISTANCE_2D: cout << "Dst"; break;
		case CONSTRAINTTYPE_ANGLE_2D: cout << "Agl"; break;
		default: cout << "Unknown"; break;
	 }
	 cout << ", V:" << this->_value << ")\n";
}


/*****************************************************************************/

