/******************************************************************************
 * Copyright 2023 AutoX. All Rights Reserved.
 *****************************************************************************/
//
// Created by tongdayang on 4/29/24.
//

#ifndef POINTVIEW_UTILS_H
#define POINTVIEW_UTILS_H
#include <QFileDialog>
#include <iostream>

#define Debug(a) \
  std::cout << __FILE__ << ", " << __LINE__ << ": " << (a) << std::endl;

QString GetOpenFileName(QString title, QString dir_path, QString file_type);
QString GetSaveFileName(QString title, QString dir_path, QString file_type);
QString GetSaveDirectory(QString title, QString dir_path);
#endif  // POINTVIEW_UTILS_H
