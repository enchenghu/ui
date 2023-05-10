#pragma once

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
#include "flidar_stat.h"

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
