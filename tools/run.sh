#!/bin/bash -e

fail() {
  echo "FAILED: $*"
  exit 1
}

[ -z "$SYSROOT_DIR" ] && fail "SYSROOT_DIR is not set"

[ -z "$YUZUOS_QEMU_BIN" ] && YUZUOS_QEMU_BIN="qemu-system-i386"
[ -z "$YUZUOS_QEMU_IMG" ] && YUZUOS_QEMU_IMG="qemu_img"
[ -z "$YUZUOS_QEMU_KERNEL" ] && YUZUOS_QEMU_KERNEL="${SYSROOT_DIR}/boot/kernel"
[ -z "$YUZUOS_QEMU_MEMORY" ] && YUZUOS_QEMU_MEMORY="128M"
[ -z "$YUZUOS_QEMU_CPU" ] && YUZUOS_QEMU_CPU="max"
[ -z "$YUZUOS_QEMU_ARG" ] && YUZUOS_QEMU_ARG="
  $YUZUOS_QEMU_EXTRA_ARG 
  -kernel $YUZUOS_QEMU_KERNEL 
  -m $YUZUOS_QEMU_MEMORY
  -cput $YUZUOS_QEMU_CPU
  -serial stdio
  -drive file=${YUZUOS_QEMU_IMG},format=raw,index=0,media=disk"
[ -e /dev/kvm ] && [ -r /dev/kvm ] && [ -w /dev/kvm ] && QEMU_KVM="-enable-kvm"

$YUZUOS_QEMU_BIN $YUZUOS_QEMU_ARG $QEMU_KVM
