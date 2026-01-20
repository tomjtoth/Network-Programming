

#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

// tested with prev tasks' source code,
// smaller BUFSIZE means more stops & sleeps during DEV
#define BUFSIZE 256

void sigint_handler(int signo)
{
    (void)signo;
    const char msg[] = "\n\n\tSIGINT received, exiting...\n\n";

    // both of the below are async-signal-safe
    // as in https://man7.org/linux/man-pages/man7/signal-safety.7.html
    write(STDERR_FILENO, msg, sizeof(msg) - 1);
    _exit(1);
}

int main(int argc, char *argv[])
{
    int fd;
    char buf[BUFSIZE];
    ssize_t n;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s FILE\n", argv[0]);
        return 1;
    }

    // override default SIGINT handler
    signal(SIGINT, sigint_handler);

    fd = open(argv[1], O_RDONLY);
    if (fd < 0)
    {
        perror("open");
        return 1;
    }

    while ((n = read(fd, buf, BUFSIZE)) > 0)
    {
        write(STDOUT_FILENO, buf, n);

#ifdef DEV
        // sleep only when compiled with `-DDEV` flag
        usleep(500000);
#endif
    }

    if (n < 0)
    {
        perror("read");
    }

    close(fd);
    return 0;
}
