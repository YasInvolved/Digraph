#include "SIMDMatrix.h"

#ifdef _MSC_VER
	#include <intrin.h>
#else
	#include <cpuid.h>
#endif

bool isAVX2Supported()
{
	std::vector<int> cpuInfo(4);

#ifdef _MSC_VER
	__cpuid(cpuInfo.data(), 0);
#else
	__get_cpuid(0, (unsigned int*)&cpuInfo[0], (unsigned int*)&cpuInfo[1],
		(unsigned int*)&cpuInfo[2], (unsigned int*)&cpuInfo[3]);
#endif

	int numIds = cpuInfo[0];
	if (numIds < 7) return false;

#ifdef _MSC_VER
	__cpuidex(cpuInfo.data(), 7, 0);
#else
	__cpuid_count(7, 0, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
#endif

	// check if bit 5 of EBX
	return (cpuInfo[1] & 0x20) != 0;
}

static void* alloc_aligned(size_t size, size_t alignment)
{
	size_t safeSize = (size + alignment - 1) & ~(alignment - 1);

#ifdef _MSC_VER
	return _aligned_malloc(safeSize, alignment);
#else
	return std::aligned_alloc(alignment, safeSize);
#endif
}

static void free_aligned(void* ptr)
{
#ifdef _WIN32
	_aligned_free(ptr);
#else
	std::free(ptr);
#endif
}

using namespace linear_algebra;

static constexpr size_t SIMD_ALIGNMENT = 32ull;

SIMDMatrix::SIMDMatrix(size_t rc)
	: m_rows(rc), m_cols(rc)
{
	initialize();
}

SIMDMatrix::SIMDMatrix(size_t rows, size_t cols)
	: m_rows(rows), m_cols(cols)
{
	initialize();
}

void SIMDMatrix::initialize()
{
	// rows aligned to 4 and cols aligned to 8
	m_stride = (m_cols + 7) & ~7;
	m_strideRow = (m_rows + 3) & ~3;

	size_t bytes = m_strideRow * m_stride * sizeof(float);
	m_data = (float*)alloc_aligned(bytes, SIMD_ALIGNMENT);

	if (!m_data)
		throw std::bad_alloc();

	std::memset(m_data, 0, bytes);
}

SIMDMatrix::~SIMDMatrix()
{
	if (!m_data)
		return;

	free_aligned(m_data);
	m_data = nullptr;
}

SIMDMatrix::SIMDMatrix(const SIMDMatrix& other)
	: m_rows(other.m_rows), m_cols(other.m_cols), m_stride(other.m_stride), m_strideRow(other.m_strideRow)
{
	size_t bytes = m_strideRow * m_stride * sizeof(float);
	m_data = (float*)alloc_aligned(bytes, SIMD_ALIGNMENT);
	std::memcpy(m_data, other.m_data, bytes);
}

SIMDMatrix& SIMDMatrix::operator=(const SIMDMatrix& other)
{
	if (this == &other)
		return *this;

	size_t neededBytes = other.m_strideRow * other.m_stride * sizeof(float);
	size_t currentBytes = m_strideRow * m_stride * sizeof(float);

	if (neededBytes != currentBytes)
	{
		free_aligned(m_data);
		m_data = (float*)alloc_aligned(neededBytes, SIMD_ALIGNMENT);
	}

	m_rows = other.m_rows;
	m_cols = other.m_cols;
	m_stride = other.m_stride;
	m_strideRow = other.m_strideRow;
	std::memcpy(m_data, other.m_data, neededBytes);

	return *this;
}

SIMDMatrix::SIMDMatrix(SIMDMatrix&& other) noexcept
	: m_rows(other.m_rows),
	m_cols(other.m_cols),
	m_stride(other.m_stride),
	m_strideRow(other.m_strideRow),
	m_data(other.m_data)
{
	other.m_data = nullptr;
	other.m_rows = 0;
	other.m_cols = 0;
	other.m_stride = 0;
	other.m_strideRow = 0;
}

SIMDMatrix& SIMDMatrix::operator=(SIMDMatrix&& other) noexcept
{
	if (this == &other)
		return *this;

	free_aligned(m_data);
	m_data = other.m_data;
	m_rows = other.m_rows;
	m_cols = other.m_cols;
	m_stride = other.m_stride;
	m_strideRow = other.m_strideRow;

	other.m_data = nullptr;
	other.m_rows = 0;
	other.m_cols = 0;
	other.m_stride = 0;
	return *this;
}

SIMDMatrix SIMDMatrix::operator+(const SIMDMatrix& other)
{
	if (m_rows != other.m_rows || m_cols != other.m_cols)
		throw std::runtime_error("Attempting to add 2 different matrices");

	assert(m_stride == other.m_stride);
	assert((m_stride * sizeof(float)) % SIMD_ALIGNMENT == 0);

	SIMDMatrix mat(m_rows, m_cols);

	for (size_t i = 0; i < m_rows; i++)
	{
		for (size_t j = 0; j < m_stride; j+=8)
		{
			size_t ix = i * m_stride + j;
			const float* inA = &m_data[ix];
			const float* inB = &other.m_data[ix];
			float* out = &mat.m_data[ix];

			__m256 vecA = _mm256_load_ps(inA);
			__m256 vecB = _mm256_load_ps(inB);
			__m256 vecRes = _mm256_add_ps(vecA, vecB);

			_mm256_store_ps(out, vecRes);
		}
	}

	return mat;
}

float SIMDMatrix::get(size_t row, size_t col) const
{
	if (row >= m_rows || col >= m_cols) {
		throw std::out_of_range("Matrix index out of bounds");
	}

	return m_data[row * m_stride + col];
}

void SIMDMatrix::set(size_t row, size_t col, float value)
{
	if (row >= m_rows || col >= m_cols)
		throw std::out_of_range("Matrix index out of bounds");

	m_data[row * m_stride + col] = value;
}

SIMDMatrix SIMDMatrix::Identity(size_t size)
{
	SIMDMatrix mat(size);
	
	size_t limit = std::min(mat.m_rows, mat.m_cols);
	for (size_t i = 0; i < limit; i++)
		mat.m_data[i * mat.m_stride + i] = 1.0f;

	return mat;
}

SIMDMatrix linear_algebra::operator*(const SIMDMatrix& lhs, const SIMDMatrix& rhs)
{
	if (lhs.m_cols != rhs.m_rows)
		throw std::invalid_argument("Invalid argument: Multiplied matrix column count must be equal to the row count of matrix multiplied by");

	SIMDMatrix result(lhs.m_rows, rhs.m_cols);

	for (size_t i = 0; i < result.m_rows; i += 4)
		for (size_t j = 0; j < result.m_stride; j += 8)
		{
			__m256 c0 = _mm256_setzero_ps();
			__m256 c1 = _mm256_setzero_ps();
			__m256 c2 = _mm256_setzero_ps();
			__m256 c3 = _mm256_setzero_ps();

			for (size_t k = 0; k < lhs.m_cols; k++)
			{
				__m256 rowRhs = _mm256_load_ps(&rhs.m_data[k * rhs.m_stride + j]);

				__m256 a0 = _mm256_set1_ps(lhs.m_data[i * lhs.m_stride + k]);
				c0 = _mm256_fmadd_ps(a0, rowRhs, c0);

				__m256 a1 = _mm256_set1_ps(lhs.m_data[(i + 1) * lhs.m_stride + k]);
				c1 = _mm256_fmadd_ps(a1, rowRhs, c1);

				__m256 a2 = _mm256_set1_ps(lhs.m_data[(i + 2) * lhs.m_stride + k]);
				c2 = _mm256_fmadd_ps(a2, rowRhs, c2);

				__m256 a3 = _mm256_set1_ps(lhs.m_data[(i + 3) * lhs.m_stride + k]);
				c3 = _mm256_fmadd_ps(a3, rowRhs, c3);
			}

			_mm256_store_ps(&result.m_data[i * result.m_stride + j], c0);
			_mm256_store_ps(&result.m_data[(i + 1) * result.m_stride + j], c1);
			_mm256_store_ps(&result.m_data[(i + 2) * result.m_stride + j], c2);
			_mm256_store_ps(&result.m_data[(i + 3) * result.m_stride + j], c3);
		}

	_mm256_zeroupper();
	return result;
}