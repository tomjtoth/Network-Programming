#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void sig_chld(int signo)
{

    pid_t pid;
    int stat;

    int prevno = errno;

    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
    // https://man7.org/linux/man-pages/man7/signal-safety.7.html
    {
        const char *msg = "child %d terminated\n";
        write(STDERR_FILENO, msg, strlen(msg));
    }

    errno = prevno;
}
