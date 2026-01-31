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

#pragma once

namespace linear_algebra
{
	template <typename T>
	concept ScalarType = std::is_arithmetic_v<T> && std::convertible_to<T, float>;

	class SIMDMatrix
	{
	public:
		SIMDMatrix()
			: m_rows(0), m_cols(0), m_stride(0), m_strideRow(0), m_data(nullptr)
		{ }

		SIMDMatrix(size_t rc);
		SIMDMatrix(size_t rows, size_t cols);
		~SIMDMatrix();

		// copy ctors
		SIMDMatrix(const SIMDMatrix& other);
		SIMDMatrix& operator=(const SIMDMatrix& other);

		// move ctors
		SIMDMatrix(SIMDMatrix&& other) noexcept;
		SIMDMatrix& operator=(SIMDMatrix&& other) noexcept;


		bool isSquare() const { return m_cols == m_rows; }
		bool isZero() const;

		float get(size_t row, size_t col) const;
		void set(size_t row, size_t col, float value);

		size_t getRowCount() const { return m_rows; }
		size_t getColCount() const { return m_cols; }

		SIMDMatrix operator+(const SIMDMatrix&);
		inline SIMDMatrix operator+=(const SIMDMatrix& other)
		{
			*this = *this + other;
			return *this;
		}
		
		friend SIMDMatrix operator*(const SIMDMatrix& rhs, ScalarType auto lhs) noexcept
		{
			SIMDMatrix result = rhs;
			__m256 scalarVec = _mm256_set1_ps(static_cast<float>(lhs));

			for (size_t i = 0; i < result.m_rows; i++)
			{
				for (size_t j = 0; j < result.m_stride; j+=8)
				{
					size_t ix = i * rhs.m_stride + j;
					const float* inputPtr = &rhs.m_data[ix];
					float* resultPtr = &result.m_data[ix];
					__m256 vecA = _mm256_load_ps(inputPtr);
					__m256 vecRes = _mm256_mul_ps(vecA, scalarVec);
					_mm256_store_ps(resultPtr, vecRes);
				}
			}

			_mm256_zeroupper();
			return result;
		}

		inline friend SIMDMatrix operator*(ScalarType auto lhs, const SIMDMatrix& rhs) noexcept
		{
			return rhs * lhs;
		}

		SIMDMatrix& operator*=(ScalarType auto lhs) noexcept
		{
			__m256 scalarVec = _mm256_set1_ps(static_cast<float>(lhs));

			for (size_t i = 0; i < m_rows; i++)
			for (size_t j = 0; j < m_stride; j += 8)
			{
				float* inputPtr = &m_data[i * m_stride + j];
				__m256 vecMat = _mm256_load_ps(inputPtr);
				__m256 res = _mm256_mul_ps(vecMat, scalarVec);
				_mm256_store_ps(inputPtr, res);
			}

			_mm256_zeroupper();
			return *this;
		}

		friend SIMDMatrix operator*(const SIMDMatrix& lhs, const SIMDMatrix& rhs);
		SIMDMatrix& operator*=(const SIMDMatrix& lhs)
		{
			*this = *this * lhs;
			return *this;
		}

		static SIMDMatrix Identity(size_t size);
		
	private:
		void initialize();

	private:
		size_t m_rows, m_cols, m_stride, m_strideRow;
		float* m_data;
	};

	// no need for pow -1, -2, 1/2 etc.
	SIMDMatrix pow(const SIMDMatrix& mat, uint64_t pow);
}