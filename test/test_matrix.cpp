#include <gtest/gtest.h>
#include "SIMDMatrix.h"

static constexpr size_t MATRIX_SIZE_LIMIT = 204; // Max. 2MB of heap mem used

using SIMDMatrix = linear_algebra::SIMDMatrix;

TEST(SIMDMatrix, IdentityScalarMultiplication)
{
	for (size_t size = 1; size <= MATRIX_SIZE_LIMIT; size++)
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
	for (size_t size = 1; size <= MATRIX_SIZE_LIMIT; size++)
	{
		SIMDMatrix mat = SIMDMatrix::Identity(size);
		mat += SIMDMatrix::Identity(size) * 27;
		
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
	for (size_t size = 1; size <= MATRIX_SIZE_LIMIT; size++)
	{
		SIMDMatrix matA = SIMDMatrix::Identity(size) * 3;
		SIMDMatrix matB = SIMDMatrix::Identity(size) * 9;

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