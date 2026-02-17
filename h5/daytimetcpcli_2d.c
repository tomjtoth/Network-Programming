#include "unp.h"

#define PORT 50000

int main(int argc, char **argv)
{
	int sockfd, n, conn_num;
	char recvline[MAXLINE + 1];
	struct sockaddr_in servaddr, localaddr;
	socklen_t len;

	if (argc != 3)
		err_quit("usage: a.out <IPaddress> <conn-num>");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT); /* daytime server */
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
		err_quit("inet_pton error for %s", argv[1]);

	conn_num = atoi(argv[2]);

	while (conn_num > 0)
	{
		if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			err_sys("socket error");

		if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) < 0)
			err_sys("connect error");

		// Get local port assigned by kernel
		len = sizeof(localaddr);
		if (getsockname(sockfd, (SA *)&localaddr, &len) < 0)
			err_sys("getsockname error");

		printf("Connection %d: local port %d\n",
			   conn_num,
			   ntohs(localaddr.sin_port));

		while ((n = read(sockfd, recvline, MAXLINE)) > 0)
		{
			recvline[n] = 0; /* null terminate */
			if (fputs(recvline, stdout) == EOF)
				err_sys("fputs error");
		}
		if (n < 0)
			err_sys("read error");

		close(sockfd);
		conn_num--;
	}

	exit(0);
}
