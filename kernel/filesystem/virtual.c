#include <kernel/filesystem/virtual.h>
#include <kernel/filesystem/ext2.h>
#include <kernel/drivers/ide.h>
#include <kernel/system/sys.h>
#include <kernel/utils/log.h>
#include <kernel/utils/stdlib.h>
#include <kernel/utils/math.h>
#include <kernel/utils/string.h>
#include <stddef.h>

static struct dlist_head virt_fs_mount_list;
static struct dlist_head virt_fs_type_list;

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
  log_info("Virtual FS: Root mounted name = %s, type = %s\n", name, type->name);
}

void virt_fs_init()
{
  dlist_head_init(&virt_fs_mount_list);
  dlist_head_init(&virt_fs_type_list);

  virt_fs_mount_root("/dev/hda", ext2_fs_type_get());

  log_info("Virtual FS: Initialized\n");
}
