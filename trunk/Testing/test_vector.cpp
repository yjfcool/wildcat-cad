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
#include <gtest/gtest.h>
#include <Utility/vector.h>
#include <Utility/matrix.h>


/*** Locally Defined Values ***/
//None


/***********************************************~***************************************************/


// The fixture for testing class WCVector4.
class WCVector4Test : public testing::Test { };


// Tests that the default constructor sets all values to zero.
TEST(WCVector4Test, DetaultConstructor) {
	WCVector4 vec;
	EXPECT_DOUBLE_EQ(0.0, vec.I());
	EXPECT_DOUBLE_EQ(0.0, vec.J());
	EXPECT_DOUBLE_EQ(0.0, vec.K());
	EXPECT_DOUBLE_EQ(0.0, vec.L());
}


// Tests that primary constructor sets values properly.
TEST(WCVector4Test, PrimaryConstructor) {
	WCVector4 vec(1.0, 2.0, 3.0, 4.0);
	EXPECT_DOUBLE_EQ(1.0, vec.I());
	EXPECT_DOUBLE_EQ(2.0, vec.J());
	EXPECT_DOUBLE_EQ(3.0, vec.K());
	EXPECT_DOUBLE_EQ(4.0, vec.L());
}


// Tests that primary constructor sets values properly except dont set L value.
TEST(WCVector4Test, PrimaryConstructorWithoutL) {
	WCVector4 vec(1.0, 2.0, 3.0);
	EXPECT_DOUBLE_EQ(1.0, vec.I());
	EXPECT_DOUBLE_EQ(2.0, vec.J());
	EXPECT_DOUBLE_EQ(3.0, vec.K());
	EXPECT_DOUBLE_EQ(1.0, vec.L());
}


// Tests that primary constructor sets values to a single value.
TEST(WCVector4Test, SingleValueConstructor) {
	WCVector4 vec(3.0);
	EXPECT_DOUBLE_EQ(3.0, vec.I());
	EXPECT_DOUBLE_EQ(3.0, vec.J());
	EXPECT_DOUBLE_EQ(3.0, vec.K());
	EXPECT_DOUBLE_EQ(3.0, vec.L());
}


// Tests that primary constructor sets values from an array pointer.
TEST(WCVector4Test, ArrayPointerConstructor) {
	double data[4] = { 1.0, 2.0, 3.0, 4.0 };
	WCVector4 vec(data);
	EXPECT_DOUBLE_EQ(1.0, vec.I());
	EXPECT_DOUBLE_EQ(2.0, vec.J());
	EXPECT_DOUBLE_EQ(3.0, vec.K());
	EXPECT_DOUBLE_EQ(4.0, vec.L());
}


// Tests that primary constructor sets values from a sized array pointer.
TEST(WCVector4Test, SizedArrayPointerConstructor) {
	double data[4] = { 1.0, 2.0, 3.0, 4.0 };
	WCVector4 vec(4, data);
	EXPECT_DOUBLE_EQ(1.0, vec.I());
	EXPECT_DOUBLE_EQ(2.0, vec.J());
	EXPECT_DOUBLE_EQ(3.0, vec.K());
	EXPECT_DOUBLE_EQ(4.0, vec.L());

	WCVector4 vec2(2, data);
	EXPECT_DOUBLE_EQ(1.0, vec2.I());
	EXPECT_DOUBLE_EQ(2.0, vec2.J());
	EXPECT_DOUBLE_EQ(0.0, vec2.K());
	EXPECT_DOUBLE_EQ(0.0, vec2.L());	
}


// Tests that primary constructor sets values from another vector.
TEST(WCVector4Test, CopyConstructor) {
	WCVector4 vec1(1.0, 2.0, 3.0, 4.0);
	WCVector4 vec2(vec1);
	EXPECT_DOUBLE_EQ(1.0, vec2.I());
	EXPECT_DOUBLE_EQ(2.0, vec2.J());
	EXPECT_DOUBLE_EQ(3.0, vec2.K());
	EXPECT_DOUBLE_EQ(4.0, vec2.L());
}


