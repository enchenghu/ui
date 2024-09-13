add_library(xradar SHARED
    driver/Utils.cc
    driver/Raf_Api.cc
    driver/arbe_driver.cc
    xradar_setting.cpp
    xradar.cpp
    )
target_include_directories(xradar PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(xradar ${PCL_LIBRARIES} ${QTX}::Widgets pcap utils)