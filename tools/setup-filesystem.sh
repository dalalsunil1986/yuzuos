#!/bin/bash

[ -z "$MOUNT_DIR" ] && fail "MOUNT_DIR is not defined"
[ -z "$SOURCE_DIR" ] && fail "SOURCE_DIR is not defined"
[ -d "$SOURCE_DIR/base" ] || fail "$SOURCE_DIR/base not found"

echo "SetupFileSystem: Base system"
if command -v rsync >/dev/null; then
  rsync -aH --inplace "$SOURCE_DIR"/base/ $MOUNT_DIR/
  rsync -aH --inplace sysroot/ $MOUNT_DIR/
else
  echo "SetupFileSystem: Install rsync to speed up"
  cp -PdR "$SOURCE_DIR"/base/* $MOUNT_DIR
  cp -PdR sysroot/* $MOUNT_DIR
fi

echo "SetupFileSystem: OK"
