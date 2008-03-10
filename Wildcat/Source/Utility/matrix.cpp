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


/*** Included Header Files ***/
#include "Utility/matrix.h"
#include "Utility/vector.h"


/*** Platform Included Header Files ***/
#ifdef __APPLE__
#include <vecLib/cblas.h>
#include <vecLib/clapack.h>
#endif

#ifdef __WIN32__
//...
#endif


/***********************************************~***************************************************/


WCMatrix4::WCMatrix4(const bool &identity) {
	//Make sure to zero the entire matrix
	memset(this->_data, 0, 16 * sizeof(WPFloat));
	//If identity is specified, set it so
	if (identity) {
		this->_data[0] = 1.0;
		this->_data[5] = 1.0;
		this->_data[10] = 1.0;
		this->_data[15] = 1.0;
	}
}


WCMatrix4::WCMatrix4(const WPFloat* data) {
	//Copy all 16 elements over if non-null
	if (data != NULL)
		memcpy(this->_data, data, 16 * sizeof(WPFloat));
	//Otherwise just set to identity
	else 
		this->SetIdentity();
}	


WCMatrix4::WCMatrix4(const WPFloat &a0, const WPFloat &a1, const WPFloat &a2, const WPFloat &a3,
					 const WPFloat &b0, const WPFloat &b1, const WPFloat &b2, const WPFloat &b3,
					 const WPFloat &c0, const WPFloat &c1, const WPFloat &c2, const WPFloat &c3,
					 const WPFloat &d0, const WPFloat &d1, const WPFloat &d2, const WPFloat &d3) {
	this->_data[0] = a0;	this->_data[1] = a1;	this->_data[2] = a2;	this->_data[3] = a3;
	this->_data[4] = b0;	this->_data[5] = b1;	this->_data[6] = b2;	this->_data[7] = b3;
	this->_data[8] = c0;	this->_data[9] = c1;	this->_data[10]= c2;	this->_data[11]= c3;
	this->_data[12]= d0;	this->_data[13]= d1;	this->_data[14]= d2;	this->_data[15]= d3;
}


WCMatrix4::WCMatrix4(const WCMatrix4 &m) {
	this->_data[0] = m._data[0];	this->_data[1] = m._data[1];	this->_data[2] = m._data[2];	this->_data[3] = m._data[3];
	this->_data[4] = m._data[4];	this->_data[5] = m._data[5];	this->_data[6] = m._data[6];	this->_data[7] = m._data[7];
	this->_data[8] = m._data[8];	this->_data[9] = m._data[9];	this->_data[10]= m._data[10];	this->_data[11]= m._data[11];
	this->_data[12]= m._data[12];	this->_data[13]= m._data[13];	this->_data[14]= m._data[14];	this->_data[15]= m._data[15];
}

	
void WCMatrix4::SetIdentity(void) {
	this->_data[0] = 1.0;	this->_data[1] = 0.0;	this->_data[2] = 0.0;	this->_data[3] = 0.0;
	this->_data[4] = 0.0;	this->_data[5] = 1.0;	this->_data[6] = 0.0;	this->_data[7] = 0.0;
	this->_data[8] = 0.0;	this->_data[9] = 0.0;	this->_data[10]= 1.0;	this->_data[11]= 0.0;
	this->_data[12]= 0.0;	this->_data[13]= 0.0;	this->_data[14]= 0.0;	this->_data[15]= 1.0;
}


WCMatrix4 WCMatrix4::FromGLMatrix(GLfloat *data) {
	WCMatrix4 retMat(data[0], data[4], data[8],  data[12],
					 data[1], data[5], data[9],  data[13],
					 data[2], data[6], data[10], data[14],
					 data[3], data[7], data[11], data[15]);
	return retMat;
}


void WCMatrix4::GLLoadMatrix(void) {
	GLfloat data[16] = { (GLfloat)this->_data[0], (GLfloat)this->_data[4], (GLfloat)this->_data[8], (GLfloat)this->_data[12],
						 (GLfloat)this->_data[1], (GLfloat)this->_data[5], (GLfloat)this->_data[9], (GLfloat)this->_data[13],
						 (GLfloat)this->_data[2], (GLfloat)this->_data[6], (GLfloat)this->_data[10], (GLfloat)this->_data[14],
						 (GLfloat)this->_data[3], (GLfloat)this->_data[7], (GLfloat)this->_data[11], (GLfloat)this->_data[15] };
	glLoadMatrixf(data);
}


