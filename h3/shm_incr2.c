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
void my_lock_wait();
void my_lock_release();

int main(int argc, char **argv)
{
	int fd, i, nloop, zero = 0;
	int *ptr;
	pid_t pid;

	if (argc != 3)
	{
		perror("usage: incr2 <pathname> <#loops>");
		exit(1);
	}
	nloop = atoi(argv[2]);

	/* 4open file, initialize to 0, map into memory */
	if ((fd = open(argv[1], O_RDWR | O_CREAT, FILE_MODE)) < 0)
	{
		perror("open failed");
		exit(1);
	}
	if (write(fd, &zero, sizeof(int)) < 0)
	{
		perror("write failed");
		exit(1);
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
	/* 4create, initialize, and unlink file lock */
	my_lock_init(LOCKFILE);
#endif

	setbuf(stdout, NULL); /* stdout is unbuffered */
	if ((pid = fork()) < 0)
	{
		perror("fork failed");
		exit(1);
	}

	if (pid == 0)
	{ /* child */
		for (i = 0; i < nloop; i++)
		{

#ifdef DISABLE_LOCKING
			printf("child: %d\n", tmp = *ptr);
			usleep(1); /* try first without usleep() */
			*ptr = ++tmp;
#else
			my_lock_wait();
			printf("child: %d\n", (*ptr)++);
			my_lock_release();
#endif
		}
		exit(0);
	}

	/* 4parent */
	for (i = 0; i < nloop; i++)
	{
#ifdef DISABLE_LOCKING
		printf("parent: %d\n", tmp = *ptr);
		usleep(1); /* try first without usleep() */
		*ptr = ++tmp;
#else
		my_lock_wait();
		printf("parent: %d\n", (*ptr)++);
		my_lock_release();
#endif
	}
	exit(0);
}
