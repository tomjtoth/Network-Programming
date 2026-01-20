#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

const int BUFSIZE = 1024;

int main(int argc, char *argv[])
{
    int fd, i, line_len = 0;
    ssize_t n, w_ret_val;
    // declare elements of those 2 to be char
    char buf[BUFSIZE], line[BUFSIZE];

    if (argc > 1)
    {
        // open file (passed arg)
        fd = open(argv[1], O_RDONLY);
        if (fd < 0)
        {
            // abort on failure
            fprintf(stderr, "  Failed to open '%s'.\n", argv[1]);
            return 1;
        }
    }
    else
    {
        fd = STDIN_FILENO;
    }

    // try reading a line
    while ((n = read(fd, buf, BUFSIZE)) > 0)
    {
        for (i = 0; i < n; i++)
        {
            // populate line buffer char-by-char
            line[line_len++] = buf[i];

            if (buf[i] == '\n' || line_len == BUFSIZE)
            {
                // if succeeded, print twice
                w_ret_val = write(STDOUT_FILENO, line, line_len);
                w_ret_val = write(STDOUT_FILENO, line, line_len);

                // ignoring retval of write
                (void)w_ret_val;
                line_len = 0;
            }
        }
    }

    if (n < 0)
    {
        fprintf(stderr, "  Failed to read line\n");
    }

    if (fd != STDIN_FILENO)
    {
        close(fd);
    }

    return 0;
}
