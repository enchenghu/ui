find_package(OpenMP REQUIRED)


add_library(xradar_2d SHARED
        xradar_2d.cpp
        driver/xradar_2d_driver.cc
        )
target_include_directories(xradar_2d PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${OpenCV_INCLUDES})
target_link_libraries(xradar_2d ${PCL_LIBRARIES} ${QTX}::Widgets utils OpenMP::OpenMP_CXX ${OpenCV_LIBS})
