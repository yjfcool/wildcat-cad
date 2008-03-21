#ifndef __WGCSL_CONSTRAINT_TAG_H
#define __WGCSL_CONSTRAINT_TAG_H


/*** Included Header Files ***/
#include "Constraint/wgcsl.h"


/*** Locally Defined Values ***/
#define CONSTRAINTTAG_POINT_2D					1
#define CONSTRAINTTAG_LINE_2D					2
#define CONSTRAINTTAG_VARIABLECIRCLE_2D			3
#define CONSTRAINTTYPE_DISTANCE_2D				4
#define CONSTRAINTTYPE_ANGLE_2D					5


/*****************************************************************************/


class CTag {
protected:
	int									_type;								//!< The type of tag
public:
	virtual int GetType(void)					{ return this->_type; }		//!< Virtual function to return the type of tag
	virtual int GetDOF(void)=0;												//!< Virtual function that return the degrees of freedom
	virtual void Print(int indent=0)=0;										//!< Virtual function to print the contents to cout
};


/*****************************************************************************/


class CPointTag2D : public CTag {
private:
	CPointTag2D(const CPointTag2D&);										//!< Disallow copy constructor
	CPointTag2D& operator=(const CPointTag2D&);								//!< Disallow equals operator
protected:
	float								_x, _y;								//!< The X and Y coordinates of the point
public:
	CPointTag2D(float x, float y);											//!< Default constructor
	~CPointTag2D()								{ }							//!< Default destructor
	int GetDOF(void)							{ return 2; }				//!< Get the degrees of freedom
	void Print(int indent=0);												//!< Print the contents of the point tag to cout
};


/*****************************************************************************/


class CLineTag2D : public CTag {
private:
	CLineTag2D(const CLineTag2D&);											//!< Disallow copy constructor
	CLineTag2D& operator=(const CLineTag2D&);								//!< Disallow equals operator
protected:
	float								_x, _y, _z;							//!< The X, Y, and Z values for the line
public:
	CLineTag2D(float x, float y, float z);									//!< Default constructor
	~CLineTag2D()								{ }							//!< Default destructor
	int GetDOF(void)							{ return 2; }				//!< Get the degrees of freedom
	void Print(int indent=0);												//!< Print the contents of the line tag to cout
};


/*****************************************************************************/


class CVariableCircleTag2D : public CTag {
private:
	CVariableCircleTag2D(const CVariableCircleTag2D&);						//!< Disallow copy constructor
	CVariableCircleTag2D& operator=(const CVariableCircleTag2D&);			//!< Disallow equals operator
protected:
	float								_x, _y, _z;							//!< The X, Y, and Z values for the line
public:
	CVariableCircleTag2D(float x, float y, float z);						//!< Default constructor
	~CVariableCircleTag2D()						{ }							//!< Default destructor	
	int GetDOF(void)							{ return 3; }				//!< Get the degrees of freedom
	void Print(int indent=0);												//!< Print the contents of the line tag to cout
};


/*****************************************************************************/


class CConstraintTag2D : public CTag {
private:
	CConstraintTag2D(int type, float value);								//!< Default private constructor
	CConstraintTag2D(const CConstraintTag2D&);								//!< Disallow copy constructor
	CConstraintTag2D& operator=(const CConstraintTag2D&);					//!< Disallow equals operator
protected:
	float		_value;														//!< The value of the constraint
public:
	~CConstraintTag2D()							{ }							//!< Default destructor
	static CConstraintTag2D* Distance(float value)							//!< Distance constraint constructor
		{ return new CConstraintTag2D(CONSTRAINTTYPE_DISTANCE_2D, value); }	
	static CConstraintTag2D* Angle(float value)								//!< Angle constraint constructor
		{ return new CConstraintTag2D(CONSTRAINTTYPE_ANGLE_2D, value); }
	int GetType(void)							{ return this->_type; }		//!< Get the type of the tag
	int GetDOF(void)							{ return 1; }				//!< Get the degrees of freedom
	void Print(int indent=0);												//!< Print the contents of the constraint to cout
};


/*****************************************************************************/


#endif //__WGCSL_CONSTRAINT_TAG_H