WCMatrix WCMatrix4::ToMatrix(void) { 
	return WCMatrix(4, 4, this->_data); 
}


void WCMatrix4::ToElement(xercesc::DOMElement *element) {
	std::cout << "!!!!!***** Need to implement WCMatrix4::ToElement\n";
}


void WCMatrix4::FromElement(xercesc::DOMElement *element) {
	std::cout << "!!!!!***** Need to implement WCMatrix4::FromElement\n";
}


WCMatrix4 WCMatrix4::operator+(const WCMatrix4 &m) {
	WCMatrix4 retMat(this->_data[0] + m._data[0],  this->_data[1] + m._data[1], 
					this->_data[2] + m._data[2],  this->_data[3] + m._data[3],
					this->_data[4] + m._data[4],  this->_data[5] + m._data[5],
					this->_data[6] + m._data[6],  this->_data[7] + m._data[7],
					this->_data[8] + m._data[8],  this->_data[9] + m._data[9], 
					this->_data[10]+ m._data[10], this->_data[11]+ m._data[11],
					this->_data[12]+ m._data[12], this->_data[13]+ m._data[13],
					this->_data[14]+ m._data[14], this->_data[15]+ m._data[15]);
	return retMat;
}


WCMatrix4 WCMatrix4::operator-(const WCMatrix4 &m) {
	WCMatrix4 retMat(this->_data[0] - m._data[0],  this->_data[1] - m._data[1], 
					this->_data[2] - m._data[2],  this->_data[3] - m._data[3],
					this->_data[4] - m._data[4],  this->_data[5] - m._data[5],
					this->_data[6] - m._data[6],  this->_data[7] - m._data[7],
					this->_data[8] - m._data[8],  this->_data[9] - m._data[9], 
					this->_data[10]- m._data[10], this->_data[11]- m._data[11],
					this->_data[12]- m._data[12], this->_data[13]- m._data[13],
					this->_data[14]- m._data[14], this->_data[15]- m._data[15]);
	return retMat;
}


WCMatrix4 WCMatrix4::operator*(const WPFloat &scalar) {
	WCMatrix4 retMat(this->_data[0] * scalar, this->_data[1] * scalar, this->_data[2] * scalar, this->_data[3] * scalar,
					this->_data[4] * scalar, this->_data[5] * scalar, this->_data[6] * scalar, this->_data[7] * scalar,
					this->_data[8] * scalar, this->_data[9] * scalar, this->_data[10]* scalar, this->_data[11]* scalar,
					this->_data[12]* scalar, this->_data[13]* scalar, this->_data[14]* scalar, this->_data[15]* scalar);
	return retMat;
}


WCVector4 WCMatrix4::operator*(const WCVector4 &v) const {
	WCVector4 retVec(
		(this->_data[0] *v._data.d[0]) + (this->_data[1] * v._data.d[1]) + (this->_data[2] * v._data.d[2]) + (this->_data[3] * v._data.d[3]),
		(this->_data[4] *v._data.d[0]) + (this->_data[5] * v._data.d[1]) + (this->_data[6] * v._data.d[2]) + (this->_data[7] * v._data.d[3]),
		(this->_data[8] *v._data.d[0]) + (this->_data[9] * v._data.d[1]) + (this->_data[10]* v._data.d[2]) + (this->_data[11]* v._data.d[3]),
		(this->_data[12]*v._data.d[0]) + (this->_data[13]* v._data.d[1]) + (this->_data[14]* v._data.d[2]) + (this->_data[15]* v._data.d[3]));
	return retVec;
}


