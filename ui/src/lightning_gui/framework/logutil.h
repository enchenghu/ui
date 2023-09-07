/**
 * Copyright(c): Black Sesame Technologies(Shanghai)Co.,Ltd. All rights reserved.
 * Filename:
 * Author: Benson.wang
 * Date: 2021年 02月 01日
**/

#ifndef LOG_UTILS_H
#define LOG_UTILS_H
#include <unistd.h>
#include <sys/syscall.h>
#include <string>
#define gettid() syscall(__NR_gettid)


#ifndef LOG_TAG
#define LOG_TAG "avm"
#endif

#define BST_DEBUG 1

std::string get_stime(void);

#define BLOGE(format, ...) printf("%s %ld %ld E " LOG_TAG ": " format "\n", get_stime().c_str(), getpid(), gettid(), ##__VA_ARGS__)
#define BLOGI(format, ...) printf("%s %ld %ld I " LOG_TAG ": " format "\n", get_stime().c_str(), getpid(), gettid(), ##__VA_ARGS__)
#define BLOGD(format, ...) printf("%s %ld %ld D " LOG_TAG ": " format "\n", get_stime().c_str(), getpid(), gettid(), ##__VA_ARGS__)


#if BST_DEBUG

#define LOGE(...) BLOGE(__VA_ARGS__)
#define LOGI(...) BLOGI(__VA_ARGS__)
#define LOGD(...) BLOGD(__VA_ARGS__)
#else
#define LOGE(...) BLOGE(__VA_ARGS__)
#define LOGI(...) BLOGI(__VA_ARGS__)
#define LOGD(...) 
#endif

#endif //LOG_UTILS_H