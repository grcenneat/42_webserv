#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 8090
#define MAX 512

int	numClient=0;
int	clientSock[MAX];

int	getMaxfd(int);

int main( )
{
	int		connSock, listenSock;
	struct sockaddr_in s_addr, c_addr;
	socklen_t len;
	int		i, n;
	char	rcvBuffer[BUFSIZ];
	int		maxfd;

	fd_set read_fds;

	listenSock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&s_addr, 0, sizeof(s_addr));
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(PORT);

	if(bind(listenSock, (struct sockaddr *) &s_addr, sizeof(s_addr)) == -1) {
		printf("Can not Bind\n");
		return -1;
	}

	if(listen(listenSock, 5) == -1) {
		printf("listen Fail\n");
		return -1;
	}

	while(1) {
		maxfd = getMaxfd(listenSock) + 1;

		FD_ZERO(&read_fds);
		FD_SET(listenSock, &read_fds);

		for(i = 0; i < numClient; i++)
			FD_SET(clientSock[i], &read_fds);

		if(select(maxfd, &read_fds, NULL, NULL, NULL) < 0 ) {
			printf("select error\n");
			exit(-1);
		}

		printf("waiting---\n");
		//listenSock 인 3번 FD에 변화가 생기면, 연결요청이 들어온 것
		if(FD_ISSET(listenSock, &read_fds)) {
			//연결이 수립되면 connSock은 4번 FD가 된다.
			connSock=accept(listenSock, (struct sockaddr *) &c_addr, &len);
			clientSock[numClient++] = connSock;
		}
		// 이제 select는 3번 FD에선 새로운 Client 연결 요청을 감시하고, 4번 FD에선 데이터가 수신되는지 감시하게 된다.

		for(i = 0; i < numClient; i++) {
			// 몇번 client FD에서 데이터가 온건지 체크
			if(FD_ISSET(clientSock[i], &read_fds) ) {
				if((n=read(clientSock[i], rcvBuffer,sizeof(rcvBuffer)))!=0){
					rcvBuffer[n]='\0';
					//에코!
					printf("[client Number: %d receive - [%s]\n", clientSock[i], rcvBuffer);
					write(clientSock[i], rcvBuffer, n);
					printf("send	- [%s]\n", rcvBuffer);
				} else {
					printf("EOF\n");
					close(clientSock[i]);
					if(i != numClient-1)
						clientSock[i] = clientSock[numClient-1];
					numClient--;
					continue;
				}
			}
		}
	}
}

int
getMaxfd(int n)
{
	int	max = n;
	int	i;

	// clientSock [4][5][][][] ... 이면 Maxfd는 6이 된다
	for(i = 0; i < numClient; i++) {
		if(clientSock[i] > max)
			max = clientSock[i];
	}

	return max;
}