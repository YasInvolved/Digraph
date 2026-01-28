#include <gtest/gtest.h>
#include "SIMDMatrix.h"

using SIMDMatrix = linear_algebra::SIMDMatrix;

TEST(SIMDMatrix_test, IdentityInitialization)
{
	SIMDMatrix mat = SIMDMatrix::Identity(4);

	for (size_t i = 0; i < 4; i++)
	for (size_t j = 0; j < 4; j++)
	{
		float val = mat.get(i, j);
		if (i == j)
			EXPECT_FLOAT_EQ(val, 1.0f);
		else
			EXPECT_FLOAT_EQ(val, 0.0f);
	}
}

TEST(SIMDMatrix_test, IdentityAdditionTest)
{
	SIMDMatrix mat1 = SIMDMatrix::Identity(4);
	SIMDMatrix mat2 = SIMDMatrix::Identity(4);

	auto result = mat1 + mat2;

	EXPECT_EQ(mat1.getRowCount(), result.getRowCount());
	EXPECT_EQ(mat1.getColCount(), result.getColCount());
	EXPECT_EQ(mat2.getRowCount(), result.getRowCount());
	EXPECT_EQ(mat2.getColCount(), result.getColCount());

	for (size_t i = 0; i < result.getRowCount(); i++)
	for (size_t j = 0; j < result.getColCount(); j++)
	{
		float val = result.get(i, j);
		if (i == j)
			EXPECT_FLOAT_EQ(val, 2.0f);
		else
			EXPECT_FLOAT_EQ(val, 0.0f);
	}
}

TEST(SIMDMatrix_test, IdentityScalarMulTest)
{
	SIMDMatrix mat = SIMDMatrix::Identity(4);
	
	auto result = mat * 5;

	EXPECT_EQ(mat.getRowCount(), result.getRowCount());
	EXPECT_EQ(mat.getColCount(), result.getColCount());

	for (size_t i = 0; i < mat.getRowCount(); i++)
	for (size_t j = 0; j < mat.getColCount(); j++)
	{
		float val = result.get(i, j);
		if (i == j)
			EXPECT_FLOAT_EQ(val, 5.0f);
		else
			EXPECT_FLOAT_EQ(val, 0.0f);
	}
}

TEST(SIMDMatrix_test, IdentityScalarMulTest1)
{
	SIMDMatrix mat = SIMDMatrix::Identity(4);
	mat *= 5;

	EXPECT_EQ(mat.getRowCount(), 4);
	EXPECT_EQ(mat.getColCount(), 4);

	for (size_t i = 0; i < mat.getRowCount(); i++)
	for (size_t j = 0; j < mat.getColCount(); j++)
	{
		float val = mat.get(i, j);
		if (i == j)
			EXPECT_FLOAT_EQ(val, 5.0f);
		else
			EXPECT_FLOAT_EQ(val, 0.0f);
	}
}

TEST(SIMDMatrix_test, LargeMatricesScalarMulTest)
{
	static constexpr size_t SIZE_LIMIT = 102; // k * k * 100 <= 1 048 576 (1mb)
	
	for (size_t s = 1; s < SIZE_LIMIT; s++)
	{
		SIMDMatrix mat = SIMDMatrix::Identity(s);
		mat *= 5;

		EXPECT_EQ(mat.getRowCount(), s);
		EXPECT_EQ(mat.getColCount(), s);

		for (size_t i = 0; i < mat.getRowCount(); i++)
		for (size_t j = 0; j < mat.getColCount(); j++)
		{
			float val = mat.get(i, j);
			if (i == j)
				EXPECT_FLOAT_EQ(val, 5.0f);
			else
				EXPECT_FLOAT_EQ(val, 0.0f);
		}
	}
}

TEST(SIMDMatrix_test, LatgeMatricesAdditionTest)
{
	static constexpr size_t SIZE_LIMIT = 102; // k * k * 100 <= 1'048'576 (1mb)

	for (size_t s = 1; s < SIZE_LIMIT; s++)
	{
		SIMDMatrix mat = SIMDMatrix::Identity(s);
		mat += mat;

		EXPECT_EQ(mat.getRowCount(), s);
		EXPECT_EQ(mat.getColCount(), s);

		for (size_t i = 0; i < mat.getRowCount(); i++)
			for (size_t j = 0; j < mat.getColCount(); j++)
			{
				float val = mat.get(i, j);
				if (i == j)
					EXPECT_FLOAT_EQ(val, 2.0f);
				else
					EXPECT_FLOAT_EQ(val, 0.0f);
			}
	}
}