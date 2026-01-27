# task 3.a

From the [man page](https://man7.org/linux/man-pages/man2/lstat.2.html):

> **lstat()** is identical to stat(), except that if path is a symbolic
> link, then it returns information about the link itself, not the
> file that the link refers to.
>
> **fstat()** is identical to stat(), except that the file about which
> information is to be retrieved is specified by the file descriptor
> fd.

From the [stat(3type)](https://man7.org/linux/man-pages/man3/stat.3type.html) man page:

```c
#include <sys/stat.h>

struct stat {
    dev_t      st_dev;      /* ID of device containing file */
    ino_t      st_ino;      /* Inode number */
    mode_t     st_mode;     /* File type and mode */
    nlink_t    st_nlink;    /* Number of hard links */
    uid_t      st_uid;      /* User ID of owner */
    gid_t      st_gid;      /* Group ID of owner */
    dev_t      st_rdev;     /* Device ID (if special file) */
    off_t      st_size;     /* Total size, in bytes */
    blksize_t  st_blksize;  /* Block size for filesystem I/O */
    blkcnt_t   st_blocks;   /* Number of 512 B blocks allocated */

    /* Since POSIX.1-2008, this structure supports nanosecond
        precision for the following timestamp fields.
        For the details before POSIX.1-2008, see VERSIONS.  */

    struct timespec  st_atim;  /* Time of last access */
    struct timespec  st_mtim;  /* Time of last modification */
    struct timespec  st_ctim;  /* Time of last status change */

#define st_atime  st_atim.tv_sec  /* Backward compatibility */
#define st_mtime  st_mtim.tv_sec
#define st_ctime  st_ctim.tv_sec
};
```
