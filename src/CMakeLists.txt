cmake_minimum_required(VERSION 3.10)

set(lib_name "TinyTIFF")
set(lib_nameXX "TinyTIFFXX")


include(CMakePackageConfigHelpers)
include(GenerateExportHeader)
include(CheckSymbolExists)


message(STATUS "Resolving GIT Version")

set(_build_version "unknown")

find_package(Git)
if(GIT_FOUND)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    OUTPUT_VARIABLE TINYTIFF_GITVERSION
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  message( STATUS "GIT hash: ${TINYTIFF_GITVERSION} (from ${PROJECT_SOURCE_DIR})")
else()
  message(STATUS "GIT not found")
endif()


string(TIMESTAMP TINYTIFF_COMPILETIME "%Y-%m-%d %H:%M:%S")
configure_file(${CMAKE_CURRENT_LIST_DIR}/tinytiff_version.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/tinytiff_version.h)


check_symbol_exists(strcpy_s "string.h" HAVE_STRCPY_S)
check_symbol_exists(fopen_s "stdio.h" HAVE_FOPEN_S)
check_symbol_exists(fread_s "stdio.h" HAVE_FREAD_S)
check_symbol_exists(sprintf_s "stdio.h" HAVE_SPRINTF_S)
check_symbol_exists(strcat_s "string.h" HAVE_STRCAT_S)
check_symbol_exists(memcpy_s "string.h" HAVE_MEMCPY_S)
check_symbol_exists(memset_s "string.h" HAVE_MEMSET_S)
check_symbol_exists(strnlen_s "string.h" HAVE_STRNLEN_S)




# add libraries for C and C++ separately
add_library(${lib_name} )
set_property(TARGET ${lib_name} PROPERTY VERSION "${PROJECT_VERSION}")
set_property(TARGET ${lib_name} PROPERTY OUTPUT_NAME "${lib_name}${TinyTIFF_LIBNAME_ADDITION}")
target_include_directories(${lib_name}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
if (BUILD_SHARED_LIBS)
    set_target_properties(${lib_name} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS "ON")
else()
    target_compile_definitions(${lib_name} PUBLIC TINYTIFF_STATIC_DEFINE)
endif(BUILD_SHARED_LIBS)



if (TinyTIFF_BUILD_WITH_ADDITIONAL_DEBUG_OUTPUT)
    target_compile_definitions(${lib_name} PRIVATE TINYTIFF_ADDITIONAL_DEBUG_MESSAGES)
endif()
if (TinyTIFF_USE_WINAPI_FOR_FILEIO)
    target_compile_definitions(${lib_name} PRIVATE TINYTIFF_USE_WINAPI_FOR_FILEIO)
endif()
if (HAVE_STRCPY_S)
    target_compile_definitions(${lib_name} PRIVATE HAVE_STRCPY_S)
endif()
if (HAVE_FOPEN_S)
    target_compile_definitions(${lib_name} PRIVATE HAVE_FOPEN_S)
endif()
if (HAVE_SPRINTF_S)
    target_compile_definitions(${lib_name} PRIVATE HAVE_SPRINTF_S)
endif()
if (HAVE_STRCAT_S)
    target_compile_definitions(${lib_name} PRIVATE HAVE_STRCAT_S)
endif()
if (HAVE_FREAD_S)
    target_compile_definitions(${lib_name} PRIVATE HAVE_FREAD_S)
endif()
if (HAVE_MEMCPY_S)
    target_compile_definitions(${lib_name} PRIVATE HAVE_MEMCPY_S)
endif()
if (HAVE_MEMSET_S)
    target_compile_definitions(${lib_name} PRIVATE HAVE_MEMSET_S)
endif()
if (HAVE_STRNLEN_S)
    target_compile_definitions(${lib_name} PRIVATE HAVE_STRNLEN_S)
endif()


add_library(${lib_nameXX} INTERFACE)
target_link_libraries(${lib_nameXX} INTERFACE ${lib_name})
# ... add an alias with the correct namespace
add_library(TinyTIFF::${lib_name} ALIAS ${lib_name})
add_library(TinyTIFF::${lib_nameXX} ALIAS ${lib_nameXX})





# Set up source files
set_property(SOURCE tinytiffreader.c tinytiffreader.h PROPERTY LANGUAGE "C")
set_property(SOURCE tinytiffwriter.c tinytiffwriter.h PROPERTY LANGUAGE "C")
set_property(SOURCE tiff_definitions_internal.h tinytiff_defs.h PROPERTY LANGUAGE "C")
target_sources(${lib_name} PRIVATE
    tinytiff_ctools_internal.c
    tinytiffreader.c
    tinytiffwriter.c
)


target_sources(${lib_name} PRIVATE FILE_SET privateHEADERS TYPE HEADERS
    FILES
    tiff_definitions_internal.h
    tinytiff_ctools_internal.h
)

target_sources(${lib_name} PUBLIC FILE_SET HEADERS TYPE HEADERS
    FILES
        tinytiff_defs.h
        tinytiffreader.h
        tinytiffwriter.h
)


set_property(SOURCE tinytiffreader.hxx tinytiff_tools.hxx PROPERTY LANGUAGE "CXX")
target_sources(${lib_nameXX} PUBLIC FILE_SET HEADERS TYPE HEADERS
    FILES
        tinytiffreader.hxx
        tinytiff_tools.hxx
)


generate_export_header(${lib_name} BASE_NAME tinytiff)




# Installation
include(GNUInstallDirs)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${lib_name}Version.cmake
                             VERSION ${PROJECT_VERSION}
                             COMPATIBILITY AnyNewerVersion )

install(TARGETS ${lib_name}
        EXPORT ${lib_name}_TARGETS
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
install(EXPORT ${lib_name}_TARGETS
        FILE ${lib_name}Config.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${lib_name}
        NAMESPACE TinyTIFF::
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${lib_name}Version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${lib_name}  )

write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${lib_nameXX}Version.cmake
                             VERSION ${PROJECT_VERSION}
                             COMPATIBILITY AnyNewerVersion )

install(TARGETS ${lib_nameXX}
        EXPORT ${lib_nameXX}_TARGETS
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
install(EXPORT ${lib_nameXX}_TARGETS
        FILE ${lib_nameXX}Config.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${lib_nameXX}
        NAMESPACE TinyTIFF::
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${lib_nameXX}Version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${lib_nameXX}  )


install(FILES ${CMAKE_CURRENT_BINARY_DIR}/tinytiff_export.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} )
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/tinytiff_version.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} )
configure_file(${PROJECT_SOURCE_DIR}/readme.txt.in ${CMAKE_CURRENT_BINARY_DIR}/readme.txt @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/readme.txt" DESTINATION "${TinyTIFF_DOC_INSTALL_DIR}/${lib_name}/" )
install(FILES "${PROJECT_SOURCE_DIR}/LICENSE" DESTINATION "${TinyTIFF_DOC_INSTALL_DIR}/${lib_name}/" )
install(FILES "${PROJECT_SOURCE_DIR}/README.md" DESTINATION "${TinyTIFF_DOC_INSTALL_DIR}/${lib_name}/" )






