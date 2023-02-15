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

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include "mainwindow.h"
#include "viewpanel.h"
#include "Utils.h"
#include <arbe_msgs/arbeSlamMsg.h>
#include <sensor_msgs/Imu.h>
#include <geometry_msgs/TwistWithCovarianceStamped.h>
#include "common.hpp"

//static std::vector<TSlamObj> ObjectsPerFrame;
static arbe_msgs::arbeSlamMsg::ConstPtr slamMsg;
static bool usedForDisplay = false;

static uint8_t  slam_server_reply[MAX_BUFFER_LENGTH];
static float host_vel; // Ego Velocity
static uint16_t frame_num; 
static bool SlamLastPacket = false;
static std::mutex g_objects_mutex;

static bool cameraBuffering;
static bool slamDisplayOnCam;
static bool slamOnCamEcoMode;
static bool colorObjectsByClass = false;
static bool disp_dist_on_cam;
static bool reset_local_frame = true;

static int16_t classes_to_show = -1;

static float dview_x = 0;
static float dview_y = 0;

bool get_slam_validity()
{
    if(slamMsg)
        return true;
    return false;
}

arbe_msgs::arbeTSlamMetadata get_slam_metadata()
{
	return slamMsg->meta_data;
}

bool get_cameraBuffering()
{
	return cameraBuffering;
}
void set_cameraBuffering(bool flag)
{
	cameraBuffering = flag;
}

bool get_slamOnCam()
{
	return slamDisplayOnCam;
}
void set_slamOnCam(bool flag)
{
	slamDisplayOnCam = flag;
}
bool get_slamOnCam_ecoMode()
{
	return slamOnCamEcoMode;
}
void set_slamOnCam_ecoMode(bool flag)
{
	slamOnCamEcoMode = flag;
}
bool get_colorObjByClass()
{
    return colorObjectsByClass;
}
void set_colorObjByClass(bool flag)
{
    colorObjectsByClass = flag;
}
bool get_distOnCam()
{
	return disp_dist_on_cam;
}
void set_distOnCam(bool flag)
{
	disp_dist_on_cam = flag;
}


float get_dview_x()
{
    return dview_x;
}
void set_dview_x(float val)
{
    dview_x = val;
}

float get_dview_y()
{
    return dview_y;
}
void set_dview_y(float val)
{
    dview_y = val;
}

static bool isFreeSpace_enabled = true;
bool get_FreeSpace_enabled()
{
	return isFreeSpace_enabled;
}
void set_FreeSpace_enabled(bool flag)
{
	isFreeSpace_enabled = flag;
}

static bool isFunnel_enabled = true;
bool get_funnel_enabled()
{
	return isFunnel_enabled;
}
void set_funnel_enabled(bool flag)
{
	isFunnel_enabled = flag;
}

