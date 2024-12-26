cmake_minimum_required(VERSION 3.5)

project(pointview)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# init_qt: Let's do the CMake job for us
set(CMAKE_AUTOMOC ON) # For meta object compiler
set(CMAKE_AUTORCC ON) # Resource files
set(CMAKE_AUTOUIC ON) # UI files


# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Find the QtWidgets library
find_package(Qt6 QUIET COMPONENTS Concurrent OpenGL Widgets)
if (NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Concurrent OpenGL Widgets Network PrintSupport Multimedia MultimediaWidgets REQUIRED)
    set(QTX Qt5)
else ()
    set(QTX Qt6)
endif ()

find_package(VTK REQUIRED)
find_package(PCL REQUIRED COMPONENTS visualization)
find_package(yaml-cpp REQUIRED)
find_package(Boost REQUIRED)
find_package(CURL REQUIRED)
find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(glog REQUIRED libglog)

message("************************************************************************************")
message("  Qt version: ${QTX}, VTK version: ${VTK_MAJOR_VERSION}.${VTK_MINOR_VERSION}, PCL version: ${PCL_VERSION}")
message("************************************************************************************")

include_directories(${PCL_INCLUDE_DIRS})
add_definitions(${PCL_DEFINITIONS})

include_directories(src)
# utils
add_subdirectory(src/utils)

# tools
add_subdirectory(src/tools)

# devices
add_subdirectory(src/devices)

# app
set(project_SOURCES src/main.cpp src/mainwindow.cpp src/device_factory.cpp src/app_control.cpp)

add_executable(${PROJECT_NAME} ${project_SOURCES} resource/icon.qrc)

target_link_libraries(${PROJECT_NAME}
        ${PCL_LIBRARIES}
        ${QTX}::Widgets
        utils
        tools
        fakelidar
        hesai_pandar128
        hesai_qt64
        hesai_qt128
        xlidar
        xradar
        blidar
        udp_replayer
        xrt_replayer
        hesai_ft120
        xradar_2d
        ${glog_LIBRARIES}
        )

# for VTK7
if (${VTK_MAJOR_VERSION} LESS 9)
    target_link_libraries(${PROJECT_NAME} vtkGUISupportQt)
endif ()
