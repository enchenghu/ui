add_library(tools SHARED
        coordinate_axis.cpp
        grid.cpp
        point_distance_measure.cpp
        scale_bar.cpp
        circle.cpp
        )
target_link_libraries(tools ${PCL_LIBRARIES} ${QTX}::Widgets utils)
