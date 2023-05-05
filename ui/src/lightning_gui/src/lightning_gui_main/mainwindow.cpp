

#include "mainwindow.h"
#include <std_msgs/UInt8MultiArray.h>
#include <std_msgs/Bool.h>
#include <std_msgs/String.h>
#include <geometry_msgs/TwistWithCovarianceStamped.h>
#include <sensor_msgs/Imu.h>
#include <fcntl.h>
#include <termios.h>

#ifndef __has_include
  static_assert(false, "__has_include not supported");
#else
#  if __has_include(<filesystem>)
#    include <filesystem>
     namespace fs = std::filesystem;
#  elif __has_include(<experimental/filesystem>)
#    include <experimental/filesystem>
     namespace fs = std::experimental::filesystem;
#  elif __has_include(<boost/filesystem.hpp>)
#    include <boost/filesystem.hpp>
     namespace fs = boost::filesystem;
#  endif
#endif

int terminating;
QString save_folder = ".";
void MainWindow::find_serial_ports()
{
	std::vector<std::string> ports;
	fs::path kdr_path{"/proc/tty/drivers"};
	if (fs::exists(kdr_path))
	{
		std::ifstream ifile(kdr_path.generic_string());
		std::string line;
		std::vector<std::string> prefixes;
		while (std::getline(ifile, line))
		{
			std::vector<std::string> items;
			auto it = line.find_first_not_of(' ');
			while (it != std::string::npos)
			{

				auto it2 = line.substr(it).find_first_of(' ');
				if (it2 == std::string::npos)
				{
					items.push_back(line.substr(it));
					break;
				}
				it2 += it;
				items.push_back(line.substr(it, it2 - it));
				it = it2 + line.substr(it2).find_first_not_of(' ');
			}
			if (items.size() >= 5)
			{
				if (items[4] == "serial" && items[0].find("serial") != std::string::npos)
				{
					prefixes.emplace_back(items[1]);
				}
			}
		}
		ifile.close();
		for (auto& p: fs::directory_iterator("/dev"))
		{
			for (const auto& pf : prefixes)
			{
				auto dev_path = p.path().generic_string();
				if (dev_path.size() >= pf.size() && std::equal(dev_path.begin(), dev_path.begin() + pf.size(), pf.begin()))
				{
					ports.emplace_back(dev_path);
					//printf("dev_path %s\n",dev_path.c_str());
					std::size_t found=dev_path.find("ttyUSB");
					if (found!=std::string::npos)
						debug_port_combo->insertItem(0,tr(dev_path.c_str()));
				}
			}
		}
	}
	ROS_INFO("Created list of available serial ports");
}

void MainWindow::find_video_devices()
{
	std::vector<std::string> devices;
	for (auto& p: fs::directory_iterator("/dev"))
	{
		auto dev_path = p.path().generic_string();
		devices.emplace_back(dev_path);
		std::size_t found=dev_path.find("video");
		if (found == 5)
		{
			video_device_combo->insertItem(0,tr(dev_path.c_str()));
			//ROS_DEBUG("found %s \n",dev_path.c_str());
		}
	}
	ROS_INFO("Created list of available usb cameras");
}

MainWindow::MainWindow()
{
	textEdit = new QPlainTextEdit;
	createActions();
	createMenus();
	readSettings();
	viewPanel = viewpanel::Instance();
	int width = 1024;
	int height = 300;
	viewPanel->setMinimumSize(width, height);
	setCentralWidget(viewPanel);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	viewPanel->printView();
	//viewPanel->print_cam_widg_location();
	event->accept();
	writeSettings();
	int ret = system("pkill -f rqt_bag");
	close();
	terminating = 1;
	//ret = 	system("rosnode kill --all");
}

