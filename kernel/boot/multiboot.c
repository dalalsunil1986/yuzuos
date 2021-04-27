#include <kernel/boot/multiboot.h>
#include <kernel/utils/log.h>
#include <kernel/system/sys.h>

void multiboot_init(uint32_t magic, uint32_t addr)
{
  if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
  {
    log_error("Multiboot: Invalid magic number = 0x%x\n", (unsigned)magic);
    sys_stop();
  }

  struct multiboot_info *mbi = (struct multiboot_info *)addr;
  log_info("Multiboot: Flags = 0x%x\n", (unsigned)mbi->flags);

  if (MULTIBOOT_CHECK_FLAG(mbi->flags, 0))
    log_info("Multiboot: Memory lower = %uKB, upper = %uKB\n", (unsigned)mbi->mem_lower, (unsigned)mbi->mem_upper);

  if (MULTIBOOT_CHECK_FLAG(mbi->flags, 1))
    log_info("Multiboot: Boot device = 0x%x\n", (unsigned)mbi->boot_device);

  if (MULTIBOOT_CHECK_FLAG(mbi->flags, 2))
    log_info("Multiboot: Cmdline = %s\n", (char *)mbi->cmdline);

  if (MULTIBOOT_CHECK_FLAG(mbi->flags, 3))
  {
    struct multiboot_mod_list *mod;
    int i;

    log_info("Multiboot: Mods count = %d, addr = 0x%x\n", (int)mbi->mods_count, (int)mbi->mods_addr);
    for (i = 0, mod = (struct multiboot_mod_list *)mbi->mods_addr; i < mbi->mods_count; i++, mod++)
      log_info("Multiboot: * Mod start = 0x%x, end = 0x%x, cmdline = %s\n", (unsigned)mod->mod_start, (unsigned)mod->mod_end, (char *)mod->cmdline);
  }

  if (MULTIBOOT_CHECK_FLAG(mbi->flags, 4) && MULTIBOOT_CHECK_FLAG(mbi->flags, 5))
  {
    log_info("Multiboot: Both bits 4 and 5 are set\n");
    return;
  }

  if (MULTIBOOT_CHECK_FLAG(mbi->flags, 4))
  {
    struct multiboot_aout_symbol_table *multiboot_aout_sym = &(mbi->u.aout_sym);
    log_info("Multiboot: Aout symbol table tabsize = 0x%0x, strsize = 0x%x, addr = 0x%x\n", (unsigned)multiboot_aout_sym->tabsize,
             (unsigned)multiboot_aout_sym->strsize, (unsigned)multiboot_aout_sym->addr);
  }

  if (MULTIBOOT_CHECK_FLAG(mbi->flags, 5))
  {
    struct multiboot_elf_section_header_table *multiboot_elf_sec = &(mbi->u.elf_sec);
    log_info("Multiboot: Elf section num = %u, size = 0x%x, addr = 0x%x, shndx = 0x%x\n", (unsigned)multiboot_elf_sec->num, (unsigned)multiboot_elf_sec->size,
             (unsigned)multiboot_elf_sec->addr, (unsigned)multiboot_elf_sec->shndx);
  }

  if (MULTIBOOT_CHECK_FLAG(mbi->flags, 6))
  {
    log_info("Multiboot: Mmap addr = 0x%x, length = 0x%x\n", (unsigned)mbi->mmap_addr, (unsigned)mbi->mmap_length);
    for (struct multiboot_mmap_entry *mmap = (struct multiboot_mmap_entry *)mbi->mmap_addr; (unsigned long)mmap < mbi->mmap_addr + mbi->mmap_length;
         mmap = (struct multiboot_mmap_entry *)((unsigned long)mmap + mmap->size + sizeof(mmap->size)))
      log_info("Multiboot: * Mmap size = 0x%x, base_addr = 0x%x%08x, length = 0x%x%08x, type = 0x%x\n", (unsigned)mmap->size, (unsigned)(mmap->addr >> 32),
               (unsigned)(mmap->addr & 0xffffffff), (unsigned)(mmap->len >> 32), (unsigned)(mmap->len & 0xffffffff), (unsigned)mmap->type);
  }

  log_info("Multiboot: Initialized\n");
}
