#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>

#define BUFSIZE 100

#define FIFO_DBL "/tmp/fifo_doubler"

int main()
{
    int fd_dbl = open(FIFO_DBL, O_RDONLY);
    if (fd_dbl < 0)
    {
        fprintf(stderr, "opening %s failed", FIFO_DBL);
        exit(1);
    }

    char buf[BUFSIZE];
    ssize_t n;

    while ((n = read(fd_dbl, buf, sizeof(buf))) > 0)
    {
        // doubler's input is also sanitized
        // write the received input twice to stdout
        write(STDOUT_FILENO, buf, n);
        write(STDOUT_FILENO, buf, n);
    }

    if (n < 0)
    {
        perror("reading pipe failed");
        exit(1);
    }

    close(fd_dbl);

    return 0;
}
