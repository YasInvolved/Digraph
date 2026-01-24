add_executable(Digraph "main.cpp" "pch.h")
set_target_properties(Digraph PROPERTIES
	OUTPUT_NAME_RELEASE "digraph"
	OUTPUT_NAME_DEBUG "digraph-debug"
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