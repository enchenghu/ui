cmake_minimum_required(VERSION 3.5)
include(CMakeCommon/Common.cmake)
set_exe_build_option()
project(ISPTuningToolM1 VERSION 0.1 LANGUAGES CXX)
set_cmake_option()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(CMAKE_GENERATOR_PLATFORM x64)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(CMAKE_GENERATOR_PLATFORM x86)
endif()
#set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG")
add_subdirectory(DatabaseOrm)
add_subdirectory(M1FlashLib)
add_subdirectory(SSUSBLib/SSUSBLib)
add_subdirectory(AWBCalibrationDLL)

find_package(Qt6 COMPONENTS Core Widgets Charts Network DataVisualization REQUIRED)

find_package("spdlog")

file(GLOB COMMON_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/Common/Others/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Common/Others/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Common/UI/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Common/UI/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Common/UI/*.ui"
)

file(GLOB FPNC_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/FPNC/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/FPNC/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/FPNC/*.ui"
)

file(GLOB DEIR_SRC
    "${CMAKE_CURRENT_SOURCE_DIR}/Deir/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Deir/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Deir/*.ui"
)

auto_add_executable("${DATABASEORM_SRC};${COMMON_SRC};${FPNC_SRC};${DEIR_SRC}")

target_sources(ISPTuningToolM1
  PRIVATE
    qregister.h qregister.cpp qregister.ui
    qpipiclient.h
    qpipiclient.cpp
    qgpioset.h
    qgpioset.cpp
    qtransfermanager.h
)

add_compile_definitions("-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE")
add_compile_definitions("-D/openmp")

if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel"))
    set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE ON)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE M1FlashLib SSUSBLib/SSUSBLib AWBCalibrationDLL ${CMAKE_CURRENT_SOURCE_DIR}/third_party/include)

target_link_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/third_party/lib)
# 链接库
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    Qt6::Charts
    Qt6::Network
    Qt6::DataVisualization
    SSUSBLib
    spdlog::spdlog_header_only
    sqlite_orm::sqlite_orm
    sqlcipher::sqlcipher
    M1FlashLib
    AWBCalibrationDLL
    LicenseLib)

set_target_properties(${PROJECT_NAME} PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

copy_db_to_exe_dir()
copy_file_to_exe_dir("${CMAKE_CURRENT_LIST_DIR}/config")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/bin/LicenseLib.dll
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

# set chcp before build
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND chcp)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()


deployqt()