// Tests that setting individual element values works.
TEST(WCVector4Test, SetElementValues) {
	WCVector4 vec;
	vec.I(1.0);
	vec.J(2.0);
	vec.K(3.0);
	vec.L(4.0);
	EXPECT_DOUBLE_EQ(1.0, vec.I());
	EXPECT_DOUBLE_EQ(2.0, vec.J());
	EXPECT_DOUBLE_EQ(3.0, vec.K());
	EXPECT_DOUBLE_EQ(4.0, vec.L());
}


// Tests that setting all element values works.
TEST(WCVector4Test, SetValues) {
	WCVector4 vec;
	vec.Set(1.0, 2.0, 3.0, 4.0);
	EXPECT_DOUBLE_EQ(1.0, vec.I());
	EXPECT_DOUBLE_EQ(2.0, vec.J());
	EXPECT_DOUBLE_EQ(3.0, vec.K());
	EXPECT_DOUBLE_EQ(4.0, vec.L());
}


// Tests setting values via equals.
TEST(WCVector4Test, BasicEquals) {
	WCVector4 vec1(1.0, 2.0, 3.0, 4.0);
	WCVector4 vec2(99.123, 292.2393, 39.29238,48349.0);
	vec2 = vec1;
	EXPECT_DOUBLE_EQ(1.0, vec2.I());
	EXPECT_DOUBLE_EQ(2.0, vec2.J());
	EXPECT_DOUBLE_EQ(3.0, vec2.K());
	EXPECT_DOUBLE_EQ(4.0, vec2.L());
}


// Tests converting WCVector4 to WCVector.
TEST(WCVector4Test, VectorConversion) {
	WCVector4 vec4(1.0, 2.0, 3.0, 4.0);
	WCVector vec = vec4.ToVector();
	EXPECT_EQ(4, vec.Size());
	EXPECT_DOUBLE_EQ(1.0, vec.Get(0));
	EXPECT_DOUBLE_EQ(2.0, vec.Get(1));
	EXPECT_DOUBLE_EQ(3.0, vec.Get(2));
	EXPECT_DOUBLE_EQ(4.0, vec.Get(3));
}


// Tests converting WCVector4 to WCMatrix.
TEST(WCVector4Test, MatrixConversion) {
	WCVector4 vec4(1.0, 2.0, 3.0, 4.0);
	WCMatrix mat = vec4.ToMatrix();
	EXPECT_EQ(1, mat.GetNumRow());
	EXPECT_EQ(4, mat.GetNumCol());
	EXPECT_DOUBLE_EQ(1.0, mat.Get(0,0));
	EXPECT_DOUBLE_EQ(2.0, mat.Get(0,1));
	EXPECT_DOUBLE_EQ(3.0, mat.Get(0,2));
	EXPECT_DOUBLE_EQ(4.0, mat.Get(0,3));
}
 
 
/*
xercesc::DOMElement* ToElement(xercesc::DOMNode *parent, const std::string &name);				//!< Serialize the object
void FromElement(xercesc::DOMElement *element);													//!< Deserialize the object
*/


// Tests adding vectors.
TEST(WCVector4Test, VectorAddition) {
	WCVector4 vec;
	WCVector4 vec1(1.0, 2.0, 3.0, 4.0);
	WCVector4 vec2(10.0, 20.0, 30.0, 40.0);
	vec = vec1 + vec2;
	EXPECT_DOUBLE_EQ(11.0, vec.I());
	EXPECT_DOUBLE_EQ(22.0, vec.J());
	EXPECT_DOUBLE_EQ(33.0, vec.K());
	EXPECT_DOUBLE_EQ(44.0, vec.L());
}


// Tests subtracting vectors.
TEST(WCVector4Test, VectorSubtraction) {
	WCVector4 vec;
	WCVector4 vec1(1.0, 2.0, 3.0, 4.0);
	WCVector4 vec2(10.0, 20.0, 30.0, 40.0);
	vec = vec1 - vec2;
	EXPECT_DOUBLE_EQ(-9.0, vec.I());
	EXPECT_DOUBLE_EQ(-18.0, vec.J());
	EXPECT_DOUBLE_EQ(-27.0, vec.K());
	EXPECT_DOUBLE_EQ(-36.0, vec.L());
}


