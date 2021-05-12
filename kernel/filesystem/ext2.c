#include <kernel/filesystem/ext2.h>
#include <kernel/utils/log.h>
#include <kernel/utils/stdlib.h>
#include <kernel/utils/string.h>
#include <kernel/utils/stat.h>
#include <kernel/utils/errno.h>
#include <kernel/utils/math.h>
#include <kernel/system/sys.h>
#include <kernel/system/limits.h>
#include <kernel/memory/physical.h>
#include <stddef.h>

struct vfs_mount *ext2_fs_mount(const char *name, const char *path, struct vfs_type *type);
struct vfs_inode *ext2_fs_inode_alloc(struct vfs_sb *sb);
void ext2_fs_inode_read(struct vfs_inode *inode);
struct vfs_inode *ext2_fs_lookup(struct vfs_inode *dir, struct vfs_dentry *dentry);

static struct vfs_type ext2_fs_type = {
    .name = "ext2",
    .mount = ext2_fs_mount};

static struct vfs_file_op ext2_fs_file_op = {};
static struct vfs_inode_op ext2_fs_file_inode_op = {};

static struct vfs_file_op ext2_fs_dir_op = {};
static struct vfs_inode_op ext2_fs_dir_inode_op = {
    .lookup = ext2_fs_lookup};

static struct vfs_inode_op ext2_fs_special_inode_op = {};

static struct vfs_super_op ext2_fs_super_op = {
    .inode_alloc = ext2_fs_inode_alloc,
    .inode_read = ext2_fs_inode_read};

char *ext2_fs_bread(struct vfs_sb *sb, uint32_t block, uint32_t size)
{
  return virt_fs_bread(sb->devname, block * (sb->blocksize / VFS_BYTES_P_SECTOR), size);
}

char *ext2_fs_bread_block(struct vfs_sb *sb, uint32_t block)
{
  return ext2_fs_bread(sb, block, sb->blocksize);
}

struct ext2_group_desc *ext2_fs_group_desc_get(struct vfs_sb *sb, uint32_t group)
{
  struct ext2_group_desc *description = calloc(1, sizeof(struct ext2_group_desc));
  struct ext2_sb *ext2_sb = (struct ext2_sb *)sb->info;
  uint32_t block = ext2_sb->s_first_data_block + 1 + group / EXT2_GROUPS_P_BLOCK(ext2_sb);
  char *buffer = ext2_fs_bread_block(sb, block);
  uint32_t offset = group % EXT2_GROUPS_P_BLOCK(ext2_sb) * sizeof(struct ext2_group_desc);
  memcpy(description, buffer + offset, sizeof(struct ext2_group_desc));
  return description;
}

struct ext2_inode *ext2_fs_inode_get(struct vfs_sb *sb, ino_t ino)
{
  struct ext2_sb *ext2_sb = (struct ext2_sb *)sb->info;
  uint32_t group = EXT2_GET_GROUP_FROM_INODE(ext2_sb, ino);
  struct ext2_group_desc *gdp = ext2_fs_group_desc_get(sb, group);
  uint32_t block = gdp->bg_inode_table + EXT2_GET_RELATIVE_INODE_IN_GROUP(ext2_sb, ino) / EXT2_INODES_P_BLOCK(ext2_sb);
  uint32_t offset = (EXT2_GET_RELATIVE_INODE_IN_GROUP(ext2_sb, ino) % EXT2_INODES_P_BLOCK(ext2_sb)) * sizeof(struct ext2_inode);
  char *table_buf = ext2_fs_bread_block(sb, block);

  return (struct ext2_inode *)(table_buf + offset);
}

struct vfs_inode *ext2_fs_inode_alloc(struct vfs_sb *sb)
{
  struct vfs_inode *inode = virt_fs_inode_alloc();
  inode->sb = sb;
  return inode;
}

void ext2_fs_inode_read(struct vfs_inode *inode)
{
  struct ext2_inode *ext2_inode = ext2_fs_inode_get(inode->sb, inode->ino);
  inode->mode = ext2_inode->i_mode;
  inode->gid = ext2_inode->i_gid;
  inode->uid = ext2_inode->i_uid;
  inode->nlink = ext2_inode->i_links_count;
  inode->size = ext2_inode->i_size;
  inode->atime = ext2_inode->i_atime;
  inode->ctime = ext2_inode->i_ctime;
  inode->mtime = ext2_inode->i_mtime;
  inode->blksize = PHYS_MM_BLOCK;
  inode->blocks = ext2_inode->i_blocks;
  inode->flags = ext2_inode->i_flags;
  inode->info = ext2_inode;

  if (S_ISREG(inode->mode))
  {
    inode->op = &ext2_fs_file_inode_op;
    inode->fop = &ext2_fs_file_op;
  }
  else if (S_ISDIR(inode->mode))
  {
    inode->op = &ext2_fs_dir_inode_op;
    inode->fop = &ext2_fs_dir_op;
  }
  else
  {
    inode->op = &ext2_fs_special_inode_op;
    virt_fs_inode_special_init(inode, inode->mode, ext2_inode->i_block[0]);
  }
}

