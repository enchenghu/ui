#!/bin/bash
#
cd "$( dirname "${BASH_SOURCE[0]}" )"
cd ../..
#
VERSION=$(cat src/mainwindow.ui| grep -Eo "[0-9]{1,}\.[0-9]{1,}\.[0-9]{1,}" | head -n 1)
mkdir -p release/build
cd release/build
# build
cmake -DCMAKE_BUILD_TYPE=Release ../..
make -j12
cd ..
# pointview pkg dir
mkdir pointview
mkdir pointview/bin
mkdir pointview/libs
mkdir pointview/libs/third_party
mkdir pointview/qt
# copy bin
cp ./build/pointview pointview/bin
# copy library
ldd ./build/pointview | grep /build/ | awk 'NF == 4 { system("cp " $3 " pointview/libs") }'
ldd ./build/pointview | grep -v /build/ | awk 'NF == 4 { system("cp " $3 " pointview/libs/third_party") }'
# copy qt
cp -r /opt/qt515/lib pointview/qt
cp -r /opt/qt515/plugins pointview/qt
# copy config
cp -r ../config pointview
# copy script
cp ../scripts/release/pointview.sh pointview
# copy desktop entry
cp ../scripts/release/pointview.desktop pointview
cp ../scripts/release/generate_entry.sh pointview
cp ../resource/film.png pointview
# compress pkg
zip -q -r pointview_v${VERSION}_ubuntu20.04_release.zip pointview

