add_library(fakelidar SHARED
        fakelidar.cpp
        fake_point_generator.cpp
        )
target_link_libraries(fakelidar ${PCL_LIBRARIES} ${QTX}::Widgets utils)
