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

#include "Per_radar_settings.hpp"


const static float default_camera_intrinsic[9] = {1526.97,0,934.05,
											   0,1533.03,537.37,
											   0,0,1};


Per_radar_settings* Per_radar_settings::m_pInstance = NULL;

Per_radar_settings::Per_radar_settings(uint8_t n_radars)
{
	ant_height.resize(n_radars, 1);
	ant_pitch.resize(n_radars, 0);
	cam_euler_a.resize(n_radars, 0);
	cam_euler_b.resize(n_radars, 0);
	cam_euler_g.resize(n_radars, 0);
	threshold_4d.resize(n_radars, 0);
	is_camera_calibrated.resize(n_radars, 0);
	std::vector<float> tmp(std::begin(default_camera_intrinsic), std::end(default_camera_intrinsic));
	camera_intrinsic.resize(n_radars, tmp);
	radar_for_text = -1;
}

void Per_radar_settings::resize(uint8_t n)
{
	ant_height.resize(n, 1);
	ant_pitch.resize(n, 0);
	cam_euler_a.resize(n, 0);
	cam_euler_b.resize(n, 0);
	cam_euler_g.resize(n, 0);
	threshold_4d.resize(n, 0);
	is_camera_calibrated.resize(n, 0);
	radar_for_text = -1;
}

Per_radar_settings* Per_radar_settings::Instance(uint8_t n)
{
	if (!m_pInstance || n >0)
		m_pInstance = new Per_radar_settings(n);

    return m_pInstance;
}

arbe_msgs::arbeSettingsPerRadar Per_radar_settings::to_msg()
{
	arbe_msgs::arbeSettingsPerRadar msg;
	msg.n_radars = (uint8_t)ant_height.size();
	msg.radar_for_text = radar_for_text;
	msg.ant_height = ant_height;
	msg.ant_pitch = ant_pitch;
	msg.cam_euler_a = cam_euler_a;
	msg.cam_euler_b = cam_euler_b;
	msg.cam_euler_g = cam_euler_g;

	return msg;
}

void Per_radar_settings::set_camera_intrinsic(int m, float intrinsic[3][3])
{
	camera_intrinsic[m].clear();
	for(uint8_t i = 0; i < 3; i++)
		for(uint8_t j = 0; j < 3; j++)
			camera_intrinsic[m].push_back(intrinsic[i][j]);
}

void Per_radar_settings::get_camera_intrinsic(int m, float intrinsic[3][3])
{
	camera_intrinsic[m].clear();
	for(uint8_t i = 0; i < 3; i++)
		for(uint8_t j = 0; j < 3; j++)
			intrinsic[i][j] = camera_intrinsic[m][3*i + j];
}