WCMatrix4 WCMatrix4::operator*(const WCMatrix4 &m) {
	WCMatrix4 retMat(
		(this->_data[0] * m._data[0]) + (this->_data[1] * m._data[4]) + (this->_data[2] * m._data[8])  + (this->_data[3] * m._data[12]),
		(this->_data[0] * m._data[1]) + (this->_data[1] * m._data[5]) + (this->_data[2] * m._data[9])  + (this->_data[3] * m._data[13]),
		(this->_data[0] * m._data[2]) + (this->_data[1] * m._data[6]) + (this->_data[2] * m._data[10]) + (this->_data[3] * m._data[14]),
		(this->_data[0] * m._data[3]) + (this->_data[1] * m._data[7]) + (this->_data[2] * m._data[11]) + (this->_data[3] * m._data[15]),
		(this->_data[4] * m._data[0]) + (this->_data[5] * m._data[4]) + (this->_data[6] * m._data[8])  + (this->_data[7] * m._data[12]),
		(this->_data[4] * m._data[1]) + (this->_data[5] * m._data[5]) + (this->_data[6] * m._data[9])  + (this->_data[7] * m._data[13]),
		(this->_data[4] * m._data[2]) + (this->_data[5] * m._data[6]) + (this->_data[6] * m._data[10]) + (this->_data[7] * m._data[14]),
		(this->_data[4] * m._data[3]) + (this->_data[5] * m._data[7]) + (this->_data[6] * m._data[11]) + (this->_data[7] * m._data[15]),
		(this->_data[8] * m._data[0]) + (this->_data[9] * m._data[4]) + (this->_data[10]* m._data[8])  + (this->_data[11]* m._data[12]),
		(this->_data[8] * m._data[1]) + (this->_data[9] * m._data[5]) + (this->_data[10]* m._data[9])  + (this->_data[11]* m._data[13]),
		(this->_data[8] * m._data[2]) + (this->_data[9] * m._data[6]) + (this->_data[10]* m._data[10]) + (this->_data[11]* m._data[14]),
		(this->_data[8] * m._data[3]) + (this->_data[9] * m._data[7]) + (this->_data[10]* m._data[11]) + (this->_data[11]* m._data[15]),
		(this->_data[12]* m._data[0]) + (this->_data[13]* m._data[4]) + (this->_data[14]* m._data[8])  + (this->_data[15]* m._data[12]),
		(this->_data[12]* m._data[1]) + (this->_data[13]* m._data[5]) + (this->_data[14]* m._data[9])  + (this->_data[15]* m._data[13]),
		(this->_data[12]* m._data[2]) + (this->_data[13]* m._data[6]) + (this->_data[14]* m._data[10]) + (this->_data[15]* m._data[14]),
		(this->_data[12]* m._data[3]) + (this->_data[13]* m._data[7]) + (this->_data[14]* m._data[11]) + (this->_data[15]* m._data[15]));
	return retMat;
}


WCMatrix4& WCMatrix4::operator=(const WCMatrix4 &m) {
	this->_data[0] = m._data[0];	this->_data[1] = m._data[1];	this->_data[2] = m._data[2];	this->_data[3] = m._data[3];
	this->_data[4] = m._data[4];	this->_data[5] = m._data[5];	this->_data[6] = m._data[6];	this->_data[7] = m._data[7];
	this->_data[8] = m._data[8];	this->_data[9] = m._data[9];	this->_data[10]= m._data[10];	this->_data[11]= m._data[11];
	this->_data[12]= m._data[12];	this->_data[13]= m._data[13];	this->_data[14]= m._data[14];	this->_data[15]= m._data[15];
	return *this;
}


bool WCMatrix4::operator==(const WCMatrix4 &m) {
	if ((this->_data[0] == m._data[0]) && (this->_data[1] == m._data[1]) && 
		(this->_data[2] == m._data[2]) && (this->_data[3] == m._data[3]) &&
		(this->_data[4] == m._data[4]) && (this->_data[5] == m._data[5]) && 
		(this->_data[6] == m._data[6]) && (this->_data[7] == m._data[7]) &&
		(this->_data[8] == m._data[8]) && (this->_data[9] == m._data[9]) && 
		(this->_data[10] == m._data[10]) && (this->_data[11] == m._data[11]) &&
		(this->_data[12] == m._data[12]) && (this->_data[13] == m._data[13]) && 
		(this->_data[14] == m._data[14]) && (this->_data[15] == m._data[15])) return true;
	return false;
}

	
WPFloat WCMatrix4::NormL2(void) {
	WPFloat retVal = 1.0;
	return retVal;
}


