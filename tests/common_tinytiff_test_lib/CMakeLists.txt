cmake_minimum_required(VERSION 3.10)

find_package(TIFF)
add_library(common_tinytiff_test_lib STATIC )
target_compile_features(common_tinytiff_test_lib PUBLIC cxx_std_11)
target_include_directories(common_tinytiff_test_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_sources(common_tinytiff_test_lib
  PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/tinytiffhighrestimer.cpp
  PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/tinytiffhighrestimer.h
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



# Installation
include(GNUInstallDirs)
install(TARGETS common_tinytiff_test_lib EXPORT common_tinytiff_test_lib_TARGETS
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
