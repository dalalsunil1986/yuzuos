#include <kernel/filesystem/virtual.h>
#include <kernel/filesystem/ext2.h>
#include <kernel/drivers/ide.h>
#include <kernel/system/sys.h>
#include <kernel/system/limits.h>
#include <kernel/utils/log.h>
#include <kernel/utils/stdlib.h>
#include <kernel/utils/math.h>
#include <kernel/utils/string.h>
#include <kernel/utils/errno.h>
#include <kernel/task/scheduler.h>
#include <stddef.h>

static struct dlist_head virt_fs_mount_list;
static struct dlist_head virt_fs_type_list;

struct vfs_mount *virt_fs_mount_get(struct vfs_dentry *dentry)
{
  struct vfs_mount *iter;
  dlist_foreach_entry(iter, &virt_fs_mount_list, list)
  {
    if (iter->mount == dentry)
      return iter;
  }
  return NULL;
}

struct vfs_dentry *virt_fs_dentry_alloc(const char *name, struct vfs_dentry *parent)
{
  struct vfs_dentry *dentry = calloc(1, sizeof(struct vfs_dentry));
  dentry->name = strdup(name);
  dentry->parent = parent;
  dlist_head_init(&dentry->list);

  if (parent)
    dentry->sb = parent->sb;

  return dentry;
}

struct vfs_inode *virt_fs_inode_alloc()
{
  struct vfs_inode *inode = calloc(1, sizeof(struct vfs_inode));
  inode->blocks = 0;
  inode->size = 0;
  return inode;
}

struct vfs_file *virt_fs_file_alloc()
{
  struct vfs_file *file = calloc(1, sizeof(struct vfs_file));
  file->max_count = INT_MAX;
  return file;
}

char *virt_fs_bread(const char *devname, sector_t sector, uint32_t size)
{
  // FIXME better error handling
  struct ata_device *device = ide_ata_get(devname);
  if (!device)
    return NULL;

  char *buffer = calloc(DIV_CEIL(size, VFS_BYTES_P_SECTOR) * VFS_BYTES_P_SECTOR, sizeof(char));
  if (!buffer)
    return NULL;

  if (ide_ata_read(device, (uint16_t *)buffer, sector, DIV_CEIL(size, VFS_BYTES_P_SECTOR)) < 0)
    return NULL;

  return buffer;
}

int virt_fs_getattr(struct vfs_mount *mnt, struct vfs_dentry *dentry, struct kstat *stat)
{
  struct vfs_inode *inode = dentry->inode;
  if (inode->op->getattr)
    return inode->op->getattr(mnt, dentry, stat);

  stat->dev = inode->sb->dev;
  stat->ino = inode->ino;
  stat->mode = inode->mode;
  stat->nlink = inode->nlink;
  stat->uid = inode->uid;
  stat->gid = inode->gid;
  stat->rdev = inode->rdev;
  stat->atim = inode->atime;
  stat->mtim = inode->mtime;
  stat->ctim = inode->ctime;
  stat->size = inode->size;
  stat->blocks = inode->blocks;
  stat->blksize = inode->blksize;

  if (!stat->blksize)
  {
    struct vfs_sb *sb = inode->sb;
    uint32_t blocks = (stat->size + sb->blocksize - 1) >> sb->blocksize_bits;
    stat->blocks = (sb->blocksize / VFS_BYTES_P_SECTOR) * blocks;
    stat->blksize = sb->blocksize;
  }
  return 0;
}

int virt_fs_fd_find(int limit)
{
  for (int i = limit; i < 256; i++)
    if (!sched_process_get()->files->fd[i])
      return i;
  return -EINVAL;
}

int virt_fs_fstat(int fd, struct kstat *stat)
{
  if (fd < 0)
    return -EBADF;

  struct vfs_file *file = sched_process_get()->files->fd[fd];
  if (!file)
    return -EBADF;

  return virt_fs_getattr(file->mount, file->dentry, stat);
}

void virt_fs_type_add(struct vfs_type *type)
{
  dlist_add_tail(&type->list, &virt_fs_type_list);
}

void virt_fs_mount_root(const char *name, struct vfs_type *type)
{
  struct vfs_mount *mount = type->mount(name, "/", type);
  if (!mount)
    sys_panic("Virtual FS: Failed to mount root file system", NULL);

  dlist_add_tail(&mount->list, &virt_fs_mount_list);
  sched_process_get()->fs->mount = mount;
  sched_process_get()->fs->root = mount->root;

  log_info("Virtual FS: Root mounted name = %s, type = %s\n", name, type->name);
}

void virt_fs_init()
{
  dlist_head_init(&virt_fs_mount_list);
  dlist_head_init(&virt_fs_type_list);

  virt_fs_mount_root("/dev/hda", ext2_fs_type_get());

  log_info("Virtual FS: Initialized\n");
}
