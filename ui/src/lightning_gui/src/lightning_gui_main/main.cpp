#include <ros/ros.h>
#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QProgressDialog>
#include <QDebug>
#include <QString>
#include "mainwindow.h"
#include <iostream>
#include <fstream>
#include <sys/ioctl.h>


int main(int argc, char **argv)
{
	ros::init(argc, argv, "lidar_gui");
	ros::Time::init();
#if 1
	QApplication app(argc, argv);
	QCoreApplication::setOrganizationName("Lightning Lidar");
	QCoreApplication::setApplicationName("Lightning V1.0 Lidar ROS GUI");
	QCoreApplication::setApplicationVersion(QT_VERSION_STR);
	QCommandLineParser parser;
	parser.setApplicationDescription(QCoreApplication::applicationName());
	parser.addHelpOption();
	parser.addVersionOption();
	parser.addPositionalArgument("file", "The file to open.");
	parser.process(app);
#endif
#if 1
	MainWindow mainWin;
	const QRect availableGeometry = QApplication::desktop()->availableGeometry(&mainWin);
	mainWin.resize(availableGeometry.width() / 2, availableGeometry.height() * 2 / 3);
	mainWin.move((availableGeometry.width() - mainWin.width()) / 2, (availableGeometry.height() - mainWin.height()) / 2);
	mainWin.show();
	return app.exec();
#endif

}
