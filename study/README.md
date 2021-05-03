## webserv 공부 참고 자료

1. [열혈강의 TCP/IP 소켓 프로그래밍 YouTube](https://www.youtube.com/watch?v=zueHweOVc38&list=PL7mmuO705dG265DxvC-oZgKViaRGt2-eS&index=3)  

C언어로 간단한 소켓 통신을 구현해본다. blocking, non-blocking 개념과 select 함수, HTTP 규격 등을 소개해줘서 webserv 구현에 필수적인 개념들을 간단하게 알 수 있다.   
#1 ~ #4, #11 ~ #14, #25, #26, #35, #36 만 들으면 됨.   

2. [How to build a simple HTTP server](https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa)

It's worth reading.

3. 책 《HTTP 완벽가이드》  

3장 HTTP 메시지  
5장 웹서버  
8장 통합점: 게이트웨이, 터널, 릴레이

> 이 책은 구성이 좋고 설명이 잘 되어 있어 이해하기에 좋은데, RFC 2616 기준이라 구현할 때는 주의해야함.  

4. RFC 공식문서  
> RFC란?  

IETF에서 제공, 관리하는 문서로, 인터넷 개발에 있어서 필요한 기술, 연구 결과, 절차 등을 기술해놓은 메모다. 거의 모든 인터넷 표준은 항상 RFC로 문서화가 되어있으며, 인터넷 개발에 관련된 기술을 연구하거나 알고있는 사람은 누구나 RFC 문서를 작성할 수 있다. RFC 문서가 필요한 단계를 통과하게 되면 IETF에서는 문서에 번호를 붙여주게 되는데, RFC ****의 형식으로 번호가 순서대로 부여된다. 일단 일련 번호를 부여 받고 출판되면, RFC는 절대 폐지되거나 수정되지 않는다. 만약 어떤 RFC 문서가 수정이 필요하다면, 저자는 수정된 문서를 다른 RFC 문서로 다시 출판해야 한다. HTTP/1.1 은 원래 RFC 2616 이었는데 2014년에 RFC 7230 ~ RFC 7235 으로 대체되었다.

+ https://tools.ietf.org/html/rfc7230
+ https://tools.ietf.org/html/rfc7231
+ https://tools.ietf.org/html/rfc7232
+ https://tools.ietf.org/html/rfc7233
+ https://tools.ietf.org/html/rfc7234
+ https://tools.ietf.org/html/rfc7235

5. CGI (Common Gateway Interface)
웹 상에서 주고 받는 리소스가 점점 복잡해짐에 따라 HTTP 프로토콜만으로는 모든 요청을 처리할 수 없게 되었다.  
이에 따라 웹서버와 외부 어플리케이션이 게이트웨이를 통해 통신할 수 있게 하기 위해 CGI가 등장하였다. CGI는 HTTP 요청에 따라 프로그램을 실행하고, 프로그램의 출력을 수집하고, HTTP 응답으로 회신할 수 있게 웹 서버가 사용하는 표준화된 인터페이스 집합이다.  
ex) `GET /query-db.cgi?newproducts HTTP/1.1` -> 이런 요청이 들어오면 외부 어플리케이션을 통해 요청 처리

+ https://tools.ietf.org/html/rfc3875
+ https://www.tutorialspoint.com/cplusplus/cpp_web_programming.htm