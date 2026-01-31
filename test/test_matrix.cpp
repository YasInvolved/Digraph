//	MIT License
//	
//	Copyright(c) 2026 Jakub B¹czyk
//	
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files(the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions :
//	
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//	
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.

#include <gtest/gtest.h>
#include <random>
#include "SIMDMatrix.h"

static constexpr size_t MATRIX_SIZE_LIMIT = 51;
using SIMDMatrix = linear_algebra::SIMDMatrix;

static std::random_device dev;
static std::mt19937 mersenneTwister(dev());

static SIMDMatrix naiveMultiplication(const SIMDMatrix& lhs, const SIMDMatrix& rhs)
{
	// Assertion is enough here. writing code manually. 
	// SIMDMatrix is treated as some kind of lib
	assert(lhs.getColCount() == rhs.getRowCount());
	SIMDMatrix result(lhs.getRowCount(), rhs.getColCount());

	for (size_t i = 0; i < result.getRowCount(); i++)
	for (size_t j = 0; j < result.getColCount(); j++)
	{
		float dp = 0.0f;

		for (size_t k = 0; k < lhs.getColCount(); k++)
			dp += lhs.get(i, k) * rhs.get(k, j);

		result.set(i, j, dp);
	}

	return result;
}

static SIMDMatrix naiveAddition(const SIMDMatrix& lhs, const SIMDMatrix& rhs)
{
	assert(lhs.getColCount() == rhs.getColCount());
	assert(lhs.getRowCount() == rhs.getRowCount());

	SIMDMatrix result(lhs.getRowCount(), lhs.getColCount());

	for (size_t i = 0; i < result.getRowCount(); i++)
	for (size_t j = 0; j < result.getColCount(); j++)
		result.set(i, j, lhs.get(i, j) + rhs.get(i, j));

	return result;
}

static SIMDMatrix naiveScalarMul(float scalar, const SIMDMatrix& rhs)
{
	SIMDMatrix result(rhs.getRowCount(), rhs.getColCount());

	for (size_t i = 0; i < result.getRowCount(); i++)
	for (size_t j = 0; j < result.getColCount(); j++)
	{
		result.set(i, j, rhs.get(i, j) * scalar);
	}

	return result;
}

static float genRandFloat(float min, float max)
{
	std::uniform_real_distribution<float> dist(min, max);
	return dist(mersenneTwister);
}

static SIMDMatrix genRandMatrix(size_t rows, size_t cols, float min, float max)
{
	SIMDMatrix mat(rows, cols);
	std::uniform_real_distribution<float> dist(min, max);

	for (size_t i = 0; i < rows; i++)
	for (size_t j = 0; j < cols; j++)
	{
		mat.set(i, j, dist(mersenneTwister));
	}

	return mat;
}

TEST(SIMDMatrix, IdentityScalarMultiplication)
{
	for (size_t size = 2; size <= MATRIX_SIZE_LIMIT; size++)
	{
		SIMDMatrix mat = SIMDMatrix::Identity(size);
		mat *= 5;

		for (size_t i = 0; i < size; i++)
		for (size_t j = 0; j < size; j++)
		{
			float val = mat.get(i, j);
			if (i == j)
				EXPECT_FLOAT_EQ(val, 5.0f);
			else
				EXPECT_FLOAT_EQ(val, 0.0f);
		}
	}
}

TEST(SIMDMatrix, IdentityAddition)
{
	for (size_t size = 2; size <= MATRIX_SIZE_LIMIT; size++)
	{
		SIMDMatrix mat = SIMDMatrix::Identity(size);
		mat += SIMDMatrix::Identity(size) * 27.0f;
		
		for (size_t i = 0; i < size; i++)
		for (size_t j = 0; j < size; j++)
		{
			float val = mat.get(i, j);
			if (i == j)
				EXPECT_FLOAT_EQ(val, 28.0f);
			else
				EXPECT_FLOAT_EQ(val, 0.0f);
		}
	}
}

TEST(SIMDMatrix, IdentityMultiplication)
{
	for (size_t size = 2; size <= MATRIX_SIZE_LIMIT; size++)
	{
		SIMDMatrix matA = SIMDMatrix::Identity(size) * 3.0f;
		SIMDMatrix matB = SIMDMatrix::Identity(size) * 9.0f;

		SIMDMatrix res = matA * matB;

		for (size_t i = 0; i < size; i++)
		for (size_t j = 0; j < size; j++)
		{
			float val = res.get(i, j);
			if (i == j)
				EXPECT_FLOAT_EQ(val, 27.0f);
			else
				EXPECT_FLOAT_EQ(val, 0.0f);
		}
	}
}

TEST(SIMDMatrix, Multiplication)
{
	for (size_t i = 2; i <= MATRIX_SIZE_LIMIT; i++)
	for (size_t j = 2; j <= MATRIX_SIZE_LIMIT; j++)
	{
		SIMDMatrix mat1 = genRandMatrix(i, j, 0.0f, 10.0f);
		SIMDMatrix mat2 = genRandMatrix(j, i, 0.0f, 10.0f);

		SIMDMatrix res = mat1 * mat2;
		SIMDMatrix resCmp = naiveMultiplication(mat1, mat2);

		ASSERT_EQ(res.getRowCount(), resCmp.getColCount());
		ASSERT_EQ(res.getColCount(), resCmp.getColCount());

		for (size_t r = 0; r < res.getRowCount(); r++)
		for (size_t c = 0; c < res.getColCount(); c++)
			EXPECT_NEAR(res.get(r, c), resCmp.get(r, c), 5e-3);
	}
}

TEST(SIMDMatrix, Addition)
{
	for (size_t i = 0; i < MATRIX_SIZE_LIMIT; i++)
	for (size_t j = 0; j < MATRIX_SIZE_LIMIT; j++)
	{
		SIMDMatrix mat1 = genRandMatrix(i, j, 0.0f, 10.0f);
		SIMDMatrix mat2 = genRandMatrix(i, j, 0.0f, 10.0f);

		auto res = mat1 + mat2;
		auto resCmp = naiveAddition(mat1, mat2);

		for (size_t r = 0; r < res.getRowCount(); r++)
		for (size_t c = 0; c < res.getColCount(); c++)
			EXPECT_NEAR(res.get(r, c), resCmp.get(r, c), 5e-3);
	}
}

TEST(SIMDMatrix, ScalarMultiplication)
{
	for (size_t i = 0; i < MATRIX_SIZE_LIMIT; i++)
	for (size_t j = 0; j < MATRIX_SIZE_LIMIT; j++)
	{
		float scalar = genRandFloat(0.0f, 10.0f);
		SIMDMatrix mat = genRandMatrix(i, j, 0.0f, 10.0f);

		auto res = scalar * mat;
		auto resCmp = naiveScalarMul(scalar, mat);

		for (size_t r = 0; r < res.getRowCount(); r++)
		for (size_t c = 0; c < res.getColCount(); c++)
			EXPECT_NEAR(res.get(r, c), resCmp.get(r, c), 5e-3);
	}
}