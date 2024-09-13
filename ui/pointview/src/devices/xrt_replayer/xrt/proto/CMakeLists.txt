# xrt proto
cmake_policy(SET CMP0071 NEW)
find_package(Protobuf REQUIRED)
protobuf_generate_cpp(PROTO_SRCS PROTO_HEADERS
   header.proto
   shm.proto
   record.proto
   proto_desc.proto
   safety_level.proto
   error_code.proto
   pointcloud.proto
   sensor_image.proto
)
add_library(xrt_proto SHARED
    ${PROTO_HEADERS}
    ${PROTO_SRCS}
    )
target_link_libraries(xrt_proto ${PROTOBUF_LIBRARIES})
