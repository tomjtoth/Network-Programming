#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
/* default permissions for new files */
#define LOCKFILE "/tmp/MKLCKXXXXXX"

void my_lock_init(char *pathname);
void my_lock_wait(void);
void my_lock_release(void);

int main(int argc, char **argv)
{
    int fd, i, nloop;
    int *ptr;
    struct stat sb;

    if (argc != 3)
    {
        fprintf(stderr, "usage: %s <pathname> <#loops>\n", argv[0]);
        exit(1);
    }

    nloop = atoi(argv[2]);

    // open or create shared file
    if ((fd = open(argv[1], O_RDWR | O_CREAT, FILE_MODE)) < 0)
    {
        perror("open failed");
        exit(1);
    }

    // initialize shared memory ONLY ONCE
    if (fstat(fd, &sb) < 0)
    {
        perror("fstat failed");
        exit(1);
    }

    // initialize shared memory ONLY ONCE
    if ((__u_long)sb.st_size < sizeof(int))
    {
        int zero = 0;
        if (write(fd, &zero, sizeof(int)) != sizeof(int))
        {
            perror("write");
            exit(1);
        }
    }

    ptr = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED)
    {
        perror("mmap failed");
        exit(1);
    }
    close(fd);

#ifdef DISABLE_LOCKING
    int tmp;
#else
    my_lock_init(LOCKFILE);
#endif

    setbuf(stdout, NULL);
    int my_pid = getpid();

    for (i = 0; i < nloop; i++)
    {

#ifdef DISABLE_LOCKING
        printf("pid(%d): %d\n", my_pid, tmp = *ptr);
        usleep(1); /* try first without usleep() */
        *ptr = ++tmp;
#else
        my_lock_wait();
        printf("pid(%d): %d\n", my_pid, (*ptr)++);
        my_lock_release();
#endif
    }

    return 0;
}
