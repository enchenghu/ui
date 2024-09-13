add_library(hesai_qt64 SHARED
    hesai_qt64.cpp
    driver/driver_qt64.cc
)
target_include_directories(hesai_qt64 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(hesai_qt64 ${PCL_LIBRARIES} ${QTX}::Widgets utils)
