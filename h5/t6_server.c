#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFSIZE 65535
#define PORT 50000

int main()
{
    int fd_sock;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len;
    char buf[BUFSIZE];
    ssize_t n;

    fd_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd_sock < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
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

    cli_len = sizeof(cli_addr);

    // iterate in main loop
    for (;;)
    {
        // get n bytes from socket, store in buffer
        // and store client address
        n = recvfrom(fd_sock, buf, BUFSIZE, 0,
                     (struct sockaddr *)&cli_addr, &cli_len);

        // move on to handling next connection on error
        if (n < 0)
            continue;

        printf("received %d bytes\n", n);
    }

    close(fd_sock);
    return EXIT_SUCCESS;
}
