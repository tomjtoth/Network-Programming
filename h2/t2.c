#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int do_child(char *path, char *arg)
{
    // reading args stops at the 1st NULL (?)
    char *argv[3] = {path, arg, NULL};

    int rc = execvp(path, argv);

    // exec* return only if they fail
    perror("execv failed");
    return rc;
}

int main(int argc, char *argv[])
{
    int i, child_stat;

    if (argc < 2 || argc > 3)
    {
        fprintf(stderr, "Usage: %s <program_path> [arg]\n", argv[0]);
        exit(1);
    }

    char *path = argv[1];
    char *arg = (argc == 3) ? argv[2] : NULL;

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("forking failed");
        exit(1);
    }

    if (pid == 0)
        // child does its thing and exits with RC of binary passed in to path
        exit(do_child(path, arg));

    // parent process prints loop
    for (i = 1; i <= 5; i++)
        printf(" i = %d \n", i);

    // parent waits for child and retrieves its exit code
    int rc = waitpid(pid, &child_stat, WNOHANG);
    if (rc < 0)
    {
        perror("waitpid failed");
        exit(1);
    }

    // parent prints exit code description
    // https://linux.die.net/man/2/waitpid
    if (WIFEXITED(child_stat))
    {
        fprintf(
            stdout,
            "child exited with code %d\n",

            // This macro should only be employed if WIFEXITED returned true
            WEXITSTATUS(child_stat));
    }
    else
    {
        printf("Child errored out\n");
    }

    exit(0);
}