cmake_minimum_required(VERSION 2.8.3)
project(autox_ui)

## Compile as C++11, supported in ROS Kinetic and newer
add_compile_options(-std=c++17)
add_definitions(-w)
## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  rviz
  roscpp
  rospy
  sensor_msgs
  pcl_ros
  pcl_conversions
  arbe_api_common
  std_msgs
  image_transport
  cv_bridge
  sensor_msgs
  rqt_gauges
  arbe_msgs
)

catkin_package()

## This setting causes Qt's "MOC" generation to happen automatically.
set(CMAKE_AUTOMOC ON)

find_package(Qt5 5.12 COMPONENTS Core Widgets Gui Charts REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5SerialPort REQUIRED)


## This plugin includes Qt widgets, so we must include Qt.
## We'll use the version that rviz used so they are compatible.
if(rviz_QT_VERSION VERSION_LESS "5")
  message(STATUS "Using Qt4 based on the rviz_QT_VERSION: ${rviz_QT_VERSION}")
  find_package(Qt4 ${rviz_QT_VERSION} EXACT REQUIRED QtCore QtGui)
  ## pull in all required include dirs, define QT_LIBRARIES, etc.
  include(${QT_USE_FILE})
else()
  message(STATUS "Using Qt5 based on the rviz_QT_VERSION: ${rviz_QT_VERSION}")
  find_package(Qt5 ${rviz_QT_VERSION} EXACT REQUIRED Core Widgets)
  ## make target_link_libraries(${QT_LIBRARIES}) pull in all required dependencies
  set(QT_LIBRARIES Qt5::Widgets)
endif()

#[[qt5_wrap_cpp(MOC_FILES
  src/arbe_points_publisher/include/arbe_points_publisher/arbe_points_publisher.hpp
  )]]

## I prefer the Qt signals and slots to avoid defining "emit", "slots",
## etc because they can conflict with boost signals, so define QT_NO_KEYWORDS here.
#add_definitions(-DQT_NO_KEYWORDS)

## System dependencies are found with CMake's conventions
# find_package(Boost REQUIRED COMPONENTS system)


## Uncomment this if the package has a setup.py. This macro ensures
## modules and global scripts declared therein get installed
## See http://ros.org/doc/api/catkin/html/user_guide/setup_dot_py.html
# catkin_python_setup()

################################################
## Declare ROS messages, services and actions ##
################################################

## To declare and build messages, services or actions from within this
## package, follow these steps:
## * Let MSG_DEP_SET be the set of packages whose message types you use in
##   your messages/services/actions (e.g. std_msgs, actionlib_msgs, ...).
## * In the file package.xml:
##   * add a build_depend tag for "message_generation"
##   * add a build_depend and a exec_depend tag for each package in MSG_DEP_SET
##   * If MSG_DEP_SET isn't empty the following dependency has been pulled in
##     but can be declared for certainty nonetheless:
##     * add a exec_depend tag for "message_runtime"
## * In this file (CMakeLists.txt):
##   * add "message_generation" and every package in MSG_DEP_SET to
##     find_package(catkin REQUIRED COMPONENTS ...)
##   * add "message_runtime" and every package in MSG_DEP_SET to
##     catkin_package(CATKIN_DEPENDS ...)
##   * uncomment the add_*_files sections below as needed
##     and list every .msg/.srv/.action file to be processed
##   * uncomment the generate_messages entry below
##   * add every package in MSG_DEP_SET to generate_messages(DEPENDENCIES ...)

## Generate messages in the 'msg' folder
# add_message_files(
#   FILES
#   Message1.msg
#   Message2.msg
# )

## Generate services in the 'srv' folder
# add_service_files(
#   FILES
#   Service1.srv
#   Service2.srv
# )

## Generate actions in the 'action' folder
# add_action_files(
#   FILES
#   Action1.action
#   Action2.action
# )

