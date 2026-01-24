add_executable(Digraph "main.cpp" "pch.h" "SIMDMatrix.h" "SIMDMatrix.cpp")
set_target_properties(Digraph PROPERTIES
	OUTPUT_NAME_RELEASE "digraph"
	OUTPUT_NAME_DEBUG "digraph-debug"
	RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin"
	PDB_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin"
	RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin"
	PDB_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin"
)

target_precompile_headers(Digraph
	PRIVATE "pch.h"
)

target_link_libraries(Digraph
	PRIVATE fmt::fmt argparse::argparse nlohmann_json
)

target_include_directories(Digraph
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}"
)

if (MSVC)
	target_compile_options(Digraph PRIVATE "/arch:AVX2")
else()
	target_compile_options(Digraph PRIVATE "-mavx2" "-mfma")
endif()