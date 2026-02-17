#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 65535
#define PORT 50000

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <server-ip-address>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int fd_sock;
    struct sockaddr_in serv_addr;
    ssize_t n = 0;

    fd_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd_sock < 0)
    {
        perror("creating socket failed");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0)
    {
        perror("Invalid address");
        exit(1);
    }

    while (n < BUFSIZE)
    {
        char buf[n];
        memset(&buf, 0, n);

        if (sendto(fd_sock, buf, n, 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
            perror("sendto failed");
            exit(EXIT_FAILURE);
        }
        else
        {
            printf("sent %d bytes\n", n);
        }

        n++;
    }

    close(fd_sock);
    return EXIT_SUCCESS;
}
