cmake_minimum_required(VERSION 3.10)

set(EXAMPLE_NAME tinytiffwriter_test)

add_executable(${EXAMPLE_NAME}
    tinytiffwriter_test.cpp
)
target_link_libraries(${EXAMPLE_NAME} TinyTIFF)
target_link_libraries(${EXAMPLE_NAME} common_tinytiff_test_lib)

add_test(NAME ${EXAMPLE_NAME} COMMAND ${EXAMPLE_NAME} WORKING_DIRECTORY ${CMAKE_INSTALL_BINDIR})

# Installation
install(TARGETS ${EXAMPLE_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
