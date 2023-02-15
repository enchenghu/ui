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
#include "Slam_color.hpp"


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

void Slam_Color::get_class_color(uint8_t cl, float &red, float &green, float &blue, std::string &fc_txt)
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
