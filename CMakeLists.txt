cmake_minimum_required(VERSION 3.0)

######################################################################################################
# Project Name and Version
project(libTinyTIFF LANGUAGES CXX VERSION 2.0.0.0)

# set search path for CMake files
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

######################################################################################################
# compile options:
if(NOT DEFINED TinyTIFF_BUILD_SHARED_LIBS)
    option(TinyTIFF_BUILD_SHARED_LIBS "Build as shared library" ON)
endif()
if(NOT DEFINED TinyTIFF_BUILD_STATIC_LIBS)
    option(TinyTIFF_BUILD_STATIC_LIBS "Build as static library" OFF)
endif()
if(NOT DEFINED TinyTIFF_BUILD_DECORATE_LIBNAMES_WITH_BUILDTYPE)
    option(TinyTIFF_BUILD_DECORATE_LIBNAMES_WITH_BUILDTYPE "If set, the build-type (debug/release/...) is appended to the library name" ON)
endif()
if(NOT DEFINED TinyTIFF_BUILD_TESTS)
    option(TinyTIFF_BUILD_TESTS "Build the tests and examples" ON)
endif()
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
    option(CMAKE_INSTALL_PREFIX "Install directory" ${CMAKE_CURRENT_SOURCE_DIR}/install)
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


######################################################################################################
# check for dependency libs


######################################################################################################
# now add subdirectories with the library code ...
add_subdirectory(src)

# ... and optionally the examples
if(TinyTIFF_BUILD_TESTS)
    add_subdirectory(tests)
endif()



