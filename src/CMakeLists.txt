add_executable(GraphMatrix "main.cpp" "pch.h")

target_precompile_headers(GraphMatrix 
	PRIVATE "pch.h"
)

target_link_libraries(GraphMatrix 
	PRIVATE fmt::fmt argparse::argparse
)

target_include_directories(GraphMatrix 
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}"
)