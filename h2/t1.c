#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFSIZE 256

int main()
{
    int i;
    char buf[BUFSIZE];

    fork();
    for (i = 1; i <= 5; i++)
    {
        int len = sprintf(buf, "i = %d (PID %d)\n", i, getpid());

        write(STDOUT_FILENO, buf, len);
        usleep(50000);
    }

    exit(0);
}