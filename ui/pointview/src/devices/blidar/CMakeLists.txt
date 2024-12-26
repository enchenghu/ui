add_library(blidar SHARED
        blidar.cpp
        driver/blidar_driver.cc
        adc/blidar_adc_plot.cpp
        )
target_include_directories(blidar PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(blidar ${PCL_LIBRARIES} ${QTX}::Widgets utils)
