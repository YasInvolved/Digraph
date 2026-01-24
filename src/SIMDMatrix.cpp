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

SIMDMatrix::SIMDMatrix()
	: m_size(0), m_array(nullptr)
{}

SIMDMatrix::SIMDMatrix(size_t size)
	: m_size(size),
	m_array(new float[size * size])
{
}

SIMDMatrix::~SIMDMatrix()
{
	delete[] m_array;
}

void SIMDMatrix::set(size_t row, size_t col, float value)
{
	assert(m_array != nullptr); // using an unitialized matrix
	assert(row <= m_size && col <= m_size);
	m_array[row * m_size + col] = value;
}

float SIMDMatrix::get(size_t row, size_t col)
{
	assert(m_array != nullptr); // using an unitialized matrix
	assert(row <= m_size && col <= m_size);
	return m_array[row * m_size + col];
}

SIMDMatrix SIMDMatrix::operator+(const SIMDMatrix& m)
{
	SIMDMatrix result(m_size);
	if (m_size != m.m_size)
		throw std::runtime_error("You can't add matrices with different sizes");

	uint64_t n = m_size * m_size;

	size_t i = 0;
	for (; i < n - 8; i += 8)
	{
		__m256 vecA = _mm256_loadu_ps(&m_array[i]);
		__m256 vecB = _mm256_loadu_ps(&m.m_array[i]);

		__m256 sum = _mm256_add_ps(vecA, vecB);

		_mm256_storeu_ps(&result.m_array[i], sum);
	}

	for (; i < n; i++)
		result.m_array[i] = m_array[i] + m.m_array[i];

	return result;
}

SIMDMatrix SIMDMatrix::operator*(const SIMDMatrix& m)
{
	// multiplying 2 square matrices of the same size results in
	// square matrix
	SIMDMatrix result(m_size);

	for (size_t i = 0; i < m_size; i++)
	for (size_t j = 0; j < m_size; j++)
	{
		// set all registers to contain m_array[i * m_size + j]
		__m256 valA = _mm256_set1_ps(m_array[i * m_size + j]);

		size_t k = 0;
		for (; k <= m_size - 8; k += 8)
		{
			__m256 vecC = _mm256_loadu_ps(&result.m_array[i * m_size + k]);
			__m256 vecB = _mm256_loadu_ps(&m.m_array[j * m_size + k]);

			vecC = _mm256_fmadd_ps(valA, vecB, vecC);

			_mm256_storeu_ps(&result.m_array[i * m_size + k], vecC);
		}

		for (; k < m_size; k++) {
			result.m_array[i * m_size + k] += m_array[i * m_size + j] * m_array[j * m_size + k];
		}
	}

	return result;
}

SIMDMatrix SIMDMatrix::Identity(size_t size)
{
	SIMDMatrix m(size);

	for (size_t i = 0; i < size; i++)
		m.set(i, i, 1.0f);

	return m;
}