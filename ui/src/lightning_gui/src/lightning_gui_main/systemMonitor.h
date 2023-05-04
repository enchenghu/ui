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


class SystemMonitor : public QTabWidget
{
    Q_OBJECT
public:
    SystemMonitor(QTabWidget* parent = 0);
    virtual ~SystemMonitor(){}

private:
    QWidget* edfaWidget;
	QGridLayout* edfaLayout;
	QGroupBox* edfaBox;
	QGridLayout* edfaBoxLayout;
	QGroupBox* basicBox;
	QGridLayout* basicBoxLayout;
    QGroupBox* stateBox;
	QGridLayout* stateBoxLayout;
	QGroupBox* warnBox;
	QGridLayout* warnBoxLayout;
    std::vector<QLineEdit*> edfaStateLinesV;
    std::vector<QLabel*> edfaWarnLEDV;
    std::vector<QLineEdit*> edfaDeviceInfoLinesV;
    void setReadOnlyLineEdit(QLineEdit* line);
    void creatEDFATab();
    void setLED(QLabel* label, int color);

};