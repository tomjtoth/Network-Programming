#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>

// https://man7.org/linux/man-pages/man7/inode.7.html
void print_file_info(struct stat *statbuf)
{
    struct stat sb = (*statbuf);
    mode_t mode = sb.st_mode;

    printf("%s\nUID = %d\nGID = %d\nSize = %d\n",
           S_ISREG(mode)    ? "Regular file"
           : S_ISDIR(mode)  ? "Directory"
           : S_ISLNK(mode)  ? "Symbolic link"
           : S_ISCHR(mode)  ? "Character device"
           : S_ISBLK(mode)  ? "Block device"
           : S_ISFIFO(mode) ? "FIFO / Pipe"
           : S_ISSOCK(mode) ? "Socket"
                            : "Unknown",

           sb.st_uid,
           sb.st_gid,
           sb.st_size);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <fd>", argv[0]);
        exit(1);
    }

    int fd = atoi(argv[1]);
    struct stat statbuf;

    int rc = fstat(fd, &statbuf);
    if (rc < 0)
    {
        perror("fstat failed");
        exit(1);
    }

    print_file_info(&statbuf);

    return 0;
}
