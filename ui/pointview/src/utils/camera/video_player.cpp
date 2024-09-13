#include "video_player.h"

#include <QTime>

#include "ui_video_player.h"
namespace autox {
namespace pointview {

VideoPlayer::VideoPlayer(std::shared_ptr<DeviceContext> device_context)
    : QMainWindow(device_context->getDisplayContext()->getParent()),
      open_button_(new QPushButton(this)),
      videoWidget(new QVideoWidget(this)),
      ui(new Ui::videoPlayer) {
  device_context_ = device_context;
  ui->setupUi(this);
  open_button_->setText("Open");
  properties_.push_back({"Video Player", open_button_});
  connect(open_button_.get(), SIGNAL(clicked(bool)), this,
          SLOT(on_openButtonClicked()));
  device_context_->addProperties(properties_);

  player = new QMediaPlayer(this);
  playlist = new QMediaPlaylist(player);
  playlist->setCurrentIndex(0);
  playlist->setPlaybackMode(QMediaPlaylist::Loop);
  player->setPlaylist(playlist);
  player->setNotifyInterval(1000);
  videoWidget->setAspectRatioMode(Qt::IgnoreAspectRatio);
  ui->gridLayout_2->addWidget(videoWidget);

  player->setVideoOutput(videoWidget);
  initPlayer();
  connect(player, &QMediaPlayer::stateChanged, this,
          &VideoPlayer::onStateChanged);
  connect(player, &QMediaPlayer::positionChanged, this,
          &VideoPlayer::onPositionChanged);
  connect(player, &QMediaPlayer::durationChanged, this,
          &VideoPlayer::onDurationChanged);
  connect(ui->horizontalSlider, &QSlider::sliderMoved, this,
          [this](int value) { player->setPosition(value); });
  connect(ui->open, SIGNAL(clicked(bool)), this, SLOT(openVideo()));
  connect(ui->play, SIGNAL(clicked(bool)), this, SLOT(playVideo()));
  connect(ui->pause, SIGNAL(clicked(bool)), this, SLOT(pauseVideo()));
  connect(ui->stop, SIGNAL(clicked(bool)), this, SLOT(stopPlay()));
}

VideoPlayer::~VideoPlayer() { delete ui; }

void VideoPlayer::on_openButtonClicked() {
  this->move(this->parentWidget()->geometry().center() - this->rect().center());
  this->show();
}

void VideoPlayer::playVideo() { this->player->play(); }

void VideoPlayer::openVideo() {
  if (player->state() == QMediaPlayer::PlayingState) player->stop();
  QString currentpath = QDir::homePath();
  QString digtitle = "please choose video file";
  QString filefliter = "all files(*.*)";
  QString allfiles =
      QFileDialog::getOpenFileName(this, digtitle, currentpath, filefliter);
  if (allfiles.isEmpty()) {
    QMessageBox::critical(this, "warnning", "no video file", QMessageBox::Yes);
    return;
  }
  playlist->clear();
  QFileInfo fileinfo(allfiles);
  ui->label->setText(fileinfo.fileName());
  playlist->addMedia(QUrl::fromLocalFile(allfiles));
  player->play();
}

void VideoPlayer::pauseVideo() { this->player->pause(); }

void VideoPlayer::stopPlay() { initPlayer(); }

void VideoPlayer::onStateChanged(QMediaPlayer::State state) {
  ui->play->setEnabled(!(state == QMediaPlayer::PlayingState));
  ui->pause->setEnabled(state == QMediaPlayer::PlayingState);
  ui->stop->setEnabled(state == QMediaPlayer::PlayingState);
}

void VideoPlayer::onDurationChanged(qint64 duration) {
  ui->horizontalSlider->setMaximum(duration);
  int secs = duration / 1000;
  int mins = secs / 60;
  secs = secs % 60;
  durationTime = QString::asprintf("%d:%d", mins, secs);
  ui->label_2->setText(positionTime + "/" + durationTime);
}

void VideoPlayer::onPositionChanged(qint64 position) {
  if (ui->horizontalSlider->isSliderDown()) return;
  ui->horizontalSlider->setSliderPosition(position);
  int secs = position / 1000;
  int mins = secs / 60;
  secs = secs % 60;
  positionTime = QString::asprintf("%d:%d", mins, secs);
  ui->label_2->setText(positionTime + "/" + durationTime);
}

void VideoPlayer::showEvent(QShowEvent* event) {
  initPlayer();
  player->setVideoOutput(videoWidget);
}

void VideoPlayer::closeEvent(QCloseEvent* event) {
  initPlayer();
  QVideoWidget* ptr = nullptr;
  player->setVideoOutput(ptr);
}

void VideoPlayer::initPlayer() {
  if (playlist) playlist->clear();
  if (player) player->stop();
  if (ui) {
    ui->horizontalSlider->setSliderPosition(0);
    ui->label->setText("no file");
    ui->label_2->setText("0s/0s");
  }
}

}  // namespace pointview

}  // namespace autox
