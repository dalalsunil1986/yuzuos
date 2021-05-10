#!/bin/bash -e

TOOLS_DIR=$(pwd)/tools
CROSS_DIR=$TOOLS_DIR/cross
BUILD_DIR=$TOOLS_DIR/build
CACHE_DIR=$TOOLS_DIR/cache
PATCHES_DIR=$SOURCE_DIR/tools/patches

CORES=$(nproc)
PREFIX=$CROSS_DIR
TARGET=i686-yuzuos
MIRROR=https://ftp.gnu.org/gnu

BINUTILS_VERSION=2.35.1
BINUTILS_PACKAGE=binutils-$BINUTILS_VERSION
BINUTILS=$BINUTILS_PACKAGE.tar.xz

GCC_VERSION=10.2.0
GCC_PACKAGE=gcc-$GCC_VERSION
GCC=$GCC_PACKAGE.tar.xz

function cleanup {
  echo "Removing $BUILD_DIR/binutils"
  rm -rf $BUILD_DIR/binutils
  echo "Removing $BUILD_DIR/$BINUTILS_PACKAGE"
  rm -rf $BUILD_DIR/$BINUTILS_PACKAGE
  echo "Removing $BUILD_DIR/gcc"
  rm -rf $BUILD_DIR/gcc
  echo "Removing $BUILD_DIR/$GCC_PACKAGE"
  rm -rf $BUILD_DIR/$GCC_PACKAGE
}

trap cleanup EXIT

mkdir -pv $BUILD_DIR $CACHE_DIR $CROSS_DIR
mkdir -pv $SYSROOT_DIR $SYSROOT_DIR/usr/include

cp -RT $SOURCE_DIR/libs/c/include $SYSROOT_DIR/usr/include

cd $CACHE_DIR
wget -nc $MIRROR/binutils/$BINUTILS
wget -nc $MIRROR/gcc/$GCC_PACKAGE/$GCC

if [ ! -d $BUILD_DIR/$BINUTILS_PACKAGE ]; then
  cd $BUILD_DIR
  tar -xf $CACHE_DIR/$BINUTILS

  cd $BUILD_DIR/$BINUTILS_PACKAGE
  git init
  git add .
  git commit -m 'Initial commit'
  git apply --stat $PATCHES_DIR/binutils.patch
  git apply --check $PATCHES_DIR/binutils.patch
  git am --signoff < $PATCHES_DIR/binutils.patch 
fi

if [ ! -d $BUILD_DIR/$GCC_PACKAGE ]; then
  cd $BUILD_DIR
  tar -xf $CACHE_DIR/$GCC

  cd $BUILD_DIR/$GCC_PACKAGE
  git init
  git add .
  git commit -m 'Initial commit'
  git apply --stat $PATCHES_DIR/gcc.patch
  git apply --check $PATCHES_DIR/gcc.patch
  git am --signoff < $PATCHES_DIR/gcc.patch 
fi

mkdir $BUILD_DIR/binutils
cd $BUILD_DIR/binutils
../$BINUTILS_PACKAGE/configure --target=$TARGET --prefix=$PREFIX --with-sysroot=$SYSROOT_DIR --disable-nls --disable-werror
make -j $CORES
make install

cd $BUILD_DIR/$GCC_PACKAGE
./contrib/download_prerequisites
 
mkdir $BUILD_DIR/gcc
cd $BUILD_DIR/gcc
../$GCC_PACKAGE/configure --target=$TARGET --prefix=$PREFIX --with-sysroot=$SYSROOT_DIR --disable-nls --enable-languages=c
make -j $CORES all-gcc all-target-libgcc
make install-gcc install-target-libgcc
