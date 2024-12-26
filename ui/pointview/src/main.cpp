#include <QApplication>

#include <glog/logging.h>

#include "mainwindow.h"

void(writer)(const char* data, int size) { LOG(ERROR) << data; }

int main(int argc, char* argv[]) {
  QApplication a(argc, argv);
  // check global config path
  QDir* folder = new QDir;
  QString config_path = QDir::homePath() + "/.pointview/";
  if (folder->exists(config_path)) {
    qDebug() << "config folder exist: " << config_path;
  } else {
    try {
      folder->mkpath(config_path);
    } catch (const std::exception& e) {
      qDebug() << e.what();
      return -1;
    }
    try {
      folder->mkpath(config_path + "/log/");
    } catch (const std::exception& e) {
      qDebug() << e.what();
    }
    qDebug() << "creat config folder: " << config_path;
    QString cmd = "cp ./config/*  " + config_path;
    try {
      system(cmd.toStdString().c_str());
    } catch (const std::exception& e) {
      qDebug() << e.what();
    }
  }
  // init glog
  google::InitGoogleLogging(argv[0]);
  FLAGS_log_dir = config_path.toStdString() + "log";
  FLAGS_alsologtostderr = 1;
  google::InstallFailureSignalHandler();
  google::InstallFailureWriter(&writer);
  // init main window
  autox::pointview::MainWindow w;
  w.show();
  return a.exec();
}
