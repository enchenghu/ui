add_library(blidar SHARED
    blidar.cpp
    driver/driver.cc
    )
target_include_directories(blidar PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(blidar ${PCL_LIBRARIES} ${QTX}::Widgets utils)
