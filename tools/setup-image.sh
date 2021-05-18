#!/bin/bash

TOOLS_DIR=$(pwd)/tools
export MOUNT_DIR=$TOOLS_DIR/mount

image_size() {
  expr "$(du -sk "$1" | cut -f1)" / 1024
}

IMAGE_SIZE=$(($(image_size sysroot) + $(image_size $SOURCE_DIR/base) + 100))
IMAGE_SIZE=${IMAGE_SIZE:-600}
IMAGE_SIZE_BYTES=$((IMAGE_SIZE * 1024 * 1024))
IMAGE_FILE=qemu_img
IMAGE_EXIST=0

fail() {
  echo "FAILED: $*"
  exit 1
}
export -f fail

[ -z "$SOURCE_DIR" ] && fail "SOURCE_DIR is not defined"

if [ "$(id -u)" != 0 ]; then
  exec sudo -E -- "$0" "$@" || fail "Need root privileges"
else
  : "${SUDO_UID:=0}" "${SUDO_GID:=0}"
fi

if [ -f $IMAGE_FILE ]; then
  echo "SetupImage: Checking if image exist"

  IMAGE_EXIST=1
  result=0
  e2fsck -f -y $IMAGE_FILE || result=$?
  
  if [ $result -ge 4 ]; then
    rm -f $IMAGE_FILE
    IMAGE_EXIST=0
    echo "SetupImage: Removed existing image"
  else
    echo "SetupImage: OK"
  fi
fi

if [ $IMAGE_EXIST -ne 1 ]; then
  echo "SetupImage: Creating image ($IMAGE_FILE)"
  qemu-img create -q -f raw $IMAGE_FILE $IMAGE_SIZE_BYTES || fail "Creating image"
  chown "$SUDO_UID":"$SUDO_GID" $IMAGE_FILE || fail "Adjusting image permissions"
  echo "SetupImage: OK"

  echo "SetupImage: Creating file system"
  mke2fs -q $IMAGE_FILE || fail "Creating file system"
  echo "SetupImage: OK"
fi

if [ $IMAGE_EXIST -eq 1 ];  then
  IMAGE_SIZE_BYTES_OLD=$(wc -c < $IMAGE_FILE)
  if [ $IMAGE_SIZE_BYTES -gt "$IMAGE_SIZE_BYTES_OLD" ]; then
    echo "SetupImage: Resizing image"
    qemu-img resize -f raw $IMAGE_FILE $IMAGE_SIZE_BYTES || fail "Resizing image"
    if ! resize2fs $IMAGE_FILE; then
      rm -f $IMAGE_FILE
      IMAGE_EXIST=0
      echo "SetupImage: Removed existing image"
    fi
    echo "SetupImage: OK"
  fi
fi

echo "SetupImage: Mounting file system"
mkdir -p $MOUNT_DIR
if ! eval "mount $IMAGE_FILE $MOUNT_DIR"; then
  fail "File system mount"
else
  echo "SetupImage: OK"
fi

cleanup() {
  if [ -d $MOUNT_DIR ]; then
    echo "SetupImage: Unmounting file system"
    umount $MOUNT_DIR || ( sleep 1 && sync && umount $MOUNT_DIR )
    rm -rf $MOUNT_DIR
    echo "SetupImage: OK"
  fi
}
trap cleanup EXIT

$SOURCE_DIR/tools/setup-filesystem.sh