void MainWindow::open( void )
{
	/* Kill any previously left rqt_bag window */
	int ret = system("pkill -f rqt_bag");

	playback_file =  QFileDialog::getOpenFileName(
	  this,
	  "Open Recordings",
	  QDir::currentPath(),
	  "Rosbag files (*.bag);;Unity bin files (*.bin)",0,QFileDialog::DontUseNativeDialog);

	if( !playback_file.isNull() )
	{
		qDebug() << "selected file path : " << playback_file.toUtf8();
	}
        setCurrentFile(playback_file);

	if(playback_file.toStdString().find(".bag") != std::string::npos)
	{
		char rqt_bag_launch_command[300];
		/* Create rqt_bag launch command */
		sprintf(rqt_bag_launch_command,"roslaunch autox_ui lightning_rqt_bag.launch bag:=%c%c%s%c%c&",
			'"','\047',(char *)playback_file.toStdString().c_str(),'\047','"');
		ROS_DEBUG("launch playback menu control: %s",rqt_bag_launch_command);
		/* wait for rqt_bag to start displaying a wait cursor */
		QApplication::setOverrideCursor(Qt::WaitCursor);
		int ret = system(rqt_bag_launch_command);
		sleep(4); 
		QApplication::restoreOverrideCursor();
	} else if (playback_file.toStdString().find(".bin") != std::string::npos)
	{
	}
}

