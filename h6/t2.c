#include <stdlib.h>
#include <syslog.h>
#include <string.h>

int main(void)
{
    openlog("h6t2", LOG_PID | LOG_CONS, LOG_USER);

    syslog(LOG_INFO, "this is an info");

    syslog(LOG_ERR, "this is a formatted error message: '%s'", "from parameter");

    syslog(LOG_DEBUG, "this is debug info");

    closelog();

    return EXIT_SUCCESS;
}