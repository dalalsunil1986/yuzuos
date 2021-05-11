#include <kernel/filesystem/virtual.h>
#include <kernel/filesystem/ext2.h>
#include <kernel/system/sys.h>
#include <kernel/utils/log.h>
#include <stddef.h>

static struct dlist_head virt_fs_mount_list;
static struct dlist_head virt_fs_type_list;

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
