//talk_client_select.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX 512

char	quit[ ] = "exit";
int	numClient=0;

int main(int argc, char *argv[ ])
{
	int	connSock;
	struct sockaddr_in s_addr;
	int	len, i, n;
	char	rcvBuffer[BUFSIZ], sbuf[BUFSIZ];
	int	maxfd;

	if(argc < 3) {
		printf("Usage: talk_clinet IP_address port_num");
		return -1;
	}

	fd_set read_fds;

	connSock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&s_addr, 0, sizeof(s_addr));
	s_addr.sin_addr.s_addr = inet_addr(argv[1]);
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(atoi(argv[2]));

	if(connect(connSock, (struct sockaddr *) &s_addr, sizeof(s_addr)) == -1) {
		printf("Can not connect\n");
		return -1;
	}

	while(1) {
		maxfd = connSock + 1;
		FD_ZERO(&read_fds);
		FD_SET(0, &read_fds);
		FD_SET(connSock, &read_fds);

		if(select(maxfd, &read_fds, NULL, NULL, NULL) < 0) {
			printf("select error\n");
			exit(-1);
		}

		printf("client waiting---\n");
		if(FD_ISSET(0, &read_fds)) {
			if((n = read(0, sbuf, BUFSIZ)) > 0) {
				if (write(connSock, sbuf, n) != n) {
					printf("Talk Server fail in sending\n");
				}
				if(strncmp(sbuf, quit, 4) == 0) {
					break;
				}
			}
		}
		if(FD_ISSET(connSock, &read_fds)) {
			if((n=read(connSock, rcvBuffer,sizeof(rcvBuffer))) !=0) {
					rcvBuffer[n - 1]='\0';
					printf("receive[%s]\n", rcvBuffer);
			}
			if(strncmp(rcvBuffer, quit, 4) == 0) {
				break;
			}
		}
	}
}