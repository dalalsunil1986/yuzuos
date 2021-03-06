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
#include <stdarg.h>

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
  dlist_head_init(&dentry->subdirs);

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
  atomic_set(&file->count, 1);
  return file;
}

void virt_fs_inode_special_init(struct vfs_inode *inode, umode_t mode, dev_t dev)
{
  inode->mode = mode;
  if (S_ISCHR(mode))
  {
    //FIXME implement chrdev file op
    inode->rdev = dev;
  }
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

int virt_fs_getattr(struct vfs_mount *mnt, struct vfs_dentry *dentry, struct stat *stat)
{
  struct vfs_inode *inode = dentry->inode;
  if (inode->op->getattr)
    return inode->op->getattr(mnt, dentry, stat);

  stat->st_dev = inode->sb->dev;
  stat->st_ino = inode->ino;
  stat->st_mode = inode->mode;
  stat->st_nlink = inode->nlink;
  stat->st_uid = inode->uid;
  stat->st_gid = inode->gid;
  stat->st_rdev = inode->rdev;
  stat->st_atim = inode->atime;
  stat->st_mtim = inode->mtime;
  stat->st_ctim = inode->ctime;
  stat->st_size = inode->size;
  stat->st_blocks = inode->blocks;
  stat->st_blksize = inode->blksize;

  if (!stat->st_blksize)
  {
    struct vfs_sb *sb = inode->sb;
    uint32_t blocks = (stat->st_size + sb->blocksize - 1) >> sb->blocksize_bits;
    stat->st_blocks = (sb->blocksize / VFS_BYTES_P_SECTOR) * blocks;
    stat->st_blksize = sb->blocksize;
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

int virt_fs_fstat(int fd, struct stat *stat)
{
  if (fd < 0)
    return -EBADF;

  struct vfs_file *file = sched_process_get()->files->fd[fd];
  if (!file)
    return -EBADF;

  return virt_fs_getattr(file->mount, file->dentry, stat);
}

int virt_fs_path_find(const char *path, int flags, mode_t mode, struct nameidata *nameidata)
{
  nameidata->mount = sched_process_get()->fs->mount;

  int i = 0;
  if (path[i] == '/')
  {
    nameidata->dentry = sched_process_get()->fs->mount->mount;
    while (path[i] == '/')
      i++;
  }
  else
    nameidata->dentry = sched_process_get()->fs->root;

  char name[NAME_MAX];
  int length = strlen(path);
  while (i < length)
  {
    memset(name, 0, sizeof(name));

    for (int j = 0; path[i] != '/' && i < length; i++, j++)
      name[j] = path[i];

    while (path[i] == '/' && i < length)
      i++;

    struct vfs_dentry *iter = NULL;
    struct vfs_dentry *child = NULL;
    dlist_foreach_entry(iter, &nameidata->dentry->subdirs, list)
    {
      if (!strcmp(name, iter->name))
      {
        child = iter;
        break;
      }
    }

    if (child)
    {
      nameidata->dentry = child;
      if (i == length && flags & O_CREAT && flags & O_EXCL)
        return -EEXIST;
    }
    else
    {
      child = virt_fs_dentry_alloc(name, nameidata->dentry);

      struct vfs_inode *inode = NULL;
      if (nameidata->dentry->inode->op->lookup)
        inode = nameidata->dentry->inode->op->lookup(nameidata->dentry->inode, child);

      if (inode == NULL)
      {
        if (i == length && flags & O_CREAT)
          inode = nameidata->dentry->inode->op->create(nameidata->dentry->inode, child, i == length ? mode : S_IFDIR);
        else
          return -ENOENT;
      }
      else if (i == length && flags & O_CREAT && flags & O_EXCL)
        return -EEXIST;

      child->inode = inode;
      dlist_add_tail(&child->list, &nameidata->dentry->subdirs);
      nameidata->dentry = child;
    }

    struct vfs_mount *mount = virt_fs_mount_get(nameidata->dentry);
    if (mount)
      nameidata->mount = mount;
  }

  return 0;
}

int virt_fs_open(const char *path, int flags, ...)
{
  int fd = virt_fs_fd_find(0);
  mode_t mode = 0;
  if (flags & O_CREAT)
  {
    va_list ap;
    va_start(ap, flags);
    mode = va_arg(ap, int);
    va_end(ap);
  }

  struct nameidata nameidata;
  int result = virt_fs_path_find(path, flags, mode, &nameidata);
  if (result < 0)
    return result;

  struct vfs_file *file = virt_fs_file_alloc();
  file->dentry = nameidata.dentry;
  file->mount = nameidata.mount;
  file->flags = flags;
  file->mode = OPEN_FMODE(flags);
  file->op = nameidata.dentry->inode->fop;

  if (file->mode & FMODE_READ)
    file->mode |= FMODE_CAN_READ;
  if (file->mode & FMODE_WRITE)
    file->mode |= FMODE_CAN_WRITE;

  if (file->op && file->op->open)
  {
    result = file->op->open(nameidata.dentry->inode, file);
    if (result < 0)
    {
      free(file);
      return result;
    }
  }

  atomic_inc(&file->dentry->inode->count);
  sched_process_get()->files->fd[fd] = file;
  return fd;
}

ssize_t virt_fs_fread(int fd, char *buffer, size_t count)
{
  if (fd < 0)
    return -EBADF;

  struct vfs_file *file = sched_process_get()->files->fd[fd];
  if (!file)
    return -EBADF;

  if (file->mode & FMODE_CAN_READ)
    return file->op->read(file, buffer, count, file->pos);

  return -EINVAL;
}

char *virt_fs_read(const char *path)
{
  int fd = virt_fs_open(path, O_RDWR);
  if (fd < 0)
    return NULL;

  struct stat *stat = calloc(1, sizeof(struct stat));
  if (!stat)
    return NULL;

  if (virt_fs_fstat(fd, stat) < 0)
    return NULL;

  char *buffer = calloc(stat->st_size, sizeof(char));
  if (!buffer)
    return NULL;

  if (virt_fs_fread(fd, buffer, stat->st_size) < 0)
    return NULL;

  return buffer;
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

  ext2_fs_init();
  virt_fs_mount_root("/dev/hda", ext2_fs_type_get());

  log_info("Virtual FS: Initialized\n");
}
