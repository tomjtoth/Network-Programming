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
#define FIFO_DBL "/tmp/fifo_doubler"

int main()
{

    if (mkfifo(FIFO_CON, 0666) < 0)
    {
        fprintf(stderr, "mkfifo %s failed", FIFO_CON);
        exit(1);
    };

    if (mkfifo(FIFO_DBL, 0666) < 0)
    {
        fprintf(stderr, "mkfifo %s failed", FIFO_DBL);
        exit(1);
    };

    char *msg = "FIFOs created, launch reader and doubler now.\n";
    write(STDOUT_FILENO, msg, strlen(msg));

    int fd_con = open(FIFO_CON, O_RDONLY);
    if (fd_con < 0)
    {
        fprintf(stderr, "opening %s failed", FIFO_CON);
        exit(1);
    }

    int fd_dbl = open(FIFO_DBL, O_WRONLY);
    if (fd_dbl < 0)
    {
        fprintf(stderr, "opening %s failed", FIFO_DBL);
        exit(1);
    }

    // both use these vars
    char buf[BUFSIZE];
    ssize_t n;

    int pos = 0;
    char c;

    while ((n = read(fd_con, &c, 1)) > 0)
    {
        // shift to uppercase 1 by 1
        buf[pos] = toupper((unsigned char)c);
        pos++;

        // send forward and reset on LF
        if (c == '\n')
        {
            if (write(fd_dbl, buf, pos) != pos)
            {
                fprintf(stderr, "writing to %s failed", FIFO_DBL);
                exit(1);
            };
            pos = 0;
        }
    }

    if (n < 0)
    {
        fprintf(stderr, "reading %s failed", FIFO_CON);
        exit(1);
    }

    close(fd_con);
    close(fd_dbl);

    if (unlink(FIFO_CON) < 0)
        fprintf(stderr, "can't unlink %s", FIFO_CON);

    if (unlink(FIFO_DBL) < 0)
        fprintf(stderr, "can't unlink %s", FIFO_DBL);

    return 0;
}
