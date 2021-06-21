#include <kernel/drivers/pci.h>
#include <kernel/utils/log.h>
#include <kernel/utils/stdlib.h>
#include <kernel/asm/io.h>

static struct dlist_head pci_list;

void pci_bus_scan(uint8_t bus);

uint32_t pci_field_read(uint32_t addr, uint8_t offset)
{
  outl(PCI_ADDR_PORT, addr | (offset & 0xfc));
  return inl(PCI_VALUE_PORT);
}

uint16_t pci_hdr_type_get(uint32_t addr)
{
  uint32_t reg = pci_field_read(addr, 0xC);
  return (reg >> 16) & 0xFF;
}

uint16_t pci_vendor_id_get(uint32_t addr)
{
  uint32_t reg = pci_field_read(addr, 0x0);
  return reg & 0xFFFF;
}

uint32_t pci_addr_get(uint8_t bus, uint8_t slot, uint8_t func)
{
  uint32_t lbus = (uint32_t)bus;
  uint32_t lslot = (uint32_t)slot;
  uint32_t lfunc = (uint32_t)func;

  return (lbus << 16) | (lslot << 11) | (lfunc << 8) | 0x80000000;
}

uint16_t pci_class_code_get(uint32_t addr)
{
  uint32_t reg = pci_field_read(addr, 0x8);
  return (reg >> 24) & 0xFF;
}

uint16_t pci_subclass_code_get(uint32_t addr)
{
  uint32_t reg = pci_field_read(addr, 0x8);
  return (reg >> 16) & 0xFF;
}

uint16_t pci_device_id_get(uint32_t addr)
{
  uint32_t reg = pci_field_read(addr, 0x0);
  return (reg >> 16) & 0xFFFF;
}

uint16_t pci_secondary_bus_get(uint32_t addr)
{
  uint32_t reg = pci_field_read(addr, 0x18);
  return (reg >> 8) & 0xFF;
}

void pci_device_set(uint8_t bus, uint8_t device, uint8_t function)
{
  uint32_t addr = pci_addr_get(bus, device, function);
  uint16_t vendor_id = pci_vendor_id_get(addr);

  if (vendor_id != PCI_INVALID_VENDOR_ID)
  {
    uint16_t device_id = pci_device_id_get(addr);
    uint16_t class_code = pci_class_code_get(addr);
    uint16_t sub_class = pci_subclass_code_get(addr);

    if (class_code != PCI_CLASS_CODE_BRIDGE_DEVICE)
    {
      uint32_t bar0 = pci_field_read(addr, PCI_BAR0);
      struct pci_device *device = calloc(1, sizeof(struct pci_device));
      device->addr = addr;
      device->device_id = device_id;
      device->vendor_id = vendor_id;
      device->bar0 = bar0;

      dlist_add_tail(&device->list, &pci_list);
      log_info("PCI: Addr = 0x%x, class_code = 0x%x, sub_class = 0x%x, vendor_id = 0x%x, device_id = 0x%x, bar0 = 0x%x\n", addr, class_code, sub_class, vendor_id, device_id, bar0);
    }
  }
}

void pci_check_func(uint8_t bus, uint8_t device, uint8_t func)
{
  pci_device_set(bus, device, func);

  uint32_t addr = pci_addr_get(bus, device, func);
  uint16_t base_class = pci_class_code_get(addr);
  uint16_t sub_class = pci_subclass_code_get(addr);
  if ((base_class == PCI_CLASS_CODE_BRIDGE_DEVICE) && (sub_class == PCI_SUBCLASS_PCI_TO_PCI_BRIDGE))
    pci_bus_scan(pci_secondary_bus_get(addr));
}

void pci_bus_scan(uint8_t bus)
{
  uint8_t func = 0;
  for (uint8_t device = 0; device < 32; device++)
  {
    uint32_t addr = pci_addr_get(bus, device, func);
    uint16_t type = pci_hdr_type_get(addr);
    if ((type & PCI_DEVICE_MULTI_FUNC) != 0)
    {
      for (func = 0; func < 7; func++)
        pci_check_func(bus, device, func);
    }
    else
      pci_check_func(bus, device, 0);
  }
}

void pci_init()
{
  dlist_head_init(&pci_list);

  uint16_t header = pci_hdr_type_get(pci_addr_get(0, 0, 0));
  if ((header & PCI_DEVICE_MULTI_FUNC) == 0)
    pci_bus_scan(0);
  else
  {
    for (uint8_t func = 0; func < 8; func++)
    {
      if (pci_vendor_id_get(pci_addr_get(0, 0, func)) == PCI_INVALID_VENDOR_ID)
        break;
      uint8_t bus = func;
      pci_bus_scan(bus);
    }
  }

  log_info("PCI: Initialized\n");
}
