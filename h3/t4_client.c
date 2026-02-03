#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 100
#define PORT 50000

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <server-ip>\n", argv[0]);
        exit(1);
    }

    int fd_sock;
    struct sockaddr_in serv_addr;
    char buf[BUFSIZE];
    ssize_t n;

    fd_sock = socket(AF_INET, SOCK_STREAM, 0);
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

    if (connect(fd_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("faled to connect to server");
        exit(1);
    }

    while ((n = read(STDIN_FILENO, buf, sizeof(buf))) > 0)
    {
        if (write(fd_sock, buf, n) != n)
        {
            perror("write to server failed");
            exit(1);
        }
    }

    shutdown(fd_sock, SHUT_WR);

    close(fd_sock);
    return 0;
}
