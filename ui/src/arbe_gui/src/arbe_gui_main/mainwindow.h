/*
 * Copyright (C) 2019 Arbe - http://www.arberobotics.com/ 
 * 
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Arbe nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define ONLY_SHOW_UI 0
#include <QMainWindow>
#include <QtWidgets>
#include <QProgressDialog>
#include <std_msgs/Bool.h>
#include <std_msgs/Float32.h>
#include <std_msgs/String.h>
#include <QInputDialog>


class QAction;
class QMenu;
class QPlainTextEdit;
class QListWidget;
class QStackedWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
	MainWindow();
protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private Q_SLOTS:
	void open();
	void stopPlayback();
	void setSaveFolder();
	void about();
	void quit();
	void convertRosbag();
    void CtrlPanel_Control();
    void PcPanel_Control();
	void RecRvizPointcloudControl();

	void SetTestDescription( void );

private:
	void createActions();
	void createMenus();
	void readSettings();
	void writeSettings();
	void setCurrentFile(const QString &fileName);
	void find_serial_ports();
	void find_video_devices();

	QPlainTextEdit *textEdit;
	QString curFile;
	QMenu *fileMenu;
	QMenu *viewMenu;
	QMenu *systemMenu;
    QMenu *recordingMenu;
	QMenu *helpMenu;
	QMenu *viewFMCWMenu;

	QToolBar *fileToolBar;
	QToolBar *editToolBar;
	QAction *newAct;
	QAction *openAct;
	QAction *setRecFolderAct;
	QAction *closeAct;
	QAction *exitAct;
	QAction *cutAct;
	QAction *aboutAct;
	QAction *updateAct;
	QAction *aboutQtAct;
	QAction *configAct;
	QAction *debugAct;
	QAction *controlAct;
	QAction *convertAct;
	QAction *TestDescriptionAct;

	QListWidget *contentsWidget;
	QComboBox *debug_port_combo;
	QComboBox *video_device_combo;
	QComboBox *radar_region_combo;
	QString openFilesPath;


    QAction *showCtrlDockAct;
    QAction *showPcPanelAct;

	QAction *recRvizPointcloudAct;

};

#endif
