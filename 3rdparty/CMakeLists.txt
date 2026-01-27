add_subdirectory(fmt)
add_subdirectory(argparse)
add_subdirectory(json)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(googletest)