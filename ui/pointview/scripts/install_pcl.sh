#!/usr/bin/env bash

###############################################################################
# Copyright 2019 AutoX Inc. All Rights Reserved.
###############################################################################


# Fail on first error.
set -e

cd /tmp
git clone --branch pcl-1.9.1 --depth 1 https://github.com/PointCloudLibrary/pcl.git
cd pcl
sed -i "114s#Eigen::Matrix<double, 6, 6> hessian#hessian_.setZero()#" registration/include/pcl/registration/impl/ndt.hpp
sed -i "120s#hessian#hessian_#" registration/include/pcl/registration/impl/ndt.hpp
sed -i "128s#hessian#hessian_#" registration/include/pcl/registration/impl/ndt.hpp
sed -i "143s#hessian#hessian_#" registration/include/pcl/registration/impl/ndt.hpp
sed -i "480 a \      Eigen::Matrix<double, 6, 6> hessian_;\n" registration/include/pcl/registration/registration.h
sed -i "479 a \      Eigen::Matrix<double, 6, 6> getHessianMatrix();\n" registration/include/pcl/registration/registration.h
sed -i "$ a \template <typename PointSource, typename PointTarget, typename Scalar>\nEigen::Matrix<double, 6, 6>\npcl::Registration<PointSource, PointTarget, Scalar>::getHessianMatrix() {\n  return hessian_;\n}\n" registration/include/pcl/registration/impl/registration.hpp
sed -i "233 a \      using Registration<PointSource, PointTarget>::hessian_;" registration/include/pcl/registration/ndt.h

mkdir build
cd build

# The container built with -march=native may not work on all systems since GCC
#  could enable modern instruction set (ex. avx512f) that could only be
#  available on the system that is building the container. PCL_ENABLE_SSE=ON
#  (default option) sets -march=native along with -msse and -fpmath=sse. 
# The PCL_ENABLE_SSE=OFF fixes this issue however this causes memory alignment 
#  issue during runtime due to avx being diabled in the PCL libraries while our
#  applications are being built with it.
# Manually add avx2
cmake \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_RULE_MESSAGES=OFF \
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    -D PCL_ENABLE_SSE=OFF \
    -D CMAKE_CXX_FLAGS="-mavx2 --std=c++14" \
    ..
make -j "$(nproc)"
make install

cd /tmp
rm -rf *
