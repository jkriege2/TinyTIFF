project(extcmake_tinytiff_test CXX)
cmake_minimum_required(VERSION 3.10)

set(EXAMPLE_NAME extcmake_tinytiff_test)

find_package(TinyTIFFShared REQUIRED)
set(libTinyTiff "TinyTIFFShared")
if (NOT TinyTIFFShared_FOUND)
	find_package(TinyTIFFStatic REQUIRED)
	set(libTinyTiff "TinyTIFFStatic")
endif()

add_executable(${EXAMPLE_NAME}
	extcmake_tinytiff_test.cpp
)
target_link_libraries(${EXAMPLE_NAME} ${libTinyTiff})
