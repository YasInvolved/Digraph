#pragma once

bool isAVX2Supported();

namespace linear_algebra
{
	class SIMDMatrix
	{
	public:
		SIMDMatrix();
		SIMDMatrix(size_t size);

		void set(size_t row, size_t col, float value);
		float get(size_t row, size_t col) const;

		SIMDMatrix operator+(const SIMDMatrix& m);
		SIMDMatrix operator*(const SIMDMatrix& m);

		static SIMDMatrix Identity(size_t size);

	private:
		size_t m_size; // width and length in one variable, it's a square matrix
		std::unique_ptr<float> m_array;
	};
}