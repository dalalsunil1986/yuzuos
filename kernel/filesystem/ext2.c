#include <kernel/filesystem/ext2.h>
#include <kernel/utils/log.h>
#include <stddef.h>

struct vfs_mount *ext2_fs_mount(const char *name, const char *path, struct vfs_type *type);

static struct vfs_type ext2_fs_type = {
    .name = "ext2",
    .mount = ext2_fs_mount};

struct vfs_mount *ext2_fs_mount(const char *name, const char *path, struct vfs_type *type)
{
  (void)name;
  (void)path;
  (void)type;
  return NULL;
}

struct vfs_type *ext2_fs_type_get()
{
  return &ext2_fs_type;
}

void ext2_fs_init()
{
  virt_fs_type_add(&ext2_fs_type);

  log_info("Ext2 FS: Initialized\n");
}
