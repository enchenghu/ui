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
