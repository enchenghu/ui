#ifndef VIDEO_PLAYER_H
#define VIDEO_PLAYER_H

#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSpacerItem>
#include <QTimer>

#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QMediaPlaylist>
#include <QtMultimediaWidgets/QGraphicsVideoItem>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>

#include "utils/common/device_context.h"
#include "utils/config.h"
#include "utils/property_tree.h"
namespace Ui {
class videoPlayer;
}
namespace autox {
namespace pointview {
class VideoPlayer : public QMainWindow {
  Q_OBJECT

 public:
  VideoPlayer(std::shared_ptr<DeviceContext> device_context);
  ~VideoPlayer();

 private slots:
  void onStateChanged(QMediaPlayer::State state);
  void onDurationChanged(qint64 duration);
  void onPositionChanged(qint64 position);
  void on_openButtonClicked();

  void openVideo();

  void playVideo();

  void pauseVideo();

  void stopPlay();

 protected:
  void showEvent(QShowEvent* event) override;
  void closeEvent(QCloseEvent* event) override;

 private:
  Ui::videoPlayer* ui;
  std::shared_ptr<QPushButton> open_button_;
  PropertyPairs properties_;
  std::shared_ptr<DeviceContext> device_context_;
  QMediaPlayer* player;
  QMediaPlaylist* playlist;
  QGraphicsScene* scene;
  QVideoWidget* videoWidget;
  QGraphicsVideoItem* videoitem;
  QString durationTime;
  QString positionTime;
  void initPlayer();
};
}  // namespace pointview

}  // namespace autox
#endif  // VIDEO_PLAYER_H
