# create a dummy library out of matrix files
add_library(simdmatrix_lib STATIC ../src/SIMDMatrix.cpp ../src/SIMDMatrix.h)
target_include_directories(simdmatrix_lib
	PUBLIC ../src
)

target_precompile_headers(simdmatrix_lib PUBLIC matlib_pch.h)

if (MSVC)
	target_compile_options(simdmatrix_lib PUBLIC "/arch:AVX2")
else()
	target_compile_options(simdmatrix_lib PUBLIC "-mavx2" "-mfma")
endif()

add_executable(simdmatrix_test test_matrix.cpp)
target_link_libraries(simdmatrix_test
	PRIVATE gtest_main simdmatrix_lib
)

include(GoogleTest)
gtest_discover_tests(simdmatrix_test)