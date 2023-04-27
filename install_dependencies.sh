#!/bin/bash

apt-get update && apt-get install autoconf libtool -y || exit 1
CURR_DIR=$(pwd)
mkdir ec_dev && cd ec_dev && git clone https://gitlab.com/etherlab.org/ethercat.git || exit 1
cd ethercat && git checkout stable-1.5 && ./bootstrap || exit 1
./configure --prefix=/usr/local/etherlab --disable-8139too --disable-eoe --enable-generic --disable-kernel || exit 1
make && make install && ldconfig || exit 1
cd "${CURR_DIR}" && rm -rf ec_dev || exit 1
