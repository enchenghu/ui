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

#include "main.h"

ros::NodeHandlePtr n;
//ros::Subscriber objects_sub;
static arbe_msgs::arbeSlamMsg slamMsg;
int s; 
static float host_vel; // Ego Velocity
std::string can_port;

int can_open(void)
{
	struct sockaddr_can addr;
	struct ifreq ifr;

	printf("CAN Sockets Demo\r\n");

	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("Socket");
		return 1;
	}

	int canfd_enabled = 1;
	setsockopt(s, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &canfd_enabled, sizeof(int));

	strcpy( ifr.ifr_name, can_port.c_str() );
	ioctl(s, SIOCGIFINDEX, &ifr);

	memset(&addr, 0, sizeof(addr));
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("Bind");
		return 1;
	}
	return 0;
}

int can_close(void)
{
	if (close(s) < 0) {
		perror("Close");
		return 1;
	}
}

int can_send(const canfd_frame & msg)
{

	if (write(s, &msg, sizeof(struct canfd_frame)) != sizeof(struct canfd_frame)) {
		perror("Write");
		return 1;
	}
	return 0;
}


arbe_msgs::arbeTSlamObj get_object(uint16_t i){
	return slamMsg.ObjectsBuffer[i];
}

void slam_read_callback(const arbe_msgs::arbeSlamMsg::ConstPtr& array)
{
	int recv_size = 0;
	TSlamObj* SlamObject;
	TSlamHeader_V1_1 SlamHeader;
	slamMsg = *array;
	uint32_t num_of_object = (uint32_t)slamMsg.ObjectsBuffer.size();
	host_vel = slamMsg.meta_data.HostVelocity;
	ROS_INFO("Ego Velocity (m/s) = %f",host_vel);
	ROS_INFO("Num of Slam Objects %d",num_of_object);

	for (int i = 0; i < num_of_object; i++)
	{

		arbe_msgs::arbeTSlamObj SlamObject =get_object(i);

		ROS_INFO("ID %d",SlamObject.ID);
		ROS_INFO("X %f",SlamObject.position.x);
		ROS_INFO("Y %f",SlamObject.position.y);
		ROS_INFO("Z %f",SlamObject.position.z);

		/* Prepare can message of a slam object and send it */
		canfd_frame frame;
		frame.can_id = 0x617;
		frame.len = sizeof(TSlamObj);

		memcpy (&frame.data[0], &SlamObject, sizeof(TSlamObj));
		can_send(frame);
	}
}

/* Main function */
int main(int argc, char **argv)
{
	pthread_t display_thread_id;
	int ret;

	ros::init(argc, argv, "arbe_slam_can_publisher");
	ros::Time::init();

	n = boost::make_shared<ros::NodeHandle>();

	n->getParam("/arbe_slam_can_publisher/can_port", can_port);

	/* Open the can interface */
	can_open();

	//objects_sub = n->subscribe("/arbe/processed/objects", 1, slam_read_callback);
	//ROS_INFO("Arbe Slam object Can publisher subscribed to Arbe /arbe/processed/objects");

	ros::Rate loop_rate(30);
	while (ros::ok())
	{
		ros::spinOnce();
		loop_rate.sleep();
	}

	/* Close the can interface */
	can_close();
}

