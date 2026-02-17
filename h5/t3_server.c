#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFSIZE 100
#define PORT 50000

int main(void)
{
    int fd_listen, fd_conn;
    struct sockaddr_in serv_addr;

    char *buf[BUFSIZE];
    int pos = 0;
    char c;
    ssize_t n;

    fd_listen = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_listen < 0)
    {
        perror("socket");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(fd_listen, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("binding to serv_addr failed");
        exit(1);
    }

    listen(fd_listen, 5);

    for (;;)
    {
        fd_conn = accept(fd_listen, NULL, NULL);
        if (fd_conn < 0)
        {
            // don't exit on failed connection of client,
            // just move on, lol
            continue;
        }

        // expecting sanitized data, BUFSIZE at most
        while ((n = read(fd_conn, buf, BUFSIZE)) > 0)
        {
            write(STDOUT_FILENO, buf, n);
        }

        close(fd_conn);
    }
}
