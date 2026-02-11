#define _POSIX_C_SOURCE 200112L

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define BUFSIZE 100

void debug(char *msg)
{
#ifdef DEBUG
    char buf[strlen(msg) + 7];

    int x = snprintf(buf, sizeof(buf), " ==> %s\n", msg);
    write(STDOUT_FILENO, buf, x);
#endif
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <server-domain-name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sock;
    struct addrinfo serv_addr, *res;
    struct servent *echo_entry;
    char buf[BUFSIZE];
    ssize_t n;
    char *msg;

    // get standard TCP port of echo service 7?
    echo_entry = getservbyname("echo", "tcp");
    if (echo_entry == NULL)
    {
        fprintf(stderr, "getservbyname failed\n");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.ai_family = AF_INET;
    serv_addr.ai_socktype = SOCK_STREAM;

    // resolve server name to IP addr and store it in result
    if (getaddrinfo(argv[1], NULL, &serv_addr, &res) != 0)
    {
        perror("getaddrinfo");
        exit(EXIT_FAILURE);
    }

    debug("resolved server addr");

    // assign echo_entry.s_port to res.ai_addr.sin_port
    ((struct sockaddr_in *)res->ai_addr)->sin_port = echo_entry->s_port;

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

    freeaddrinfo(res);

    debug("connected to server");

    // read at most BUFSIZE from STDIN
    while ((n = read(STDIN_FILENO, buf, BUFSIZE)) > 0)
    {
        // lines within limit end with \n
        if (buf[n - 1] != '\n')
        {
            debug("last char was not LF");
            char c;
            while (read(STDIN_FILENO, &c, 1) > 0 && c != '\n')
            {

                debug("read 1 char");
                // no-op, just read and discard everything up to LF
            };

            debug("LF reached, skipping to next line");

            // read next line
            continue;
        }

        // send line
        if (write(sock, buf, n) != n)
        {
            perror("write");
            exit(EXIT_FAILURE);
        }

        debug("msg sent");

        // receive echo
        if (read(sock, buf, n) != n)
        {
            perror("read");
            exit(1);
        }

        debug("echo arrived back");

        // print line
        if (write(STDOUT_FILENO, buf, n) != n)
        {
            perror("write to stdout");
            exit(EXIT_FAILURE);
        }

        debug("echo written to stdout");
    }

    close(sock);
    return EXIT_SUCCESS;
}
