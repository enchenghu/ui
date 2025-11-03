cmake_minimum_required(VERSION 3.14)

project(AWBCalibrationDLL LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(M1_AWBCalibTool)

find_package(Qt6 COMPONENTS Core Widgets Charts Network REQUIRED)

add_library(AWBCalibrationDLL SHARED
  imagewidget.cpp
  imagewidget.h
  AWBCalibrationDLL_global.h
  awbcalibrationdll.cpp
  awbcalibrationdll.h
  mainwindow.h mainwindow.cpp mainwindow.ui
  AWBCalibrationDef.h
  calibration.h calibration.cpp calibration.ui
  chartviewcalibration.h chartviewcalibration.cpp
  awbburn.h awbburn.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE M1_AWBCalibTool)

target_link_libraries(AWBCalibrationDLL PRIVATE Qt6::Widgets Qt6::Charts M1_AWBTool)

target_compile_definitions(AWBCalibrationDLL PRIVATE AWBCALIBRATIONDLL_LIBRARY)

set_target_properties(AWBCalibrationDLL PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)