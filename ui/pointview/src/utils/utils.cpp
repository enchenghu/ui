/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/
//
// Created by tongdayang on 6/14/24.
//

#include "utils.h"

#include <iomanip>

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

int OpenMessageBox(QString title_text, QString informative_text,
                   QFlags<QMessageBox::StandardButton> buttons) {
  QMessageBox msgBox;
  msgBox.setText(title_text);
  msgBox.setInformativeText(informative_text);
  msgBox.setStandardButtons(buttons);
  return msgBox.exec();
}

std::string Uint8toHex(uint8_t i) {
  std::stringstream stream;
  stream << std::setfill('0') << std::setw(sizeof(uint8_t) * 2) << std::hex
         << static_cast<int>(i);
  return stream.str();
}

void Int2Bytes(int num, std::vector<uint8_t>& array) {
  uint16_t value = static_cast<uint16_t>(num);
  array.push_back(static_cast<uint8_t>(value >> 8));
  array.push_back(static_cast<uint8_t>(value & 0xff));
}