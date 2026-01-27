#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>

#define BUFSIZE 100
#define FIFO_SRV "/tmp/fifo_srv"
#define FIFO_CLI "/tmp/fifo_cli"

#ifdef SERVER
#define FIFO_SRV_PERM O_RDONLY
#define FIFO_CLI_PERM O_WRONLY
#else
#define FIFO_SRV_PERM O_WRONLY
#define FIFO_CLI_PERM O_RDONLY

#define SERVER 1

#endif

int main()
{
#ifdef SERVER

    mkfifo(FIFO_SRV, 0666);
    mkfifo(FIFO_CLI, 0666);

    char *msg = "FIFOs created, launch client now.\n";
    write(STDERR_FILENO, msg, strlen(msg));

#endif

    // both open both FIFOs, only with different permissions
    int fd_srv = open(FIFO_SRV, FIFO_SRV_PERM);
    if (fd_srv < 0)
    {
        perror("opening fifo_srv failed");
        exit(1);
    }

    int fd_cli = open(FIFO_CLI, FIFO_CLI_PERM);
    if (fd_cli < 0)
    {
        perror("opening fifo_cli failed");
        exit(1);
    }

    char buf[BUFSIZE];
    ssize_t n;

#ifdef SERVER

    int pos = 0;
    char c;

    while ((n = read(fd_srv, &c, 1)) > 0)
    {
        if (pos < BUFSIZE)
        {
            buf[pos] = toupper((unsigned char)c);
        }
        pos++;

        if (c == '\n')
        {
            // send back if under 100 long
            // else discard
            if (pos <= BUFSIZE)
            {
                write(fd_cli, buf, pos);
            }

            pos = 0;
        }
    }

    if (n < 0)
    {
        perror("reading pipe failed");
        exit(1);
    }

    close(fd_srv);
    close(fd_cli);

    if (unlink(FIFO_SRV) < 0)
        fprintf(stderr, "can't unlink %s", FIFO_SRV);

    if (unlink(FIFO_CLI) < 0)
        fprintf(stderr, "can't unlink %s", FIFO_CLI);

#else

    // read 100 bytes at most from stdin
    while ((n = read(STDIN_FILENO, buf, sizeof(buf))) > 0)
    {
        // send them to the server
        if (write(fd_srv, buf, n) != n)
        {
            perror("sending to server failed");
            exit(1);
        }
    }

    if (n < 0)
    {
        perror("read from stdin failed");
        exit(1);
    }

    close(fd_srv);

    while ((n = read(fd_cli, buf, sizeof(buf))) > 0)
    {
        write(STDOUT_FILENO, buf, n);
    }

    if (n < 0)
    {
        perror("read from server FIFO failed");
        exit(1);
    }

    close(fd_cli);

    char *msg = "FIFOs closed, stop server.";
    write(STDERR_FILENO, msg, strlen(msg));

#endif

    return 0;
}
