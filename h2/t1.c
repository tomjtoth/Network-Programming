#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 256

int main(int argc, char *argv[])
{
    int i, p, num_procs = 5;
    pid_t pid;
    char buf[BUFSIZE];

    if (argc > 1)
    {
        num_procs = atoi(argv[1]);
        if (num_procs < 1 || num_procs > 50)
        {
            const char *msg = "1st arg must be 1-50 if passed\n";
            write(STDERR_FILENO, msg, strlen(msg));
            exit(1);
        }
    }

    // create N child processes
    for (p = 1; p < num_procs; p++)
    {
        pid = fork();
        if (pid < 0)
        {
            perror("fork failed");
            exit(1);
        }

        // child breaks the loop, only the parent forks
        if (pid == 0)
            break;
    }

    for (i = 1; i <= 5; i++)
    {
        int len = sprintf(buf, "i = %d (PID %d)\n", i, getpid());
        write(STDOUT_FILENO, buf, len);

#ifdef SLEEP
        // sleep only when compiled with `-DSLEEP=500000` flag
        usleep(SLEEP);
#endif
    }

    exit(0);
}