// Tests scalar multiplication.
TEST(WCVector4Test, ScalarMultiplication) {
	WCVector4 vec(99.123, -292.2393, 39.29238,48349.0);
	WPFloat x = 237.237;
	vec = vec * x;
	EXPECT_DOUBLE_EQ(23515.643151, vec.I());
	EXPECT_DOUBLE_EQ(-69329.974814100002, vec.J());
	EXPECT_DOUBLE_EQ(9321.6063540600007, vec.K());
	EXPECT_DOUBLE_EQ(11470171.713, vec.L());
}


// Tests scalar division.
TEST(WCVector4Test, ScalarDivision) {
	WCVector4 vec(99.123, -292.2393, 39.29238,48349.0);
	WPFloat x = 237.237;
	vec = vec / x;
	EXPECT_DOUBLE_EQ(0.41782268364546848, vec.I());
	EXPECT_DOUBLE_EQ(-1.2318453698200533, vec.J());
	EXPECT_DOUBLE_EQ(0.16562500790348891, vec.K());
	EXPECT_DOUBLE_EQ(203.8004189902924, vec.L());
}


// Tests vector4-vector4 multiplication.
TEST(WCVector4Test, VectorVectorMultiplication) {
	WCVector4 vec1(99.123, -292.2393, 39.29238,48349.0);
	WCVector4 vec2(23.32, 2389.39, -94.02, 0.38329);
	WCMatrix4 mat = vec1 * vec2;
	//First row
	EXPECT_DOUBLE_EQ(2311.5483600000002, mat.Get(0,0));
	EXPECT_DOUBLE_EQ(236843.50497000001, mat.Get(0,1));
	EXPECT_DOUBLE_EQ(-9319.5444599999992, mat.Get(0,2));
	EXPECT_DOUBLE_EQ(37.992854670000007, mat.Get(0,3));
	//Second row
	EXPECT_DOUBLE_EQ(-6815.0204760000006, mat.Get(1,0));
	EXPECT_DOUBLE_EQ(-698273.66102700005, mat.Get(1,1));
	EXPECT_DOUBLE_EQ(27476.338985999999, mat.Get(1,2));
	EXPECT_DOUBLE_EQ(-112.01240129700001, mat.Get(1,3));
	//Third row
	EXPECT_DOUBLE_EQ(916.29830160000006, mat.Get(2,0));
	EXPECT_DOUBLE_EQ(93884.819848200001, mat.Get(2,1));
	EXPECT_DOUBLE_EQ(-3694.2695675999998, mat.Get(2,2));
	EXPECT_DOUBLE_EQ(15.0603763302, mat.Get(2,3));
	//Fourth row
	EXPECT_DOUBLE_EQ(1127498.6799999999, mat.Get(3,0));
	EXPECT_DOUBLE_EQ(115524617.11, mat.Get(3,1));
	EXPECT_DOUBLE_EQ(-4545772.9799999995, mat.Get(3,2));
	EXPECT_DOUBLE_EQ(18531.68821, mat.Get(3,3));
}


/*
 //Overloaded Operators
 WCMatrix  operator*(const WCVector &vector) const;												//!< Vector multiplication
*/ 


// Tests vector-matrix multiplication.
TEST(WCVector4Test, VectorMatrixMultiplication) {
	WCVector4 vec(99.123, -292.2393, 39.29238,48349.0);
	WCMatrix4 mat(23.32, 2389.39, -94.02, 0.38329,
				  0.328, -93.3832, 3289.328932, 239.329832,
				  1.349, 239.329323, -0.45, 1.0,
				  3490.349349, -239239.0230, 32923.23923, 21.29329);
	WCVector4 out = vec * mat;
	EXPECT_DOUBLE_EQ(168757169.37409121, out.I());
	EXPECT_DOUBLE_EQ(-11566693985.462324, out.J());
	EXPECT_DOUBLE_EQ(1590835085.1206815, out.K());
	EXPECT_DOUBLE_EQ(959644.98087187228, out.L());
}


// Tests scalar addition and assignment (+=).
TEST(WCVector4Test, ScalarAdditionAssignment) {
	WCVector4 vec(1.0, 2.0, 3.0, 4.0);
	WPFloat x = 34.9239329;
	vec += x;
	EXPECT_DOUBLE_EQ(35.9239329, vec.I());
	EXPECT_DOUBLE_EQ(36.9239329, vec.J());
	EXPECT_DOUBLE_EQ(37.9239329, vec.K());
	EXPECT_DOUBLE_EQ(38.9239329, vec.L());
}


