#!/bin/bash -e

fail() {
  echo "FAILED: $*"
  exit 1
}

[ -z "$BUILD_DIR" ] && fail "BUILD_DIR is not set"
[ -z "$CACHE_DIR" ] && fail "CACHE_DIR is not set"
[ -z "$CROSS_DIR" ] && fail "CROSS_DIR is not set"
[ -z "$SOURCE_DIR" ] && fail "SOURCE_DIR is not set"
[ -z "$SYSROOT_DIR" ] && fail "SYSROOT_DIR is not set"
[ -z "$PREFIX" ] && fail "PREFIX is not set"
[ -z "$TARGET" ] && fail "TARGET is not set"

PATCHES_DIR=$SOURCE_DIR/tools/patches

CACHE_FILE="toolchain_github_actions.tar.gz"
CORES=$(nproc)
MIRROR=https://ftp.gnu.org/gnu

BINUTILS_VERSION=2.35.1
BINUTILS_PACKAGE=binutils-$BINUTILS_VERSION
BINUTILS=$BINUTILS_PACKAGE.tar.xz

GCC_VERSION=10.2.0
GCC_PACKAGE=gcc-$GCC_VERSION
GCC=$GCC_PACKAGE.tar.xz

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

mkdir -pv $SYSROOT_DIR/usr/include/ukernel

cp -r $SOURCE_DIR/libs/c/include/* $SYSROOT_DIR/usr/include
cp -r $SOURCE_DIR/kernel/include/ukernel/* $SYSROOT_DIR/usr/include/ukernel

echo "Cross dir: $CROSS_DIR"
echo "Build dir: $BUILD_DIR"
echo "Cache dir: $CACHE_DIR"
echo "Binutils: $BINUTILS"
echo "GCC: $GCC"
echo ""

cd $CACHE_DIR
echo "Binutils: Downloading"
wget -nc $MIRROR/binutils/$BINUTILS
echo "Gcc: Downloading"
wget -nc $MIRROR/gcc/$GCC_PACKAGE/$GCC

if [ ! -d $BUILD_DIR/$BINUTILS_PACKAGE ]; then
  echo "Binutils: Extracting"
  cd $BUILD_DIR
  tar -xf $CACHE_DIR/$BINUTILS

  patch_file=$PATCHES_DIR/binutils.patch
  echo "Binutils: Patching using ($patch_file)"
  cd $BUILD_DIR/$BINUTILS_PACKAGE
  git init > /dev/null
  git add . > /dev/null
  git commit -m 'Initial commit' > /dev/null
  git apply $patch_file > /dev/null
fi

if [ ! -d $BUILD_DIR/$GCC_PACKAGE ]; then
  echo "GCC: Extracting"
  cd $BUILD_DIR
  tar -xf $CACHE_DIR/$GCC

  patch_file=$PATCHES_DIR/gcc.patch
  echo "GCC: Patching using ($patch_file)"
  cd $BUILD_DIR/$GCC_PACKAGE
  git init > /dev/null
  git add . > /dev/null
  git commit -m 'Initial commit' > /dev/null
  git apply $patch_file > /dev/null
fi

mkdir -pv $BUILD_DIR/binutils
cd $BUILD_DIR/binutils
echo "Binutils: Configure"
../$BINUTILS_PACKAGE/configure --target=$TARGET --prefix=$PREFIX --with-sysroot=$SYSROOT_DIR --disable-nls --disable-werror
echo "Binutils: Compile"
make -j $CORES
make install

cd $BUILD_DIR/$GCC_PACKAGE
echo "GCC: Compile prerequisites"
./contrib/download_prerequisites
 
mkdir -pv $BUILD_DIR/gcc
cd $BUILD_DIR/gcc
echo "GCC: Configure"
../$GCC_PACKAGE/configure --target=$TARGET --prefix=$PREFIX --with-sysroot=$SYSROOT_DIR --disable-nls --enable-languages=c
echo "GCC: Compile"
make -j $CORES all-gcc all-target-libgcc
make install-gcc install-target-libgcc

if [ "$TOOLCHAIN_CACHE" = "yes" ] ; then
  cd $CACHE_DIR
  rm -f $CACHE_FILE
  
  echo "Cache: Compacting"
  tar -czf $CACHE_FILE -C $CROSS_DIR $(ls -A $CROSS_DIR)
fi
