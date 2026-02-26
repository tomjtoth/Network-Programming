#define _POSIX_C_SOURCE 200112L

#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/wait.h>

#define BUFSIZE 1024

const char *HOSTNAME = "dx6-cs-02.pc.helsinki.fi";
const char *SERVICE_NAME = "UNIX_TL";

// setting this in beginning of main, everything else only reads it
int verbose = 0;

/**
 * connect to the remote server,
 * returns the created socket
 */
int conn_remote(uint16_t port)
{
    int sock;
    struct addrinfo serv_addr, *res;
    struct servent *unix_tl_entry;

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.ai_family = AF_INET;
    serv_addr.ai_socktype = SOCK_STREAM;

    // resolve server name to IP addr and store it in result
    if (getaddrinfo(HOSTNAME, NULL, &serv_addr, &res) != 0)
    {
        perror("getaddrinfo");
        exit(EXIT_FAILURE);
    }

    // if port is not provided use the default
    if (port == 0)
    {
        // get standard TCP port of UNIX_TL service
        unix_tl_entry = getservbyname(SERVICE_NAME, "tcp");
        if (unix_tl_entry == NULL)
        {
            fprintf(stderr, "getservbyname failed\n");
            exit(EXIT_FAILURE);
        }
        port = unix_tl_entry->s_port;
    }

    ((struct sockaddr_in *)res->ai_addr)->sin_port = port;

    // assign unix_tl_entry.s_port to res.ai_addr.sin_port

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

    if (verbose > 0)
        printf("  => connected to server %s:%d\n", HOSTNAME, ntohs(port));

    freeaddrinfo(res);

    return sock;
}
/**
 * send message to socket
 */
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
    printf("  => sent %ld bytes: '%s'\n", m, msg);
}

/**
 * read from socket into buffer (also output to stdout)
 * returns read bytes
 */
ssize_t get(int sock, char *buf)
{
    ssize_t n;

    if ((n = read(sock, buf, BUFSIZE)) < 0)
    {
        perror("read");
        exit(EXIT_FAILURE);
    }

    write(STDOUT_FILENO, buf, n);

    return n;
}

/**
 * parse buf and return port from fixed position
 */
int extract_port(char buf[BUFSIZE], int pos)
{
    int port = 0;

    // parse by the digit without memcpy
    for (int decimal = 10000; decimal >= 1; decimal /= 10)
    {
        int distance_from_zero = buf[pos] - '0';

        if (verbose > 1)
            printf("  => port = %05d + ('%c' - '0' == %d) * %d\n",
                   port, buf[pos], distance_from_zero, decimal);

        port += distance_from_zero * decimal;
        pos++;
    }

    if (verbose > 0)
        // I'm using the same pos var for moving my carriage, so must adjust here
        printf("  => resolved port from buf at pos %d: %d\n", pos - 5, port);

    return port;
}

/**
 * connect to remote at `port` and send contents of file pointed to by `fd`
 */
void send_file(int port, int fd)
{
    char buf[BUFSIZE];
    ssize_t n;
    int sock = conn_remote(htons(port));

    // BUFSIZE chunks from file
    while ((n = read(fd, buf, BUFSIZE)) > 0)
    {
        // and send to receiver
        if (write(sock, buf, n) != n)
        {
            // abort on error
            perror("failed to send file");
            exit(EXIT_FAILURE);
        }
    }

    // terminate connection to receiver
    close(sock);
}

void handle_F_line(char buf[BUFSIZE], int pos, int conn)
{
    // buf ends with 5 digits for the port number + LF
    int port = extract_port(buf, pos - 6);

    // starts at pos 2 and ends 7 before the end
    int filepath_len = pos - 2 - 7;

    char filepath[filepath_len];
    memcpy(filepath, buf + 2, filepath_len);

    // making it a valid C string
    filepath[filepath_len] = '\0';

    if (verbose > 0)
        printf("  => resolved filename from msg: '%s'\n", filepath);

    int fd;

    if ((fd = open(filepath, O_RDONLY)) < 0)
    {
        perror("open failed");
        exit(EXIT_FAILURE);
    }

    struct stat statbuf;
    int rc = fstat(fd, &statbuf);
    if (rc < 0)
    {
        perror("fstat failed");
        exit(EXIT_FAILURE);
    }

    // convert size to string
    char sizebuf[32];
    snprintf(sizebuf, sizeof(sizebuf), "%ld", (long)statbuf.st_size);
    put(conn, sizebuf);

    send_file(port, fd);

    // close file descriptor pointing to the file
    close(fd);
}

