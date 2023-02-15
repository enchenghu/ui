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

#include <QDebug>
#include "pointcloud_node.h"
#include "arbe_msgs/arbeDspJobmsg.h"
#include "arbe_msgs/arbeThresholds.h"
#include "arbe_msgs/arbeNewPcMsg.h"
#include <arbe_msgs/arbeGUIsettings.h>
#include "arbe_msgs/arbeTAlgoDetectorThrOutput.h"
#include <std_msgs/UInt32MultiArray.h>
#include <QSettings>

static arbe_msgs::arbeNewPcMsg PointCloudMsg;
static arbe_msgs::arbeTAlgoDetectorThrOutput AlgoDetectorThrMsg;

static arbe_msgs::arbeDspJobmsg DspJobmsg;
static arbe_msgs::arbeThresholds Thresholdsmsg;
static arbe_msgs::arbeGUIsettings controls_data;
int  console_level = (int)ros::console::levels::Warn;

std_msgs::UInt8MultiArray  PcFrameData;
std_msgs::UInt8MultiArray  ObjectsPerFrame;
std_msgs::UInt8MultiArray  DspJobData;
std_msgs::UInt8MultiArray  ThresholdData;

ros::Subscriber pc_frame_sub;
ros::Publisher arbe_pc_frame_pub;

ros::Subscriber rosbag_sub;
ros::Subscriber rosbag_sub_single_radar;
ros::Subscriber ctrl_sub;
ros::Subscriber gui_commands_sub;
ros::Subscriber gui_controls_sub;
ros::Subscriber targets_sub;

ros::Publisher arbe_bin_detections_pub;
ros::Publisher arbe_slam_pub;
ros::Publisher arbe_dspjob_pub;
ros::Publisher noise_vector_pub;
ros::Publisher arbe_thresholds_detections_pub;
ros::Publisher arbe_status_pub;
ros::Publisher arbe_binary_pcl_pub;
ros::Publisher arbe_pcl_pub;
//ros::Publisher arbe_info_markers;
ros::Publisher arbe_radar_calibration_pub;
ros::Publisher arbe_debug_data_count_pub;
ros::Publisher algo_detector_thr_pub;

static uint16_t usFrameCounter=0;
uint32_t countTargets = 0;
bool terminating = false;

TArbeApiMailBox tArbeApiMailBox;

/* IP parmeters */
int ctrl_sock = 0;
int logs_sock = 0;
int radar_sock = 0;

FILE * file;
int rosbag_recording = 1;
int recording = 0;

std::string ColoringType = "Elevation";
float ant_euler_alpha = 0;
static bool isFloatingText_enabled = true;

void *radar_ethernet_logger_thread(void* args);

/*
   This callback is triggered when binary pointcloud data is published
   either by rosbag/rqt_bag (when using a .rosbag recording) or by the radar thread
   when we are receiving UDP data from a real radar
*/
void bin_pointcloud_read_callback(const std_msgs::UInt8MultiArray::ConstPtr& array)
{
	int recv_size = array->data.size();
	if (recv_size>MAX_BUFFER_LENGTH)
		recv_size=MAX_BUFFER_LENGTH;
	ROS_DEBUG("bin_pointcloud_read_callback: read a message of %d bytes", recv_size);
	/* pass the recived data to the RAF */
	std::copy(array->data.begin(),array->data.begin()+recv_size,data_server_reply);

	RAF_DRV_AccumulateRxData(data_server_reply, recv_size);
}

