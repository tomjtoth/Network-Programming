#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define PORT 50000

long elapsed_time(struct timeval start, struct timeval end)
{
    return (end.tv_sec - start.tv_sec) * 1000000L +
           (end.tv_usec - start.tv_usec);
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s <server-ip-address> <total-bytes> <bytes-per-write>\n", argv[0]);
        exit(1);
    }

    int fd_sock,
        pos = 0,
        bytes_sent = 0,
        timer_started = 0,
        bytes_total = atoi(argv[2]),
        bytes_per_write = atoi(argv[3]);

    struct timeval t1, t2;

    struct sockaddr_in serv_addr;
    char buf[bytes_per_write];

    // should this be where I start timing?
    // gettimeofday(&t1, NULL);

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

    gettimeofday(&t1, NULL);
    if (connect(fd_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("faled to connect to server");
        exit(1);
    }
    gettimeofday(&t2, NULL);
    printf("  INFO: establishing connection took %ld microseconds\n", elapsed_time(t1, t2));

    while (bytes_sent < bytes_total)
    {
        // populate buffer with random UPPERCASE letters
        buf[pos++] = 'A' + (rand() % 26);
        bytes_sent++;

        // send to server when batch size reached
        // or partial batch when total_bytes reached
        if (pos == bytes_per_write || bytes_sent == bytes_total)
        {
            if (timer_started == 0)
            {
                gettimeofday(&t1, NULL);
                timer_started = 1;
            }

            if (write(fd_sock, buf, pos) != pos)
            {
                perror("write to server failed");
                exit(1);
            }
            pos = 0;
        }
    }

    gettimeofday(&t2, NULL);
    printf("  INFO: %ld microseconds elapsed between 1st and last write\n", elapsed_time(t1, t2));

    shutdown(fd_sock, SHUT_WR);

    close(fd_sock);
    return 0;
}
