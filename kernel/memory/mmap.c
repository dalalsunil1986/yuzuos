#include <kernel/memory/mmap.h>
#include <kernel/memory/virtual.h>
#include <kernel/task/scheduler.h>
#include <kernel/utils/math.h>
#include <kernel/utils/log.h>
#include <kernel/utils/stdlib.h>
#include <kernel/utils/string.h>

struct process_vm *mmap_find_unmapped_vm(uint32_t addr, uint32_t len)
{
  struct process_mm *mm = sched_process_get()->mm;
  struct process_vm *vma = calloc(1, sizeof(struct process_vm));
  vma->mm = mm;

  if (!addr || addr < mm->brk_end)
    addr = MAX(mm->cache, mm->brk_end);

  len = PAGE_ALIGN(len);

  uint32_t addr_found = addr;
  if (dlist_is_empty(&mm->list))
    dlist_add(&vma->list, &mm->list);
  else
  {
    struct process_vm *iter = NULL;
    dlist_foreach_entry(iter, &mm->list, list)
    {
      struct process_vm *next = dlist_entry_next(iter, list);
      bool is_last = dlist_is_last(&iter->list, &mm->list);

      if (addr + len <= iter->start)
      {
        dlist_add(&iter->list, &mm->list);
        break;
      }
      else if (addr >= iter->end && (is_last || (!is_last && addr + len <= next->start)))
      {
        dlist_add(&vma->list, &iter->list);
        break;
      }
      else if (!is_last && (iter->end <= addr && addr < next->start) && next->start - iter->end >= len)
      {
        dlist_add(&vma->list, &iter->list);
        addr_found = next->start - len;
        break;
      }
    }
  }

  if (addr_found)
  {
    vma->start = addr_found;
    vma->end = addr_found + len;
    mm->cache = vma->end;
  }

  return vma;
}

struct process_vm *mmap_find_vm(struct process_mm *mm, uint32_t addr)
{
  struct process_vm *iter = NULL;
  dlist_foreach_entry(iter, &mm->list, list)
  {
    if (iter->start <= addr && addr < iter->end)
      return iter;
  }
  return NULL;
}

int mmap_expand_vm(struct process_vm *vm, uint32_t address)
{
  address = PAGE_ALIGN(address);
  if (address <= vm->end)
    return 0;

  if (dlist_is_last(&vm->list, &sched_process_get()->mm->list))
    vm->end = address;
  else
  {
    struct process_vm *next = dlist_entry_next(vm, list);
    if (address <= next->start)
      vm->end = address;
    else
    {
      dlist_remove(&vm->list);

      struct process_vm *vm_expand = mmap_find_unmapped_vm(0, address - vm->start);
      memcpy(vm, vm_expand, sizeof(struct process_vm));
      free(vm_expand);
    }
  }
  return 0;
}

uint32_t mmap_map(uint32_t addr, size_t len, int fd)
{
  uint32_t addr_aligned = ALIGN_DOWN(addr, PHYS_MM_BLOCK);
  struct vfs_file *process_file = fd >= 0 ? sched_process_get()->files->fd[fd] : NULL;
  struct process_vm *vm = mmap_find_vm(sched_process_get()->mm, addr_aligned);

  if (!vm)
    vm = mmap_find_unmapped_vm(addr_aligned, len);
  else if (vm->end < addr + len)
    mmap_expand_vm(vm, addr + len);

  if (process_file)
  {
    process_file->op->mmap(process_file, vm);
    vm->file = process_file;
  }
  else
  {
    uint32_t virtual = vm->start;
    while (virtual < vm->end)
    {
      uint32_t physical = (uint32_t)phys_mm_block_alloc();
      virt_mm_map_addr(sched_process_get()->page_dir, physical, virtual, PAGE_TBL_PRESENT | PAGE_TBL_WRITABLE | PAGE_TBL_USER);
      virtual += PHYS_MM_BLOCK;
    }
  }

  return addr ? addr : vm->start;
}

uint32_t mmap_brk(uint32_t addr, size_t len)
{
  struct process_mm *mm = sched_process_get()->mm;
  struct process_vm *vm = mmap_find_vm(mm, addr);
  uint32_t new_brk = PAGE_ALIGN(addr + len);
  mm->brk = new_brk;

  if (!vm || vm->end >= new_brk)
    return 0;

  struct process_vm *new_vm = calloc(1, sizeof(struct process_vm));
  memcpy(new_vm, vm, sizeof(struct process_vm));

  if (new_brk > mm->brk)
    mmap_expand_vm(new_vm, new_brk);
  else
    new_vm->end = new_brk;

  if (vm->file)
    vm->file->op->mmap(vm->file, new_vm);
  else
  {
    if (new_vm->end > vm->end)
    {
      uint32_t frames = (new_vm->end - vm->end) / PHYS_MM_BLOCK;
      uint32_t physical = (uint32_t)phys_mm_block_n_alloc(frames);
      uint32_t virtual = vm->end;

      while (virtual < new_vm->end)
      {
        virt_mm_map_addr(sched_process_get()->page_dir, physical, virtual, PAGE_TBL_PRESENT | PAGE_TBL_WRITABLE | PAGE_TBL_USER);

        virtual += PHYS_MM_BLOCK;
        physical += PHYS_MM_BLOCK;
      }
    }
    else if (new_vm->end < vm->end)
    {
      uint32_t virtual = new_vm->end;
      while (addr < vm->end)
      {
        virt_mm_addr_unmap(sched_process_get()->page_dir, virtual);
        virtual += PHYS_MM_BLOCK;
      }
    }
  }
  memcpy(vm, new_vm, sizeof(struct process_vm));

  return 0;
}
