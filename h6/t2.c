#include <unistd.h>
#include <stdlib.h>
#include <syslog.h>

int main()
{
    openlog("h6t2", LOG_PID | LOG_CONS, LOG_USER);

    usleep(500000);
    syslog(LOG_DEBUG, "LOG_DEBUG looks like this");

    usleep(500000);
    syslog(LOG_NOTICE, "LOG_NOTICE looks like this");

    usleep(500000);
    syslog(LOG_INFO, "LOG_INFO looks like this");

    usleep(500000);
    syslog(LOG_CRIT, "LOG_CRIT looks like this");

    usleep(500000);
    syslog(LOG_ERR, "LOG_ERR looks like this: '%s'", "with additional format");

    closelog();

    return EXIT_SUCCESS;
}
