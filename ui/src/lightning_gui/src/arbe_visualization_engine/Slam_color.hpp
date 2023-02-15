#ifndef _SLAM_COLOR_H
#define _SLAM_COLOR_H


class Slam_Color{
    protected:
        float color_a[64][3];
        Slam_Color(uint64_t seed = 42);
        static Slam_Color* m_pInstance;
    public:
        static Slam_Color* Instance();
        void get_color(uint16_t id, float &r, float &g, float &b);
        void get_color_ui8(uint16_t id, uint8_t &r, uint8_t &g, uint8_t &b);
        void get_class_color(uint8_t cl, float &red, float &green, float &blue, std::string &fc_txt);
};


#endif
