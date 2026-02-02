#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>

#define BUFSIZE 100

#define FIFO_CON "/tmp/fifo_converter"

int main(int argc, char *argv[])
{
    int fd_in = STDIN_FILENO;

    if (argc > 1)
    {
        fd_in = open(argv[1], O_RDONLY);
        if (fd_in < 0)
        {
            fprintf(stderr, "opening %s failed", argv[1]);
            exit(1);
        }
    }

    int fd_con = open(FIFO_CON, O_WRONLY);
    if (fd_con < 0)
    {
        fprintf(stderr, "opening %s failed", FIFO_CON);
        exit(1);
    }

    char buf[BUFSIZE];
    ssize_t n;

    // read 100 bytes at most from stdin
    while ((n = read(fd_in, buf, sizeof(buf))) > 0)
    {
        // if the last read char is LF then the line is under 100 char
        // and can be sent forward
        // else discarding it
        if (buf[n - 1] == '\n' && write(fd_con, buf, n) != n)
        {
            fprintf(stderr, "writing to %s failed", FIFO_CON);
            exit(1);
        }
    }

    if (n < 0)
    {
        perror("read from stdin failed");
        exit(1);
    }

    close(fd_con);

    return 0;
}
