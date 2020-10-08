cmake_minimum_required(VERSION 3.0)

set(EXAMPLE_NAME tinytiffwriter_speedtest)

add_executable(${EXAMPLE_NAME}
	tinytiffwriter_speedtest.cpp
)
if(TinyTIFF_BUILD_STATIC_LIBS)
    target_link_libraries(${EXAMPLE_NAME} TinyTIFF)
elseif(TinyTIFF_BUILD_SHARED_LIBS)
    target_link_libraries(${EXAMPLE_NAME} TinyTIFFShared)
endif()
target_link_libraries(${EXAMPLE_NAME} common_tinytiff_test_lib)

add_test(NAME ${EXAMPLE_NAME} COMMAND ${EXAMPLE_NAME} WORKING_DIRECTORY ${CMAKE_INSTALL_BINDIR})

# Installation
install(TARGETS ${EXAMPLE_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
