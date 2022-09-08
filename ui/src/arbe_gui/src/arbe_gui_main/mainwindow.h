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

bool get_rec_topics(bool &all_pp, bool &camera, bool &all_debug, bool &dl_training, bool &fusion, bool &parsed, bool &raw_topics, bool &settings_topics, bool &legacy_pc, bool &nav_topics, bool &object_list);
bool get_rec_all_pp();
std::string get_save_folder();
bool get_rec_rviz_pointcloud();
uint8_t get_video_dev_num();

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
	void init_cmnd_line_subs( void );
	void on_cmnd_line_distdisp_callback(std_msgs::Bool);
    void on_cmnd_dview_x_callback(std_msgs::Float32);
    void on_cmnd_dview_y_callback(std_msgs::Float32);
	void on_cmnd_camera_file_callback(std_msgs::String);
protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private Q_SLOTS:
	void open();
	void stopPlayback();
	void setSaveFolder();
	void about();
	void ConfigDialog();
	//void DebugDialog();

	void load_settings();
	void save_settings();
	void updateSettingsStrings( void );
	void quit();
	void enableCamera();
	void launchUSBCam();
	void launchGigECam();
	void enableFreeSpaceTopView();
	void radarReset();
	void convertRosbag();
	void LoggingControl();
	void RecCheckAll_Control();
	void RecUncheckAll_Control();
	void RecAllPostProcessing_Control();
    void RecCamera_Control();
    void RecDebug_Control();
    void RecDLTraining_Control();
    void RecFusion_Control();
    void RecParsed_Control();
    void RecNav_Control();
	void RecObjList_Control();
    void RecRawTopics_Control();
    void RecSettingsTopics_Control();
    void RecLegacyPointcloud_Control();

	void RecRvizPointcloudControl();
	void selectDetectionsType();
	void selectSlam();
	void resetOmegaCalib( void );
	void imposeOmegaZero( void );
	void restoreDefaults( void );
	void dispFSOnPc( void );
	void EnableAggregation( void );
	void setDLTrainingPlayback( void );
    void setDLTrainingView( void );
    void setClassesToShow(void );
    void EnableGeoLocalization( void );
	void ResetLocalCartesian( void );
	void enableSlam( void );
	void enableClassification( void );
	void enableFreeSpace( void );
	void resetSlam( void );
	void ChamberModeControl();
	void read_radar_debug_data();

	void load_camera_calibration();
	void toggle_camera_buffering();
	void toggle_slam_on_cam();
    void toggle_FS_on_cam();
	void toggle_funnel_on_cam();
	void toggle_discard_out_of_el_context();
	void toggle_asphalt_text();
	void toggle_dist_on_cam();
	void toggle_slam_display_economy();
    void toggle_slam_color_by_class();
	void slam_reconfigure();
	void SetTestDescription( void );

private:
	void createActions();
	void createMenus();
	void createToolBars();
	void createStatusBar();
	void readSettings();
	void writeSettings();
	void setCurrentFile(const QString &fileName);
	void find_serial_ports();
	void find_video_devices();

	QPlainTextEdit *textEdit;
	QString curFile;
	QMenu *fileMenu;
	QMenu *aiMenu;
	QMenu *viewMenu;
	QMenu *cameraMenu;
	QMenu *systemMenu;
    QMenu *recordingMenu;
	QMenu *helpMenu;
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
	QAction *cameraAct;
	QAction *GigEcameraAct;
	QAction *USBcameraAct;
	QAction *freeSpaceTopVeiwAct;
	QAction *resetAct;
	QAction *loggingAct;
	QAction *ChamberAct;
	QAction *DetectionsTypeAct;
	QAction *ShowSlamAct;
	QAction *EnableGeogAct;
	QAction *EnableAggregationAct;
    QAction *DLTrainingViewAct;
	QAction *DLTrainingPlaybackAct;
    QAction *DLTrainingChooseAllClassesAct;
    QAction *DLTrainingChooseClass0Act;
    QAction *DLTrainingChooseClass1Act;
    QAction *DLTrainingChooseClass2Act;
    QAction *DLTrainingChooseClass3Act;
    QAction *DLTrainingChooseClass4Act;
    QAction *ResetLocalCartesianAct;
	QAction *EnableSlamAct;
	QAction *EnableClassificationAct;
	QAction *EnableFreeSpaceAct;
	QAction *ResetSlamAct;
	QAction *ResetOmegaCalibAct;
	QAction *ImposeOmegaZeroAct;
	QAction *dispFSPolyOnPcAct;
	QAction *restoreDefaultsAct;
	QAction *TestDescriptionAct;
	QListWidget *contentsWidget;
	QComboBox *debug_port_combo;
	QComboBox *video_device_combo;
	QComboBox *radar_region_combo;
	QLineEdit *bias_4d_edit;
	QLineEdit *noise_level_edit;
	QString openFilesPath;

	QAction *recCheckAllAct;
	QAction *recUncheckAllAct;
    QAction *recAllProcessedTopicsAct;
    QAction *recCameraAct;
    QAction *recDebugAct;
    QAction *recDLTrainingAct;
    QAction *recFusionAct;
    QAction *recParsedAct;
    QAction *recRawTopicsAct;
    QAction *recSettingsTopicsAct;
    QAction *recNavTopicsAct;
    QAction *recLegacyPcAct;
	QAction *recObjectListAct;

	QAction *recRvizPointcloudAct;
	QAction *loadCamCalibAct;
	QAction *slamReconfigureAct;
	QAction *allowCameraBufferingForSyncAct;
	QAction *toggleSlamOnImageAct;
	QAction *toggleFSOnImageAct;
	QAction *toggleFunnelOnImageAct;
	QAction *toggleDiscardOutOfContextAct;
	QAction *toggleAsphaltTextSlidersAct;

	QAction *toggleDispDistOnImageAct;
	QAction *toggleSlamDisplayEconomyModeAct;
    QAction *toggleSlamColorByClassAct;
	QAction *captureRadarDebugDataAct;
	QCheckBox *ntc_3d_checkbox;
	QCheckBox *ntc_4d_checkbox;
	QCheckBox *cfar_3d_checkbox;
	QCheckBox *cfar_4d_checkbox;
	QCheckBox *enable_phase_checkbox;
	QCheckBox *enable_thr_metadata_checkbox;

	bool slamDispOnCamEcoMode;
	bool cameraBuffering;
	bool slamDispOnCam;
	bool FSDispOnCam = true;
	bool FunnelDispOnCam = true;
	bool slamDispDistOnCam;
    bool slamColorByClass;
	bool discardOutOfElContext = false;
};

#endif
