cmake_minimum_required(VERSION 3.10)

set(lib_name "TinyTIFF")
set(libsh_name "TinyTIFFShared")

# Set up source files
set(SOURCES
    tinytiffreader.c
    tinytiffwriter.c
)
set_property(SOURCE ${SOURCES} PROPERTY LANGUAGE "C")
set(HEADERS_C
    tinytiffreader.h
    tinytiffwriter.h
)
set(HEADERS_CXX
    tinytiffreader.hxx
)
set(HEADERS
    ${HEADERS_C}
    ${HEADERS_CXX}
)


include(CMakePackageConfigHelpers)
include(GenerateExportHeader)
include(CheckSymbolExists)
check_symbol_exists(strcpy_s "string.h" HAVE_STRCPY_S)
check_symbol_exists(fopen_s "stdio.h" HAVE_FOPEN_S)
check_symbol_exists(fread_s "stdio.h" HAVE_FREAD_S)
check_symbol_exists(sprintf_s "stdio.h" HAVE_SPRINTF_S)
check_symbol_exists(strcat_s "string.h" HAVE_STRCAT_S)


if(TinyTIFF_BUILD_SHARED_LIBS)
    add_library(${libsh_name} SHARED  ${SOURCES} ${HEADERS})
    set_property(TARGET ${libsh_name} PROPERTY VERSION "${PROJECT_VERSION}")
    set_property(TARGET ${libsh_name} PROPERTY OUTPUT_NAME  "${libsh_name}${TinyTIFF_LIBNAME_ADDITION}")
    target_include_directories(${libsh_name} INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>  
    )
    set_target_properties(${libsh_name} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS "ON")

    generate_export_header(${libsh_name} BASE_NAME tinytiff)
    target_include_directories(${libsh_name} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
    target_include_directories(${libsh_name} INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
    target_compile_definitions(${libsh_name} PRIVATE ${libsh_name}_EXPORTS)
    if (TinyTIFF_USE_WINAPI_FOR_FILEIO)
        target_compile_definitions(${libsh_name} PRIVATE TINYTIFF_USE_WINAPI_FOR_FILEIO)
    endif()
    if (HAVE_STRCPY_S)
        target_compile_definitions(${libsh_name} PRIVATE HAVE_STRCPY_S)
    endif()
    if (HAVE_FOPEN_S)
        target_compile_definitions(${libsh_name} PRIVATE HAVE_FOPEN_S)
    endif()
    if (HAVE_SPRINTF_S)
        target_compile_definitions(${libsh_name} PRIVATE HAVE_SPRINTF_S)
    endif()
    if (HAVE_STRCAT_S)
        target_compile_definitions(${libsh_name} PRIVATE HAVE_STRCAT_S)
    endif()
    if (HAVE_FREAD_S)
        target_compile_definitions(${libsh_name} PRIVATE HAVE_FREAD_S)
    endif()
endif()

if(TinyTIFF_BUILD_STATIC_LIBS)
    add_library(${lib_name} STATIC ${SOURCES} ${HEADERS})
    set_property(TARGET ${lib_name} PROPERTY VERSION "${PROJECT_VERSION}")
    set_property(TARGET ${lib_name} PROPERTY OUTPUT_NAME "${lib_name}${TinyTIFF_LIBNAME_ADDITION}")
    target_include_directories(${lib_name} INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>  
    )

    if(NOT TinyTIFF_BUILD_SHARED_LIBS)
      generate_export_header(${lib_name} BASE_NAME tinytiff)
    endif()
    target_include_directories(${lib_name} INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
    target_include_directories(${lib_name} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
    target_include_directories(${lib_name} INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
    target_compile_definitions(${lib_name} PUBLIC TINYTIFF_STATIC_DEFINE)
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
endif()




# Installation
include(GNUInstallDirs)
if(TinyTIFF_BUILD_SHARED_LIBS)
        write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${libsh_name}Version.cmake
                                     VERSION ${PROJECT_VERSION}
                                     COMPATIBILITY AnyNewerVersion )
        install(TARGETS ${libsh_name} EXPORT ${libsh_name}_TARGETS
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	)
	install(EXPORT ${libsh_name}_TARGETS
		FILE ${libsh_name}Config.cmake 
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${libsh_name}
	)
	install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${libsh_name}Version.cmake" 
			DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${libsh_name}  )

endif(TinyTIFF_BUILD_SHARED_LIBS)

if(TinyTIFF_BUILD_STATIC_LIBS)
        write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${lib_name}Version.cmake
                                     VERSION ${PROJECT_VERSION}
                                     COMPATIBILITY AnyNewerVersion )
        install(TARGETS ${lib_name} EXPORT ${lib_name}_TARGETS
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
		INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	)
	install(EXPORT ${lib_name}_TARGETS
		FILE ${lib_name}Config.cmake 
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${lib_name}
	)
	install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${lib_name}Version.cmake" 
			DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${lib_name}  )

endif(TinyTIFF_BUILD_STATIC_LIBS)

install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT Headers)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/tinytiff_export.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} )
configure_file(${CMAKE_SOURCE_DIR}/readme.txt.in ${CMAKE_CURRENT_BINARY_DIR}/readme.txt @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/readme.txt" DESTINATION "${TinyTIFF_DOC_INSTALL_DIR}/${lib_name}/" )
install(FILES "${CMAKE_SOURCE_DIR}/LICENSE" DESTINATION "${TinyTIFF_DOC_INSTALL_DIR}/${lib_name}/" )
install(FILES "${CMAKE_SOURCE_DIR}/README.md" DESTINATION "${TinyTIFF_DOC_INSTALL_DIR}/${lib_name}/" )


