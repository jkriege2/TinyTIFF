cmake_minimum_required(VERSION 3.10)

find_package(TIFF)
add_library(common_tinytiff_test_lib STATIC )
target_compile_features(common_tinytiff_test_lib PUBLIC cxx_std_11)
target_include_directories(common_tinytiff_test_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_sources(common_tinytiff_test_lib
  PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/tinytiffhighrestimer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test_results.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/testimage_tools.cpp
  PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/tinytiffhighrestimer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/test_results.h
        ${CMAKE_CURRENT_SOURCE_DIR}/testimage_tools.h
)

if (TIFF_FOUND)
	target_sources(common_tinytiff_test_lib
	  PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/libtiff_tools.cpp
	  PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/libtiff_tools.h
	)
	target_compile_definitions(common_tinytiff_test_lib PUBLIC TINYTIFF_TEST_LIBTIFF)
	target_link_libraries(common_tinytiff_test_lib PUBLIC TIFF::TIFF)
	target_include_directories(common_tinytiff_test_lib PUBLIC ${TIFF_INCLUDE_DIRS})
endif()


