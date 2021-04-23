### fcntl 함수에 대하여
fcntl 시스템호출은 이미 열려있는 파일(fd)의 특성 제어를 위해서 사용된다.

* fcntl 함수 원형
```
#include <unistd.h>
#include <fcntl.h>

int fcntl(int fd, int cmd);
int fcntl(int fd, int cmd, long arg);
int fcntl(int fd, int cmd, struct flock *lock);
```
* fd에 대한 특성은 cmd에 의해 제어된다.

* webserv 코드 내 fcntl 사용
```
fcntl(_fd, F_SETFL, O_NONBLOCK);
```
* F_SETFL : arg 에 지정된 값으로 파일지정자 fd 의 플래그를 재 설정한다. 현재는 단지 O_APPEND, O_NONBLOCK, O_ASYNC 만을 설정할수 있다. 다른 플래그들 (O_WRONLY 와 같은) 은 영향을 받지 않는다.
* 우리 과제에선, 클라이언트와 서버 사이의 I/O는 하나의 select만 이용해서 non-blocking 이어야 한다는 조건이 있으므로, fcntl 함수를 이용해 fd를 non-blocking으로 변경해준다.
