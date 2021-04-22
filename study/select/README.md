### I/O Multiplexing과 Select 함수
Webserv 과제는 단일 프로세스 Multiplexing 서버를 구현하는 과제이다. I/O Multiplexing은 한 프로세스 내에서 여러 non-blocking socket(I/O streams)을 검사하여, 활성화된 socket의 데이터를 처리하는 개념이다.
select() 함수를 사용하면 이게 가능한데, 한 곳에 모아놓은 여러 개의 파일 디스크립터(소켓)를 동시에 관찰할 수 있다.

> 만약 select() 함수를 사용하지 않고, 여러 클라이언트의 연결 요청을 받아야 한다면 ?  
> 하나의 서버에 여러 클라이언트의 요청이 들어올 때, 각 연결마다 fork()로 프로세스를 생성해서 처리해야 한다. 이 방법은 복잡하고, 자원 소모도 심하다.  

커널은 지정된 시간 동안 여러 사건 중 하나 이상이 발생할 때 까지 대기하고, 사건이 발생하면 반환된다.  
시간을 지정하지 않으면 변화가 발생할 때 까지 blocking 상태가 되어 대기하고, 시간을 지정하면 지정된 시간 동안 아무 변화가 없어도 시간이 되면 반환된다.

```c
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int select(int maxfd, fd_set *readfds, fd_set *writefds, ft_set *exceptfds, struct timeval *timeout);
```
**return value**  
-1(에러), 0(타임아웃), 0보다 큰 경우: 변경된 파일 디스크립터의 수

**maxfd** : 검사 대상이 되는 파일 디스크립터의 최대 개수  
**readfds** : 수신할 데이터가 있는지를 검사할 소켓(fd)들의 목록을 담는 구조체. 입력 스트림에 변화가 발생했다는 것은 수신할 데이터가 존재한다는 뜻이다.  
**writefds** : blocking되지 않고 바로 자료를 전송할 수 있는지 검사할 소켓(fd)들의 목록을 담는 구조체.  
**exceptfds** : "예외가 발생했는지" 확인하고자 하는 소켓들의 정보.  
**timeout**: 함수 호출 후, 무한대기 상태에 빠지지 않도록 대기 시간을 지정한다.  

### fd_set
fd_set은 file descriptor 를 저장하는 구조체인데, 그냥 배열로 생각하면 편하다.

| fd = 0 | 1    | 2    | 3    | 4    | ...  | Maxfd - 1 |
| ------ | ---- | ---- | ---- | ---- | ---- | --------- |
| 0      | 0    | 0    | 0    | 0    |      | 0         |

select() 함수에서 해당 fd_set에 변화가 발생하면 변화가 감지된 FD에 해당하는 bit가 1로 바뀐다.  
이 비트 배열의 조작을 편리하게 하기 위해 몇 가지 매크로 함수가 정의되어 있다.

| 함수 선언                        | 기능                                                         |
| -------------------------------- | ------------------------------------------------------------ |
| FD_ZERO(fd_set *fdset);          | *fdset 의 모든 비트를 지운다                                 |
| FD_SET(int fd, fd_set *fdset);   | *fdset 중 소켓 fd에 해당하는 비트를 1로 한다                 |
| FD_CLR(int fd, fd_set *fdset);   | *fdset 중 소켓 fd에 해당하는 비트를 0으로 한다                 |
| FD_ISSET(int fd, fd_set *fdset); | *fdset 중 소켓 fd에 해당하는 비트가 설정(SET) 되어 있으면 양수인 fd 반환 |


### 예제 코드
> echo 프로그램은 클라이언트가 보낸 메시지를 그대로 돌려주는 프로그램이고,  
> talk 프로그램은 서버가 메시지를 보내면 클라이언트가 받고, 클라이언트가 메시지를 보내면 서버가 받는다.  

> talk 프로그램 실행방법
```shell
$ gcc -Wall -Werror -Wextra talk_server_select.c -o talk_server
$ gcc -Wall -Werror -Wextra talk_client_select.c -o client_server
./server 9200

```

```shell
# 다른 콘솔에서
./client_server 127.0.0.1 9200

```

> echo 프로그램  
`echo_sercer_select.c` 컴파일 후 서버 실행 후 클라이언트에서 테스트 하는 법  
`nc 127.0.0.1 8090` 으로 접속해볼 수 있음.   
shell 을 하나 더 열어서 `nc 127.0.0.1 8090` 으로 접속 후 에코테스트를 해보면 Client Number가 증가해 다중 접속이 잘 되고 있는 걸 확인할 수 있다.   