WPFloat WCMatrix4::NormInf(void) {
	WPFloat retVal = 1.0;
	return retVal;
}


WPFloat WCMatrix4::Determinant(void) {
	WPFloat retVal = 1.0;
	return retVal;
}


WCMatrix4 WCMatrix4::Inverse(void) {
/*
	//Create the new output matrix
	WCMatrix4 C(*this);
	WCMatrix4 work;
	__CLPK_integer M = 4;	
	__CLPK_integer N =4;		
	__CLPK_integer IPIV[4];	
	__CLPK_integer INFO = 0;	

	//First, compute the LU-decomposition matrix
	dgetrf_(&M, &N, C.GetData(), &N, IPIV, &INFO);
	if (INFO != 0) std::cout << "dgetrf: " << INFO << std::endl;	
	//Then get the inverse
	dgetri_(&N, C.GetData(), &N, IPIV, work.GetData(), &N, &INFO);	
	if (INFO != 0) std::cout << "dgetri: " << INFO << std::endl;	
	return C;
*/
	//Temporary variables
	WCMatrix4 dst;
	WPFloat tmp[12];
	WPFloat src[16];
	WPFloat det;

  // transpose matrix
    for (int i = 0; i < 4; i++) { 
        src[i]        = this->_data[i*4]; 
        src[i + 4]    = this->_data[i*4 + 1]; 
        src[i + 8]    = this->_data[i*4 + 2]; 
        src[i + 12]   = this->_data[i*4 + 3]; 
    } 
    // calculate pairs for first 8 elements (cofactors)
    tmp[0]  = src[10] * src[15]; 
    tmp[1]  = src[11] * src[14]; 
    tmp[2]  = src[9]  * src[15]; 
    tmp[3]  = src[11] * src[13]; 
    tmp[4]  = src[9]  * src[14]; 
    tmp[5]  = src[10] * src[13]; 
    tmp[6]  = src[8]  * src[15]; 
    tmp[7]  = src[11] * src[12]; 
    tmp[8]  = src[8]  * src[14]; 
    tmp[9]  = src[10] * src[12]; 
    tmp[10] = src[8]  * src[13]; 
    tmp[11] = src[9]  * src[12]; 
    // calculate first 8 elements (cofactors)
    dst._data[0]  = tmp[0]*src[5] + tmp[3]*src[6] + tmp[4]*src[7]; 
    dst._data[0] -= tmp[1]*src[5] + tmp[2]*src[6] + tmp[5]*src[7]; 
    dst._data[1]  = tmp[1]*src[4] + tmp[6]*src[6] + tmp[9]*src[7]; 
    dst._data[1] -= tmp[0]*src[4] + tmp[7]*src[6] + tmp[8]*src[7]; 
    dst._data[2]  = tmp[2]*src[4] + tmp[7]*src[5] + tmp[10]*src[7]; 
    dst._data[2] -= tmp[3]*src[4] + tmp[6]*src[5] + tmp[11]*src[7]; 
    dst._data[3]  = tmp[5]*src[4] + tmp[8]*src[5] + tmp[11]*src[6]; 
    dst._data[3] -= tmp[4]*src[4] + tmp[9]*src[5] + tmp[10]*src[6]; 
    dst._data[4]  = tmp[1]*src[1] + tmp[2]*src[2] + tmp[5]*src[3]; 
    dst._data[4] -= tmp[0]*src[1] + tmp[3]*src[2] + tmp[4]*src[3]; 
    dst._data[5]  = tmp[0]*src[0] + tmp[7]*src[2] + tmp[8]*src[3]; 
    dst._data[5] -= tmp[1]*src[0] + tmp[6]*src[2] + tmp[9]*src[3]; 
    dst._data[6]  = tmp[3]*src[0] + tmp[6]*src[1] + tmp[11]*src[3]; 
    dst._data[6] -= tmp[2]*src[0] + tmp[7]*src[1] + tmp[10]*src[3]; 
    dst._data[7]  = tmp[4]*src[0] + tmp[9]*src[1] + tmp[10]*src[2]; 
    dst._data[7] -= tmp[5]*src[0] + tmp[8]*src[1] + tmp[11]*src[2]; 
    // calculate pairs for second 8 elements (cofactors)
    tmp[0]  = src[2]*src[7]; 
    tmp[1]  = src[3]*src[6]; 
    tmp[2]  = src[1]*src[7]; 
    tmp[3]  = src[3]*src[5]; 
    tmp[4]  = src[1]*src[6]; 
    tmp[5]  = src[2]*src[5];
    tmp[6]  = src[0]*src[7]; 
    tmp[7]  = src[3]*src[4]; 
    tmp[8]  = src[0]*src[6]; 
    tmp[9]  = src[2]*src[4]; 
    tmp[10] = src[0]*src[5]; 
    tmp[11] = src[1]*src[4]; 
    // calculate second 8 elements (cofactors)
    dst._data[8]  = tmp[0]*src[13] + tmp[3]*src[14] + tmp[4]*src[15]; 
    dst._data[8] -= tmp[1]*src[13] + tmp[2]*src[14] + tmp[5]*src[15]; 
    dst._data[9]  = tmp[1]*src[12] + tmp[6]*src[14] + tmp[9]*src[15]; 
    dst._data[9] -= tmp[0]*src[12] + tmp[7]*src[14] + tmp[8]*src[15]; 
    dst._data[10] = tmp[2]*src[12] + tmp[7]*src[13] + tmp[10]*src[15]; 
    dst._data[10]-= tmp[3]*src[12] + tmp[6]*src[13] + tmp[11]*src[15]; 
    dst._data[11] = tmp[5]*src[12] + tmp[8]*src[13] + tmp[11]*src[14]; 
    dst._data[11]-= tmp[4]*src[12] + tmp[9]*src[13] + tmp[10]*src[14]; 
    dst._data[12] = tmp[2]*src[10] + tmp[5]*src[11] + tmp[1]*src[9]; 
    dst._data[12]-= tmp[4]*src[11] + tmp[0]*src[9] + tmp[3]*src[10]; 
    dst._data[13] = tmp[8]*src[11] + tmp[0]*src[8] + tmp[7]*src[10]; 
    dst._data[13]-= tmp[6]*src[10] + tmp[9]*src[11] + tmp[1]*src[8]; 
    dst._data[14] = tmp[6]*src[9] + tmp[11]*src[11] + tmp[3]*src[8]; 
    dst._data[14]-= tmp[10]*src[11] + tmp[2]*src[8] + tmp[7]*src[9]; 
    dst._data[15] = tmp[10]*src[10] + tmp[4]*src[8] + tmp[9]*src[9]; 
    dst._data[15]-= tmp[8]*src[9] + tmp[11]*src[10] + tmp[5]*src[8]; 
    // calculate determinant 
    det=src[0]*dst._data[0]+src[1]*dst._data[1]+src[2]*dst._data[2]+src[3]*dst._data[3]; 
    // calculate this->_datarix inverse
    det = 1/det; 
    for (int j = 0; j < 16; j++) 
        dst._data[j] *= det;
		
	//Return the matrix
	return dst;
}


