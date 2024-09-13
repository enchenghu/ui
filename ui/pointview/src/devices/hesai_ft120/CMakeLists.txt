include_directories(
        HesaiLidar_SDK_2.0/libhesai
        HesaiLidar_SDK_2.0/libhesai/Lidar
        HesaiLidar_SDK_2.0/libhesai/UdpParser
        HesaiLidar_SDK_2.0/libhesai/UdpParser/include
        HesaiLidar_SDK_2.0/libhesai/UdpParser/src
        HesaiLidar_SDK_2.0/libhesai/UdpProtocol
        HesaiLidar_SDK_2.0/libhesai/Source/include
        HesaiLidar_SDK_2.0/libhesai/Container/include
        HesaiLidar_SDK_2.0/libhesai/Container/src
        HesaiLidar_SDK_2.0/libhesai/UdpParserGpu
        HesaiLidar_SDK_2.0/libhesai/UdpParserGpu/include
        HesaiLidar_SDK_2.0/libhesai/UdpParserGpu/src
        HesaiLidar_SDK_2.0/libhesai/PtcClient/include
        HesaiLidar_SDK_2.0/libhesai/PtcParser
        HesaiLidar_SDK_2.0/libhesai/PtcParser/src
        HesaiLidar_SDK_2.0/libhesai/PtcParser/include
        HesaiLidar_SDK_2.0/libhesai/Logger/include
        HesaiLidar_SDK_2.0/libhesai/include
        HesaiLidar_SDK_2.0/driver
        HesaiLidar_SDK_2.0/third_party
)

link_directories(
        HesaiLidar_SDK_2.0/make_sdk
)
file(GLOB SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/HesaiLidar_SDK_2.0/make_sdk/libhesai/*.a")

find_package(Boost REQUIRED COMPONENTS system filesystem thread iostreams)
include_directories(

        ${Boost_INCLUDE_DIRS}
)
find_package(OpenSSL REQUIRED)

add_library(hesai_ft120
        hesai_ft120.h
        hesai_ft120.cpp
        )

target_link_libraries(hesai_ft120
        ${SOURCE_FILES}
        ${PCL_LIBRARIES} ${QTX}::Widgets utils
        ${Boost_LIBRARIES}
        ${OPENSSL_LIBRARIES}
        )
