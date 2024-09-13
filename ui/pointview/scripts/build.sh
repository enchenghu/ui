#!/bin/bash
cd "$( dirname "${BASH_SOURCE[0]}" )/.."
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j12
