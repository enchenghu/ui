/*
 * Copyright 2019-2020 Autoware Foundation. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ********************
 *  v0.1.0: drwnz (david.wong@tier4.jp) *
/*
 * Copyright (C) 2019 Arbe - http://www.arberobotics.com/ 
 * 
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Arbe nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

#ifndef ARBE_POINTS_PUBLISHER_HPP
#define ARBE_POINTS_PUBLISHER_HPP

#include "rviz/selection/selection_manager.h"
#include "rviz/viewport_mouse_event.h"
#include "rviz/display_context.h"
#include "rviz/selection/forwards.h"
#include "rviz/properties/property_tree_model.h"
#include "rviz/properties/property.h"
#include "rviz/properties/color_property.h"
#include "rviz/properties/vector_property.h"
#include "rviz/properties/float_property.h"
#include "rviz/properties/int_property.h"

#ifndef Q_MOC_RUN  // See: https://bugreports.qt-project.org/browse/QTBUG-22829
#include <ros/node_handle.h>
#include <ros/publisher.h>
#include "rviz/tool.h"
#include "rviz/default_plugin/tools/selection_tool.h"
#include "rviz/selection/selection_manager.h"
#include <QCursor>
#include <QObject>
#endif

#include <pcl/point_cloud.h>
#include <pcl_conversions/pcl_conversions.h>
#include <sensor_msgs/PointCloud2.h>
#include <visualization_msgs/Marker.h>

const int max_selected_points = 200000;
const int num_of_pointcloud_childs = 18; // our pointcloud record have 17 child

struct  EIGEN_ALIGN16 ArbePointXYZRGBGeneric
{
	PCL_ADD_POINT4D;
	PCL_ADD_RGB;
	float range;
	float azimuth;
	float elevation;
	float doppler;
	float power;
	float range_bin;
	float azimuth_bin;
	float elevation_bin;
	float doppler_bin;
	float power_value;
	float phase_value;
	uint32_t timestamp_sec;
	uint32_t timestamp_nsec;
	float snr;
	uint32_t radar_id;
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW     // make sure our new allocators are aligned
};

POINT_CLOUD_REGISTER_POINT_STRUCT (ArbePointXYZRGBGeneric,           // here we assume a XYZRGB + "detectionData" (as fields)
				(float, x, x)
				(float, y, y)
				(float, z, z)
				(float, rgb, rgb)
				(float, range, range)
				(float, azimuth, azimuth)
				(float, elevation, elevation)
				(float, doppler, doppler)
				(float, power, power)
				(float, range_bin, range_bin)
				(float, azimuth_bin, azimuth_bin)
				(float, elevation_bin, elevation_bin)
				(float, doppler_bin, doppler_bin)
				(float, power_value, power_value)
				(float, phase_value, phase_value)
				(uint32_t, timestamp_sec, timestamp_sec)
				(uint32_t, timestamp_nsec, timestamp_nsec)
				(float, snr, snr)
				(uint32_t, radar_id, radar_id)
)

namespace rviz_plugin_arbe_points_publisher
{
class ArbePointsPublisher;

class ArbePointsPublisher : public rviz::SelectionTool
{
  Q_OBJECT
public:
  ArbePointsPublisher();
  virtual ~ArbePointsPublisher();
  virtual int processMouseEvent(rviz::ViewportMouseEvent& event);
  virtual int processKeyEvent(QKeyEvent* event, rviz::RenderPanel* panel);

public Q_SLOTS:
  void updateTopic();

protected:
  int processSelectedArea();
  ros::NodeHandle node_handle_;
  ros::Publisher rviz_selected_publisher_;
  ros::Subscriber pointcloud_subscriber_;

  std::string tf_frame_;
  std::string rviz_cloud_topic_;
  std::string subscribed_cloud_topic_;

  sensor_msgs::PointCloud2 selected_points_;
  pcl::PointCloud<ArbePointXYZRGBGeneric> cloud;

  bool selecting_;
  int num_selected_points_;
};
}  // namespace rviz_plugin_selected_points_publisher

#endif  // SELECTED_POINTS_PUBLISHER_HPP 
