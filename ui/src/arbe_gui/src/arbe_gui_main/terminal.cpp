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

#include <ros/console.h>
#include <ros/ros.h>
#include <sys/ioctl.h>
#include <string.h>
#include <iostream>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <QPushButton>
#include <std_msgs/String.h>

using namespace std;

extern void radar_disconnect();
extern void radar_stop_transmit();
extern int terminating;
bool logging_enabled;
extern QPushButton *radar_connect_button;
extern QPushButton *radar_start_stop_button;
extern bool radar_playing;
int fd;
const char RecordFileName[40] = "arbe-radar-log";
FILE * log_file;
QString radar_debug_port;

/*
   Implement system software reset and uart logging
*/

#define RESET_TTY_DEVICE	"/dev/ttyUSB1"

#define IOCTL_GPIOGET		0x8000
#define IOCTL_GPIOSET 		0x8001

#define SYSTEM_RESET_VALUE  0x80008040 // GPIO15=1, GPIO6=0
#define SYSTEM_ACTIVE_VALUE 0x00408040 // GPIO15=0, GPIO6=1	

int set_interface_attribs(int fd, int speed)
{
	struct termios tty;

	if (tcgetattr(fd, &tty) < 0) {
		ROS_DEBUG("Error from tcgetattr: %s\n", strerror(errno));
		return -1;
	}

	cfsetospeed(&tty, (speed_t)speed);
	cfsetispeed(&tty, (speed_t)speed);

	tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
	tty.c_cflag &= ~CSIZE;
	tty.c_cflag |= CS8;         /* 8-bit characters */
	tty.c_cflag &= ~PARENB;     /* no parity bit */
	tty.c_cflag &= ~CSTOPB;     /* only need 1 stop bit */
	tty.c_cflag &= ~CRTSCTS;    /* no hardware flowcontrol */

	/* setup for non-canonical mode */
	tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
	tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	tty.c_oflag &= ~OPOST;

	/* fetch bytes as they become available */
	tty.c_cc[VMIN] = 1;
	tty.c_cc[VTIME] = 1;

	if (tcsetattr(fd, TCSANOW, &tty) != 0) {
		ROS_DEBUG("Error from tcsetattr: %s\n", strerror(errno));
		return -1;
	}
	return 0;
}

void set_mincount(int fd, int mcount)
{
	struct termios tty;

	if (tcgetattr(fd, &tty) < 0) {
		ROS_DEBUG("Error tcgetattr: %s\n", strerror(errno));
		return;
	}

	tty.c_cc[VMIN] = mcount ? 1 : 0;
	tty.c_cc[VTIME] = 5;        /* half second timer */

	if (tcsetattr(fd, TCSANOW, &tty) < 0)
		ROS_DEBUG("Error tcsetattr: %s\n", strerror(errno));
}

void log_start_recording( void )
{
	char FileName[40];
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	sprintf(FileName,"%s-%d%02d%02d-%02d%02d%02d.log",
		RecordFileName, tm.tm_year + 1900,
		tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min,
		tm.tm_sec);

	printf("Log File=%s\n\r",FileName);
	log_file = fopen(FileName, "wb");
	if (log_file != NULL) {
		printf("start recording radar serial log.\n\r");
	}
}

void log_stop_recording( void )
{
	printf("stop recording radar serial log.\n\r");
	fclose(log_file);
}

void *radar_logger_thread(void* args)
{
	ros::NodeHandlePtr n = boost::make_shared<ros::NodeHandle>();
	ros::Publisher arbe_serial_pub = n->advertise<std_msgs::String>("/arbe/raw/serial_console",1);
	std_msgs::String serial_data;

	ros::Rate loop_rate(100);
	fd = open(radar_debug_port.toStdString().c_str(), O_RDWR | O_NOCTTY | O_SYNC);
	if (fd < 0) {
		ROS_DEBUG("Error opening %s: %s\n", radar_debug_port.toStdString().c_str(), strerror(errno));
		return NULL;
	}
	/*baudrate 115200, 8 bits, no parity, 1 stop bit */
	set_interface_attribs(fd, B115200);

	log_start_recording();

	/* simple noncanonical input */
	while (ros::ok() && logging_enabled && (!terminating))
	{
		unsigned char buf[200];
		int rdlen;

		rdlen = read(fd, buf, sizeof(buf) - 1);
		if (rdlen > 0) {
			buf[rdlen] = 0;
			fwrite(buf, rdlen, 1, log_file);

			std::stringstream ss;
			ss << buf << rdlen;

			/* Publish the serial data as /arbe_serial_data rosbag topic */
			serial_data.data = ss.str();
			arbe_serial_pub.publish(serial_data);
			//ROS_DEBUG("%s", serial_data.data.c_str());
		} else if (rdlen < 0) {
			ROS_DEBUG("Error from read: %d: %s try to reopen the port\n", rdlen, strerror(errno));
			close(fd);
			sleep(5);
			fd = open(radar_debug_port.toStdString().c_str(), O_RDWR | O_NOCTTY | O_SYNC);
			if (fd < 0) {
				ROS_DEBUG("Error opening %s: %s\n", radar_debug_port.toStdString().c_str(), strerror(errno));
			}
			/*baudrate 115200, 8 bits, no parity, 1 stop bit */
			set_interface_attribs(fd, B115200);
			
		} else {  /* rdlen == 0 */
			ROS_DEBUG("Timeout from read\n");
			close(fd);
			sleep(3);
			fd = open(radar_debug_port.toStdString().c_str(), O_RDWR | O_NOCTTY | O_SYNC);
			if (fd < 0) {
				ROS_DEBUG("Error opening %s: %s\n", radar_debug_port.toStdString().c_str(), strerror(errno));
				return NULL;
			}
		}               
		/* repeat read to get full message */
		ros::spinOnce();
		loop_rate.sleep();
	}
	log_stop_recording();
	close(fd);
}
