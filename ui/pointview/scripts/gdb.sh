#!/bin/bash
cd "$( dirname "${BASH_SOURCE[0]}" )/.."
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_FLAGS="-Og -g3"
make -j12
gdb ./pointview