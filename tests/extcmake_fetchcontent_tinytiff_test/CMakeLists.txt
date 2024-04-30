# set minimum required CMake-Version
cmake_minimum_required(VERSION 3.23)

# set Project name
project(extcmake_fetchcontent_tinytiff_test LANGUAGES CXX)

# include TinyTIFF via FetchContent-API:
# ... first load the FetchContent-API:
include(FetchContent) # once in the project to include the module
# ... now declare TinyTIFF
FetchContent_Declare(TinyTIFF
                     GIT_REPOSITORY https://github.com/jkriege2/TinyTIFF.git
                     # GIT_TAG        3.1.0.0
                     )
                     
# ... finally make TinyTIFF available
FetchContent_MakeAvailable(TinyTIFF)

add_executable(${PROJECT_NAME}
	extcmake_fetchcontent_tinytiff_test.cpp
)

target_link_libraries(${PROJECT_NAME} TinyTIFF::TinyTIFF)
