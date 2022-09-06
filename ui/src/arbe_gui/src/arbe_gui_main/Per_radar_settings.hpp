#ifndef _PER_RADAR_SETTINGS_H
#define _PER_RADAR_SETTINGS_H

#include <iostream>
#include <vector>
#include "arbe_msgs/arbeSettingsPerRadar.h"

class Per_radar_settings{
protected:
    static Per_radar_settings* m_pInstance;

public:
	Per_radar_settings(uint8_t n_radars);
    static Per_radar_settings* Instance(uint8_t n = 0);
    arbe_msgs::arbeSettingsPerRadar to_msg();
	void resize(uint8_t n);
	void set_camera_intrinsic(int m, float intrinsic[3][3]);
	void get_camera_intrinsic(int m, float intrinsic[3][3]);

public:
    std::vector<float> ant_height;
    std::vector<float> ant_pitch;
    std::vector<float> cam_euler_a;
    std::vector<float> cam_euler_b;
    std::vector<float> cam_euler_g;
	std::vector<std::vector<float> > camera_intrinsic;
	std::vector<bool> is_camera_calibrated;
	std::vector<int> threshold_4d;
	std::vector<int> noise_level;
	std::vector<int> bias_4d;
	std::vector<int> DynamicAzimuth;
	std::vector<int> DynamicElevation;

	int radar_for_text;
};


#endif
