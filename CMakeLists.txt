cmake_minimum_required(VERSION 3.21)
project(Digraph)

set(CMAKE_CXX_STANDARD 20 CACHE STRING "" FORCE)
set(CMAKE_CXX_STANDARD_REQUIRED True CACHE BOOL "" FORCE)

option(DIGRAPH_BUILD_TESTS "Build Digraph Tests" ON)

add_subdirectory(3rdparty)
add_subdirectory(src)

if (DIGRAPH_BUILD_TESTS)
	enable_testing()
	add_subdirectory(test)
endif()