WCMatrix4 WCMatrix4::Transpose(void) {
	WCMatrix4 tmp(this->_data[0], this->_data[4], this->_data[8], this->_data[12],
				 this->_data[1], this->_data[5], this->_data[9], this->_data[13],
				 this->_data[2], this->_data[6], this->_data[10], this->_data[14],
				 this->_data[3], this->_data[7], this->_data[11], this->_data[15]);
	return tmp;
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCMatrix4 &matrix) {
	void* addr = (void*)&matrix;
	
	//Print out basic surface info
	out << "Matrix4(" << addr << ")\n";	
	out << matrix._data[0] << "\t" << matrix._data[1] << "\t" << matrix._data[2] << "\t" << matrix._data[3] << std::endl;
	out << matrix._data[4] << "\t" << matrix._data[5] << "\t" << matrix._data[6] << "\t" << matrix._data[7] << std::endl;
	out << matrix._data[8] << "\t" << matrix._data[9] << "\t" << matrix._data[10]<< "\t" << matrix._data[11]<< std::endl;
	out << matrix._data[12]<< "\t" << matrix._data[13]<< "\t" << matrix._data[14]<< "\t" << matrix._data[15]<< std::endl;
	//Return the output stream
	return out;
}


/***********************************************~***************************************************/


WCMatrix::WCMatrix(const WPUInt &numRow, const WPUInt &numCol) : _numRow(numRow), _numCol(numCol), _data(NULL) {
	//Calculate the size of the matrix
	int size = numRow * numCol;
	//Allocate space for the matrix
	this->_data = new WPFloat[size];
	//Set all elements in the matrix to 0
	memset(this->_data, 0, numRow * numCol * sizeof(WPFloat));
}


