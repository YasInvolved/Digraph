#pragma once

bool isAVX2Supported();

class SIMDMatrix
{
public:
	SIMDMatrix();
	SIMDMatrix(size_t size);

	~SIMDMatrix();

	void set(size_t row, size_t col, float value);
	float get(size_t row, size_t col);

	SIMDMatrix operator+(const SIMDMatrix& m);
	SIMDMatrix operator*(const SIMDMatrix& m);

	static SIMDMatrix Identity(size_t size);

private:
	size_t m_size; // width and length in one variable, it's a square matrix
	float* m_array;
};