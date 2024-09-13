add_library(hesai_pandar128 SHARED
        hesai_pandar128.cpp
        hesai/functional_safety.cc
        hesai/laser_ts.cc
        hesai/pandar128.cc
        )
target_include_directories(hesai_pandar128 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(hesai_pandar128 ${PCL_LIBRARIES} ${QTX}::Widgets utils)