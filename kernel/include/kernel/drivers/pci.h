#pragma once

#define PCI_BAR0 0x10
#define PCI_BAR1 0x14
#define PCI_BAR2 0x18
#define PCI_BAR3 0x1C
#define PCI_BAR4 0x20
#define PCI_BAR5 0x24

#define PCI_DEVICE_MULTI_FUNC 0x80
#define PCI_INVALID_VENDOR_ID 0xFFFF

#define PCI_ADDR_PORT 0xCF8
#define PCI_VALUE_PORT 0xCFC

#define PCI_CLASS_CODE_BRIDGE_DEVICE 0x06
#define PCI_SUBCLASS_PCI_TO_PCI_BRIDGE 0x04

void pci_init();
