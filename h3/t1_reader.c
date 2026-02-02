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

int main()
{

    int fd_con = open(FIFO_CON, O_WRONLY);
    if (fd_con < 0)
    {
        fprintf(stderr, "opening %s failed", FIFO_CON);
        exit(1);
    }

    char buf[BUFSIZE];
    ssize_t n;

    // read 100 bytes at most from stdin
    while ((n = read(STDIN_FILENO, buf, sizeof(buf))) > 0)
    {
        // if the last read char is LF
        // the line is under 100 char and can be sent forward
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
