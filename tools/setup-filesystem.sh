#!/bin/bash

[ -z "$MOUNT_DIR" ] && fail "MOUNT_DIR is not defined"
[ -z "$SOURCE_DIR" ] && fail "SOURCE_DIR is not defined"

echo "SetupFileSystem: Base system"
if command -v rsync >/dev/null; then
  rsync -aH --inplace sysroot/ $MOUNT_DIR/
else
  echo "SetupFileSystem: Install rsync to speed up"
  $CP -PdR sysroot/* $MOUNT_DIR
fi

echo "SetupFileSystem: OK"
