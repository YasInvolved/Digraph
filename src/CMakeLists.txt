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

add_executable(Digraph "main.cpp" "pch.h" "SIMDMatrix.h" "SIMDMatrix.cpp")
set_target_properties(Digraph PROPERTIES
	OUTPUT_NAME_RELEASE "digraph"
	OUTPUT_NAME_DEBUG "digraph-debug"
	RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin"
	PDB_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin"
	RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin"
	PDB_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin"
)

if (MSVC)
	target_compile_options(Digraph PRIVATE "/arch:AVX2" "$<$<CONFIG:Release>:/Z7>")
	target_link_options(Digraph PRIVATE "$<$<CONFIG:Release>:/DEBUG:NONE>")
else()
	target_compile_options(Digraph PRIVATE "-mavx2" "-mfma" "$<$<CONFIG:Release>:-g0>")
	
	if (APPLE)
		target_link_options(Digraph PRIVATE "$<$<CONFIG:Release>:-Wl,-x>")
	else()
		target_link_options(Digraph PRIVATE "$<$<CONFIG:Release>:-s>")
	endif()
endif()

target_precompile_headers(Digraph
	PRIVATE "pch.h"
)

target_link_libraries(Digraph
	PRIVATE fmt::fmt argparse::argparse nlohmann_json
)

target_include_directories(Digraph
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}"
)