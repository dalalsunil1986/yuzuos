[ -z "$QEMU_BIN" ] && QEMU_BIN="qemu-system-i386"
[ -z "$QEMU_ARG" ] && QEMU_ARG="-kernel kernel/kernel -serial stdio"
[ -e /dev/kvm ] && [ -r /dev/kvm ] && [ -w /dev/kvm ] && QEMU_KVM="-enable-kvm"

$QEMU_BIN $QEMU_ARG $QEMU_KVM
