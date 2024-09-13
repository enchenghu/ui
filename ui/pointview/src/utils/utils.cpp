/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/
//
// Created by tongdayang on 6/14/24.
//

#include "utils.h"

QString GetOpenFileName(QString title, QString dir_path, QString file_type) {
  QString config_file =
      QFileDialog::getOpenFileName(nullptr, title, dir_path, file_type, nullptr,
                                   QFileDialog::DontUseNativeDialog);
  return config_file;
}

QString GetSaveFileName(QString title, QString dir_path, QString file_type) {
  QString fileName =
      QFileDialog::getSaveFileName(nullptr, title, dir_path, file_type, nullptr,
                                   QFileDialog::DontUseNativeDialog);
  return fileName;
}

QString GetSaveDirectory(QString title, QString dir_path) {
  QString save_folder_ = QFileDialog::getExistingDirectory(
      nullptr, title, dir_path,
      QFileDialog::DontUseNativeDialog | QFileDialog::ShowDirsOnly |
          QFileDialog::DontResolveSymlinks);
  return save_folder_;
}