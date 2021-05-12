#pragma once

#define O_ACCMODE 00000003
#define O_RDONLY 00000000
#define O_WRONLY 00000001
#define O_RDWR 00000002
#define O_CREAT 00000100  /* not fcntl */
#define O_EXCL 00000200   /* not fcntl */
#define O_NOCTTY 00000400 /* not fcntl */
#define O_TRUNC 00001000  /* not fcntl */
#define O_APPEND 00002000
#define O_NONBLOCK 00004000
#define O_DSYNC 00010000  /* used to be O_SYNC, see below */
#define FASYNC 00020000   /* fcntl, for BSD compatibility */
#define O_DIRECT 00040000 /* direct disk access hint */
#define O_LARGEFILE 00100000
#define O_DIRECTORY 00200000 /* must be a directory */
#define O_NOFOLLOW 00400000  /* don't follow links */
#define O_NOATIME 01000000
#define O_CLOEXEC 02000000 /* set close_on_exec */