static bool dl_training_playback = false;
void set_dl_training_playback(bool flag)
{
	dl_training_playback = flag;
}
bool get_dl_training_playback()
{
	return dl_training_playback;
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




static bool aggregate_pc = false;
void set_aggregation(bool agg)
{
	aggregate_pc = agg;
}
bool get_aggregation()
{
	return aggregate_pc;
}

static bool localize_enable = false;
void set_localization_enable_disable(bool loc)
{
	localize_enable = loc;
}
bool get_localization_enable()
{
	return localize_enable;
}

/*
Frame_objects* Frame_objects::m_pInstance = NULL;

Frame_objects::Frame_objects()
{
    //objects.clear();
}

Frame_objects* Frame_objects::Instance()
{
    if (!m_pInstance)
        m_pInstance = new Frame_objects;
}

TSlamObj& Frame_objects::object(uint index)
{
    return objects[index];
}

void Frame_objects::new_frame()
{
    objects.clear();
}


void Frame_objects::add_object(TSlamObj obj)
{
    objects.push_back(obj);
}

*/

void flag_used4display(){
	usedForDisplay=true;
}

void flag_notused4display(){
	usedForDisplay=false;
}

bool get_used4display(){
	return usedForDisplay;
}

uint16_t get_frame_num(){
	return frame_num;
}

void set_frame_num(uint16_t fn){
	frame_num=fn;
}

//uint16_t get_host_vel_uint16(){
//	return host_vel;
//}
float get_host_vel(){
	return host_vel;
}

void set_host_vel(float vel){
	host_vel = vel;
}

bool get_SlamLastPacket(){
	return SlamLastPacket;
}

void falsify_slamLastPacket(){
	SlamLastPacket = false;
}


void truesify_slamLastPacket(){
	SlamLastPacket = true;
}

void objects_mutex_lock(){
	g_objects_mutex.lock();
}

void objects_mutex_unlock(){
	g_objects_mutex.unlock();
}

Slam_Color* Slam_Color::m_pInstance = NULL;

Slam_Color::Slam_Color(uint64_t seed)
{
    std::srand(seed);
    uint8_t I=0;
    std::vector<uint8_t> ind;

	for(uint8_t i = 0; i < 64; i++)
		ind.push_back(i);

	std::random_shuffle(ind.begin(),ind.end());

    for(uint8_t i = 0; i < 4; i++){
        float r = i * 0.2 + 0.2;
        for(uint8_t j = 0; j < 4; j++){
            float g = j * 0.2 + 0.2;
            for(uint8_t k = 0; k < 4; k++,I++){
                float b = k * 0.2 + 0.2;

				color_a[ind[I]][0] = r;
				color_a[ind[I]][1] = g;
				color_a[ind[I]][2] = b;
			}
		}

	}
}

Slam_Color* Slam_Color::Instance()
{
    if (!m_pInstance)
        m_pInstance = new Slam_Color;

    return m_pInstance;
}

void Slam_Color::get_color(uint16_t id, float &r, float &g, float &b)
{
	uint8_t i = (uint8_t)(id % 64);

	r = color_a[i][0];
	g = color_a[i][1];
	b = color_a[i][2];
}

void Slam_Color::get_color_ui8(uint16_t id, uint8_t &r, uint8_t &g, uint8_t &b)
{
    float red,green,blue;
    get_color(id,red,green,blue);
    r = (uint8_t)(255 * red);
    g = (uint8_t)(255 * green);
    b = (uint8_t)(255 * blue);
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

    min*=10;
    max*=10;
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





static float euler_mat[3][3] = {{1,0,0},{0,1,0},{0,0,1}};
void set_euler_mat(float alpha, float beta, float gamma)
{
    float rotx_alpha[3][3] = {{1,0,0},{0,1,0},{0,0,1}};
    float roty_beta[3][3] = {{1,0,0},{0,1,0},{0,0,1}};
    float rotx_gamma[3][3] = {{1,0,0},{0,1,0},{0,0,1}};

    for(uint8_t i = 0; i < 3; i++)
        for(uint8_t j = 0; j < 3; j++)
            euler_mat[i][j]=0;

    rotx_alpha[1][1] = cos(alpha);
    rotx_alpha[2][2] = rotx_alpha[1][1];
    rotx_alpha[2][1] = sin(alpha);
    rotx_alpha[1][2] = -rotx_alpha[2][1];


    roty_beta[0][0] = cos(beta);
    roty_beta[2][2] = roty_beta[0][0];
    roty_beta[0][2] = sin(beta);
    roty_beta[2][0] = -roty_beta[0][2];

    rotx_gamma[1][1] = cos(gamma);
    rotx_gamma[2][2] = rotx_gamma[1][1];
    rotx_gamma[2][1] = sin(gamma);
    rotx_gamma[1][2] = -rotx_gamma[2][1];

    for(uint8_t i = 0; i < 3; i++)
        for(uint8_t j = 0; j < 3; j++){
            for(uint8_t k = 0; k < 3; k++){
                for(uint8_t l = 0; l < 3; l++){
                    euler_mat[i][j] += rotx_gamma[i][k]*roty_beta[k][l]*rotx_alpha[l][j];
                }
            }
        }
}

void euler_rotate(TaregtCartesian &cart_detection)
{
    double X[3] = {cart_detection.x, cart_detection.y, cart_detection.z};
    double out[3] = {0, 0, 0};

    for(uint8_t i = 0; i < 3; i++)
    {
        for(uint8_t j = 0; j < 3; j++)
        {
            out[i] += euler_mat[i][j] * X[j];
        }
    }
    cart_detection.x = out[0];
    cart_detection.y = out[1];
    cart_detection.z = out[2];
}

void tiltTarget(TaregtCartesian &cart_detection)
{
    double X[3] = {cart_detection.x, cart_detection.y, cart_detection.z};
    double out[3] = {cart_detection.x, 0, 0};

    for(uint8_t i = 1; i < 3; i++)
    {
        for(uint8_t j = 1; j < 3; j++)
        {
            out[i] += euler_mat[i][j] * X[j];
        }
    }
    cart_detection.y = out[1];
    cart_detection.z = out[2];
}

void set_reset_mapping(bool flag)
{
    reset_local_frame = flag;
}

bool get_reset_mapping()
{
    return reset_local_frame;
}

int16_t get_classes_to_show()
{
    return classes_to_show;
}
void set_classes_to_show(int16_t cls)
{
    classes_to_show = cls;
}

void get_class_color(uint8_t cl, float &red, float &green, float &blue, std::string &fc_txt)
{
    switch(cl)
    {
    case 0:
        fc_txt = ", Pedestrian";
        // color = red
        red = 1;
        green = 0;
        blue = 0;
        break;
    case 1:
        fc_txt = ", 2-wheeler";
        // color = cyan
        red = 0;
        green = 1;
        blue = 1;
        break;
    case 2:
        fc_txt = ", vehicle";
        // color blueish
        red = 0;
        green = 0.635;
        blue = 0.929;
        break;
    case 3:
        fc_txt = ", BUG";
        // color purple
        red = 0.54;
        green = 0.17;
        blue = 0.89;
        break;
    default:
        fc_txt = "";
        // color orange yellow
        red = 1;
        green = 0.8;
        blue = 0;
        break;
    }
}

static bool disp_processed_pc = true;
bool get_disp_processed_pc()
{
    return disp_processed_pc;
}
void set_disp_processed_pc(bool flag)
{
    disp_processed_pc = flag;
}


static bool disp_objects = true;
bool get_disp_objects()
{
	return disp_objects;
}
void set_disp_objects(bool flag)
{
	disp_objects = flag;
}



float local_cart_x = 0, local_cart_y = 0;
void set_local_cart(const arbe_msgs::arbeSlamMsg::ConstPtr& msg)
{
	local_cart_x = msg->meta_data.local_catr_x;
	local_cart_y = msg->meta_data.local_catr_y;
}
void get_local_cart(float &x, float &y)
{
	x = local_cart_x;
	y = local_cart_y;
}
