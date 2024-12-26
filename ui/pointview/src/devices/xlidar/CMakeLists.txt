find_package(OpenMP REQUIRED)

add_library(xlidar SHARED
        xlidar.cpp
        driver/xlidar_driver.cc
        adc/xlidar_adc_plot.cpp
        )
target_include_directories(xlidar PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(xlidar ${PCL_LIBRARIES} ${QTX}::Widgets utils OpenMP::OpenMP_CXX)
