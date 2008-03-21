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


#ifndef __MATRIX_H__
#define __MATRIX_H__


/*** Included Header Files ***/
#include "Utility/wutil.h"


/*** Locally Defined Values ***/
//None


/***********************************************~***************************************************/
	//Matrix is row-major ordered for compatibility with BLAS functions
    //    A = |  0  1  2  3 |
    //        |  4  5  6  7 |
    //        |  8  9 10 11 |
    //        | 12 13 14 15 |
/***********************************************~***************************************************/

class WCMatrix4 {
private:
	WPFloat _data[16];																				//!< Four consecutive WPFloat elements
	friend class WCVector4;																			//!< Make sure that WCVector4 is a friend
	friend class WCVector;																			//!< Make sure that WCVector4 is a friend
	friend class WCMatrix;																			//!< Make sure that WCMatrix is a friend
public:
	WCMatrix4(const bool &identity=false);															//!< Primary constructor
	WCMatrix4(const WPFloat* data);																	//!< Optional constructor
	WCMatrix4(const WPFloat &a0, const WPFloat &a1, const WPFloat &a2, const WPFloat &a3,			//!< Elemental constructor
			  const WPFloat &b0, const WPFloat &b1, const WPFloat &b2, const WPFloat &b3,
			  const WPFloat &c0, const WPFloat &c1, const WPFloat &c2, const WPFloat &c3,
			  const WPFloat &d0, const WPFloat &d1, const WPFloat &d2, const WPFloat &d3);
	WCMatrix4(const WCMatrix4& matrix);																//!< Copy constructor
	~WCMatrix4()								{ }													//!< Default destructor
	
	//General access methods
	inline void Set(const WPUInt &row, const WPUInt &col, const WPFloat &value) {					//!< Standard Set operation
												this->_data[row*4+col] = value; }	
	inline WPFloat Get(const WPUInt &row, const WPUInt &col) const {								//!< Standard Get operation
												return this->_data[row*4+col]; }
	inline WPFloat* GetData(void)				{ return this->_data; }								//!< Return the data array
	inline void SetEmpty(void)					{ memset(this, 0, sizeof(WCMatrix4)); }				//!< Set the matrix to all zeros	
	void SetIdentity(void);																			//!< Set the matrix to identity
	
	//Conversion methods
	static WCMatrix4 FromGLMatrix(GLfloat *data);													//!< Convert from an OpenGL 4x4 matrix
	void GLLoadMatrix(void);																		//!< Load the matrix into GL
	WCMatrix ToMatrix(void);																		//!< Convert to a WCMatrix object
	void ToElement(xercesc::DOMElement *element);													//!< Serialize the object
	void FromElement(xercesc::DOMElement *element);													//!< Deserialize the object

	//Operators
	WCMatrix4 operator+(const WCMatrix4 &matrix);													//!< Matrix addition
	WCMatrix4 operator-(const WCMatrix4 &matrix);													//!< Matrix subtraction
	WCMatrix4 operator*(const WPFloat &scalar);														//!< Scalar multiplication
	WCVector4 operator*(const WCVector4 &vector) const;												//!< Vector multiplication
	WCMatrix4 operator*(const WCMatrix4 &matrix);													//!< Matrix multiplication
	WCMatrix4 &operator=(const WCMatrix4 &matrix);													//!< Matrix equation
	bool	 operator==(const WCMatrix4 &matrix);													//!< Matrix equivalence
	
	//Other Functions
	WPFloat NormL2(void);																			//!< L2 Norm
	WPFloat NormInf(void);																			//!< Infinity Norm
	WPFloat Determinant(void);																		//!< Determinant
	WCMatrix4 Inverse(void);																		//!< Inverse
	WCMatrix4 Transpose(void);																		//!< Transpose
	
	//Friend Functions
	friend std::ostream& operator<<(std::ostream& out, const WCMatrix4 &matrix);					//!< Overloaded output operator	
};


/***********************************************~***************************************************/


class WCMatrix {
private:
	WPUInt										_numRow, _numCol;									//!< Sizes for number of rows and columns 
	WPFloat*									_data;												//!< Pointer to data
	friend class WCVector4;																			//!< Make sure that WCVector4 is a friend
	friend class WCMatrix4;																			//!< Make sure that WCMatrix4 is a friend
	friend class WCVector;																			//!< Make sure that WCVector is a friend
public:
	WCMatrix(const WPUInt &numRow, const WPUInt &numCol);											//!< Default constructor
	WCMatrix(const WPUInt &numRow, const WPUInt &numCol, const WPFloat* data);						//!< Primary constructor
	WCMatrix(const WCMatrix& matrix);																//!< Copy constructor
	~WCMatrix();																					//!< Default destructor
	
	//General access methods
	inline WPUInt GetNumRow(void) const			{ return this->_numRow; }							//!< Get the number of rows
	inline WPUInt GetNumCol(void) const 		{ return this->_numCol; }							//!< Get the number of columns
	inline void Set(const WPUInt &row, const WPUInt &col, const WPFloat &value) {					//!< Standard Set operation
												this->_data[row*_numCol+col] = value; }	
	inline WPFloat Get(const WPUInt &row, const WPUInt &col) const {								//!< Standard Get operation
												return this->_data[row*_numCol+col]; }	
	inline WPFloat* GetData(void)				{ return this->_data; }								//!< Return the data array
	void SetIdentity(void);																			//!< Set the matrix to identity
	inline void SetEmpty(void);																		//!< Set the matrix to all zeros
	
	//Min-Max Functions
	WPFloat RowMinimum(const WPUInt &row);															//!< Find the row's min value
	WPFloat RowMaximum(const WPUInt &row);															//!< Find the row's max value
	WPFloat ColumnMinimum(const WPUInt &col);														//!< Find the column's min value
	WPFloat ColumnMaximum(const WPUInt &col);														//!< Find the column's max value
	
	//Conversion methods
	WCVector ToVector(void);																		//!< Convert to a WCVector object
	WCVector4 ToVector4(void);																		//!< Convert to a WCVector4 object
	WCMatrix4 ToMatrix4(void);																		//!< Convert to a WCMatrix4 object
	
	//Operators
	WCMatrix operator+(const WCMatrix &matrix);														//!< Matrix addition
	WCMatrix operator-(const WCMatrix &matrix);														//!< Matrix subtraction
	WCMatrix operator*(const WPFloat &scalar);														//!< Scalar multiplication
	WCVector operator*(const WCVector &vector);														//!< Vector multiplication
	WCMatrix operator*(const WCMatrix &matrix);														//!< Matrix multiplication
	WCMatrix &operator=(const WCMatrix &matrix);													//!< Matrix equation
	bool	 operator==(const WCMatrix &matrix);													//!< Matrix equivalence
	
	//Other Functions
	WPFloat NormL2(void);																			//!< L2 Norm
	WPFloat NormInf(void);																			//!< Infinity Norm
	WPFloat Determinant(void);																		//!< Determinant
	WCMatrix Inverse(void);																			//!< Inverse
	WCMatrix Transpose(void);																		//!< Transpose
	WCVector Solve(WCVector &vector);																//!< Solve the matrix for the vector ax=b
	
	//Friend Functions
	friend std::ostream& operator<<(std::ostream& out, const WCMatrix &matrix);						//!< Overloaded output operator		
};


/***********************************************~***************************************************/


#endif //__MATRIX_H__

	