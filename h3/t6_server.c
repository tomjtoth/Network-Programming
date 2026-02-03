#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

#define PORT 50000

long elapsed_time(struct timeval start, struct timeval end)
{
    return (end.tv_sec - start.tv_sec) * 1000000L +
           (end.tv_usec - start.tv_usec);
}

void serve_client(int fd_conn, int buf_size)
{
    char buf[buf_size];

#ifdef DEBUG
    // the more decimals given, the longer the message
    // msg is 48, so 22 decimal places shpould be allowed
    char msg_buf[70];
    int msg_buf_size;
#endif

    ssize_t n;
    struct timeval t1, t2;

    gettimeofday(&t1, NULL);
    while ((n = read(fd_conn, &buf, buf_size)) > 0)
    {
#ifdef DEBUG
        msg_buf_size = sprintf(msg_buf, "  DEBUG: received %ld byte(s), discarding them\n", n);
        write(STDOUT_FILENO, msg_buf, msg_buf_size);
#endif
    }
    gettimeofday(&t2, NULL);
    printf("%ld elapsed between 1st and last read\n", elapsed_time(t1, t2));

#ifdef DEBUG
    char *msg = "  DEBUG: client closed connection\n";
    write(STDOUT_FILENO, msg, strlen(msg));
#endif

    close(fd_conn);
    exit(0);
}

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <bytes-to-read>", argv[0]);
        exit(1);
    }

    int fd_listen, fd_conn, bytes_to_read = atoi(argv[1]);
    struct sockaddr_in serv_addr;

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

        if (fork() == 0)
        {
            // child proc of server serves the connected client
            close(fd_listen);
            serve_client(fd_conn, bytes_to_read);
        }

        close(fd_conn);
    }
}
