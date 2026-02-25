#define _POSIX_C_SOURCE 200112L

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define BUFSIZE 1024

void put(int sock, char *msg)
{
    size_t n, m;

    // appending LF, otherwise all my messages got ignored
    char buf[BUFSIZE];
    n = snprintf(buf, BUFSIZE, "%s\n", msg);

    // aborting when cannot send to server
    if ((m = write(sock, buf, n)) != n)
    {
        perror("write");
        exit(EXIT_FAILURE);
    }

    // debugging the LF issue above
    printf("  => sent %ld bytes (%s)\n", m, msg);
}

ssize_t get(int sock, char *buf)
{
    ssize_t n;

    if ((n = read(sock, buf, BUFSIZE)) < 0)
    {
        perror("read");
        exit(EXIT_FAILURE);
    }

    write(STDOUT_FILENO, buf, n);

    // I need this when extracting the port
    return n;
}

void server(int port)
{
    int fd_listen, fd_conn;
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
    serv_addr.sin_port = htons(port);

    if (bind(fd_listen, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("binding to serv_addr failed");
        exit(1);
    }

    listen(fd_listen, 5);

    for (;;)
    {
        char buf[BUFSIZE];
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
            get(fd_conn, buf);
            get(fd_conn, buf);
            get(fd_conn, buf);
            get(fd_conn, buf);
            get(fd_conn, buf);
            get(fd_conn, buf);
        }

        close(fd_conn);
    }
}

int main(int argc, char *argv[])
{
    int sock;
    char buf[BUFSIZE];

    struct addrinfo serv_addr, *res;
    struct servent *unix_tl_entry;

    // get standard TCP port of UNIX_TL service
    unix_tl_entry = getservbyname("UNIX_TL", "tcp");
    if (unix_tl_entry == NULL)
    {
        fprintf(stderr, "getservbyname failed\n");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.ai_family = AF_INET;
    serv_addr.ai_socktype = SOCK_STREAM;

    // resolve server name to IP addr and store it in result
    if (getaddrinfo("dx6-cs-02.pc.helsinki.fi", NULL, &serv_addr, &res) != 0)
    {
        perror("getaddrinfo");
        exit(EXIT_FAILURE);
    }

    // assign unix_tl_entry.s_port to res.ai_addr.sin_port
    ((struct sockaddr_in *)res->ai_addr)->sin_port = unix_tl_entry->s_port;

    sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, res->ai_addr, res->ai_addrlen) < 0)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    printf("connected to server\n");

    freeaddrinfo(res);

    get(sock, buf);
    put(sock, argv[1]);

    get(sock, buf);
    put(sock, argv[2]);

    get(sock, buf);
    get(sock, buf);

    // retrieve PORT number from last message
    get(sock, buf);

    // extracting
    char str_port[6]; // substring with '\0' at the end
    memcpy(str_port, buf + 55, 5);
    str_port[6] = '\0';
    int port = atoi(str_port);

    printf("  => resolved port from msg: %d\n", port);
    server(port);

    // get leftover messages from server
    while (get(sock, buf))
    {
    };

    close(sock);
    return EXIT_SUCCESS;
}
