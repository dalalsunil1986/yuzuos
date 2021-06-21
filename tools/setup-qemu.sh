#!/bin/bash -e

fail() {
  echo "FAILED: $*"
  exit 1
}

[ -z "$BUILD_DIR" ] && fail "BUILD_DIR is not set"
[ -z "$CACHE_DIR" ] && fail "CACHE_DIR is not set"
[ -z "$CROSS_DIR" ] && fail "CROSS_DIR is not set"

CORES=$(nproc)
MIRROR=https://download.qemu.org

QEMU_VERSION=6.0.0
QEMU_PACKAGE=qemu-$QEMU_VERSION
QEMU=$QEMU_PACKAGE.tar.xz

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
