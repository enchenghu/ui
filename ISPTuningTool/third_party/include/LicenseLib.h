// LicenseLib.h: LicenseLib DLL 的主标头文件
//

#pragma once
// #define USE_MSVC

#ifdef USE_MSVC
#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif
#include "resource.h"		// 主符号
#endif

#ifndef USE_MSVC
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
	//初始化DLL资源
#ifdef USE_MSVC
	_declspec(dllexport) int LicenseLib_GetHardwareInfo(CString &strSerialNumber);//获取硬件信息
	_declspec(dllexport) int LicenseLib_GenerateLicense(UINT nUserLevel, CString strSerialNumber, CString &strKey);//生成license信息
	_declspec(dllexport) bool LicenseLib_IsAuthorized();//本地是否已经授权
	_declspec(dllexport) bool LicenseLib_IsAuthorizedRegisterTable(int iSensorID, CString &strKey, CString &strLevelEnable);//注册表中是否已经授权
	_declspec(dllexport) bool LicenseLib_Register(CString strKey);//注册
	_declspec(dllexport) bool LicenseLib_RegisterEx(int iSensorID, CString strKey);//注册
	_declspec(dllexport) bool LicenseLib_GetKey(CString &strKey);//获取注册码
	_declspec(dllexport) bool LicenseLib_GetKeyEx(int iSensorID, CString &strKey, CString &strLevelEnable);//根据SensorID，获取注册码,模块使能开关
	_declspec(dllexport) bool LicenseLib_GetLevelEnable(CString &strLevelEnable);//获取用户等级权限
	_declspec(dllexport) bool LicenseLib_GetLicensePlainText(CString strFilePath, CString &strPlainText);//获取license解密后文本
	_declspec(dllexport) bool LicenseLib_Save(CString strPlainText);//将文本保存为加密文件
#else
#define MAX_KEY_SIZE 32
	_declspec(dllexport) int LicenseLib_GetHardwareInfo(char* szSerialNumber, int iSize);//获取硬件信息
	_declspec(dllexport) int LicenseLib_GenerateLicense(uint32_t nUserLevel, const char* szSerialNumber, char szKey[MAX_KEY_SIZE]);//生成license信息
	_declspec(dllexport) bool LicenseLib_IsAuthorized();//本地是否已经授权
    _declspec(dllexport) bool LicenseLib_IsAuthorizedRegisterTable(
        int iSensorID, char szKey[MAX_KEY_SIZE], char szLevelEnable[MAX_KEY_SIZE]); // 注册表中是否已经授权
    _declspec(dllexport) bool LicenseLib_Register(const char szKey[MAX_KEY_SIZE]); // 注册
    _declspec(dllexport) bool LicenseLib_RegisterEx(int iSensorID, const char szKey[MAX_KEY_SIZE]); // 注册
    _declspec(dllexport) bool LicenseLib_GetKey( char szKey[MAX_KEY_SIZE]); // 获取注册码
    _declspec(dllexport) bool LicenseLib_GetKeyEx(
        int iSensorID, char szKey[MAX_KEY_SIZE],
        char szLevelEnable[MAX_KEY_SIZE]); // 根据SensorID，获取注册码,模块使能开关
    _declspec(dllexport) bool LicenseLib_GetLevelEnable(char szLevelEnable[MAX_KEY_SIZE]); // 获取用户等级权限
#endif
#ifdef __cplusplus
}
#endif