void MainWindow::setSaveFolder()
{

	save_folder =  QFileDialog::getExistingDirectory(this, "Choose folder for saving bags", QDir::currentPath(),
		QFileDialog::DontUseNativeDialog | QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if( !save_folder.isNull() )
	{
		qDebug() << "selected default recording folder : " << save_folder.toUtf8();
	}
	else
	{
		save_folder = QString(".");
	}
}


void MainWindow::stopPlayback( void )
{
}

char rosbag_convert_command[500];
char rosbag_convert_script_path[200];
void MainWindow::convertRosbag( void )
{
	sprintf(rosbag_convert_script_path,"python3 ");
	sprintf(rosbag_convert_script_path + strlen(rosbag_convert_script_path),SRC_DIR);
	sprintf(rosbag_convert_script_path + strlen(rosbag_convert_script_path),"/scripts/rosbag_to_bin_and_mp4.py");
	QString selectedFilter;
	QStringList files;
	files = QFileDialog::getOpenFileNames(
		                this, tr("Select rosbag files to convert)"),
		                openFilesPath,
		                tr("Rosbag files (*.bag)"),
		                &selectedFilter,
		                QFileDialog::DontUseNativeDialog);

	QProgressDialog progress("rosbag conversion in progress", "Cancel", 0, files.count(), this);
	progress.setWindowModality(Qt::WindowModal);

	for (int i = 0; i < files.count(); i++)
	{
		progress.setValue(i);
		QCoreApplication::processEvents();
		openFilesPath = files[i];
		ROS_DEBUG("selected file %s",files[i].toStdString().c_str());
		sprintf(rosbag_convert_command,"%s",rosbag_convert_script_path);
		sprintf(rosbag_convert_command+strlen(rosbag_convert_command)," %s",
			files[i].toStdString().c_str());
		printf("rosbag_convert_command is %s\n", rosbag_convert_command);
		int ret = system(rosbag_convert_command);
		if (progress.wasCanceled())
		    break;
	}
	progress.setValue(files.count());
	QMessageBox msgBox;
	msgBox.setText("Coversion completed");
	msgBox.exec();
}



void MainWindow::pcOffset_Control( void )
{
	viewPanel->ctrlShowPcOffset(pcOffsetDockAct->isChecked());
}

void MainWindow::CtrlPanel_Control( void )
{
	viewPanel->ctrlShowWindows(showCtrlDockAct->isChecked());
}

void MainWindow::PcPanel_Control( void )
{
	viewPanel->ctrlPcPanel(showPcPanelAct->isChecked());
}
void MainWindow::resetViews( void )
{
	viewPanel->resetViews();
}
void MainWindow::increasePointSize( void )
{
	viewPanel->increasePointSize();
}

void MainWindow::decreasePointSize( void )
{
	viewPanel->decreasePointSize();
}

void MainWindow::RecRvizPointcloudControl( void )
{
	if(recRvizPointcloudAct->isChecked())
	{
		QMessageBox::StandardButton answer = QMessageBox::question(
			this,
			tr("Record Processed Pointcloud"),
			tr("This will record the processed PointCloud2 messages published to Rviz in addition to the binary pointcloud received from the Radar. Continue?"),
		QMessageBox::Yes | QMessageBox::No);

		if(answer == QMessageBox::Yes) {
			//rec_rviz_pointcloud = true;
		}
	} else {
		//rec_rviz_pointcloud = false;
	}
}

void MainWindow::helpUsePC()
{
	QMessageBox::information(this, tr("点云显示界面操作说明"),
	tr("点击<b>[Start PC]</b>按钮: 开启点云,再点击关闭 <br> \
		点击<b>[PC Single]</b>按钮: 单帧显示点云 <br> \
		点击<b>[PC Contin]</b>按钮: 连续显示点云 <br> \
		点击<b>[Pure]</b>按钮: 不组包显示点云 <br>   \
		快捷方式<b>[Ctrl + a]</b>: 全屏显示点云, 再按关闭 <br> \
		快捷方式<b>[Ctrl + r]</b>: 显示视角重置 <br> \
		快捷方式<b>[Ctrl + Alt + s]</b>: 进入选择点云数据点模式, 再按关闭 <br> \
		快捷方式<b>[Ctrl + 1]</b>:0.005步进增大点云点的尺寸 <br> \
		快捷方式<b>[Ctrl + 2]</b>:0.005步进减小点云点的尺寸 <br> \
		操作<b>[按住鼠标左键]</b>: 旋转拖动点云视角 <br> \
		操作<b>[按住shift+按住鼠标左键]</b>: 平行拖动点云视角"));
}

void MainWindow::about()
{
	QMessageBox::about(this, tr("Program Info"),
	tr("<b>Autox</b> Lightning v0.3 ROS GUI"));
}

void MainWindow::quit( void )
{
    writeSettings();
	int ret = system("pkill -f rqt_bag");
}

void MainWindow::createActions()
{

	openAct = new QAction(tr("&Open Recording"), this);
	openAct->setStatusTip(tr("Play a recording"));
	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
#if 1
	setRecFolderAct = new QAction(tr("&Set Recording Folder"), this);
	setRecFolderAct->setStatusTip(tr("Set default path for recording"));
	connect(setRecFolderAct, SIGNAL(triggered()), this, SLOT(setSaveFolder()));

	closeAct = new QAction(tr("&Close Recording"), this);
	closeAct->setStatusTip(tr("Stop Playback"));
	connect(closeAct, SIGNAL(triggered()), this, SLOT(stopPlayback()));

	convertAct = new QAction(tr("Convert Rosba&g"), this);
	convertAct->setStatusTip(tr("Convert a rosbag recording to binary and mp4"));
	connect(convertAct, SIGNAL(triggered()), this, SLOT(convertRosbag()));

	TestDescriptionAct = new QAction(tr("Set Test Descri&ption"), this);
	TestDescriptionAct->setStatusTip(tr("Add a test drive description"));
	connect(TestDescriptionAct, SIGNAL(triggered()), this, SLOT(SetTestDescription()));

#endif
	exitAct = new QAction(tr("E&xit"), this);
	exitAct->setStatusTip(tr("Exit the application"));
	connect(exitAct, SIGNAL(triggered()), this, SLOT(quit()));

	aboutAct = new QAction(tr("&About this app"), this);
	aboutAct->setStatusTip(tr("Show the application's About box"));
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

	mannualAct = new QAction(tr("&How to use PC"), this);
	mannualAct->setStatusTip(tr("Show the application's About box"));
	connect(mannualAct, SIGNAL(triggered()), this, SLOT(helpUsePC()));

	updateAct = new QAction(tr("&System update"), this);

#if 1
	showCtrlDockAct = new QAction(tr("Control Panel"), this);
	//recParsedAct->setStatusTip(tr("When recording, record *all* parsed topics"));
	showCtrlDockAct->setCheckable(true);
	showCtrlDockAct->setChecked(true);
	connect(showCtrlDockAct, SIGNAL(triggered()), this, SLOT(CtrlPanel_Control()));

	pcOffsetDockAct = new QAction(tr("PC Offset"), this);
	//recParsedAct->setStatusTip(tr("When recording, record *all* parsed topics"));
	pcOffsetDockAct->setCheckable(true);
	pcOffsetDockAct->setChecked(true);
	connect(pcOffsetDockAct, SIGNAL(triggered()), this, SLOT(pcOffset_Control()));

	showPcPanelAct = new QAction(tr("PointCloud Panel"), this);
	//recParsedAct->setStatusTip(tr("When recording, record *all* parsed topics"));
	showPcPanelAct->setCheckable(true);
	showPcPanelAct->setChecked(true);
	connect(showPcPanelAct, SIGNAL(triggered()), this, SLOT(PcPanel_Control()));

	QShortcut  *shortcut = new QShortcut(QKeySequence(tr("ctrl+a")), this);
	connect(shortcut, SIGNAL(activated()), this,SLOT(fullScreen()));

	QShortcut  *shortcut_r = new QShortcut(QKeySequence(tr("ctrl+r")), this);
	connect(shortcut_r, SIGNAL(activated()), this,SLOT(resetViews()));

	QShortcut  *shortcut_1 = new QShortcut(QKeySequence(tr("ctrl+1")), this);
	connect(shortcut_1, SIGNAL(activated()), this,SLOT(increasePointSize()));

	QShortcut  *shortcut_2 = new QShortcut(QKeySequence(tr("ctrl+2")), this);
	connect(shortcut_2, SIGNAL(activated()), this,SLOT(decreasePointSize()));
#endif

}

void MainWindow::fullScreen()
{
	static bool showAll = false;
	viewPanel->ctrlShowWindows(showAll);
	viewPanel->ctrlPcPanel(showAll);
	viewPanel->ctrlShowPcOffset(showAll);
	showCtrlDockAct->setChecked(showAll);
	showPcPanelAct->setChecked(showAll);
	pcOffsetDockAct->setChecked(showAll);
	showAll = !showAll;
}

void MainWindow::createMenus()
{
	fileMenu = menuBar()->addMenu(tr("&File"));
	fileMenu->addAction(openAct);
	fileMenu->addAction(closeAct);
	fileMenu->addAction(setRecFolderAct);
	fileMenu->addSeparator();
	fileMenu->addAction(convertAct);
	fileMenu->addAction(TestDescriptionAct);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);
	menuBar()->addSeparator();
	systemMenu = menuBar()->addMenu(tr("&Settings"));
#if 1
	systemMenu->addSeparator();
#endif
	//systemMenu->addAction(captureRadarDebugDataAct);
	menuBar()->addSeparator();
	viewFMCWMenu = menuBar()->addMenu(tr("&View"));
	viewFMCWMenu->addAction(showCtrlDockAct);
	viewFMCWMenu->addAction(showPcPanelAct);
	viewFMCWMenu->addAction(pcOffsetDockAct);

	menuBar()->addSeparator();
	helpMenu = menuBar()->addMenu(tr("&帮助"));
	//helpMenu->addAction(aboutAct);
	//helpMenu->addAction(updateAct);
	helpMenu->addAction(mannualAct);

}

void MainWindow::readSettings()
{
	QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
	const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
	if (geometry.isEmpty()) {
		const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
		resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
		move((availableGeometry.width() - width()) / 2,
		     (availableGeometry.height() - height()) / 2);
	} else {
		restoreGeometry(geometry);
	}
}

void MainWindow::writeSettings()
{
	QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
	settings.setValue("geometry",saveGeometry());
}


void MainWindow::setCurrentFile(const QString &fileName)
{
	curFile = fileName;
	textEdit->document()->setModified(false);
	setWindowModified(false);

	QString shownName = curFile;
	if (curFile.isEmpty())
		shownName = "No playback file selected";
	setWindowFilePath(shownName);
}

void MainWindow::SetTestDescription( void )
{

}

