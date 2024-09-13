#!/usr/bin/env bash

PROJECT_ROOT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd -P )"
cd $PROJECT_ROOT_DIR
cp ./pointview.desktop ./pointview.desktop.bak
echo "Icon=${PROJECT_ROOT_DIR}/film.png" >> ./pointview.desktop.bak
echo "Exec=\"${PROJECT_ROOT_DIR}/pointview.sh\" %f" >> ./pointview.desktop.bak
mv ./pointview.desktop.bak ~/.local/share/applications/pointview.desktop