struct vfs_mount *ext2_fs_mount(const char *name, const char *path, struct vfs_type *type)
{
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
  vfs_sb->sop = &ext2_fs_super_op;

  struct vfs_inode *inode = ext2_fs_inode_alloc(vfs_sb);
  inode->ino = EXT2_ROOT_INO;
  ext2_fs_inode_read(inode);

  struct vfs_dentry *dentry = virt_fs_dentry_alloc(path, NULL);
  dentry->sb = vfs_sb;
  dentry->inode = inode;
  vfs_sb->root = dentry;

  struct vfs_mount *mount = calloc(1, sizeof(struct vfs_mount));
  mount->sb = vfs_sb;
  mount->root = dentry;
  mount->mount = dentry;

  return mount;
}

int ext2_fs_ino_find(struct vfs_sb *sb, uint32_t block, const void *value)
{
  const char *name = (const char *)value;
  char *buffer = ext2_fs_bread_block(sb, block);
  uint32_t size = 0;
  char name_tmp[NAME_MAX];

  struct ext2_dir_entry *entry = (struct ext2_dir_entry *)buffer;
  while (size < sb->blocksize)
  {
    memcpy(name_tmp, entry->name, entry->name_len);
    name_tmp[entry->name_len] = 0;

    if (!strcmp(name_tmp, name))
      return entry->ino;

    size = size + entry->rec_len;
    entry = (struct ext2_dir_entry *)((char *)entry + entry->rec_len);
  }
  return -ENOENT;
}

int ext2_fs_bread_rec(struct vfs_sb *sb, int level, uint32_t block, const void *value, ext2_fs_action_t action)
{
  if (level > 0)
  {
    int result = -ENOENT;

    uint32_t *buffer = (uint32_t *)ext2_fs_bread_block(sb, block);
    for (int i = 0, nblocks = sb->blocksize / 4; i < nblocks; i++)
      if ((result = ext2_fs_bread_rec(sb, level - 1, buffer[i], value, action)) >= 0)
        break;

    return result;
  }
  else
    return action(sb, block, value);
}

void ext2_fs_bread_n_rec(struct vfs_sb *sb, uint32_t block, char *buffer[], loff_t ppos, uint32_t *target_pos, size_t count, int level)
{
  if (level > 0)
  {
    uint32_t *buffer_block = (uint32_t *)ext2_fs_bread_block(sb, block);
    for (uint32_t i = 0, times = sb->blocksize / 4; *target_pos < ppos + count && i < times; i++)
      ext2_fs_bread_n_rec(sb, buffer_block[i], buffer, ppos, target_pos, count, level - 1);
  }
  else
  {
    char *buffer_block = ext2_fs_bread_block(sb, block);
    int pos_start = (ppos > *target_pos) ? ppos - *target_pos : 0;
    uint32_t pos_end = ((ppos + count) < (*target_pos + sb->blocksize)) ? (*target_pos + sb->blocksize - ppos - count) : 0;
    memcpy(*buffer, buffer_block + pos_start, sb->blocksize - pos_start - pos_end);

    *target_pos += sb->blocksize;
    *buffer += sb->blocksize - pos_start - pos_end;
  }
}

struct vfs_inode *ext2_fs_lookup(struct vfs_inode *dir, struct vfs_dentry *dentry)
{
  struct ext2_inode *ext2_inode = dir->info;
  struct vfs_sb *sb = dir->sb;

  for (uint32_t i = 0, ino = 0; i < ext2_inode->i_blocks; i++)
  {
    if (!ext2_inode->i_block[i])
      continue;

    if ((i < EXT2_INO_UPPER_LEVEL0 && (ino = EXT2_INO_FIND(sb, 0, ext2_inode->i_block[i], dentry->name)) > 0) ||
        ((EXT2_INO_UPPER_LEVEL0 <= i && i < EXT2_INO_UPPER_LEVEL1) && (ino = EXT2_INO_FIND(sb, 1, ext2_inode->i_block[12], dentry->name)) > 0) ||
        ((EXT2_INO_UPPER_LEVEL1 <= i && i < EXT2_INO_UPPER_LEVEL2) && (ino = EXT2_INO_FIND(sb, 2, ext2_inode->i_block[13], dentry->name)) > 0) ||
        ((EXT2_INO_UPPER_LEVEL2 <= i && i < EXT2_INO_UPPER_LEVEL3) && (ino = EXT2_INO_FIND(sb, 3, ext2_inode->i_block[14], dentry->name)) > 0))
    {
      struct vfs_inode *inode = dir->sb->sop->inode_alloc(dir->sb);
      inode->ino = ino;
      ext2_fs_inode_read(inode);
      return inode;
    }
  }
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
