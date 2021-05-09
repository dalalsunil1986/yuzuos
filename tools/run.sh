#!/bin/bash -e

[ -z "$YUZUOS_QEMU_BIN" ] && YUZUOS_QEMU_BIN="qemu-system-i386"
[ -z "$YUZUOS_QEMU_ARG" ] && YUZUOS_QEMU_ARG="-kernel kernel/kernel -serial stdio"
[ -e /dev/kvm ] && [ -r /dev/kvm ] && [ -w /dev/kvm ] && QEMU_KVM="-enable-kvm"

$YUZUOS_QEMU_BIN $YUZUOS_QEMU_ARG $QEMU_KVM
