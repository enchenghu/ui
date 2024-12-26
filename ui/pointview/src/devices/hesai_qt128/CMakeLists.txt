
add_library(hesai_qt128 SHARED
        hesai_qt128.cpp
        driver/driver_qt128.cc
        )
target_include_directories(hesai_qt128 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(hesai_qt128 ${PCL_LIBRARIES} ${QTX}::Widgets utils)