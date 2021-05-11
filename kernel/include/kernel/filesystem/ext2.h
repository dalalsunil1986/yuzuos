#pragma once

#include <kernel/filesystem/virtual.h>

void ext2_fs_init();
struct vfs_type *ext2_fs_type_get();
