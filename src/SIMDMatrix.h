#pragma once

bool isAVX2Supported();

namespace linear_algebra
{
	template <typename T>
	concept ScalarType = requires {
		std::is_arithmetic_v<T>;
		std::convertible_to<T, float>;
	};

	class SIMDMatrix
	{
	public:
		SIMDMatrix()
			: m_rows(0), m_cols(0), m_stride(0), m_data(nullptr)
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

		SIMDMatrix operator+(const SIMDMatrix&);
		
		template <ScalarType T>
		SIMDMatrix operator*(T scalar) noexcept
		{
			float scalarValue = scalar; // convert to float
			SIMDMatrix result = *this;

			for (size_t i = 0; i < result.m_rows; i++)
			{
				for (size_t j = 0; j < result.m_stride; j++)
				{
					float* resultPtr = &result.m_data[i * result.m_stride + j];
					__m256 vecA = _mm256_load_ps(resultPtr);
					__m256 vecB = _mm256_set1_ps(scalarValue);

					__m256 vecRes = _mm256_mul_ps(vecA, vecB);
					_mm256_store_ps(resultPtr, vecRes);
				}
			}

			return result;
		}

		bool isSquare() const { return m_cols == m_rows; }

		float get(size_t row, size_t col) const;
		void set(size_t row, size_t col, float value);

		static SIMDMatrix Identity(size_t size);
		
	private:
		void initialize();

	private:
		size_t m_rows, m_cols, m_stride;
		float* m_data;
	};
}