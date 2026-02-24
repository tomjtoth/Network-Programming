#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
        fprintf(stderr, "correct usage: %s <path-to-file>\n", argv[0]);

    openlog("h6t2", LOG_PID | LOG_CONS, LOG_USER);

    syslog(LOG_INFO, "Program started");

    FILE *f = fopen(argv[1], "r");
    if (!f)
    {
        syslog(LOG_ERR, "Failed to open file: %s", strerror(errno));
    }
    else
    {
        syslog(LOG_INFO, "File opened successfully");
        fclose(f);
    }

    syslog(LOG_DEBUG, "Program finishing normally");

    closelog();

    return EXIT_SUCCESS;
}