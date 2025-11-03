#pragma once

class COscillatorMap
{
public:
	COscillatorMap();
	~COscillatorMap();
	// lmk61e2 晶振表
	unsigned char m_byarrOsciMap[34][902]; // 34个寄存器，10M~90M，步长：0.1MHz, 外加37.125MHz
	void vInitOscillatorMap();

	unsigned int m_iOldHz; //单位：MHz * 10
};
