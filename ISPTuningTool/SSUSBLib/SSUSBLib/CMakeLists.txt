cmake_minimum_required(VERSION 3.5.0)

project(SSUSBLib VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SSUSBLIB_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)
add_library(SSUSBLib SHARED ${SSUSBLIB_SOURCES})
target_compile_definitions(SSUSBLib PRIVATE SSUSBLIB_EXPORTS)
target_link_directories(SSUSBLib PRIVATE ${PROJECT_SOURCE_DIR}/lib/${CMAKE_GENERATOR_PLATFORM})
target_include_directories(SSUSBLib PRIVATE ${PROJECT_SOURCE_DIR}/include)

find_program(NUGET_EXE NAMES nuget)
exec_program(${NUGET_EXE}
    ARGS install "libusb-1.0" -Version 1.0.21.11156 -ExcludeVersion -OutputDirectory ${CMAKE_BINARY_DIR}/packages)
target_link_libraries(SSUSBLib PRIVATE ${CMAKE_BINARY_DIR}/packages/libusb-1.0/build/native/libusb-1.0.targets)

target_include_directories(SSUSBLib PRIVATE ${CMAKE_BINARY_DIR}/packages/libusb-1.0/build/native/include)