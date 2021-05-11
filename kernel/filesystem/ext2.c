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

char *ext2_fs_bread(struct vfs_sb *sb, uint32_t block, uint32_t size)
{
  return virt_fs_bread(sb->devname, block * (sb->blocksize / VFS_BYTES_P_SECTOR), size);
}

char *ext2_fs_bread_block(struct vfs_sb *sb, uint32_t block)
{
  return ext2_fs_bread(sb, block, sb->blocksize);
}


struct vfs_inode *ext2_fs_inode_alloc(struct vfs_sb *sb)
{
  struct vfs_inode *inode = virt_fs_inode_init();
  inode->sb = sb;
  return inode;
}

struct vfs_mount *ext2_fs_mount(const char *name, const char *path, struct vfs_type *type)
{
  (void)path;

  struct vfs_sb *vfs_sb = calloc(1, sizeof(struct vfs_sb));
  vfs_sb->devname = strdup(name);
  vfs_sb->blocksize = EXT2_MIN_BLOCK_SIZE;

  struct ext2_sb *ext2_sb = calloc(1, sizeof(struct ext2_sb));
  char *buffer = ext2_fs_bread_block(vfs_sb, 1);
  memcpy(ext2_sb, buffer, vfs_sb->blocksize);

  if (ext2_sb->s_magic != EXT2_SUPER_MAGIC)
    sys_panic("Ext2 FS: Wrong magic value", NULL);

  vfs_sb->info = ext2_sb;
  vfs_sb->type = type;
  vfs_sb->magic = ext2_sb->s_magic;
  vfs_sb->blocksize = EXT2_BLOCK_SIZE(ext2_sb);
  vfs_sb->blocksize_bits = ext2_sb->s_log_block_size;

  struct vfs_inode *inode = ext2_fs_inode_alloc(vfs_sb);
  inode->ino = EXT2_ROOT_INO;

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
