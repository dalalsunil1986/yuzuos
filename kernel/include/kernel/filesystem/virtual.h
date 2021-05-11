#pragma once

#include <kernel/utils/dlist.h>
#include <kernel/utils/types.h>
#include <stdint.h>

#define VFS_BYTES_P_SECTOR 512

struct vfs_sb
{
  const char *devname;
  const void *info;

  uint32_t blocksize;
  uint32_t blocksize_bits;
  uint32_t magic;

  struct vfs_type *type;
};

struct vfs_mount
{
  struct vfs_sb *sb;
  struct dlist_head list;
};

struct vfs_type
{
  const char *name;
  struct vfs_mount *(*mount)(const char *name, const char *path, struct vfs_type *type);
  struct dlist_head list;
};

void virt_fs_init();
void virt_fs_type_add(struct vfs_type *type);
char *virt_fs_bread(const char *devname, sector_t sector, uint32_t size);
