#include <kernel/drivers/ide.h>
#include <kernel/interrupts/irq.h>
#include <kernel/system/io.h>
#include <kernel/system/sys.h>
#include <kernel/utils/log.h>
#include <kernel/utils/stdlib.h>

void ide_ata_delay_400ns(uint16_t addr)
{
  inb(addr + 7);
  inb(addr + 7);
  inb(addr + 7);
  inb(addr + 7);
}

uint8_t ide_ata_identify_polling(struct ata_device *device)
{
  uint8_t status;

  while (true)
  {
    status = inb(device->base_io + 7);

    if (status == 0)
      return IDE_IDENTIFY_NOT_FOUND;
    if (!(status & ATA_REG_BUSY))
      break;
  }

  while (true)
  {
    status = inb(device->base_io + 7);

    if (status & ATA_REG_DRQ)
      break;
    if (status & ATA_REG_ERROR || status & ATA_REG_DF)
      return IDE_IDENTIFY_ERROR;
  }

  return IDE_IDENTIFY_SUCCESS;
}

uint8_t ide_ata_identify(struct ata_device *device)
{
  outb(device->base_io + 6, device->master ? 0xA0 : 0xB0);
  ide_ata_delay_400ns(device->base_io);

  outb(device->base_io + 2, 0);
  outb(device->base_io + 3, 0);
  outb(device->base_io + 4, 0);
  outb(device->base_io + 5, 0);
  outb(device->base_io + 7, 0xEC);

  uint8_t status = ide_ata_identify_polling(device);
  if (status != IDE_IDENTIFY_SUCCESS)
    return status;

  if (!(inb(device->base_io + 7) & ATA_REG_ERROR))
  {
    uint16_t buffer[IDE_BUF];
    insw(device->base_io, buffer, IDE_BUF);

    return IDE_IDENTIFY_SUCCESS;
  }
  return IDE_IDENTIFY_ERROR;
}

struct ata_device *ide_detect(char *name, bool master, uint8_t irq, uint16_t base_io, uint16_t ctrl_io)
{
  struct ata_device *device = calloc(1, sizeof(struct ata_device));
  device->base_io = base_io;
  device->ctrl_io = ctrl_io;
  device->irq = irq;
  device->master = master;

  if (ide_ata_identify(device) == IDE_IDENTIFY_SUCCESS)
  {
    device->type = ATA_TYPE;
    device->name = name;
    log_info("IDE: Identified type = ATA, name = %s, base_io = 0x%x, ctrl_io = 0x%x, irq = %d\n", name, base_io, ctrl_io, irq);
    return device;
  }
  return NULL;
}

int ide_handler(struct itr_registers *registers)
{
  (void)registers;
  return ITR_CONTINUE;
}

void ide_init()
{
  irq_handler_set(ATA0_IRQ, ide_handler);

  if (ide_detect("/dev/hda", true, ATA0_IRQ, ATA0_BASE_ADDR, ATA0_CTRL_ADDR) == NULL)
    sys_panic("IDE: Failed to detect /dev/hda", false);

  log_info("IDE: Initialized\n");
}