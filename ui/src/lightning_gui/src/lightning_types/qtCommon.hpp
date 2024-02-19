#pragma once
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>
#include <netinet/tcp.h>
#include <algorithm>
#include <unistd.h>
#include <QApplication>
#include <QCheckBox>
#include <QColor>
#include <QComboBox>
#include <QDebug>
#include <QDesktopWidget>
#include <QDockWidget>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QShortcut>
#include <QSlider>
#include <QStyleOptionSlider>
#include <QTimer>
#include <QToolTip>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <QLineEdit>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

typedef enum {
    C_GRAY,
    C_RED,
    C_GREEN,
    C_YELLOW
} LED_COLOR;

void setReadOnlyLineEdit(QLineEdit* line);

void setLED(QLabel* label, LED_COLOR color);

void setButtonStyle(QPushButton* btn);

void setCheckBoxUnvaild(QCheckBox* checkBox, bool check);

void setCheckBoxUnvaild(QCheckBox* checkBox);

inline int tcpSocketCheck(int sock)
{
	struct tcp_info info; 
	int len = sizeof(info); 
	getsockopt(sock, IPPROTO_TCP, TCP_INFO, &info, (socklen_t *)&len);
	if((info.tcpi_state == TCP_ESTABLISHED))
		return 0;
	else 
		return -1;
}


#if 0
bool check_file_exist(const std::string &path) {
#ifdef _MSC_VER  
	bool ret = 0 == _access(path.c_str(), 0);
#else
	bool ret = 0 == access(path.c_str(), F_OK);
#endif
	if (!ret) {
		//LOG(INFO) << path << " not exist";
	}
	return ret;
}

size_t GetFileSize(const std::string &filepath) {
	FILE *fd = fopen(filepath.c_str(), "rb");
	if (fd == NULL) {
		//LOG(ERROR) << "Failed to open file " << filepath;
		return 0;
	}
	// Determine size of the file
	fseek(fd, 0, SEEK_END);
	size_t file_length = static_cast<size_t>(ftell(fd));
	fseek(fd, 0, SEEK_SET);
	fclose(fd);

	return file_length;
}
static uint8_t *encode_cali_data = nullptr;
static long file_size_g = 0;
uint8_t * LoadDat(const char *cali_file_path)
{
	if (NULL == cali_file_path) {
		//LOG(ERROR) << "Bad cali_file_path " LOG_END;
		return nullptr;
	}
	if (!check_file_exist(cali_file_path)) {
		//LOG(ERROR) << "Cali file " << cali_file_path << " not exist " LOG_END;
		return nullptr;
	}
	auto file_size = GetFileSize(cali_file_path);
    std::cout << "file_size is " << file_size << std::endl;
	if (file_size < 1000) {
		//LOG(ERROR) << "Bad cali file size " << file_size LOG_END;
		return nullptr;
	}

	FILE *file = fopen(cali_file_path, "rb");
	uint8_t *encode_cali_data = (uint8_t *)malloc(file_size);
    file_size_g = file_size;
	fseek(file, 0, SEEK_SET);
	fread(encode_cali_data, file_size, 1, file);
    return encode_cali_data;
}
#endif