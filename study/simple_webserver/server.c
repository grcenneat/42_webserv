#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <string.h>
#include <unistd.h>

#define PORT 9000

char	buffer[BUFSIZ] = "hello, world";

int main()
{
	int			c_socket, s_socket;
	struct sockaddr_in s_addr, c_addr;
	socklen_t	len;
	int			n;

	// 1) 소켓을 생성
	// IPv4 프로토콜을 사용하는 TCP 기반의 연결 수립
	s_socket = socket(PF_INET, SOCK_STREAM, 0);	

	// 2) 연결 요청을 수신할 주소 설정
	memset(&s_addr, 0, sizeof(s_addr));
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(PORT);
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	// 3) 소켓을 포트에 연결
	if(bind(s_socket, (struct sockaddr *) &s_addr, sizeof(s_addr)) == -1) {
		printf("Can not Bind\n");
		return -1;
	}
	// 4) 커널에 연결 요청
	if(listen(s_socket, 5) == -1) {
		printf("listen Fail\n");
		return -1;
	}
	//while문을 돌면서 클라이언트의 연결 요청을 기다린다
	while(1) {
		len = sizeof(c_addr);
		// 5) 클라이언트의 연결 요청을 수신
		// s_socket : 연결 요청이 오는지 기다리고 있는 듣기 소켓(서버쪽)
		// c_socket : 실제 연결이 이루어지는 소켓 (클라이언트쪽)
		c_socket = accept(s_socket, (struct sockaddr *) &c_addr, &len);
		n = strlen(buffer);
		// 6) 클라이언트 요청 서비스 제공
		write(c_socket, buffer, n);
		// 7) 클라이언트와 연결 종료
		close(c_socket);
	}
	// 8) 서버 종료
	close(s_socket);
}