// Scalar subtraction and assignment (-=).
TEST(WCVector4Test, ScalarSubtractionAssignment) {
	WCVector4 vec(1.0, 2.0, 3.0, 4.0);
	WPFloat x = 34.9239329;
	vec -= x;
	EXPECT_DOUBLE_EQ(-33.923932899999997, vec.I());
	EXPECT_DOUBLE_EQ(-32.923932899999997, vec.J());
	EXPECT_DOUBLE_EQ(-31.923932899999997, vec.K());
	EXPECT_DOUBLE_EQ(-30.923932899999997, vec.L());
}


// Scalar multiplication and assignment (*=).
TEST(WCVector4Test, ScalarMultiplicationAssignment) {
	WCVector4 vec(1.0, 2.0, 3.0, 4.0);
	WPFloat x = 34.9239329;
	vec *= x;
	EXPECT_DOUBLE_EQ(34.9239329, vec.I());
	EXPECT_DOUBLE_EQ(69.847865799999994, vec.J());
	EXPECT_DOUBLE_EQ(104.77179869999999, vec.K());
	EXPECT_DOUBLE_EQ(139.69573159999999, vec.L());
}


// Scalar division and assignment (/=).
TEST(WCVector4Test, ScalarDivisionAssignment) {
	WCVector4 vec(1.0, 2.0, 3.0, 4.0);
	WCVector4 cpy(vec);
	WPFloat x = 34.9239329;
	vec /= x;
	EXPECT_DOUBLE_EQ(cpy.I() / x, vec.I());
	EXPECT_DOUBLE_EQ(cpy.J() / x, vec.J());
	EXPECT_DOUBLE_EQ(cpy.K() / x, vec.K());
	EXPECT_DOUBLE_EQ(cpy.L() / x, vec.L());
}


// Vector addition and assignment (+=).
TEST(WCVector4Test, VectorAdditionAssignment) {
	WCVector4 vec1(1.0, 2.0, 3.0, 4.0);
	WCVector4 vec2(10.0, 20.0, 30.0, 40.0);
	vec1 += vec2;
	EXPECT_DOUBLE_EQ(11.0, vec1.I());
	EXPECT_DOUBLE_EQ(22.0, vec1.J());
	EXPECT_DOUBLE_EQ(33.0, vec1.K());
	EXPECT_DOUBLE_EQ(44.0, vec1.L());
}


// Vector subtraction and assignment (-=).
TEST(WCVector4Test, VectorSubtractionAssignment) {
	WCVector4 vec1(1.0, 2.0, 3.0, 4.0);
	WCVector4 vec2(10.0, 20.0, 30.0, 40.0);
	vec1 -= vec2;
	EXPECT_DOUBLE_EQ(-9.0, vec1.I());
	EXPECT_DOUBLE_EQ(-18.0, vec1.J());
	EXPECT_DOUBLE_EQ(-27.0, vec1.K());
	EXPECT_DOUBLE_EQ(-36.0, vec1.L());
}


/*
WCVector4& operator=(const WCVector &vector);													//!< Vector equation
bool operator==(const WCVector4 &vector);														//!< Vector equivalence
bool	operator!=(const WCVector4 &vector);													//!< Vector equivalence

//Other Methods
WPFloat Distance(const WCVector4 &vec) const;													//!< Calculate distance between two vector values
WPFloat Magnitude(void) const;																	//!< Calculate the magnitude of the vector
WCVector4 &Normalize(const bool &wIgnore=false);												//!< Normalize the vector
WCVector4 &Homogenize(void);																	//!< Homogenize the vector
WPFloat NormL2(void) const;																		//!< L2 Norm
WPFloat NormInf(void) const;																	//!< Infinity Norm
WPFloat DotProduct(const WCVector4 &vector) const;												//!< Return the dot product
WCVector4 CrossProduct(const WCVector4 &vector) const;											//!< Return the cross product
*/
 
 
/***********************************************~***************************************************/

