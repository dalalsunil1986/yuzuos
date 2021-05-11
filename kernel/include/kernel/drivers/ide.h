#pragma once

#include <kernel/utils/dlist.h>
#include <stdbool.h>
#include <stdint.h>

#define ATA0_BASE_ADDR 0x1F0
#define ATA0_CTRL_ADDR 0x3F0
#define ATA0_IRQ 14

#define ATA_REG_ERROR 0x01
#define ATA_REG_DF 0x20
#define ATA_REG_DRQ 0x08
#define ATA_REG_BUSY 0x80

#define IDE_IDENTIFY_ERROR 0
#define IDE_IDENTIFY_SUCCESS 1
#define IDE_IDENTIFY_NOT_FOUND 2

#define IDE_POLLING_ERROR 0
#define IDE_POLLING_SUCCESS 1

#define IDE_BUF 256

enum ata_type
{
  ATA_TYPE,
  ATAPI_TYPE
};

struct ata_device
{
  char *name;
  bool master;

  uint16_t base_io;
  uint16_t ctrl_io;
  uint8_t irq;

  enum ata_type type;
  struct dlist_head list;
};

void ide_init();
struct ata_device *ide_ata_get(const char *name);
int8_t ide_ata_read(struct ata_device *device, uint16_t *buffer, uint32_t lba, uint8_t sectors);
