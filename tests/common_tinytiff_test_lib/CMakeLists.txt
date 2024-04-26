cmake_minimum_required(VERSION 3.10)

find_package(TIFF)

if (NOT TARGET TIFF::TIFF)
    include(FetchContent)
    message(STATUS "+++ using FetchContent to retreave libTIFF")
    FetchContent_Declare(
        TIFF
        GIT_REPOSITORY https://gitlab.com/libtiff/libtiff.git
        GIT_TAG v4.6.0
        GIT_SHALLOW TRUE
    )

    set(tiff-install-default OFF)
    set(tiff-tests OFF)
    FetchContent_MakeAvailable(TIFF)

    set_target_properties(tiff tiffxx
    PROPERTIES
        C_VISIBILITY_PRESET hidden
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN YES)

    add_library(TIFF::TIFF INTERFACE IMPORTED GLOBAL)
    target_link_libraries(TIFF::TIFF INTERFACE tiffxx)
    set(TIFF_FOUND TRUE)
    install(TARGETS tiffxx tiff
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
endif()

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


