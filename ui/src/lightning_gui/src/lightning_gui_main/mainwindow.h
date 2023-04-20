

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QProgressDialog>
#include <std_msgs/Bool.h>
#include <std_msgs/Float32.h>
#include <std_msgs/String.h>
#include <QInputDialog>
#include "viewpanel.h"

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
    void pcOffset_Control();
    void PcPanel_Control();
    void resetViews();
	void RecRvizPointcloudControl();
	void fullScreen();

	void SetTestDescription( void );

private:
	void createActions();
	void createMenus();
	void readSettings();
	void writeSettings();
	void setCurrentFile(const QString &fileName);
	void find_serial_ports();
	void find_video_devices();
	viewpanel* viewPanel;
	QString playback_file;
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
    QAction *pcOffsetDockAct;
    QAction *showPcPanelAct;

	QAction *recRvizPointcloudAct;

};

#endif