/*
  Connect to the radar using a tcp socket
*/
int radar_connect()
{
	int err;
	int one = 1;
	struct sockaddr_in ctrl_serv_addr;
	struct sockaddr_in data_serv_addr;
	struct sockaddr_in logs_serv_addr;
	pthread_t eth_logs_thread_id;

	// create TCP socket that will be used to control the rada
	if ((ctrl_sock=socket(AF_INET, SOCK_STREAM, 0))==-1){
		ROS_DEBUG("ERROR: Could not create socket!");
	}
	setsockopt(ctrl_sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

	struct timeval timeout;
	timeout.tv_sec  = 3;  // after 2 seconds connect() will timeout
	timeout.tv_usec = 0;
	setsockopt(ctrl_sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

	memset(&ctrl_serv_addr, 0, sizeof(ctrl_serv_addr));
	ctrl_serv_addr.sin_family = AF_INET;
	ctrl_serv_addr.sin_addr.s_addr = inet_addr(arg_radar_ip.c_str());
	ctrl_serv_addr.sin_port = htons(arg_ctrl_port);

	if(connect(ctrl_sock, (struct sockaddr*)&ctrl_serv_addr, sizeof(ctrl_serv_addr))<0)
	{
		ROS_INFO("Failed to connect to radar IP %s", arg_radar_ip.c_str());
		return 1; /* Completely fail only if first radar didn't connect */
	}
	fcntl(ctrl_sock, F_SETFL, O_NONBLOCK); /* Set the socket to non blocking mode */

	ROS_INFO("Connecting to the Radar data socket using UDP port %d",arg_data_port);
	// create UDP socket that will be used to receive data from the Radar
	if ((radar_sock=socket(AF_INET, SOCK_DGRAM, 0))==-1){
		ROS_DEBUG("ERROR: Could not create UDP socket!");
	}

	setsockopt(radar_sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
	setsockopt(radar_sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

	memset(&data_serv_addr, 0, sizeof(data_serv_addr));
	data_serv_addr.sin_family = AF_INET;
	data_serv_addr.sin_addr.s_addr = INADDR_ANY;
	data_serv_addr.sin_port = htons(arg_data_port);

	ROS_DEBUG("data Port: %d",data_serv_addr.sin_port);

	if ( bind(radar_sock, (const struct sockaddr*)&data_serv_addr,sizeof(data_serv_addr)) < 0 ) 
	{ 
		ROS_DEBUG("Failed to connect to the Data socket");
		return 1;
	} 

	// create UDP socket that will be used to receive logs over thernet from the Radar
	ROS_INFO("Openning a UDP socket using port %d to the radar for receiving debug logs", arg_logs_port);

	if ((logs_sock=socket(AF_INET, SOCK_DGRAM, 0))==-1){
		ROS_INFO("ERROR: Could not create Logs UDP socket!");
	}

	setsockopt(logs_sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
	setsockopt(logs_sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

	memset(&logs_serv_addr, 0, sizeof(logs_serv_addr));
	logs_serv_addr.sin_family = AF_INET;
	logs_serv_addr.sin_addr.s_addr = INADDR_ANY;
	/* The logs port number is the UDP port used for PointCLoud + 1 */
	logs_serv_addr.sin_port = htons(arg_logs_port);

	if ( bind(logs_sock, (const struct sockaddr*)&logs_serv_addr,sizeof(logs_serv_addr)) < 0 ) 
	{ 
		ROS_INFO("Failed to connect to the Ethernet logger socket");
	} else {
		/* spawn the Radar Ethernet logger  thread */
		pthread_create(&eth_logs_thread_id,NULL,&radar_ethernet_logger_thread,NULL);
	}
	fcntl(logs_sock, F_SETFL, O_NONBLOCK); /* Set the socket to non blocking mode */
	ROS_INFO("Radar Connected");

	/* Set radar time as ros time */
	uint64_t time =(uint64_t)(ros::Time::now().toSec()*1000); //= (((((uint64_t)tPointCloud_V1_1->unTimeMsb) << 32)) | ((uint64_t)tPointCloud_V1_1->unTimeLsb))>>16;
	TSetTimeInfo info;
	if(0)
	{
		time = time<<16;
		info.unInitateTimeLsb = (uint32_t)((time & 0xffffffff));
		info.unInitateTimeMsb = (uint32_t)(time>>32);
		ROS_INFO("time: %lu MSB %d LSB %d AM I INVERTIBLE? %lu",time,info.unInitateTimeMsb,info.unInitateTimeLsb,
				 (((uint64_t)(info.unInitateTimeMsb)<<32 | ((uint64_t)info.unInitateTimeLsb))>>16));
	}
	else
	{
		info.unInitateTimeLsb = (uint32_t)((time & 0xffffffffffffffff));
		info.unInitateTimeMsb = (uint32_t)((time>>32) & 0xffff);
		ROS_INFO("time: %lu MSB %d LSB %d AM I INVERTIBLE? %lu",time,info.unInitateTimeLsb,info.unInitateTimeMsb,
				 (((uint64_t)(info.unInitateTimeMsb)<<32 | ((uint64_t)info.unInitateTimeLsb))));
	}
	RAF_API_SysCfg_SetTime(&tArbeApiMailBox,info);
	if(radar_base_freq>76.0 && radar_base_freq<81.0)
	{
		ROS_INFO("Requested base freq : %f",radar_base_freq);
		PTCtrlFrameControlInfo FreqInfo;
		FreqInfo->unBandwidth = -1;//hz -1=DC
		FreqInfo->unBaseFreq =radar_base_freq*1e6;//khz
		for(int type =0; type<EFrameTypeUser::FrameTypeLast; type++){
			FreqInfo->unFrameType = type;
			RAF_API_RdrCtrl_SetFrameControlData(&tArbeApiMailBox, FreqInfo);
			sleep(1);
		}
	}
	return 0;
}

/*
   Disconnect from the radar
*/
void radar_disconnect()
{
	TStopInfo stopInfo;
	ROS_DEBUG("Disconnecting from the radar socket");
	RAF_API_RdrCtrl_StopTx(&tArbeApiMailBox,&stopInfo);
	sleep(1);
	close(ctrl_sock);
	close(radar_sock);
	close(logs_sock);
}

/*
   Send a provided data buffer to the radar over the TCP control socket
*/
uint32_t STD_CALL TransmitBuffer(int messageID, uint8_t* buffer, unsigned int len)
{
	if (ctrl_sock == 0)
	{
		ROS_DEBUG("TransmitBuffer: Socket not ready yet");
		return -1;
	}

	if (write(ctrl_sock, buffer, len) < 0)
	{
		ROS_INFO("Write fail!");
	}
	else
	{
	 	ROS_INFO("Data -> Data sent");
	}
	return 0;
}

void ctrl_message_callback(const std_msgs::UInt8MultiArray::ConstPtr& array)
{
	ROS_INFO("ctrl_message_callback: transmit %d control bytes",(uint32_t)array->data.size());
	if (ctrl_sock == 0)
	{
		ROS_DEBUG("ctrl_message_callback: Socket not ready yet");
		return;
	}

	if (write(ctrl_sock, &array->data[0], array->data.size()) < 0)
	{
		ROS_INFO("Write fail!");
	}
	else
	{
	 	ROS_INFO("Data -> Data sent");
	}
}

void gui_message_callback(const std_msgs::String::ConstPtr& msg)
{
	ROS_INFO("I heard: [%s]", msg->data.c_str());
	std::string msg_str = msg->data;

	std::size_t pos=msg_str.find("goodbye");
	if (pos!=std::string::npos)
	{
		ROS_WARN("Received a goodbye command from the GUI node... stopping");
		terminating = true;
		int ret = system("rosnode kill --all");

	}
}

static bool discardOutOfElevation = false;
void set_discard_out_of_el_context(bool flag)
{
	discardOutOfElevation = flag;
}
bool get_discard_out_of_el_context()
{
	return discardOutOfElevation;
}

static bool disp_processed_pc = false;
bool get_disp_processed_pc()
{
    return disp_processed_pc;
}
void set_disp_processed_pc(bool flag)
{
    disp_processed_pc = flag;
}
int prevTh = 50;
void SetThresholdsFromGui(int threshold4d,int threshold3d, int noise_level,  int DynamicAzimuth, int DynamicElevation)
{
	if(threshold4d != prevTh)
	{
		prevTh = threshold4d;
		TSetThresholdsInfo radarThresholds;
		int threshold4d_snr = threshold4d + noise_level;
		int threshold3d_snr = threshold3d + noise_level;

		radarThresholds.opcode = SetStaticAndDynamicThresholds;
		radarThresholds.aunParams[0] = (uint32_t)((float)threshold3d_snr  / POW_TO_DB_RATIO);
		radarThresholds.aunParams[1] = (uint32_t)((float)threshold4d_snr / POW_TO_DB_RATIO);
		radarThresholds.aunParams[2] = (uint32_t)((float)DynamicAzimuth / POW_TO_DB_RATIO);
		radarThresholds.aunParams[3] = (uint32_t)((float)DynamicElevation / POW_TO_DB_RATIO);
		RAF_API_RdrCtrl_SetThresholds(&tArbeApiMailBox, &radarThresholds );		
		ROS_INFO("Set radar:%d parameters th:3d=%d,4d=%d",arg_radar_id,threshold3d_snr,threshold4d_snr);

	}
}
void gui_controls_callback(const arbe_msgs::arbeGUIsettings::ConstPtr& controls_data)
{
	ROS_DEBUG("Set coloring to %s",controls_data->ColoringType.c_str());
	ColoringType = controls_data->ColoringType;

	ROS_DEBUG("Set MinDoppler to %lf",controls_data->MinDoppler);
	MinDoppler = controls_data->MinDoppler;

	ROS_DEBUG("Set MaxDoppler to %lf",controls_data->MaxDoppler);
	MaxDoppler = controls_data->MaxDoppler;

	ROS_DEBUG("Set Color_Coding_Min_Max min:%lf max:%lf",controls_data->coloring_cc_min, controls_data->coloring_cc_max);
	Color_Coding_Min_Max::Instance()->set_min(controls_data->ColoringType, controls_data->coloring_cc_min);
	Color_Coding_Min_Max::Instance()->set_max(controls_data->ColoringType, controls_data->coloring_cc_max);

	ROS_DEBUG("Set discardOutOfElContext %d",controls_data->discardOutOfElContext);
	set_discard_out_of_el_context(controls_data->discardOutOfElContext);

	ROS_DEBUG("Set disp_processed_pc %d",controls_data->disp_processed_pc);
	set_disp_processed_pc(controls_data->disp_processed_pc);

	ROS_DEBUG("Set noise_level_db to %d",controls_data->noise_level_db);
	noise_level_db = controls_data->noise_level_db;

	ROS_DEBUG("Set selectedAzimuthBin to %d",controls_data->selectedAzimuthBin);
	selectedAzimuthBin = controls_data->selectedAzimuthBin;

	ROS_DEBUG("Set tx_started to %d",controls_data->tx_started);
	tx_started = controls_data->tx_started;

	ROS_DEBUG("gui_controls_callback controls_data->radar_id =%d",controls_data->radar_id);
	if ( controls_data->radar_id == arg_radar_id )
	{
		ROS_DEBUG("Received calibration for radar ID %d", controls_data->radar_id);
		ROS_DEBUG("Set Radar X offset to %lf",controls_data->radar_x_offset);
		radar_x_offset = controls_data->radar_x_offset;

		ROS_DEBUG("Set Radar Y offset to %lf",controls_data->radar_y_offset);
		radar_y_offset = controls_data->radar_y_offset;

		ROS_DEBUG("Set Radar Z offset to %lf",controls_data->radar_z_offset);
		radar_z_offset = controls_data->radar_z_offset;

		radar_yaw_angle = controls_data->radar_yaw_angle  * 180 / M_PI;
		ROS_DEBUG("Set Radar Yaw Angle to %lf",radar_yaw_angle);

		radar_pitch_angle = controls_data->radar_pitch_angle * 180 / M_PI;
		ROS_DEBUG("Set Radar Pitch Angle to %lf",radar_pitch_angle);

		SetThresholdsFromGui(controls_data->threshold4d, controls_data->threshold3d,
							 controls_data->noise_level_db, controls_data->DynamicAzimuth,
							 controls_data->DynamicElevation);

	}
	return;
}

/*
   This callback is called by the RAF API when debug data is ready	
*/

uint32_t aggregated_data_length = 0;
std_msgs::UInt8MultiArray debug_data;
std_msgs::UInt32MultiArray debug_info;
void STD_CALL ctrlDebugMsgRecievedCb(uint8_t* buffer, uint32_t size)
{
	ros::NodeHandlePtr n = boost::make_shared<ros::NodeHandle>();
	TMemoryDataOp * memoryDataOp = (TMemoryDataOp *)buffer;
	uint32_t current_data_length = memoryDataOp->tMemoryOperation.unCurrentDataLength;
	uint32_t total_data_length = memoryDataOp->tMemoryOperation.tMemoryOperationInfo.unValueOrTotalLength;
	aggregated_data_length += current_data_length;


	ROS_DEBUG("Received debug message of size=%d bytes out of total of %d",aggregated_data_length,total_data_length);
	debug_data.data.insert(debug_data.data.end(),memoryDataOp->tMemoryOperation.pucData, memoryDataOp->tMemoryOperation.pucData + current_data_length);

	/* Publish the debug data read counters */
	debug_info.data.clear();
	debug_info.data.push_back(total_data_length);
	debug_info.data.push_back(aggregated_data_length);
	arbe_debug_data_count_pub.publish(debug_info);

	if ( aggregated_data_length >= total_data_length )
	{
		ROS_INFO("received a full debug buffer... saving to disk\n\r");

		char FileName[40];
		const char RecordFileName[32] = "arbe-debug-radar";
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);
		sprintf(FileName,"%s%d-%d%02d%02d-%02d%02d%02d.bin",
			RecordFileName, arg_radar_id, tm.tm_year + 1900,
			tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min,
			tm.tm_sec);

		ROS_INFO("Debug File=%s\n\r",FileName);
		file = fopen(FileName, "wb");
		if (file != NULL) {
			std::vector<uint8_t>::const_iterator it = debug_data.data.begin();
			fwrite(&it[0], aggregated_data_length, 1, file);
			fclose(file);
		}

		aggregated_data_length = 0;
		debug_data.data.clear();
	}
}

/* 
   This thread reads status messages that are received from the radar over TCP
   and parses them.
*/
void *ctrl_thread(void* args)
{
	ros::NodeHandlePtr n = boost::make_shared<ros::NodeHandle>();
	std_msgs::UInt8MultiArray status_data;
	ros::Rate loop_rate(200);
	int recv_size;

	ROS_DEBUG("ctrl_thread - enter\n");
	while (ros::ok())
	{
		//ROS_DEBUG("ctrl_thread\n");
		recv_size = read(ctrl_sock,ctrl_server_reply,CTRL_BUFFER_LENGTH);
		if (recv_size < 0)
		{
			//ROS_DEBUG("Ctrl port read error!\n");
		}
		else if (recv_size > 0)
		{
			ROS_DEBUG("ctrl -> received %d bytes from socket...\n",recv_size);
			/* pass the payload to the driver */
			RAF_DRV_AccumulateRxData(ctrl_server_reply, recv_size);
		}
		ros::spinOnce();
		loop_rate.sleep();
	}
}

/*
   This thread handles recieving pointcloud data from the radar over
   ethernet and pass them to the RAF API for parsing. It also publishes
   the binary pointcloud data in case we select to record it into a
   rosbag file
*/

void *radar_data_thread(void* args)
{
	ros::NodeHandlePtr n = boost::make_shared<ros::NodeHandle>();

	std_msgs::UInt8MultiArray pointcloud_data;
	ros::Rate loop_rate(200);
	int recv_size = 0;
	uint32_t byte_count = 0;

	ROS_DEBUG("radar_data_thread - enter");

	struct sockaddr_in cliaddr;
	socklen_t len = sizeof(cliaddr);  //len is value/resuslt
	memset(&cliaddr, 0, sizeof(cliaddr));
	cliaddr.sin_family = AF_INET;
	cliaddr.sin_addr.s_addr = inet_addr(arg_radar_ip.c_str());
	cliaddr.sin_port = htons(arg_data_port);
	while (ros::ok())
	{
		recv_size = recvfrom(radar_sock,data_server_reply,MAX_BUFFER_LENGTH,MSG_WAITALL, ( struct sockaddr *) &cliaddr,&len);
		if (recv_size > 0)
		{
			pointcloud_data.data.insert(pointcloud_data.data.end(),data_server_reply,data_server_reply+recv_size);
			byte_count += recv_size;

			TPointCloud_V1_1 * pointCloud = (TPointCloud_V1_1 *)data_server_reply;
			if ( (pointCloud->ucLastPacket == 1) || (10*byte_count>MAX_BUFFER_LENGTH))
			{
				//		/* Publish the binary data received from the socket
				//		So it can be recorded using rosbag */
				//		/* Publish the pointcloud binary data */
				arbe_binary_pcl_pub.publish(pointcloud_data);

				//RAF_DRV_AccumulateRxData(&pointcloud_data.data[0], byte_count);
				ros::spinOnce();
//				loop_rate.sleep();
				pointcloud_data.data.clear();
				byte_count = 0;
			}
			ROS_DEBUG("received %d bytes from the radar over UDP",byte_count);
		}
		else{
			loop_rate.sleep();
			ros::spinOnce();
			}

	}
}
void radar_logger_CB(std_msgs::String::ConstPtr str)
{
	ROS_INFO("[Log From Radar %d :%s]", arg_radar_id, str->data.c_str());

}
void *radar_ethernet_logger_thread(void* args)
{
	ros::NodeHandlePtr n = boost::make_shared<ros::NodeHandle>();
	ros::Publisher arbe_eth_logger_pub = n->advertise<std_msgs::String>("/arbe/raw/radar_logs_over_eth/"+std::to_string(arg_radar_id),100);
	std_msgs::String eth_logger_data;

	ros::Rate loop_rate(100);
	int recv_size = 0;
	ROS_DEBUG("radar_ethernet_logger_thread - enter");

	struct sockaddr_in ethaddr;
	socklen_t len = sizeof(ethaddr);  //len is value/resuslt
	memset(&ethaddr, 0, sizeof(ethaddr));
	ethaddr.sin_family = AF_INET;
	ethaddr.sin_addr.s_addr = inet_addr(arg_radar_ip.c_str());
	ethaddr.sin_port = htons(arg_logs_port);

	while (ros::ok())
	{
		recv_size = recvfrom(logs_sock,EthLogBuffer,ETH_LOGS_BUFFER_LEN,MSG_WAITALL, ( struct sockaddr *) &ethaddr,&len);
		if (recv_size > 0)
		{
			char out_string[200];
			memset(&out_string, 0, 200);
			strncpy(out_string, EthLogBuffer, recv_size);
			eth_logger_data.data.clear();
			eth_logger_data.data.append(out_string);

			arbe_eth_logger_pub.publish(eth_logger_data);
	}
		else
		loop_rate.sleep();
		ros::spinOnce();
}
}

/*
   This callback is called by the RAF API when slam data is ready
*/
static int countObjects = 0;
void STD_CALL dataSlamObjectsMsgRecievedCb(uint8_t* buffer, uint32_t size)
{
	int ObjectsInPacket = (size - sizeof(TSlamObj))/sizeof(TSlamObj);
	countObjects += ObjectsInPacket;

	//ROS_DEBUG("Received %d slam objects size=%d size of object %ld",ObjectsInPacket,size,sizeof(TSlamObj));
	//ROS_DEBUG("last packet = %d",objects->last_packet);
	for (int i = 0; i < size; i++)
	{
		ObjectsPerFrame.data.push_back(buffer[i]);
	}
}

/*
   This callback is called by the RAF API when DSP Job data is ready
*/
void STD_CALL DspJobMsgRecievedCb(uint8_t* buffer, uint32_t size)
{
	ROS_DEBUG("Received a DSP Job size of message =%d",size);
	uint8_t* header ;
	DspJobmsg.Header.clear();
	DspJobmsg.Info.clear();
	buffer += 0;//sizeof(TDSPjobHeader);
	size -= 0;//sizeof(TDSPjobHeader);
	DspJobmsg.Header.insert(DspJobmsg.Header.begin(),header,header + 0);//+sizeof(TDSPjobHeader));
	DspJobmsg.Info.insert(DspJobmsg.Info.begin(),buffer,buffer+size);
}

void STD_CALL ThresholdMsgRecievedCb(uint8_t* buffer, uint32_t size)
{
	ROS_DEBUG("threshold array size - %d",size);
	uint8_t* header ;
	Thresholdsmsg.Header.clear();
	Thresholdsmsg.Info.clear();
	buffer += sizeof(TRAF_API_Header);
	size -= sizeof(TRAF_API_Header);
	Thresholdsmsg.Header.insert(Thresholdsmsg.Header.begin(),header,header+sizeof(TRAF_API_Header));
	Thresholdsmsg.Info.insert(Thresholdsmsg.Info.begin(),buffer,buffer+size);
}

void STD_CALL AlgoDetectorThresholdsRecievedCb(uint8_t* buffer, uint32_t size)
{
	ROS_DEBUG("Received algo detector threshold array, size - %d",size);
	buffer += sizeof(TRAF_API_Header);
	size -= sizeof(TRAF_API_Header);

	TAlgoDetectorThrInfo* ThresholdInfo= (TAlgoDetectorThrInfo *)(uint8_t*)(&(buffer[0]));

	AlgoDetectorThrMsg.RosHeader.stamp = ros::Time::now();
	AlgoDetectorThrMsg.tinfo.unFrameNumber = ThresholdInfo->unFrameNumber;
	AlgoDetectorThrMsg.tinfo.eAlgoOutput = ThresholdInfo->eAlgoOutput;
	AlgoDetectorThrMsg.tinfo.eThresholdType = ThresholdInfo->eThresholdType;
	AlgoDetectorThrMsg.tinfo.eFrameType = ThresholdInfo->eFrameType;

	buffer += sizeof(TAlgoDetectorThrInfo);
	size -= sizeof(TAlgoDetectorThrInfo);

	AlgoDetectorThrMsg.tinfo.Data.clear();


	for(uint32_t i = 0 ; i < size ; i += 4 )
	{
		uint32_t value = (buffer[i+3] << 24) | (buffer[i+2] << 16) | (buffer[i+1] << 8) | (buffer [i]);
		AlgoDetectorThrMsg.tinfo.Data.push_back(value);
	}
}


/*
   This is the main radar callback function. It is called by theradar framework
   when data from the radar is ready to be processed
*/
static uint64_t time_offset = 0;
std_msgs::String status_message;
void STD_CALL radar_callback(int32_t type, uint8_t* buffer, uint32_t size)
{
	static uint8_t ucLastPacket=1;
	TSlamMetadata * tSalmMetadata;
	switch(type)
	{
		case Status_Output:
			ROS_DEBUG("Received a status output message from the radar\n");
			break;
		case MemoryOperation_Output:
			ROS_DEBUG("Received a memory read output message of %d bytes from the radar\n",size);
			ctrlDebugMsgRecievedCb(buffer,size);
			break;
		case PointCloud_Output_V1_1:
			PcFrameData.data.insert(PcFrameData.data.begin(),buffer,buffer+size);
			arbe_pc_frame_pub.publish(PcFrameData);
			PcFrameData.data.clear();
			break;
		case Slam_Output_V1_1:
			ucLastPacket =  ((TSlamHeader_V1_1*)buffer)->last_packet;
			usFrameCounter = ((TSlamHeader_V1_1*)buffer)->frame_counter;
			tSalmMetadata = &((TSlamHeader_V1_1*)buffer)->meta_data;

			buffer = buffer + sizeof(TSlamHeader_V1_1);
			dataSlamObjectsMsgRecievedCb(buffer,size);
			if (ucLastPacket == 1)
			{
				ROS_DEBUG("Slam 1.1-> Frame Number=%d Targets=%d",usFrameCounter,countObjects);
				countObjects = 0;
				/* Publish the slam data */
				ROS_DEBUG("publisher-> slam -> data size =%d",(int)ObjectsPerFrame.data.size());
				arbe_slam_pub.publish(ObjectsPerFrame);
				/* Clear buffer */
				ObjectsPerFrame.data.clear();
			}
			break;
		case Response_Output:
		{
			uint32_t unTimeLsb =  ((TRAF_API_Header*)buffer)->unTimeLsb;
			uint32_t unTimeMsb =  ((TRAF_API_Header*)buffer)->unTimeMsb;
			uint64_t radar_time = (((((uint64_t)unTimeMsb) << 32)) | ((uint64_t)unTimeLsb))>>16;
			TPointCloud_V1_1* tPointCloud_V1_1= (TPointCloud_V1_1 *)(uint8_t*)(&(buffer));

			radar_time = (((((uint64_t)tPointCloud_V1_1->unTimeMsb) << 32)) | ((uint64_t)tPointCloud_V1_1->unTimeLsb))>>16 ;//+ time_offset;

			time_offset = uint64_t(ros::Time::now().toSec()*1000-radar_time);
			break;
		}
		case DspResponse_Output:
			DspJobMsgRecievedCb(buffer,size);
			ROS_DEBUG("publisher-> DspJob -> size =%d",(int)DspJobData.data.size());
			arbe_dspjob_pub.publish(DspJobmsg);
			break;
		case RangeThreshold_output:
			if(0){
					ROS_DEBUG("Received a range threshold output message from the radar\n");
			    //ThresholdMsgRecievedCb(buffer,size);
					arbe_thresholds_detections_pub.publish(Thresholdsmsg);
			}
			break;

		case NoiseVector_Output:
		{
			buffer += sizeof(TRAF_API_Header);
			size -= sizeof(TRAF_API_Header);
			std_msgs::UInt32MultiArray noise_vector_arr;
			noise_vector_arr.data.insert(noise_vector_arr.data.begin(),(uint32_t*)buffer,(uint32_t*)buffer+size/4 );
			noise_vector_pub.publish(noise_vector_arr);
			break;
		}
		case AlgoDetectorThr_Output:
		{
			AlgoDetectorThresholdsRecievedCb(buffer,size);
			algo_detector_thr_pub.publish(AlgoDetectorThrMsg);
			break;
		}
		default:
			break;
	}

}

void STD_CALL radar_point_cloud_callback(const std_msgs::UInt8MultiArray::ConstPtr& buffer)
{
	arbe_msgs::arbePcBins pcBins;
	arbe_msgs::arbePcFrameHeader pcHeader;
//	arbe_msgs::arbePcRes pcResolution;
	arbe_msgs::arbePcMetadata pcMetadata;


	uint32_t index =0;
	int32_t length = buffer->data.size();
	TPointCloud_V1_1* tPointCloud_V1_1= (TPointCloud_V1_1 *)(uint8_t*)(&(buffer->data[index]));

	pcHeader.time = (((((uint64_t)tPointCloud_V1_1->unTimeMsb) << 32)) | ((uint64_t)tPointCloud_V1_1->unTimeLsb))>>16 ;//+ time_offset;
	pcHeader.frame_counter = tPointCloud_V1_1->usFrameCounter;
	pcHeader.crd_count =tPointCloud_V1_1->usCrdCount;
	pcHeader.number_of_points=0;
	pcHeader.udp_lost_packages =0;
	pcHeader.is_4d = RAF_COM_CLAC_Is4d(&tPointCloud_V1_1->tMetadata);
	pcHeader.frame_type = RAF_COM_CLAC_GetFrameNumber((uint8_t*)tPointCloud_V1_1);
	pcMetadata.PcResolution.range_resolution = RAF_COM_CALC_CalcRangeCoefficient(tPointCloud_V1_1->tMetadata.range);
	pcMetadata.PcResolution.doppler_resolution = RAF_COM_CALC_CalcDopplerCoefficient(tPointCloud_V1_1->tMetadata.doppler);
	pcMetadata.PcResolution.azimuth_coefficient = RAF_COM_CALC_CalcAzimuthCoefficient(tPointCloud_V1_1->tMetadata.azimuth);
	pcMetadata.PcResolution.elevation_coefficient = RAF_COM_CALC_CalcElevationCoefficient(tPointCloud_V1_1->tMetadata.elevation);
//	pcResolution.power_factor = POW_TO_DB_RATIO;

	pcMetadata.range_offset = RAF_COM_CALC_RangeOffset(tPointCloud_V1_1->tMetadata.range);
	pcMetadata.range_res_in_db = RAF_COM_CALC_ClacRangeResInDb(&tPointCloud_V1_1->tMetadata);
	pcMetadata.azimuth_fft_size = tPointCloud_V1_1->tMetadata.azimuth.tFields.fftRange;
	pcMetadata.elevation_fft_size = tPointCloud_V1_1->tMetadata.elevation.tFields.fftRange;
	pcMetadata.azimuth_fft_padding = tPointCloud_V1_1->tMetadata.azimuth.tFields.padding;
	pcMetadata.elevation_fft_padding = tPointCloud_V1_1->tMetadata.elevation.tFields.padding;

	uint32_t BytesLengthOfTraget = RAF_COM_CALC_GetBytesLengthOfTraget(tPointCloud_V1_1->ucTrgtFmtFrmType);
	uint32_t messageCounter =0;

	while(length>0)
	{

		tPointCloud_V1_1 = (TPointCloud_V1_1*)(&(buffer->data[index]));
		if (messageCounter != tPointCloud_V1_1->usMessageNumber)
		{
			pcHeader.udp_lost_packages+=(tPointCloud_V1_1->usMessageNumber - messageCounter);
			messageCounter = tPointCloud_V1_1->usMessageNumber;
		}

		messageCounter++;

		for(uint32_t i = sizeof(TPointCloud_V1_1) ; i < tPointCloud_V1_1->unLength ; i+=BytesLengthOfTraget)
		{
			TargetGeneric tTargetGeneric = GetPhysicalBin((uint8_t *)&(buffer->data[index + i]),BytesLengthOfTraget,&tPointCloud_V1_1->tMetadata);

			pcBins.azimuth_signed_bin.push_back((int8_t)tTargetGeneric.azimuth);
			pcBins.range_bin.push_back((uint16_t)tTargetGeneric.range);
			pcBins.doppler_signed_bin.push_back((int16_t)tTargetGeneric.doppler);
			pcBins.elevation_signed_bin.push_back((int8_t)tTargetGeneric.elevation);
			pcBins.power.push_back((int16_t)tTargetGeneric.power);

			if(BytesLengthOfTraget == BIN_DATA_WITH_PHASE_SIZE_BYTES || BytesLengthOfTraget == BIN_DATA_COMPRESSED_WITH_PHASE_SIZE_BYTES)
			{
				pcBins.phase.push_back((int16_t)tTargetGeneric.phase);
			}
			else
			{
				pcBins.phase.push_back((int16_t)(0)); // No phase data
			}

		}



		pcHeader.number_of_points += (tPointCloud_V1_1->unLength - sizeof(TPointCloud_V1_1))/BytesLengthOfTraget; // Debug reasons

		index  += tPointCloud_V1_1->unLength;
		length -= tPointCloud_V1_1->unLength;

	}

	PointCloudMsg.RosHeader.stamp = ros::Time::now();

	/* Use the ROS header frame_is field to mark the radar index and rotation angle - used by the GUI */
	PointCloudMsg.RosHeader.frame_id = '{' + std::to_string(arg_radar_id) +
		"}{" + std::to_string(radar_yaw_angle) +
		"}{" + std::to_string(radar_pitch_angle) +
		"}{" + std::to_string(radar_x_offset) +
		"}{" + std::to_string(radar_y_offset) +
		"}{" + std::to_string(radar_z_offset) + "}";
	PointCloudMsg.PcHeader = pcHeader;
	PointCloudMsg.PcMetadata = pcMetadata;
	PointCloudMsg.Points = pcBins;

	/* Randomal delay to try and avoid frames from multiple radar at the same time, what causes some flicker in the display */
	float r2 = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/0.2));
	//ROS_INFO("sleep = %lf\n\r",r2);
	//sleep(r2);
	arbe_bin_detections_pub.publish(PointCloudMsg);

	/* Create and publish rviz pointcloud2 frame only if slam is not active */
//	if (get_disp_processed_pc() == 0)
//	{
//		pointcloud_display_handler();
//	}
}

/*
   provides the system time needed by the radar framework (RAF)
*/
unsigned long STD_CALL GetSystemTime()
{
	time_t tt;
	time( &tt );
	tt = tt + 8*3600;  // transform the time zone
	tm* t= gmtime( &tt );
	return tt;
}

void load_settings()
{
	QSettings settings("Arbe", "Arbe ROS GUI");
	//data_port = settings.value("data port","6003").toString();
	//ROS_INFO("Radar Data UDP port: %s",data_port.toStdString().c_str());
}

/*
  Configure radar pointcloud parameters
*/
void radar_configure_pc()
{
	int phase_enabled = 0;
	ROS_INFO("Configure Pointcloud - Phase Enabled = %d.",phase_enabled);

	/* Configure pointcloud output type */
	TConfigurePointCloudsInfo pointCloudInfo;
	pointCloudInfo.pucDestinationAddress = data_buffer;
	pointCloudInfo.unDestinationBufferSize = MAX_BUFFER_LENGTH;
	pointCloudInfo.unByteAlignment = 1;
	int phase_config =phase_enabled ? 0 : 1;
	pointCloudInfo.unPacketFormat = (uint8_t)phase_config + 2; // 3 for 80 Bit uncompressed(48Bit), 2 for 96 Bit uncompressed(56Bits - with phase)
	RAF_API_EXT_SysCfg_ConfigurePcOutput(&tArbeApiMailBox,&pointCloudInfo );

    //new API
    TConfigPCInfo tConfigPCInfo;
    if (sizeof(char *) == 8) // 64 bit pointers
    {
        tConfigPCInfo.unDestAdrLsb  = (uint32_t)((uint64_t)data_buffer & (uint64_t)0x00000000ffffffff);
        tConfigPCInfo.unDestAdrMsb  = (uint32_t)((uint64_t)data_buffer >> 32);
    } else
    {
        tConfigPCInfo.unDestAdrLsb  = (uint32_t)((uint64_t)data_buffer & (uint64_t)0x00000000ffffffff);
        tConfigPCInfo.unDestAdrMsb  = 0;
    }
    tConfigPCInfo.unDestBufSize     = MAX_BUFFER_LENGTH;
    RAF_API_EXT_SysCfg_ConfigPcBufOutput(&tArbeApiMailBox,&tConfigPCInfo);
}

Color_Coding_Min_Max* Color_Coding_Min_Max::m_pInstance = NULL;

Color_Coding_Min_Max::Color_Coding_Min_Max()
{}

Color_Coding_Min_Max* Color_Coding_Min_Max::Instance()
{
    if (!m_pInstance)
        m_pInstance = new Color_Coding_Min_Max;

    return m_pInstance;
					}

void Color_Coding_Min_Max::set_min(std::string ColoringType, float min)
{
    if ( strcmp(ColoringType.c_str(), "Doppler") == 0 )
    {
        dopp_cc_min = min;
	}
    else if ( strcmp(ColoringType.c_str(), "Doppler Gradient") == 0 )
    {
        dopp_grad_cc_min = min * 4;
    }
    else if ( strcmp(ColoringType.c_str(), "Elevation") == 0 )
    {
        //el_cc_min = min * 3;
        el_cc_min = 1.5 * min + 5;

    }
    else //amplitude
    {
        amp_cc_min = (min + 15) * 8 ;
    }
}

void Color_Coding_Min_Max::set_max(std::string ColoringType, float max)
{
    if ( strcmp(ColoringType.c_str(), "Doppler") == 0 )
    {
        dopp_cc_max = max;
			}
    else if ( strcmp(ColoringType.c_str(), "Doppler Gradient") == 0 )
    {
        dopp_grad_cc_max = max * 4;
		}
    else if ( strcmp(ColoringType.c_str(), "Elevation") == 0 )
    {
        el_cc_max = 1.5 * max + 5;//max * 3;
    }
    else //amplitude
    {
        amp_cc_max = (max + 15) * 8 ;
    }
}


void Color_Coding_Min_Max::get_values(std::string ColoringType, float &min, float &max)
{
    if ( strcmp(ColoringType.c_str(), "Doppler") == 0 )
    {
        min = dopp_cc_min;
        max = dopp_cc_max;
    }
    else if ( strcmp(ColoringType.c_str(), "Doppler Gradient") == 0 )
    {
        min = dopp_grad_cc_min;
        max = dopp_grad_cc_max;
    }
    else if ( strcmp(ColoringType.c_str(), "Elevation") == 0 )
    {
        min = el_cc_min;
        max = el_cc_max;
    }
    else //amplitude
    {
        min = amp_cc_min;
        max = amp_cc_max;
    }
}

void Color_Coding_Min_Max::get_converted_values(std::string ColoringType, float &min, float &max)
	{ 
    if ( strcmp(ColoringType.c_str(), "Doppler") == 0 )
    {
        min = dopp_cc_min;
        max = dopp_cc_max;
	} 
    else if ( strcmp(ColoringType.c_str(), "Doppler Gradient") == 0 )
    {
        min = dopp_grad_cc_min/4;
        max = dopp_grad_cc_max/4;
    }
    else if ( strcmp(ColoringType.c_str(), "Elevation") == 0 )
    {
        min = el_cc_min/1.5 - 5/1.5;//el_cc_min/3;
        max = el_cc_max/1.5 - 5/1.5;//el_cc_max/3;
    }
    else //amplitude
    {
        min = amp_cc_min/8 - 15;
        max = amp_cc_max/8 - 15;
    }
}

std::string Color_Coding_Min_Max::get_units(std::string ColoringType)
{
    if ( strcmp(ColoringType.c_str(), "Doppler") == 0 )
        return "m/s";
    else if ( strcmp(ColoringType.c_str(), "Doppler Gradient") == 0 )
        return "m/s";
    else if ( strcmp(ColoringType.c_str(), "Elevation") == 0 )
        return "m";
    else //amplitude
        return "dB";
}

/* 
   Color the pointcloud detections based on the selected coloring scheme
*/

const int n_colors_jet = 4;
static float color_jet[n_colors_jet][3] = { {0,0,255}, {0,255,0}, {255,255,0}, {255,0,0} };

float normalize(float x, float min, float span)
{
	return (x-min)/span;
}

void apply_color(float z, uint8_t &r, uint8_t &g, uint8_t &b)
{
	int idx1;        // |-- Our desired color will be between these two indexes in "color".
	int idx2;        // |
	float fractBetween = 0;  // Fraction between "idx1" and "idx2" where our value is.


	if(z <= 0) {  idx1 = idx2 = 0; }    // accounts for an input <=0
	else if(z >= 1)  {  idx1 = idx2 = n_colors_jet-1; }    // accounts for an input >=0
	else
	{
		z = z * (n_colors_jet-1);        // Will multiply value by 3.
		idx1  = uint16_t(z); // Our desired color will be after this index.
		idx2  = (idx1+1);                        	// ... and before this index (inclusive).
		fractBetween = z - float(idx1); // Distance between the two indexes (0-1).
	}
	r = (color_jet[idx2][0] - color_jet[idx1][0])*fractBetween + color_jet[idx1][0];
	g = (color_jet[idx2][1] - color_jet[idx1][1])*fractBetween + color_jet[idx1][1];
	b = (color_jet[idx2][2] - color_jet[idx1][2])*fractBetween + color_jet[idx1][2];
}

void pointcloud_color( pcl::PointCloud<ArbePointXYZRGBGeneric> &buffer, size_t j, float cc_min, float cc_max, float cc_span) // NEW FUNCTION!
{
	if (std::isnan(buffer.points[j].z)) buffer.points[j].z = 0;
	if ( strcmp(ColoringType.c_str(), "Doppler") == 0 )
	{
		/* Color by doppler */
		float rel_speed = (buffer.points[j].doppler + ego_velocity* cos(buffer.points[j].azimuth));
		if (rel_speed > cc_max) {
			buffer.points[j].r = 0;
			buffer.points[j].g = 0;
			buffer.points[j].b = 255;
		}
		else if (rel_speed < cc_min) {
			buffer.points[j].r = 255;
			buffer.points[j].g = 0;
			buffer.points[j].b = 0;
		}
		else {
			buffer.points[j].r = 0;
			buffer.points[j].g = 255;
			buffer.points[j].b = 0;
		}
	}
	else
	{
		float x, z;
		if ( strcmp(ColoringType.c_str(), "Doppler Gradient") == 0 )
		{
			x = (buffer.points[j].doppler +  ego_velocity* cos(buffer.points[j].azimuth)) ;
		}
		else if ( (strcmp(ColoringType.c_str(), "Amplitude") == 0)  || (strcmp(ColoringType.c_str(), "Amplitude-Flat") == 0 )
			|| (strcmp(ColoringType.c_str(), "Range/Doppler") == 0 ))
		{
			x = buffer.points[j].power;
		}
		else
		{
			x = buffer.points[j].z;
		}

		z = normalize(x, cc_min, cc_span);
		apply_color(z,buffer.points[j].r,buffer.points[j].g,buffer.points[j].b);
	}
	buffer.points[j].a = 0.2;
}

bool get_floating_text_enabled()
{
	return isFloatingText_enabled;
}

/*
   Calculate elaped time between frames
*/
float calculate_fps( uint64_t timestamp_ms )
{
	float frames_per_second;
	usCurrentTime = timestamp_ms;
	frames_per_second = (1000.0)/(usCurrentTime-usPreviousTime) ; // delta time is in milliseconds
	usPreviousTime = usCurrentTime;
	return frames_per_second;
}


/* Used for pointcloud sorting */
bool CompareRecords(const ArbePointXYZRGBGeneric& a, const ArbePointXYZRGBGeneric& b)
{
    if (a.azimuth < b.azimuth)
        return true;
    else if (a.azimuth > b.azimuth)
        return false;

    else if (a.range > b.range)
        return false;

    return false;
}

/* Filter the pointcloud for showing the strongest target in case 
   multiple targets are detected at the same location. we flaten the 
   view, so elevation is not counted */
void pointcloud_filter_amplitude ( size_t* array_size )
{
	/* Sort the array according to range, azimuth */
	std::sort (cloud.points.begin(), cloud.points.end(),CompareRecords);

	/* traverse through the array and pick from adjacent detections the one with the highest power. remove the rest */
	for (auto target_it = cloud.points.begin()+1;target_it <= cloud.points.end(); target_it++) 
	{
		if ( (target_it->range == (target_it-1)->range ) && (target_it->azimuth ==  (target_it-1)->azimuth) )
		{
			/* more then one target at same location! */
			if (target_it->power > (target_it-1)->power)
			{
				/* stronger target at the same location remove the weaker one */
				cloud.points.erase(target_it-1);
				*array_size = *array_size - 1;
			}
		}
	}
}

void extract_radar_orientation ( const arbe_msgs::arbeNewPcMsg pcMsg )
{
	arbe_msgs::arbeNewPcMsg PcFrame;
	PcFrame = arbe_msgs::arbeNewPcMsg(pcMsg);
	
	std::stringstream iss(PcFrame.RosHeader.frame_id);
	iss << PcFrame.RosHeader.frame_id;

	/* Extract radar ID and rotation angle from ROS header */
	iss.ignore(1, '{');
	iss >> radar_id;
	iss.ignore(1, '}');
	iss.ignore(1, '{');
	iss >> radar_yaw_angle;
	iss.ignore(1, '}');
	iss.ignore(1, '{');
	iss >> radar_pitch_angle;
	iss.ignore(1, '}');
	iss.ignore(1, '{');
	iss >> radar_x_offset;
	iss.ignore(1, '}');
	iss.ignore(1, '{');
	iss >> radar_y_offset;
	iss.ignore(1, '}');
	iss.ignore(1, '{');
	iss >> radar_z_offset;
	iss.ignore(1, '}');

	ROS_INFO("radar id: %d yaw angle %lf pitch angle %lf x offset %lf y offset %lf z offset %lf\n\r",radar_id, radar_yaw_angle,
		radar_pitch_angle, radar_x_offset, radar_y_offset, radar_z_offset);
}

void targets_read_callback(const arbe_msgs::arbeNewPcMsg pcMsg)
{
	if ( ( tx_started == false ) && (get_disp_processed_pc() == 0) )
	{
		PointCloudMsg = arbe_msgs::arbeNewPcMsg(pcMsg);

		extract_radar_orientation(PointCloudMsg);
		//ROS_INFO("Radar node -> targets_read_callback -> radar_id=%d\n\r",radar_id);
//		if ( radar_id == arg_radar_id )
//		{
//			ROS_DEBUG("Radar node %d ->  Received rosbag target message", radar_id);
//			pointcloud_display_handler();
//		}
	}
}


void pub_radar_calibration_data()
{
	ROS_INFO("pub_radar_calibration_data. radar id = %d yaw=%lf pitch = %lf",arg_radar_id, arg_radar_yaw_angle, arg_radar_pitch_angle);
	controls_data.radar_id = arg_radar_id;
	controls_data.radar_x_offset = arg_radar_x_offset;
	controls_data.radar_y_offset = arg_radar_y_offset;
	controls_data.radar_z_offset = arg_radar_z_offset;
	controls_data.radar_yaw_angle = arg_radar_yaw_angle * M_PI / 180;
	controls_data.radar_pitch_angle = arg_radar_pitch_angle * M_PI / 180;
	arbe_radar_calibration_pub.publish(controls_data);
}

/* Main function */
int main(int argc, char **argv)
{
	pthread_t ctrl_thread_id;
	pthread_t radar_data_thread_id;
	int ret;
	int one = 1;
	struct sockaddr_in data_serv_addr;

	ros::init(argc, argv, "arbe_pointcloud_receiver");
	ros::Time::init();


	ros::NodeHandle n("~");// = boost::make_shared<ros::NodeHandle>();

	arg_radar_yaw_angle = 0.0;

	n.getParam("Radar_ID", arg_radar_id);
	n.getParam("Radar_IP", arg_radar_ip);
	n.getParam("Ctrl_Port", arg_ctrl_port);
	n.getParam("Data_Port", arg_data_port);
	n.getParam("Logs_Port", arg_logs_port);
	n.getParam("Radar_Yaw_Angle", arg_radar_yaw_angle);
	n.getParam("Radar_Pitch_Angle", arg_radar_pitch_angle);
	n.getParam("Radar_X_Offset", arg_radar_x_offset);
	n.getParam("Radar_Y_Offset", arg_radar_y_offset);
	n.getParam("Radar_Z_Offset", arg_radar_z_offset);
	n.getParam("Radar_base_freq", radar_base_freq);

	n.getParam("console_level", console_level);
	ros::console::levels::Level CL = ros::console::levels::Warn;
	if(console_level<ros::console::levels::Count && console_level>=ros::console::levels::Debug)
		CL = (ros::console::levels::Level)console_level;
	if (ros::console::set_logger_level(ROSCONSOLE_DEFAULT_NAME, CL))//Debug))
	{
		ros::console::notifyLoggerLevelsChanged();
	}

	/* Create a ROS publisher for GUI settings send to the radar nodes */
	arbe_radar_calibration_pub = n.advertise<arbe_msgs::arbeGUIsettings>("/arbe/settings/radar_calibration",1,true);

	arg_radar_yaw_angle *= M_PI/180;
	radar_yaw_angle = arg_radar_yaw_angle;
	radar_pitch_angle = arg_radar_pitch_angle;
	radar_x_offset = arg_radar_x_offset;
	radar_y_offset = arg_radar_y_offset;
	radar_z_offset = arg_radar_z_offset;

	arbe_pc_frame_pub = n.advertise<std_msgs::UInt8MultiArray>("arbe/api/pointcloud/"+std::to_string(arg_radar_id),1);

	arbe_bin_detections_pub = n.advertise<arbe_msgs::arbeNewPcMsg>("/arbe/parsed/pointcloud/"+std::to_string(arg_radar_id),1);
	arbe_slam_pub = n.advertise<std_msgs::UInt8MultiArray>("/arbe/parsed/objects/"+std::to_string(arg_radar_id),1);
	arbe_dspjob_pub = n.advertise<arbe_msgs::arbeDspJobmsg>("/arbe/parsed/dspjob/"+std::to_string(arg_radar_id),1);
	arbe_thresholds_detections_pub = n.advertise<arbe_msgs::arbeThresholds>("/arbe/parsed/thresholds/"+std::to_string(arg_radar_id),1);
	arbe_status_pub = n.advertise<std_msgs::String>("/arbe/raw/status",10);
	arbe_binary_pcl_pub = n.advertise<std_msgs::UInt8MultiArray>("/arbe/raw/radar_tx/"+std::to_string(arg_radar_id),1);
	arbe_pcl_pub = n.advertise<sensor_msgs::PointCloud2>("/arbe/rviz/pointcloud_"+std::to_string(arg_radar_id), 1);
	//arbe_info_markers = n.advertise<visualization_msgs::Marker>("/arbe/rviz/car_marker", 1);
	arbe_debug_data_count_pub = n.advertise<std_msgs::UInt32MultiArray>("/arbe/raw/debug_progress",1);
	algo_detector_thr_pub = n.advertise<arbe_msgs::arbeTAlgoDetectorThrOutput>("/arbe/parsed/algo_detector_thr/"+std::to_string(arg_radar_id),10);

	/* Initialize the radar callback */
	RAF_DRV_CallbackInit(ctrl_buffer, CTRL_BUFFER_LENGTH, radar_callback);// ask Dror

	/* Initialize API mailbox */
	tArbeApiMailBox.sendCmd = TransmitBuffer;
	tArbeApiMailBox.sysCfg_GetSystemTimeMs = GetSystemTime;

	radar_configure_pc();

	/* Read Data Port number from system settings file */
	//load_settings();

	/* spawn the TCP control thread */
	ret=pthread_create(&ctrl_thread_id,NULL,&ctrl_thread,NULL);

	/* spawn the radar thread */
	ret=pthread_create(&radar_data_thread_id,NULL,&radar_data_thread,NULL);

	pc_frame_sub = n.subscribe("arbe/api/pointcloud/"+std::to_string(arg_radar_id), 1,radar_point_cloud_callback);
	ros::Subscriber arbe_eth_logger_sub = n.subscribe("/arbe/raw/radar_logs_over_eth/"+std::to_string(arg_radar_id),100,radar_logger_CB);
	ros::Subscriber arbe_eth_logger_old_sub = n.subscribe("/arbe/raw/radar_logs_over_eth",100,radar_logger_CB);

	ctrl_sub = n.subscribe("/arbe/raw/ctrl_status", 10, ctrl_message_callback);

	gui_commands_sub = n.subscribe("/arbe/settings/gui_commands", 1, gui_message_callback);
	gui_controls_sub = n.subscribe("/arbe/settings/gui_controls", 1, gui_controls_callback);

	rosbag_sub = n.subscribe("/arbe/raw/radar_tx/"+std::to_string(arg_radar_id), 1, bin_pointcloud_read_callback);
	rosbag_sub_single_radar = n.subscribe("/arbe/raw/radar_tx", 1, bin_pointcloud_read_callback); // old recording with single radar version
//	targets_sub = n.subscribe("/arbe/parsed/pointcloud", 1, targets_read_callback);

	//prepare_basic_markers();

	/* Publish this radar node calibration data so it can be picked up by the GUI */
	//pub_radar_calibration_data();

	sleep(2);
	if ( radar_connect() == 0 )
	{
		TSelectActiveSeqInfo sequenceInfo;
		sequenceInfo.eSequenceType = FineMidSeq; // Default to 4d mid if not defined
		RAF_API_RdrCtrl_SetActiveSeq(&tArbeApiMailBox,&sequenceInfo);
		status_message.data.append("alive");
		arbe_status_pub.publish(status_message);
		status_message.data.clear();
	}
	else
	{
		ROS_WARN("Failed to connect to a radar!");
	}

	sleep(1);
	/* Publish this radar node calibration data so it can be picked up by the GUI */
	//pub_radar_calibration_data();

	ros::Rate loop_rate(100);
	while (ros::ok() && (terminating == false) )
	{
		ros::spinOnce();
		loop_rate.sleep();
	}

	radar_disconnect();
	pthread_exit(NULL);
	ret = system("rosnode kill --all");
}
