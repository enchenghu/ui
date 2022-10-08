#pragma once

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>
#include<stdio.h> 
#include<stdlib.h> 
#include<string.h> 
#include<errno.h> 
#include<sys/types.h> 
#include<netinet/in.h> 

namespace autox_msgs
{
typedef struct 
{
	uint32_t indensity_0 = 0;
	uint32_t indensity_1 = 0;
	double distance = 0;
	double speed = 0;
	uint32_t reserved = 0;

} autoxFMCWPc_t;

}

