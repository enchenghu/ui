# xrt proto
add_subdirectory(xrt/proto)

add_library(xrt_replayer SHARED
        ${PROTO_HEADERS}
        ${PROTO_SRCS}
        xrt/record/file/record_file_reader.cc
        xrt/record/file/record_file_base.cc
        xrt/record/record_reader.cc
        xrt_message_dispatcher.cpp
        xrt_replayer.cpp
        xrt_lidar.cpp
        xrt_camera.cpp
        )
target_include_directories(xrt_replayer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(xrt_replayer xrt_proto ${PCL_LIBRARIES} ${QTX}::Widgets utils)
