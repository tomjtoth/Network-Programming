#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFSIZE 100
#define PORT 50000

int main()
{
    int fd_sock;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len;
    char buf[BUFSIZE], buf2[BUFSIZE * 2];
    ssize_t n;

    fd_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd_sock < 0)
    {
        perror("socket");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(fd_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("binding to serv_addr failed");
        exit(1);
    }

    for (;;)
    {
        cli_len = sizeof(cli_addr);
        n = recvfrom(fd_sock, buf, BUFSIZE, 0,
                     (struct sockaddr *)&cli_addr, &cli_len);

        if (n <= 0)
            continue;

        // duplicate data
        memcpy(buf2, buf, n);
        memcpy(buf2 + n, buf, n);

        sendto(fd_sock, buf2, 2 * n, 0,
               (struct sockaddr *)&cli_addr, cli_len);
    }
}