WCMatrix::WCMatrix(const WPUInt &numRow, const WPUInt &numCol, const WPFloat* data) : _numRow(numRow), _numCol(numCol), _data(NULL) {
	//Calculate the size of the matrix
	int size = numRow * numCol;
	//Allocate space for the matrix
	this->_data = new WPFloat[size];
	//Copy in the passed data
	memcpy(this->_data, data, size * sizeof(WPFloat));
}


WCMatrix::WCMatrix(const WCMatrix& matrix) : _numRow(matrix._numRow), _numCol(matrix._numCol), _data(NULL) {
	//Calculate the size of the matrix
	int size = this->_numRow * this->_numCol;
	//Allocate space for the matrix
	this->_data = new WPFloat[size];
	//Copy in the passed data
	memcpy(this->_data, matrix._data, size * sizeof(WPFloat));
}


WCMatrix::~WCMatrix() {
	//Just make sure to delete the allocated space
	delete this->_data;
}


void WCMatrix::SetIdentity(void) {
	//Set the entire matrix to zeros
	memset(this->_data, 0, this->_numRow * this->_numCol * sizeof(WPFloat));
	//Find the shorter dimension and work backwards towards 0,0
#ifdef __APPLE__
	for (WPUInt i = (WPUInt)std::min(this->_numRow, this->_numCol)-1; i >= 0; i--) {
#endif
#ifdef __WIN32__
	for (WPUInt i = (WPUInt)min(this->_numRow, this->_numCol)-1; i >= 0; i--) {
#endif
		//Set the element to 1.0
		this->Set(i, i, 1.0);
	}
}


void WCMatrix::SetEmpty(void) {
	//Just memset the entire array to 0
	memset(this->_data, 0, this->_numRow * this->_numCol * sizeof(WPFloat));
}


WPFloat WCMatrix::RowMinimum(const WPUInt &row) {
	//Bounds check the row number
	if (row > this->_numRow) return 0.0;
	//Find the minimum
	WPUInt rowIndex = row * this->_numCol;
	WPFloat min = this->_data[rowIndex];
	for (WPUInt i=1; i<this->_numCol; ++i)
		if (min > this->_data[rowIndex + i]) min = this->_data[rowIndex + i];
	//Return the min value
	return min;
}


WPFloat WCMatrix::RowMaximum(const WPUInt &row) {
	//Bounds check the row number
	if (row > this->_numRow) return 0.0;
	//Find the maximum
	WPUInt rowIndex = row * this->_numCol;
	WPFloat max = this->_data[rowIndex];
	for (WPUInt i=1; i<this->_numCol; ++i)
		if (max < this->_data[rowIndex + i]) max = this->_data[rowIndex + i];
	//Return the max value
	return max;
}


WPFloat WCMatrix::ColumnMinimum(const WPUInt &col) {
	//Bounds check the column number
	if (col > this->_numCol) return 0.0;
	//Find the minimum
	WPFloat min = this->_data[col];
	for (WPUInt i=1; i<this->_numRow; ++i)
		if (min > this->_data[i*this->_numCol + col]) min = this->_data[i*this->_numCol + col];
	//Return the min value
	return min;
}


WPFloat WCMatrix::ColumnMaximum(const WPUInt &col) {
	//Bounds check the column number
	if (col > this->_numCol) return 0.0;
	//Find the maximum
	WPFloat max = this->_data[col];
	for (WPUInt i=1; i<this->_numRow; ++i)
		if (max < this->_data[i*this->_numCol + col]) max = this->_data[i*this->_numCol + col];
	//Return the max value
	return max;
}


WCMatrix4 WCMatrix::ToMatrix4(void) {
	//Check to make sure the matrix diminsions agree
	if ((this->_numRow != 4) || (this->_numCol != 4)) {
		std::cout << "WCMatrix::ToMatrix4 Error - Matrix sizes do not match\n";
		return WCMatrix4();
	}
	//If all is good, create the new object
	return WCMatrix4(this->_data);
}

	
WCMatrix WCMatrix::operator+(const WCMatrix &matrix) {
	WPUInt size = this->_numRow * this->_numCol;
	//Allocate space for the new matrix
	WPFloat* data = new WPFloat[size];
	//Check to make sure the matrix diminsions agree
	if ((this->_numRow != matrix._numRow) || (this->_numCol != matrix._numCol)) {
		std::cout << "WCMatrix::operator+ Error - Matrix sizes do not match\n";
		return WCMatrix(this->_numRow, this->_numCol, data);
	}
	//Go through both matricies and add the values together and put it into the array
	for(WPUInt i = 0; i < size; i++) data[i] = this->_data[i] + matrix._data[i];
	//Return a new matrix with the values
	return WCMatrix(this->_numRow, this->_numCol, data);
}


WCMatrix WCMatrix::operator-(const WCMatrix &matrix) {
	WPUInt size = this->_numRow * this->_numCol;
	//Allocate space for the new matrix
	WPFloat* data = new WPFloat[size];
	//Check to make sure the matrix diminsions agree
	if ((this->_numRow != matrix._numRow) || (this->_numCol != matrix._numCol)) {
		std::cout << "WCMatrix::operator- Error - Matrix sizes do not match\n";
		return WCMatrix(this->_numRow, this->_numCol, data);
	}
	//Go through both matricies and subtract the values together and put it into the array
	for(WPUInt i = 0; i < size; i++) data[i] = this->_data[i] - matrix._data[i];
	//Return a new matrix with the values
	return WCMatrix(this->_numRow, this->_numCol, data);
}


WCMatrix WCMatrix::operator*(const WPFloat &scalar) {
	WPUInt size = this->_numRow * this->_numCol;
	//Allocate space for the new matrix
	WPFloat* data = new WPFloat[size];
	//Go through the matrix and multiply each element by the scalar
	for(WPUInt i = 0; i < size; i++) data[i] = this->_data[i] * scalar;
	//Return a new matrix with the values
	return WCMatrix(this->_numRow, this->_numCol, data);
}


WCVector WCMatrix::operator*(const WCVector &vector) {
	WCVector v(vector);
	//Check to make sure the matrix diminsions agree
	if (this->_numCol != vector._size) {
		std::cout << "WCMatrix::operator* Error - Matrix size does not match vector size\n";
		return v;
	}
//Apple specific optimization
#ifdef __APPLE__
	//Now perform the multiplication using BLAS function
	cblas_dgemv(CblasRowMajor, CblasNoTrans, this->_numRow, this->_numCol, 1.0, this->_data, v._size, v._data, 1, 0.0, v._data, 1);
#endif
//Windows default path
#ifdef __WIN32__
	std::cout << "WCMatrix::operator* Error - Need windows specific path\n";
#endif
	//return the new value
	return v;
}


WCMatrix WCMatrix::operator*(const WCMatrix &matrix) {
	//Create the new output matrix
	WCMatrix c(this->_numRow, matrix._numCol);
	//Check to make sure the matrix diminsions agree
	if (this->_numCol != matrix._numRow) {
		std::cout << "WCMatrix::operator* Error - Matrix sizes do not match\n";
		return c;
	}
//Apple specific optimization
#ifdef __APPLE__
	//Call the cblas_dgemm routine
	cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, 
				this->_numRow, matrix._numCol, this->_numCol, 1.0, 
				this->_data, this->_numRow, 
				matrix._data, matrix._numCol, 
				0.0, c._data, c._numCol);
#endif
//Windows default path
#ifdef __WIN32__
	std::cout << "WCMatrix::operator* with matrix - Need windows specific path\n";
#endif
	//dgemm places the results into the C matrix
	return c;
}


