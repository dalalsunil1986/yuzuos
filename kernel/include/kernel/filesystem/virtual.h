#pragma once

#include <kernel/utils/dlist.h>
#include <kernel/utils/types.h>
#include <stdint.h>

#define VFS_BYTES_P_SECTOR 512

struct vfs_dentry
{
  const char *name;
  struct vfs_sb *sb;
  struct vfs_inode *inode;
  struct vfs_dentry *parent;
  struct dlist_head list;
};

struct vfs_inode
{
  void *info;
  uint32_t blksize;
  uint32_t blocks;
  uint32_t size;
  uint32_t flags;
  ino_t ino;
  umode_t mode;
  gid_t gid;
  uid_t uid;
  nlink_t nlink;
  time_t atime;
  time_t ctime;
  time_t mtime;
  struct vfs_sb *sb;
};

struct vfs_file
{
  uint32_t flags;
  size_t max_count;
  fmode_t mode;
  loff_t pos;

  struct vfs_file_op *op;
  struct vfs_dentry *dentry;
  struct vfs_mount *mount;
};

struct vfs_file_op
{
  int (*open)(struct vfs_inode *inode, struct vfs_file *file);
  ssize_t (*read)(struct vfs_file *file, char *buf, size_t count, loff_t ppos);
};

struct vfs_sb
{
  const char *devname;
  const void *info;

  dev_t dev;
  uint32_t blocksize;
  uint32_t blocksize_bits;
  uint32_t magic;

  struct vfs_dentry *root;
  struct vfs_type *type;
};

struct vfs_mount
{
  struct vfs_sb *sb;
  struct vfs_dentry *root;
  struct vfs_dentry *mount;
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
struct vfs_inode *virt_fs_inode_alloc();
struct vfs_dentry *virt_fs_dentry_alloc(const char *name, struct vfs_dentry *parent);
