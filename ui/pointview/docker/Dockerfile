FROM ubuntu:20.04

RUN sed -i "s@http://.*archive.ubuntu.com@https://mirrors.bfsu.edu.cn@g" /etc/apt/sources.list && \
    sed -i "s@http://.*security.ubuntu.com@https://mirrors.bfsu.edu.cn@g" /etc/apt/sources.list && \
    apt -o "Acquire::https::Verify-Peer=false" update && \
    apt -o "Acquire::https::Verify-Peer=false" install -y ca-certificates

RUN DEBIAN_FRONTEND=noninteractive apt update && \
    DEBIAN_FRONTEND=noninteractive apt install -y --no-install-recommends \
                cmake \
                git \
                zip \
                build-essential \
                software-properties-common \
                libpcl-dev \
                libpcap-dev \
                libyaml-cpp-dev \
                libprotobuf-dev \
                protobuf-compiler \
                libopencv-dev \
                libgoogle-glog-dev \
                libgflags-dev \
                && \
    rm -rf /var/lib/apt/lists/*

RUN DEBIAN_FRONTEND=noninteractive apt update && \
    DEBIAN_FRONTEND=noninteractive apt install -y --no-install-recommends \
                sudo \
                gdb \
                qtcreator \
                && \
    rm -rf /var/lib/apt/lists/*

# use qt5.15 instead of qt5.12 to fix some issues of QtreeWidget rendering
RUN add-apt-repository -y ppa:beineri/opt-qt-5.15.3-focal && \
    apt update && \
    apt install -y qt515base qt515multimedia \
    libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
    libgstreamer-plugins-bad1.0-dev gstreamer1.0-plugins-base \
    gstreamer1.0-plugins-good gstreamer1.0-plugins-bad \
    gstreamer1.0-plugins-ugly gstreamer1.0-libav \
    gstreamer1.0-tools gstreamer1.0-x \
    gstreamer1.0-alsa gstreamer1.0-gl \
    gstreamer1.0-gtk3 gstreamer1.0-qt5 \
    gstreamer1.0-pulseaudio && \
    rm -rf /var/lib/apt/lists/*

ENV QTDIR="/opt/qt515"
ENV PATH="/opt/qt515/bin:$PATH"
ENV LD_LIBRARY_PATH="/opt/qt515/lib/x86_64-linux-gnu:/opt/qt515/lib:$LD_LIBRARY_PATH"
ENV PKG_CONFIG_PATH="/opt/qt515/lib/pkgconfig:$PKG_CONFIG_PATH"