WCMatrix& WCMatrix::operator=(const WCMatrix &matrix) {
	//Reset the parameters for this matrix
	this->_numRow = matrix._numRow;
	this->_numCol = matrix._numCol;
	delete this->_data;
	//Variable for the size of the matrix
	WPUInt size = this->_numRow * this->_numCol;
	//Allocate new space for the matrix
	this->_data = new WPFloat[size];
//Apple specific optimization
#ifdef __APPLE__
	//Copy each element of matrix into this using BLAS function
    cblas_dcopy(size, matrix._data, 1, this->_data, 1 );
#endif
//Windows default path
#ifdef __WIN32__
	memcpy(this->_data, matrix._data, size * sizeof(WPFloat));
#endif
	//Return the value of the matrix
	return *this;
}


bool WCMatrix::operator==(const WCMatrix &matrix) {
	//Check to make sure the matrix diminsions agree
	if ((this->_numRow != matrix._numRow) || (this->_numCol != matrix._numCol)) {
		return false;
	}	
	//Variable for the size of the matrix
	WPUInt size = this->_numRow * this->_numCol;
	//Check each element of matrix vs this, return false on the first non-match
	for(WPUInt i=0; i < size; i++) if(this->_data[i] != matrix._data[i]) return false;
	//Otherwise return true
	return true;
}


