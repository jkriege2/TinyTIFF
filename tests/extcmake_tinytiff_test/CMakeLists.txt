project(extcmake_tinytiff_test CXX)
cmake_minimum_required(VERSION 3.0)

set(EXAMPLE_NAME extcmake_tinytiff_test)

find_package(TinyTIFF REQUIRED)

add_executable(${EXAMPLE_NAME}
	extcmake_tinytiff_test.cpp
)
target_link_libraries(${EXAMPLE_NAME} TinyTIFF)
