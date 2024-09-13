add_library(device_example SHARED
        device_example.cpp
        )
target_link_libraries(device_example ${PCL_LIBRARIES} ${QTX}::Widgets utils)