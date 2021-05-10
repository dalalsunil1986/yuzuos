#!/bin/bash -e

[ -z "$YUZUOS_QEMU_BIN" ] && YUZUOS_QEMU_BIN="qemu-system-i386"
[ -z "$YUZUOS_QEMU_IMG" ] && YUZUOS_QEMU_IMG="qemu_img"
[ -z "$YUZUOS_QEMU_ARG" ] && YUZUOS_QEMU_ARG="-kernel kernel/kernel -serial stdio -drive file=${YUZUOS_QEMU_IMG},format=raw,index=0,media=disk"
[ -e /dev/kvm ] && [ -r /dev/kvm ] && [ -w /dev/kvm ] && QEMU_KVM="-enable-kvm"

$YUZUOS_QEMU_BIN $YUZUOS_QEMU_ARG $QEMU_KVM
