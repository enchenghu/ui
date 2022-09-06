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
#include "arbe_points_publisher/arbe_points_publisher.hpp"

namespace rviz_plugin_arbe_points_publisher
{
ArbePointsPublisher::ArbePointsPublisher()
{
	updateTopic();
}

ArbePointsPublisher::~ArbePointsPublisher()
{
}

void ArbePointsPublisher::updateTopic()
{
	node_handle_.param("frame_id", tf_frame_, std::string("/base_link"));
	rviz_cloud_topic_ = std::string("/arbe/rviz/selected_points");

	rviz_selected_publisher_ = node_handle_.advertise<sensor_msgs::PointCloud2>(rviz_cloud_topic_.c_str(), 1);
	num_selected_points_ = 0;
}

int ArbePointsPublisher::processKeyEvent(QKeyEvent* event, rviz::RenderPanel* panel)
{
	if (event->type() == QKeyEvent::KeyPress)
	{
		if (event->key() == 'c' || event->key() == 'C')
		{
			ROS_INFO_STREAM_NAMED("ArbePointsPublisher::processKeyEvent", "Cleaning previous selection (selected area "
						                                        "and points).");
			rviz::SelectionManager* selection_manager = context_->getSelectionManager();
			rviz::M_Picked selection = selection_manager->getSelection();
			selection_manager->removeSelection(selection);
			visualization_msgs::Marker marker;
			// Set the frame ID and timestamp.  See the TF tutorials for information on these.
			marker.header.frame_id = context_->getFixedFrame().toStdString().c_str();
			marker.header.stamp = ros::Time::now();
			marker.ns = "basic_shapes";
			marker.id = 0;
			marker.type = visualization_msgs::Marker::CUBE;
			marker.action = visualization_msgs::Marker::DELETE;
			marker.lifetime = ros::Duration();
			num_selected_points_ = 0;
		}
		else if (event->key() == 'p' || event->key() == 'P' && ( num_selected_points_ > 0 ) )
		{
			ROS_INFO_STREAM_NAMED("ArbePointsPublisher.updateTopic",
					    "Publishing " << num_selected_points_ << " selected points to topic "
						          << node_handle_.resolveName(rviz_cloud_topic_));

			selected_points_.header.stamp = ros::Time::now();
			pcl::toROSMsg(cloud,selected_points_);
			selected_points_.header.frame_id = "image_radar";
			rviz_selected_publisher_.publish(selected_points_);
		}
	}
}

int ArbePointsPublisher::processMouseEvent(rviz::ViewportMouseEvent& event)
{
	int flags = rviz::SelectionTool::processMouseEvent(event);
	if (event.alt())
	{
		selecting_ = false;
	}
	else
	{
		if (event.leftDown())
		{
			selecting_ = true;
		}
	}

	if (selecting_)
	{
		if (event.leftUp())
		{
			this->processSelectedArea();
		}
	}
	return flags;
}

int ArbePointsPublisher::processSelectedArea()
{
	rviz::SelectionManager* selection_manager = context_->getSelectionManager();
	rviz::M_Picked selection = selection_manager->getSelection();
	rviz::PropertyTreeModel* model = selection_manager->getPropertyModel();
	rviz::FloatProperty* floatchild;
	rviz::IntProperty* uint32child;

	int i = 0;
	int point_count = 0;
	cloud.points.resize(max_selected_points);
	while (model->hasIndex(i, 0))
	{
		QModelIndex child_index = model->index(i, 0);

		rviz::Property* child = model->getProp(child_index);

		if ( child->numChildren() == num_of_pointcloud_childs )
		{
			rviz::VectorProperty* subchild = (rviz::VectorProperty*)child->childAt(0);
			Ogre::Vector3 point_data = subchild->getVector();
			cloud.points[point_count].x = point_data.x;
			cloud.points[point_count].y = point_data.y;
			cloud.points[point_count].z = point_data.z;

			rviz::ColorProperty* colorchild = (rviz::ColorProperty*)child->childAt(1);
			QColor color_data = colorchild->getColor();
			cloud.points[point_count].r = color_data.red();
			cloud.points[point_count].g = color_data.green();
			cloud.points[point_count].b = color_data.blue();

			floatchild = (rviz::FloatProperty*)child->childAt(3);
			cloud.points[point_count].range = floatchild->getValue().toFloat();
			floatchild = (rviz::FloatProperty*)child->childAt(4);
			cloud.points[point_count].azimuth = floatchild->getValue().toFloat();
			floatchild = (rviz::FloatProperty*)child->childAt(5);
			cloud.points[point_count].elevation = floatchild->getValue().toFloat();
			floatchild = (rviz::FloatProperty*)child->childAt(6);
			cloud.points[point_count].doppler = floatchild->getValue().toFloat();
			floatchild = (rviz::FloatProperty*)child->childAt(7);
			cloud.points[point_count].power = floatchild->getValue().toFloat();
			floatchild = (rviz::FloatProperty*)child->childAt(8);
			cloud.points[point_count].range_bin = floatchild->getValue().toFloat();
			floatchild = (rviz::FloatProperty*)child->childAt(9);
			cloud.points[point_count].azimuth_bin = floatchild->getValue().toFloat();
			floatchild = (rviz::FloatProperty*)child->childAt(10);
			cloud.points[point_count].elevation_bin = floatchild->getValue().toFloat();
			floatchild = (rviz::FloatProperty*)child->childAt(11);
			cloud.points[point_count].doppler_bin = floatchild->getValue().toFloat();
			floatchild = (rviz::FloatProperty*)child->childAt(12);
			cloud.points[point_count].power_value = floatchild->getValue().toFloat();
			floatchild = (rviz::FloatProperty*)child->childAt(13);
			cloud.points[point_count].phase_value = floatchild->getValue().toFloat();
			uint32child = (rviz::IntProperty*)child->childAt(14);
			cloud.points[point_count].timestamp_sec = uint32child->getValue().toInt();
			uint32child = (rviz::IntProperty*)child->childAt(15);
			cloud.points[point_count].timestamp_nsec = uint32child->getValue().toInt();
			uint32child = (rviz::IntProperty*)child->childAt(16);
			cloud.points[point_count].snr = uint32child->getValue().toFloat();
			uint32child = (rviz::IntProperty*)child->childAt(17);
			cloud.points[point_count].radar_id = uint32child->getValue().toInt();
			point_count++;
		}
		i++;
  	}
	num_selected_points_ = point_count;
	cloud.width = num_selected_points_;
	cloud.height = 1;
	ROS_INFO_STREAM_NAMED("ArbePointsPublisher._processSelectedAreaAndFindPoints",
		"Number of points in the selected area: " << num_selected_points_);
	cloud.points.resize(num_selected_points_);
	return 0;
}
}  // namespace rviz_plugin_selected_points_publisher

#include <pluginlib/class_list_macros.h>
PLUGINLIB_EXPORT_CLASS(rviz_plugin_arbe_points_publisher::ArbePointsPublisher, rviz::Tool)