## Generate added messages and services with any dependencies listed here
# generate_messages(
#   DEPENDENCIES
#   std_msgs  # Or other packages containing msgs
# )

################################################
## Declare ROS dynamic reconfigure parameters ##
################################################

## To declare and build dynamic reconfigure parameters within this
## package, follow these steps:
## * In the file package.xml:
##   * add a build_depend and a exec_depend tag for "dynamic_reconfigure"
## * In this file (CMakeLists.txt):
##   * add "dynamic_reconfigure" to
##     find_package(catkin REQUIRED COMPONENTS ...)
##   * uncomment the "generate_dynamic_reconfigure_options" section below
##     and list every .cfg file to be processed

## Generate dynamic reconfigure parameters in the 'cfg' folder
# generate_dynamic_reconfigure_options(
#   cfg/DynReconf1.cfg
#   cfg/DynReconf2.cfg
# )

###################################
## catkin specific configuration ##
###################################
## The catkin_package macro generates cmake config files for your package
## Declare things to be passed to dependent projects
## INCLUDE_DIRS: uncomment this if your package contains header files
## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need
catkin_package(CATKIN_DEPENDS
    ${${PROJECT_NAME}_CATKIN_DEPS}
    message_runtime
 INCLUDE_DIRS
  LIBRARIES image_radar_driver
  CATKIN_DEPENDS roscpp
)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
 #include src/arbe_points_publisher/include/
 include src/lightning_gui_main/pc_autox/
 include ${OpenCV_INCLUDE_DIRS}
 include ${catkin_INCLUDE_DIRS}
 include ${PCL_LIBRARY_DIRS}
include ${CMAKE_BINARY_DIR}/gauges/
include os
include PolyM/include
)

add_subdirectory(os)
add_subdirectory(PolyM)

link_directories(${catkin_LIBRARY_DIRS})

## Add cmake target dependencies of the library
## as an example, code may need to be generated before libraries
## either from message generation or dynamic reconfigure
# add_dependencies(${PROJECT_NAME} ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

## Declare a C++ executable
## With catkin_make all packages are built within a single CMake context
## The recommended prefix ensures that target names across packages don't collide
add_executable(lightning_gui 
src/lightning_gui_main/main.cpp
src/lightning_gui_main/mainwindow.cpp
src/lightning_gui_main/viewpanel.cpp
src/lightning_gui_main/common.cpp
src/lightning_gui_main/osc_chart.cpp
src/lightning_gui_main/chartLightning.cpp
src/lightning_gui_main/fft_chartview.cpp
src/lightning_gui_main/qcustomplot.cpp
src/lightning_gui_main/plot_tracer.cpp
)

add_executable(lightning_server src/local_server/server.cpp)
target_link_libraries(lightning_server
   ${catkin_LIBRARIES}
 )
#[[add_executable(arbe_pointcloud_receiver 
src/arbe_pointcloud_receiver/pointcloud_node.cpp
)

add_executable(arbe_visualization_engine 
src/arbe_visualization_engine/visualization_node.cpp
src/arbe_visualization_engine/Slam_color.cpp
src/arbe_visualization_engine/Pointcloud_coloring.cpp
src/arbe_visualization_engine/vis_utils.cpp
)

add_executable(arbe_slam_can_publisher 
src/arbe_slam_can_publisher/main.cpp
)
## Declare a C++ library
add_library(arbe_points_publisher
  src/arbe_points_publisher/src/arbe_points_publisher.cpp
  ${MOC_FILES}
)
]]
## Rename C++ executable without prefix
## The above recommended prefix causes long target names, the following renames the
## target back to the shorter version for ease of user use
## e.g. "rosrun someones_pkg node" instead of "rosrun someones_pkg someones_pkg_node"
# set_target_properties(${PROJECT_NAME}_node PROPERTIES OUTPUT_NAME node PREFIX "")

## Add cmake target dependencies of the executable
## same as for the library above
# add_dependencies(${PROJECT_NAME}_node ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

