include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_library(utils SHARED

        camera/video_player.cpp
        camera/video_capture.cpp
        camera/camera_image.cpp
        common/display_context.cpp
        common/device_context.cpp
        common/device_base.cpp
        common/tool_base.h
        common/safe_queue.h
        common/lidar_base.h
        io/pcap_udp_parser.cpp
        io/pcap_udp_recorder.cpp
        io/udp_input.cpp
        io/simple_player_setting.cpp
        lidar/laser_track.cpp
        lidar/range_image.cpp
        lidar/lidar_return_info.cpp
        lidar/lidar_intrinsics.cpp
        lidar/lidar_frame_info.cpp
        pointcloud/point_selection.cpp
        pointcloud/point_exporter.cpp
        pointcloud/point_filter.cpp
        pointcloud/point_manipulator.cpp
        pointcloud/point_table.cpp
        qcustomplot/qcustomplot.cpp
        qcustomplot/qcustomplot.h
        radar/radar_image_viewer.cpp
        radar/rf_image.cpp
        radar/dynamic_axis.cpp
        visualization/pcl_visualizer.cpp
        visualization/pcl_visualizer.hpp
        visualization/interactor_style.cpp
        colormap.cpp
        config.cpp
        frame_info.cpp
        pose_setting.cpp
        log_viewer.cpp
        udp_logger.cpp
        adc_plot.cpp
        property_tree.cpp
        utils.h
        utils.cpp
        pointcloud/point_data.cpp
        )

target_link_libraries(utils ${PCL_LIBRARIES} ${QTX}::Widgets ${QTX}::PrintSupport ${QTX}::Multimedia ${QTX}::MultimediaWidgets pcap ${YAML_CPP_LIBRARIES} ${CURL_LIBRARIES} ${OpenCV_LIBS})