#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN           
#include <windows.h>
#include <stdint.h>
#else
#include <cstring>
#include <unistd.h>
#define BOOL int
#define HANDLE void*
#define TRUE 1
#define FALSE 0
#define BYTE uint8_t
#define UINT8 uint8_t
#define UINT16 uint16_t
#define UINT32 uint32_t
#define UINT64 uint64_t
#define UINT uint32_t
#define DWORD uint32_t
#define Sleep(time_ms) usleep(time_ms*1000)
#define memcpy_s(dst, dst_len, src, src_len) memcpy(dst, src, (dst_len < src_len ? dst_len : src_len))
#define __stdcall
#endif