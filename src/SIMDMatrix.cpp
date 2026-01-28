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
	m_stride = (m_cols + 7) & ~7;

	size_t bytes = m_rows * m_stride * sizeof(float);
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
	: m_rows(other.m_rows), m_cols(other.m_cols), m_stride(other.m_stride)
{
	size_t bytes = m_rows * m_stride * sizeof(float);
	m_data = (float*)alloc_aligned(bytes, SIMD_ALIGNMENT);
	std::memcpy(m_data, other.m_data, bytes);
}

SIMDMatrix& SIMDMatrix::operator=(const SIMDMatrix& other)
{
	if (this == &other)
		return *this;

	size_t neededBytes = other.m_rows * other.m_stride * sizeof(float);
	size_t currentBytes = m_rows * m_stride * sizeof(float);

	if (neededBytes != currentBytes)
	{
		free_aligned(m_data);
		m_data = (float*)alloc_aligned(neededBytes, SIMD_ALIGNMENT);
	}

	m_rows = other.m_rows;
	m_cols = other.m_cols;
	m_stride = other.m_stride;
	std::memcpy(m_data, other.m_data, neededBytes);

	return *this;
}

SIMDMatrix::SIMDMatrix(SIMDMatrix&& other) noexcept
	: m_rows(other.m_rows),
	m_cols(other.m_cols),
	m_stride(other.m_stride),
	m_data(other.m_data)
{
	other.m_data = nullptr;
	other.m_rows = 0;
	other.m_cols = 0;
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