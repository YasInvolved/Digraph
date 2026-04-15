# MIT License
# 
# Copyright(c) 2026 Jakub B?czyk
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files(the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions :
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

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
gtest_discover_tests(simdmatrix_test 
	PROPERTIES TIMEOUT 900
)