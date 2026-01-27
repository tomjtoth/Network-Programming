#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#define BUFSIZE 100

void reader(int pipefd[2])
{
    // reader pushes into the pipe,
    // closing unused direction
    close(pipefd[0]);

    char buf[BUFSIZE];
    ssize_t n;

    // read 100 bytes at most from stdin
    while ((n = read(STDIN_FILENO, buf, sizeof(buf))) > 0)
    {
        // send them into the pipe
        if (write(pipefd[1], buf, n) != n)
        {
            perror("write to pipe failed");
            exit(1);
        }
    }

    if (n < 0)
    {
        perror("read from stdin failed");
        exit(1);
    }

    close(pipefd[1]);
}

void doubler(int pipefd[2])
{
    // doubler reads from the pipe,
    // closing unused direction
    close(pipefd[1]);

    char line[BUFSIZE];
    int pos = 0;
    char c;
    ssize_t n;

    while ((n = read(pipefd[0], &c, 1)) > 0)
    {
        if (pos < BUFSIZE)
        {
            line[pos] = c;
        }
        pos++;

        if (c == '\n')
        {
            // print if the line was max 100 long
            // else discard
            if (pos <= BUFSIZE)
            {
                write(STDOUT_FILENO, line, pos);
                write(STDOUT_FILENO, line, pos);
            }

            pos = 0;
        }
    }

    if (n < 0)
    {
        perror("reading pipe failed");
        exit(1);
    }

    // close pipe
    close(pipefd[0]);
}

int main()
{
    int pipefd[2];
    pid_t pid;

    if (pipe(pipefd) < 0)
    {
        perror("pipe failed");
        exit(1);
    }

    pid = fork();
    if (pid < 0)
    {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0)
        doubler(pipefd);
    else
        reader(pipefd);

    exit(0);
}
