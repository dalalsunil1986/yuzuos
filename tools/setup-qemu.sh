#!/bin/bash -e

TOOLS_DIR=$(pwd)/tools
CROSS_DIR=$TOOLS_DIR/cross
BUILD_DIR=$TOOLS_DIR/build
CACHE_DIR=$TOOLS_DIR/cache

CORES=$(nproc)
MIRROR=https://download.qemu.org

QEMU_VERSION=6.0.0
QEMU_PACKAGE=qemu-$QEMU_VERSION
QEMU=$QEMU_PACKAGE.tar.xz

function cleanup {
  echo "Removing $BUILD_DIR/$QEMU_PACKAGE"
  rm -rf $BUILD_DIR/$QEMU_PACKAGE
}

trap cleanup EXIT

mkdir -pv $BUILD_DIR $CACHE_DIR $CROSS_DIR

cd $CACHE_DIR
wget -nc $MIRROR/$QEMU

if [ ! -d $BUILD_DIR/$QEMU_PACKAGE ]; then
  cd $BUILD_DIR
  tar -xf $CACHE_DIR/$QEMU
fi

cd $BUILD_DIR/$QEMU_PACKAGE
./configure --prefix=$CROSS_DIR --target-list=i386-softmmu
make -j $CORES
make install
