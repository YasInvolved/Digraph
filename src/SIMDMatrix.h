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
		inline SIMDMatrix operator+=(const SIMDMatrix& other)
		{
			*this = *this + other;
			return *this;
		}
		
		template <ScalarType T>
		SIMDMatrix operator*(T scalar) noexcept
		{
			float scalarValue = scalar; // implicitly convert to float
			SIMDMatrix result(m_rows, m_cols);

			assert(result.m_stride == m_stride);

			__m256 scalarVec = _mm256_set1_ps(scalarValue);

			for (size_t i = 0; i < result.m_rows; i++)
			{
				for (size_t j = 0; j < result.m_stride; j+=8)
				{
					size_t ix = i * m_stride + j;
					const float* inputPtr = &m_data[ix];
					float* resultPtr = &result.m_data[ix];
					__m256 vecA = _mm256_load_ps(inputPtr);
					__m256 vecRes = _mm256_mul_ps(vecA, scalarVec);
					_mm256_store_ps(resultPtr, vecRes);
				}
			}

			return result;
		}

		template <ScalarType T>
		inline SIMDMatrix operator*=(T scalar) noexcept
		{
			*this = *this * scalar;
			return *this;
		}

		bool isSquare() const { return m_cols == m_rows; }

		float get(size_t row, size_t col) const;
		void set(size_t row, size_t col, float value);

		size_t getRowCount() const { return m_rows; }
		size_t getColCount() const { return m_cols; }

		static SIMDMatrix Identity(size_t size);
		
	private:
		void initialize();

	private:
		size_t m_rows, m_cols, m_stride;
		float* m_data;
	};
}