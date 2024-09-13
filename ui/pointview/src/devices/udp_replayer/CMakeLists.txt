add_library(udp_replayer SHARED
    udp_replayer.cpp
    )
target_include_directories(udp_replayer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(udp_replayer ${PCL_LIBRARIES} ${QTX}::Widgets utils)