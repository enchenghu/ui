/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/
//
// Created by tongdayang on 4/29/24.
//

#ifndef POINTVIEW_UTILS_H
#define POINTVIEW_UTILS_H
#include <QFileDialog>
#include <QMessageBox>
#include <iostream>

#include <glog/logging.h>

QString GetOpenFileName(QString title, QString dir_path, QString file_type);
QString GetSaveFileName(QString title, QString dir_path, QString file_type);
QString GetSaveDirectory(QString title, QString dir_path);
int OpenMessageBox(QString title_text, QString informative_text,
                   QFlags<QMessageBox::StandardButton> buttons);
std::string Uint8toHex(uint8_t i);

#endif  // POINTVIEW_UTILS_H
