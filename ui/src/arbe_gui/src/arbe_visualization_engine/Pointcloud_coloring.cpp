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
#include "Utils.h"
#include <arbe_msgs/arbeSlamMsg.h>
#include <geometry_msgs/TwistWithCovarianceStamped.h>
#include "Pointcloud_coloring.hpp"



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

void pointcloud_color( pcl::PointCloud<ArbePointXYZRGBGeneric> &buffer, size_t j, float cc_min, float cc_max, float cc_span, std::string ColoringType, float ego_velocity, float installation_ang) // NEW FUNCTION!
{
	if (std::isnan(buffer.points[j].z)) buffer.points[j].z = 0;
	if ( strcmp(ColoringType.c_str(), "Doppler") == 0 )
	{
		/* Color by doppler */
		float rel_speed = (buffer.points[j].doppler + ego_velocity* cos(buffer.points[j].azimuth) * cos(installation_ang));
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
			x = (buffer.points[j].doppler +  ego_velocity* cos(buffer.points[j].azimuth) * cos(installation_ang)) ;
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


void pointcloud_one_color( pcl::PointCloud<ArbePointXYZRGBGeneric> &buffer, size_t j, uint8_t r, uint8_t g, uint8_t b) // NEW FUNCTION!
{
	if (std::isnan(buffer.points[j].z)) buffer.points[j].z = 0;
	buffer.points[j].r = r;
	buffer.points[j].g = g;
	buffer.points[j].b = b;
	buffer.points[j].a = 0.2;
}
