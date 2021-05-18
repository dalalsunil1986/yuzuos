#!/bin/bash -e

TOOLS_DIR=$(pwd)/tools
CROSS_DIR=$TOOLS_DIR/cross
BUILD_DIR=$TOOLS_DIR/build
CACHE_DIR=$TOOLS_DIR/cache
PATCHES_DIR=$SOURCE_DIR/tools/patches

CACHE_FILE="toolchain_github_actions.tar.gz"
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
mkdir -pv $SYSROOT_DIR $SYSROOT_DIR/usr/include/ukernel

if [ "$TOOLCHAIN_CACHE" = "yes" ] ; then
  if [ -r "$CACHE_DIR/$CACHE_FILE" ] ; then
    echo "$CACHE_FILE exists"
    
    cd $CROSS_DIR
    if tar xzf "$CACHE_DIR/$CACHE_FILE" ; then
      echo "Cache extracted"
      exit 0
    else
      echo ""
      echo "Could not extract cached toolchain"
      echo "This means the cache is broken"
      echo ""

      rm -f "$CACHE_FILE/$CACHE_FILE"
    fi
  else
    echo "$CACHE_FILE does not exist"
  fi
fi

cp -r $SOURCE_DIR/libs/c/include/* $SYSROOT_DIR/usr/include
cp -r $SOURCE_DIR/kernel/include/ukernel/* $SYSROOT_DIR/usr/include/ukernel

cd $CACHE_DIR
wget -nc $MIRROR/binutils/$BINUTILS
wget -nc $MIRROR/gcc/$GCC_PACKAGE/$GCC

if [ ! -d $BUILD_DIR/$BINUTILS_PACKAGE ]; then
  cd $BUILD_DIR
  tar -xf $CACHE_DIR/$BINUTILS

  cd $BUILD_DIR/$BINUTILS_PACKAGE
  git init > /dev/null
  git add . > /dev/null
  git commit -m 'Initial commit' > /dev/null
  git apply $PATCHES_DIR/binutils.patch > /dev/null
fi

if [ ! -d $BUILD_DIR/$GCC_PACKAGE ]; then
  cd $BUILD_DIR
  tar -xf $CACHE_DIR/$GCC

  cd $BUILD_DIR/$GCC_PACKAGE
  git init > /dev/null
  git add . > /dev/null
  git commit -m 'Initial commit' > /dev/null
  git apply $PATCHES_DIR/gcc.patch > /dev/null
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

if [ "$TOOLCHAIN_CACHE" = "yes" ] ; then
  cd $CACHE_DIR
  rm -f $CACHE_FILE
  tar -czf $CACHE_FILE -C $CROSS_DIR $(ls -A $CROSS_DIR)
fi
