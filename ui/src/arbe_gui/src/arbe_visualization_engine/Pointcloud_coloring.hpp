#ifndef _POINTCLOUD_COLORING_H
#define _POINTCLOUD_COLORING_H

#include "types.hpp"

class Color_Coding_Min_Max
{
protected:
    float dopp_cc_min = -3.0, dopp_cc_max = 3.0;
    float el_cc_min = 0, el_cc_max = 8.0;
    float dopp_grad_cc_min = -20.0, dopp_grad_cc_max = 20.0;
    float amp_cc_min = 80, amp_cc_max = 150;
    Color_Coding_Min_Max();
    static Color_Coding_Min_Max* m_pInstance;
public:
    static Color_Coding_Min_Max* Instance();

    void set_min(std::string ColoringType, float min);
    void set_max(std::string ColoringType, float max);
    void get_values(std::string ColoringType, float &min, float &max);
    void get_converted_values(std::string ColoringType, float &min, float &max);
    std::string get_units(std::string ColoringType);
};

void apply_color(float z, uint8_t &r, uint8_t &g, uint8_t &b);
void pointcloud_color(pcl::PointCloud<ArbePointXYZRGBGeneric> &buffer, size_t j, float cc_min, float cc_max, float cc_span, std::string ColoringType, float ego_velocity, float installation_ang);
void pointcloud_one_color( pcl::PointCloud<ArbePointXYZRGBGeneric> &buffer, size_t j, uint8_t r, uint8_t g, uint8_t b);

#endif
