cmake_minimum_required(VERSION 3.5)
project(M1_AWBTool VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


file(GLOB PROJECT_SOURCES
    "src/*.cpp"
    "src/*.h"
)
if (MSVC)
	add_compile_options("/source-charset:utf-8")
endif()
add_library(M1_AWBTool SHARED ${PROJECT_SOURCES})
target_compile_definitions(M1_AWBTool PRIVATE M1_AWBTool_LIBRARY)