link_directories(/usr/lib/x86_64-linux-gnu/)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(CXX_FILESYSTEM_LIBRARIES "stdc++fs")
endif()

## Specify libraries to link a library or executable target against
add_dependencies(lightning_gui ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(lightning_gui
   os
   polym
   Qt5::Charts
   Qt5PrintSupport
   Qt5SerialPort
   ${catkin_LIBRARIES}
   ${PCL_LIBRARIES}
   ${QT_LIBRARIES}
   ${CXX_FILESYSTEM_LIBRARIES}
 )

#[[add_dependencies(arbe_pointcloud_receiver ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(arbe_pointcloud_receiver
   ${catkin_LIBRARIES}
   ${PCL_LIBRARIES}
 )

add_dependencies(arbe_visualization_engine ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(arbe_visualization_engine
   ${catkin_LIBRARIES}
   ${PCL_LIBRARIES}
 )

add_dependencies(arbe_points_publisher ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(arbe_points_publisher
   ${catkin_LIBRARIES}
   ${PCL_LIBRARIES}
 )

add_dependencies(arbe_slam_can_publisher ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(arbe_slam_can_publisher
   ${catkin_LIBRARIES}
   ${PCL_LIBRARIES}
 )
]]
#############
## Install ##
#############

#[[install(TARGETS arbe_points_publisher
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
  )

install(FILES arbe_points_publisher
  plugin_description.xml
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION})
]]
# all install targets should use catkin DESTINATION variables
# See http://ros.org/doc/api/catkin/html/adv_user_guide/variables.html

## Mark executable scripts (Python etc.) for installation
## in contrast to setup.py, you can choose the destination
# install(PROGRAMS
#   scripts/my_python_script
#   DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
# )

## Mark executables and/or libraries for installation
# install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}_node
#   ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#   LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#   RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
# )

## Mark cpp header files for installation
# install(DIRECTORY include/${PROJECT_NAME}/
#   DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
#   FILES_MATCHING PATTERN "*.h"
#   PATTERN ".svn" EXCLUDE
# )

## Mark other files for installation (e.g. launch and bag files, etc.)
#install(FILES
#images/open.png
#   # myfile1
#   # myfile2
# DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
#)

#install(DIRECTORY images/
#        DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/images
#)

#############
## Testing ##
#############

## Add gtest based cpp test target and link libraries
# catkin_add_gtest(${PROJECT_NAME}-test test/test_image_radar_driver.cpp)
# if(TARGET ${PROJECT_NAME}-test)
#   target_link_libraries(${PROJECT_NAME}-test ${PROJECT_NAME})
# endif()

## Add folders to be run by python nosetests
# catkin_add_nosetests(test)
set(CMAKE_BUILD_TYPE DEBUG)
target_compile_definitions(lightning_gui PRIVATE SRC_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\")

set(ARBE_BUILD_TYPE RELEASE)
IF(ARBE_BUILD_TYPE MATCHES RELEASE)
    set (pscm_cmd "cm")
    set (pscm_status "status")
    set (pscm_arg "--cset")
    execute_process(COMMAND ${pscm_cmd} ${pscm_status} ${pscm_arg}
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      RESULT_VARIABLE pscm_result
      OUTPUT_VARIABLE pscm_ver)
    IF (pscm_result MATCHES 0)
	string(REPLACE "@" ";" VERSION_LIST ${pscm_ver})
	list(GET VERSION_LIST 0 pscm_ver)
	string(REPLACE ":" ";" VERSION_LIST ${pscm_ver})
	list(GET VERSION_LIST 1 cs_number)

	set(build_num_file ${CMAKE_CURRENT_SOURCE_DIR}/addons/lightning_gui_build.ver)
	message(STATUS "build_num_file: ${build_num_file}")
	file(WRITE ${build_num_file} "${cs_number}")
    ENDIF()
ENDIF()