WPFloat WCMatrix::NormL2(void) {
	return 1.0;
}


WPFloat WCMatrix::NormInf(void) {
	return 1.0;
}


WPFloat WCMatrix::Determinant(void) {
	return 1.0;
}


WCMatrix WCMatrix::Inverse(void) {
	//Create the new output matrix
	WCMatrix C(*this);
//Apple specific optimization
#ifdef __APPLE__
	WCMatrix work(this->_numRow, this->_numRow);
	__CLPK_integer M = this->_numRow;	
	__CLPK_integer N = this->_numRow;		
	__CLPK_integer IPIV[this->_numRow];	
	__CLPK_integer INFO = 0;	
	//First compute the LU-decomposition matrix
	dgetrf_(&M, &N, C.GetData(), &N, IPIV, &INFO);
	if (INFO != 0) std::cout << "dgetrf: " << INFO << std::endl;	
	//Then get the inverse
	dgetri_(&N, C.GetData(), &N, IPIV, work.GetData(), &N, &INFO);	
	if (INFO != 0) std::cout << "dgetri: " << INFO << std::endl;
#endif
//Windows default path
#ifdef __WIN32__
	std::cout << "WCMatrix::Inverse - Need windows specific path\n";
#endif
	return C;
}


WCMatrix WCMatrix::Transpose(void) {
	WCMatrix tmp(1, 1);
	return tmp;
}


WCVector WCMatrix::Solve(WCVector &vector) {
	WCVector soln(vector);
	//Check to make sure the matrix is square
	if (this->_numRow != this->_numCol) {
		std::cout << "WCMatrix::Solve Error - Not a square matrix\n";
		return soln;
	}
	WPUInt size = vector.Size();
//Apple specific optimization
#ifdef __APPLE__
	__CLPK_integer N = size;
	__CLPK_integer NHRS = 1;
	__CLPK_integer LDA = size;
	__CLPK_integer IPIV[size];
	__CLPK_integer LDB = size;
	__CLPK_integer INFO = 0;
	//Blas to solve
	dgesv_(&N, &NHRS, this->_data, &LDA, IPIV, soln.GetData(), &LDB, &INFO);
	if (INFO != 0)
		std::cout << "WCMatrix::Solve Error - Bad return from dgesv_ : " << INFO << std::endl;
#endif
//Windows default path
#ifdef __WIN32__
	std::cout << "WCMatrix::Solve - Need windows specific path\n";
#endif
	return soln;	
}


/***********************************************~***************************************************/


std::ostream& operator<<(std::ostream& out, const WCMatrix &matrix) {
	void* addr = (void*)&matrix;
	
	//Print out basic surface info
	out << "Matrix(" << addr << ")\n";	
	std::cout.width(7);
	std::cout.setf(std::ios_base::showpoint);
	std::cout.precision(6);
	for(WPUInt r=0; r < matrix._numRow; r++) {
		for(WPUInt c=0; c < matrix._numCol; c++)
			std::cout << matrix.Get(r, c) << "\t";
		std::cout << std::endl;
	}
	//Return the output stream
	return out;
}


/***********************************************~***************************************************/


