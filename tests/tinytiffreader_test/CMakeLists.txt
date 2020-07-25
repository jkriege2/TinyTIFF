cmake_minimum_required(VERSION 3.0)

set(EXAMPLE_NAME tinytiffreader_test)

add_executable(${EXAMPLE_NAME}
	tinytiffreader_test.cpp
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
install(FILES ${CMAKE_CURRENT_LIST_DIR}/cell.tif DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CMAKE_CURRENT_LIST_DIR}/circuit.tif DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CMAKE_CURRENT_LIST_DIR}/galaxy.tif DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CMAKE_CURRENT_LIST_DIR}/mri.tif DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CMAKE_CURRENT_LIST_DIR}/multi-channel-time-series.ome.tif DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CMAKE_CURRENT_LIST_DIR}/test16m_imagej.tif DESTINATION ${CMAKE_INSTALL_BINDIR})
