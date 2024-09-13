#!/usr/bin/env bash

###############################################################################
# Copyright 2019 AutoX Inc. All Rights Reserved.
###############################################################################


# Fail on first error.
set -e

cd /tmp

curl -sS -L --retry 3 -O http://www.vtk.org/files/release/7.1/VTK-7.1.0.tar.gz
tar xf VTK-7.1.0.tar.gz
mkdir VTK-7.1.0/build
cd VTK-7.1.0/build
cmake \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_RULE_MESSAGES=OFF \
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    -D VTK_USE_SYSTEM_JSONCPP=ON \
    -D VTK_Group_Qt=True \
    -D VTK_QT_VERSION=5 \
    ..
make -j "$(nproc)"
make install

# Fix include path. Suggestion from @Shuo Qin
sed -i 's/#include <QWidget>/#include <QtWidgets\/QWidget>/' /usr/local/include/vtk-7.1/QVTKWidget.h
sed -i 's/#include <QTimer>/#include <QtCore\/QTimer>/' /usr/local/include/vtk-7.1/QVTKWidget.h
