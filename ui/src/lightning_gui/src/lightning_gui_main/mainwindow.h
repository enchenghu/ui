

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
