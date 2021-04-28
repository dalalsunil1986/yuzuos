#include <kernel/boot/multiboot.h>
#include <kernel/utils/log.h>
#include <kernel/system/sys.h>

struct multiboot_info *multiboot_info;

void multiboot_init(uint32_t magic, uint32_t addr)
{
  if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
  {
    log_error("Multiboot: Invalid magic number = 0x%x\n", (unsigned)magic);
    sys_stop();
  }

  multiboot_info = (struct multiboot_info *)addr;
  log_info("Multiboot: Flags = 0x%x\n", (unsigned)multiboot_info->flags);

  if (MULTIBOOT_CHECK_FLAG(multiboot_info->flags, 0))
    log_info("Multiboot: Memory lower = %uKB, upper = %uKB\n", (unsigned)multiboot_info->mem_lower, (unsigned)multiboot_info->mem_upper);

  if (MULTIBOOT_CHECK_FLAG(multiboot_info->flags, 1))
    log_info("Multiboot: Boot device = 0x%x\n", (unsigned)multiboot_info->boot_device);

  if (MULTIBOOT_CHECK_FLAG(multiboot_info->flags, 2))
    log_info("Multiboot: Cmdline = %s\n", (char *)multiboot_info->cmdline);

  if (MULTIBOOT_CHECK_FLAG(multiboot_info->flags, 3))
  {
    struct multiboot_mod_list *mod;
    uint32_t i;

    log_info("Multiboot: Mods count = %d, addr = 0x%x\n", (int)multiboot_info->mods_count, (int)multiboot_info->mods_addr);
    for (i = 0, mod = (struct multiboot_mod_list *)multiboot_info->mods_addr; i < multiboot_info->mods_count; i++, mod++)
      log_info("Multiboot: * Mod start = 0x%x, end = 0x%x, cmdline = %s\n", (unsigned)mod->mod_start, (unsigned)mod->mod_end, (char *)mod->cmdline);
  }

  if (MULTIBOOT_CHECK_FLAG(multiboot_info->flags, 4) && MULTIBOOT_CHECK_FLAG(multiboot_info->flags, 5))
  {
    log_info("Multiboot: Both bits 4 and 5 are set\n");
    return;
  }

  if (MULTIBOOT_CHECK_FLAG(multiboot_info->flags, 4))
  {
    struct multiboot_aout_symbol_table *multiboot_aout_sym = &(multiboot_info->u.aout_sym);
    log_info("Multiboot: Aout symbol table tabsize = 0x%0x, strsize = 0x%x, addr = 0x%x\n", (unsigned)multiboot_aout_sym->tabsize,
             (unsigned)multiboot_aout_sym->strsize, (unsigned)multiboot_aout_sym->addr);
  }

  if (MULTIBOOT_CHECK_FLAG(multiboot_info->flags, 5))
  {
    struct multiboot_elf_section_header_table *multiboot_elf_sec = &(multiboot_info->u.elf_sec);
    log_info("Multiboot: Elf section num = %u, size = 0x%x, addr = 0x%x, shndx = 0x%x\n", (unsigned)multiboot_elf_sec->num, (unsigned)multiboot_elf_sec->size,
             (unsigned)multiboot_elf_sec->addr, (unsigned)multiboot_elf_sec->shndx);
  }

  if (MULTIBOOT_CHECK_FLAG(multiboot_info->flags, 6))
  {
    log_info("Multiboot: Mmap addr = 0x%x, length = 0x%x\n", (unsigned)multiboot_info->mmap_addr, (unsigned)multiboot_info->mmap_length);
    for (struct multiboot_mmap_entry *mmap = (struct multiboot_mmap_entry *)multiboot_info->mmap_addr; (unsigned long)mmap < multiboot_info->mmap_addr + multiboot_info->mmap_length;
         mmap = (struct multiboot_mmap_entry *)((unsigned long)mmap + mmap->size + sizeof(mmap->size)))
      log_info("Multiboot: * Mmap size = 0x%x, base_addr = 0x%x%08x, length = 0x%x%08x, type = 0x%x\n", (unsigned)mmap->size, (unsigned)(mmap->addr >> 32),
               (unsigned)(mmap->addr & 0xffffffff), (unsigned)(mmap->len >> 32), (unsigned)(mmap->len & 0xffffffff), (unsigned)mmap->type);
  }

  log_info("Multiboot: Initialized\n");
}
