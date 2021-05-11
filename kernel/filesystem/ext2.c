#include <kernel/filesystem/ext2.h>
#include <kernel/utils/log.h>
#include <kernel/utils/stdlib.h>
#include <kernel/utils/string.h>
#include <kernel/system/sys.h>
#include <stddef.h>

struct vfs_mount *ext2_fs_mount(const char *name, const char *path, struct vfs_type *type);

static struct vfs_type ext2_fs_type = {
    .name = "ext2",
    .mount = ext2_fs_mount};

struct vfs_mount *ext2_fs_mount(const char *name, const char *path, struct vfs_type *type)
{
  (void)path;

  struct vfs_sb *vfs_sb = calloc(1, sizeof(struct vfs_sb));

  struct ext2_sb *ext2_sb = calloc(1, sizeof(struct ext2_sb));
  if (ext2_sb->s_magic != EXT2_SUPER_MAGIC)
    sys_panic("Ext2 FS: Wrong magic value", NULL);

  vfs_sb->type = ext2_sb;
  vfs_sb->devname = strdup(name);
  vfs_sb->type = type;
  vfs_sb->magic = ext2_sb->s_magic;
  vfs_sb->blocksize = EXT2_BLOCK_SIZE(ext2_sb);
  vfs_sb->blocksize_bits = ext2_sb->s_log_block_size;

  struct vfs_mount *mount = calloc(1, sizeof(struct vfs_mount));
  mount->sb = vfs_sb;
  return mount;
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
