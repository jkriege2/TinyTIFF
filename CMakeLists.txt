cmake_minimum_required(VERSION 3.10)

######################################################################################################
# Project Name and Version
project(libTinyTIFF VERSION 4.0.0.0)

# set search path for CMake files
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

######################################################################################################
# ensure a build-type is set (Release is default)
include(tinytiff_build_type)

######################################################################################################
# compile options:
if(NOT DEFINED BUILD_SHARED_LIBS)
    option(BUILD_SHARED_LIBS "Build as shared library" ON)
endif()
if(NOT DEFINED TinyTIFF_USE_WINAPI_FOR_FILEIO)
    option(TinyTIFF_USE_WINAPI_FOR_FILEIO "Use WIN-API for File-IO" OFF)
endif()
if(NOT DEFINED TinyTIFF_BUILD_WITH_ADDITIONAL_DEBUG_OUTPUT)
    option(TinyTIFF_BUILD_WITH_ADDITIONAL_DEBUG_OUTPUT "Build with additional debug messages enabled" OFF)
endif()
if(NOT DEFINED TinyTIFF_BUILD_DECORATE_LIBNAMES_WITH_BUILDTYPE)
    option(TinyTIFF_BUILD_DECORATE_LIBNAMES_WITH_BUILDTYPE "If set, the build-type (debug/release/...) is appended to the library name" ON)
endif()
if(NOT DEFINED TinyTIFF_BUILD_TESTS)
    option(TinyTIFF_BUILD_TESTS "Build the tests and examples" ON)
endif()
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
    option(CMAKE_INSTALL_PREFIX "Install directory" ${CMAKE_CURRENT_SOURCE_DIR}/output)
endif()
if(NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    # We're in the root, define additional targets for developers.
    # This prepares the library to be used with CMake's FetchContent
    set(TinyTIFF_BUILD_TESTS OFF)
endif()


######################################################################################################
#evaluate the options above
if (NOT CMAKE_INSTALL_LIBDIR)
    set(CMAKE_INSTALL_LIBDIR "lib")
endif()

if (NOT CMAKE_INSTALL_BINDIR)
    set(CMAKE_INSTALL_BINDIR "bin")
endif()

if (NOT CMAKE_INSTALL_INCLUDEDIR)
    set(CMAKE_INSTALL_INCLUDEDIR "include")
endif()

if (NOT TinyTIFF_DOC_INSTALL_DIR)
    set(TinyTIFF_DOC_INSTALL_DIR "share/doc/")
endif()


# place all DLLs and EXEs in the subdirectory output of the top level directory of the build tree
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)


if(TinyTIFF_BUILD_DECORATE_LIBNAMES_WITH_BUILDTYPE)
    set(TinyTIFF_LIBNAME_ADDITION "_$<CONFIG>")
else()
    set(TinyTIFF_LIBNAME_ADDITION )
endif()

set(tinytiff_default_build_type "Debug")
get_property(TinyTIFF_isMultiConfigGenerator GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(NOT TinyTIFF_isMultiConfigGenerator)
  if(NOT CMAKE_BUILD_TYPE)
      message(WARNING "Setting build type to '${tinytiff_default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE ${tinytiff_default_build_type} CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
  endif()
endif()



######################################################################################################
# check for dependency libs


######################################################################################################
# Project Summary
message("Building TinyTIFF With these features:")
message("  BUILD_SHARED_LIBS                               = ${BUILD_SHARED_LIBS}")
message("  TinyTIFF_USE_WINAPI_FOR_FILEIO                  = ${TinyTIFF_USE_WINAPI_FOR_FILEIO}")
message("  TinyTIFF_BUILD_WITH_ADDITIONAL_DEBUG_OUTPUT     = ${TinyTIFF_BUILD_WITH_ADDITIONAL_DEBUG_OUTPUT}")
message("  TinyTIFF_BUILD_DECORATE_LIBNAMES_WITH_BUILDTYPE = ${TinyTIFF_BUILD_DECORATE_LIBNAMES_WITH_BUILDTYPE}")
message("  TinyTIFF_LIBNAME_ADDITION                       = ${TinyTIFF_LIBNAME_ADDITION}")
message("  TinyTIFF_BUILD_TESTS                            = ${TinyTIFF_BUILD_TESTS}")
message("  CMAKE_INSTALL_PREFIX                            = ${CMAKE_INSTALL_PREFIX}")

######################################################################################################
# now add subdirectories with the library code ...
add_subdirectory(src)

# ... and optionally the examples
if(TinyTIFF_BUILD_TESTS)
    add_subdirectory(tests)
endif()