int process_line(char buf[BUFSIZE], int *pos, int sock)
{
    // debugging #320
    if (buf[0] != '#' && verbose > 0)
        printf("  => processing this line:\n");

    // show me the line in any case
    write(STDOUT_FILENO, buf, *pos);

    // check 1st char on line
    switch (toupper((unsigned char)buf[0]))
    {
    case 'E':
        // convert line to lowercase char-by-char
        for (int i = 0; i < *pos; i++)
            buf[i] = tolower((unsigned char)buf[i]);

        if (verbose > 0)
        {
            printf("  => sending back the below line:\n");
            write(STDOUT_FILENO, buf, *pos);
        }

        if (write(sock, buf, *pos) != *pos)
        {
            perror("failed to send echo back");
            exit(EXIT_FAILURE);
        }

        break;

    case 'C':
        put(sock, "tkt21026-extra.c");
        break;

    case 'F':
        handle_F_line(buf, *pos, sock);
        break;

    case 'A':
        return 2;

    case 'Q':
        return 1;

    // comments and unexpected inputs
    default:
        break;
    }

    // reset carriage in any case
    *pos = 0;

    return 0;
}

void serve_client(int sock)
{
    ssize_t n;
    char c;
    int pos = 0;
    char buf[BUFSIZE];
    int res, accepted = 0;

    while ((n = read(sock, &c, 1)) > 0)
    {
        buf[pos++] = c;

        if (c == '\n')
        {
            if (res = process_line(buf, &pos, sock))
            {
                // A lines return 2
                if (res > 1)
                    accepted = 1;

                // but stopping on Q lines only
                if (res == 1)
                    break;
            }
        }
    }

    if (n < 0)
    {
        perror("read from server");
        exit(EXIT_FAILURE);
    }

    close(sock);

    if (verbose > 0)
        printf("  => childproc %d exiting\n", getpid());

    // exiting child process with my spec. exit code == 2 if accepted
    exit(accepted == 1 ? 2 : EXIT_SUCCESS);
}

volatile sig_atomic_t stop_server = 0;
void handle_sigchld(int sig)
{
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        // if child exited with my spec code 2, then concurrent server should be stopped
        if (WIFEXITED(status) && WEXITSTATUS(status) == 2)
        {
            stop_server = 1;
        }
    }
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

    // install sigchild handler in the main process
    struct sigaction sa;
    sa.sa_handler = handle_sigchld;
    sigemptyset(&sa.sa_mask);

    // disabling SA_RESTART
    sa.sa_flags = 0;
    sigaction(SIGCHLD, &sa, NULL);

    for (;;)
    {
        fd_conn = accept(fd_listen, NULL, NULL);
        if (fd_conn < 0)
        {
            if (stop_server == 1)
                break;

            // for other failures the server should just move on
            continue;
        }

        if (fork() == 0)
        {
            // child proc of server serves the connected client
            close(fd_listen);
            serve_client(fd_conn);
        }

        // parent closes connection on known-port
        close(fd_conn);
    }

    // terminate my server
    close(fd_listen);

    if (verbose > 0)
        printf("  => stopped concurrent server\n");
}

int main(int argc, char *argv[])
{

    // add more of these flags to increase verbosity
    // (last minute quality-of-life enchancement)
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0)
            verbose++;
    }

    char buf[BUFSIZE];
    int sock = conn_remote(0);

    get(sock, buf);
    put(sock, "hi");

    get(sock, buf);
    put(sock, "tjtoth");

    get(sock, buf);
    get(sock, buf);

    // retrieve PORT number from the last message
    get(sock, buf);
    int port = extract_port(buf, 55);

    get(sock, buf);

    server(port);

    // get leftover messages from server
    while (get(sock, buf))
    {
    };

    // terminate my original connection, by this time I already got your FIN
    close(sock);

    if (verbose > 0)
        printf("  => fn main ends\n");

    return EXIT_SUCCESS;
}
