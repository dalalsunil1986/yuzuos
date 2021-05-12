#include <kernel/task/elf32.h>
#include <kernel/filesystem/virtual.h>
#include <kernel/task/scheduler.h>
#include <kernel/memory/virtual.h>
#include <kernel/memory/mmap.h>
#include <kernel/utils/log.h>
#include <kernel/utils/string.h>
#include <kernel/utils/stdlib.h>

int elf_verify(struct elf32_ehdr *eheader)
{
  if (!(eheader->ident[EI_MAG0] == ELFMAG0 && eheader->ident[EI_MAG1] == ELFMAG1 &&
        eheader->ident[EI_MAG2] == ELFMAG2 && eheader->ident[EI_MAG3] == ELFMAG3))
    return -ELF_ERROR_NOT_ELF_FILE;
  if (eheader->ident[EI_CLASS] != ELFCLASS32)
    return -ELF_ERROR_NOT_SUPPORTED_PLATFORM;
  if (eheader->ident[EI_DATA] != ELFDATA2LSB)
    return -ELF_ERROR_NOT_SUPPORTED_ENCODING;
  if (eheader->ident[EI_VERSION] != EV_CURRENT)
    return -ELF_ERROR_WRONG_VERSION;
  if (eheader->machine != EM_386)
    return -ELF_ERROR_NOT_SUPPORTED_PLATFORM;
  if (eheader->type != ET_EXEC)
    return -ELF_ERROR_NOT_SUPPORTED_TYPE;
  return ELF_SUCESS;
}

struct elf32_layout *elf32_load(const char *path)
{
  char *buffer = virt_fs_read(path);
  if (!buffer)
  {
    log_error("ELF: Failed to load file path = %s\n", path);
    return NULL;
  }

  struct elf32_ehdr *eheader = (struct elf32_ehdr *)buffer;
  if (elf_verify(eheader) != ELF_SUCESS || eheader->phoff == 0)
  {
    log_error("ELF: Wrong header format for file path = %s\n", path);
    return NULL;
  }
  log_info("ELF: Loaded file path = %s\n", path);

  struct elf32_phdr *pheader = (struct elf32_phdr *)(buffer + eheader->phoff);
  while (pheader && (char *)pheader < (buffer + eheader->phoff + eheader->phentsize * eheader->phnum))
  {
    if (pheader->type != PT_LOAD)
      goto next;

    if ((pheader->flags & PF_X) != 0 && (pheader->flags & PF_R) != 0)
    {
      log_info("ELF: TEXT section vaddr = 0x%x, memsz = 0x%x\n", pheader->vaddr, pheader->memsz);
      mmap_map(pheader->vaddr, pheader->memsz, -1);
    }
    else if ((pheader->flags & PF_W) != 0 && (pheader->flags & PF_R) != 0)
    {
      log_info("ELF: DATA section vaddr = 0x%x, memsz = 0x%x\n", pheader->vaddr, pheader->memsz);
      mmap_map(pheader->vaddr, pheader->memsz, -1);
    }
    else
    {
      log_info("ELF: EHFRAME section vaddr = 0x%x, memsz = 0x%x\n", pheader->vaddr, pheader->memsz);
      mmap_map(pheader->vaddr, pheader->memsz, -1);
    }

    memset((char *)pheader->vaddr, 0, pheader->memsz);
    memcpy((char *)pheader->vaddr, buffer + pheader->offset, pheader->filesz);

  next:
    pheader++;
  }

  uint32_t heap = mmap_map(0, SCHED_HEAP_SIZE, -1);
  struct process_mm *mm = sched_process_get()->mm;
  mm->brk_start = heap;
  mm->brk = heap;
  mm->brk_end = HEAP_USER_TOP;

  struct elf32_layout *layout = calloc(1, sizeof(struct elf32_layout));
  layout->entry = eheader->entry;
  uint32_t stack = mmap_map(0, SCHED_STACK_SIZE, -1);
  layout->stack = stack + SCHED_STACK_SIZE;

  return layout;
}
