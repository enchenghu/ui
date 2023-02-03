#!/bin/sh

if [ -n "$DESTDIR" ] ; then
    case $DESTDIR in
        /*) # ok
            ;;
        *)
            /bin/echo "DESTDIR argument must be absolute... "
            /bin/echo "otherwise python's distutils will bork things."
            exit 1
    esac
fi

echo_and_run() { echo "+ $@" ; "$@" ; }

echo_and_run cd "/home/encheng/code_ui/ui/ui/src/rqt_bag/rqt_bag_plugins"

# ensure that Python install destination exists
echo_and_run mkdir -p "$DESTDIR/home/encheng/code_ui/ui/ui/install/lib/python3/dist-packages"

# Note that PYTHONPATH is pulled from the environment to support installing
# into one location when some dependencies were installed in another
# location, #123.
echo_and_run /usr/bin/env \
    PYTHONPATH="/home/encheng/code_ui/ui/ui/install/lib/python3/dist-packages:/home/encheng/code_ui/ui/ui/BUILD/lib/python3/dist-packages:$PYTHONPATH" \
    CATKIN_BINARY_DIR="/home/encheng/code_ui/ui/ui/BUILD" \
    "/usr/bin/python3" \
    "/home/encheng/code_ui/ui/ui/src/rqt_bag/rqt_bag_plugins/setup.py" \
     \
    build --build-base "/home/encheng/code_ui/ui/ui/BUILD/rqt_bag/rqt_bag_plugins" \
    install \
    --root="${DESTDIR-/}" \
    --install-layout=deb --prefix="/home/encheng/code_ui/ui/ui/install" --install-scripts="/home/encheng/code_ui/ui/ui/install/bin"
