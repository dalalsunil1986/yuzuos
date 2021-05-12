#pragma once

#include <kernel/utils/dlist.h>
#include <kernel/utils/types.h>
#include <kernel/utils/fcntl.h>
#include <kernel/utils/stat.h>
#include <stdint.h>

#define VFS_NAME_MAX 256
#define VFS_BYTES_P_SECTOR 512

#define FMODE_READ ((fmode_t)0x1)
#define FMODE_WRITE ((fmode_t)0x2)
#define FMODE_LSEEK ((fmode_t)0x4)
#define FMODE_PREAD ((fmode_t)0x8)
#define FMODE_PWRITE ((fmode_t)0x10)
#define FMODE_EXEC ((fmode_t)0x20)
#define FMODE_NDELAY ((fmode_t)0x40)
#define FMODE_EXCL ((fmode_t)0x80)
#define FMODE_WRITE_IOCTL ((fmode_t)0x100)
#define FMODE_32BITHASH ((fmode_t)0x200)
#define FMODE_64BITHASH ((fmode_t)0x400)
#define FMODE_NOCMTIME ((fmode_t)0x800)
#define FMODE_RANDOM ((fmode_t)0x1000)
#define FMODE_UNSIGNED_OFFSET ((fmode_t)0x2000)
#define FMODE_PATH ((fmode_t)0x4000)
#define FMODE_ATOMIC_POS ((fmode_t)0x8000)
#define FMODE_WRITER ((fmode_t)0x10000)
#define FMODE_CAN_READ ((fmode_t)0x20000)
#define FMODE_CAN_WRITE ((fmode_t)0x40000)
#define FMODE_OPENED ((fmode_t)0x80000)
#define FMODE_CREATED ((fmode_t)0x100000)
#define FMODE_STREAM ((fmode_t)0x200000)
#define FMODE_NONOTIFY ((fmode_t)0x4000000)
#define FMODE_NOWAIT ((fmode_t)0x8000000)
#define FMODE_NEED_UNMOUNT ((fmode_t)0x10000000)
#define FMODE_NOACCOUNT ((fmode_t)0x20000000)
#define FMODE_BUF_RASYNC ((fmode_t)0x40000000)

#define OPEN_FMODE(flag) ((fmode_t)(((flag + 1) & O_ACCMODE) | (flag & FMODE_NONOTIFY)))

struct nameidata
{
  struct vfs_dentry *dentry;
  struct vfs_mount *mount;
};

struct vfs_dentry
{
  const char *name;
  struct vfs_sb *sb;
  struct vfs_inode *inode;
  struct vfs_dentry *parent;
  struct dlist_head list;
};

struct vfs_inode_op
{
  struct vfs_inode *(*lookup)(struct vfs_inode *dir, struct vfs_dentry *dentry);
  struct vfs_inode *(*create)(struct vfs_inode *dir, struct vfs_dentry *dentry, mode_t mode);
  int (*getattr)(struct vfs_mount *mount, struct vfs_dentry *dentry, struct kstat *kstat);
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
  dev_t rdev;
  nlink_t nlink;
  time_t atime;
  time_t ctime;
  time_t mtime;

  struct vfs_inode_op *op;
  struct vfs_file_op *fop;
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
char *virt_fs_read(const char *path);
