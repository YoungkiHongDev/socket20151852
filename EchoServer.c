#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>

#define PORT 10000
#define BUFSIZE 100

char Buffer[BUFSIZE] = "Hi, I'm Server";
char rcvBuffer[BUFSIZE];
char macroBuffer1[BUFSIZE] = "안녕하세요 만나서 반가워요";
char macroBuffer2[BUFSIZE] = "내 이름은 홍영기야";
char macroBuffer3[BUFSIZE] = "나는 24살이야";
char tempBuffer[BUFSIZE];
char cmpBuffer[BUFSIZE];
char *ptr;

int main(){
	int c_socket, s_socket;
	struct sockaddr_in s_addr, c_addr;
	int len;
	int n;
	int slen;
	int scmp;

	// 1. 서버 소켓 생성
	//서버 소켓 = 클라이언트의 접속 요청을 처리(허용)해 주기 위한 소켓
	s_socket = socket(PF_INET, SOCK_STREAM, 0); //TCP/IP 통신을 위한 서버 소켓 생성
	
	//2. 서버 소켓 주소 설정
	memset(&s_addr, 0, sizeof(s_addr)); //s_addr의 값을 모두 0으로  초기화
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY); //IP 주소 설정
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(PORT);

	//3. 서버 소켓바인딩
	if(bind(s_socket,(struct sockaddr *) &s_addr, sizeof(s_addr)) == -1){ 
		//바인딩 작업 실패 시, Cannot Bind 메시지 출력 후 프로그램 종료
		printf("Cannot Bind\n");
		return -1;
	}
	
	//4.listen() 함수 실행
	if(listen(s_socket, 5) == -1){
		printf("listen Fail\n");
		return -1;
	}

	//5. 클라이언트 요청 처리
	// 요청을 허용한 후, Hello World 메세지를 전송함
	while(1){ //무한 루프
		len = sizeof(c_addr);
		printf("클라이언트 접속을 기다리는 중....\n");
		c_socket = accept(s_socket, (struct sockaddr *)&c_addr, &len); 
		//클라이언트의 요청이 오면 허용(accept)해 주고, 해당 클라이언트와 통신할 수 있도록 클라이언트 소켓(c_socket)을 반환함.

		printf("Client is connected\n");
		printf("클라이언트 접속 허용\n");
		while(1){
			n = read(c_socket, rcvBuffer, sizeof(rcvBuffer));
			printf("rcvBuffer: %s\n", rcvBuffer);

			if(strncasecmp(rcvBuffer, "quit", 4) == 0 || strncasecmp(rcvBuffer, "kill server", 11) == 0)
				break;

			if(strncasecmp(rcvBuffer, "안녕하세요", 10) == 0) //5-1 클라이언트가 "안녕하세요" 라고 메세지를 전송할 경우
				memcpy(rcvBuffer, macroBuffer1, sizeof(macroBuffer1)); //rcvBuffer에 "안녕하세요 만나서 반가워요" 복사

			if(strncasecmp(rcvBuffer, "이름이 머야?", 12) == 0) //5-2 클라이언트가 "이름이 머야?" 라고 메세지를 전송할 경우
				memcpy(rcvBuffer, macroBuffer2, sizeof(macroBuffer2)); //rcvBuffer에 "내 이름은 홍영기야" 복사

			if(strncasecmp(rcvBuffer, "몇 살이야?", 10) == 0) //5-2 클라이언트가 "몇 살이야?" 라고 메세지를 전송할 경우
				memcpy(rcvBuffer, macroBuffer3, sizeof(macroBuffer3)); //rcvBuffer에 "나는 24살이야" 복사

			if(strncasecmp(rcvBuffer, "strlen", 6) == 0){ //5-3 클라이언트가 "strlen <문자열>"라고 메세지를 전송할 경우
				ptr = strtok(rcvBuffer, " "); //strtok로 전송받은 문자열을 자르기
				strcpy(tempBuffer, strtok(NULL, " ")); //char 배열에 <문자열> 복사
				slen = strlen(tempBuffer) - 1; //문자열의 길이값을 int형 변수에 저장, 메세지를 fgets()로 입력받아서 생긴 개행문자 '\n'값 - 1
				sprintf(rcvBuffer, "%d", slen); //sprintf로 int형 문자열의 길이값을 문자열로 변환하여 rcvBuffer에 저장
			}

			if(strncasecmp(rcvBuffer, "strcmp", 6) == 0){ //5-3 클라이언트가 "strcmp <문자열1> <문자열2>"라고 메세지를 전송할 경우
				ptr = strtok(rcvBuffer, " "); //strtok로 전송받은 문자열을 자르기
				strcpy(tempBuffer, strtok(NULL, " ")); //char 배열에 <문자열1> 복사
				strcpy(cmpBuffer, strtok(NULL, " ")); //char 배열에 <문자열2> 복사
				ptr = strchr(cmpBuffer, '\n'); //메세지를 fgets()로 입력받아서 생긴 개행문자 '\n'검색
				*ptr = '\0'; //개행문자 '\n' => '\0'null로 덮어쓰기
				scmp = strcmp(tempBuffer, cmpBuffer); //문자열의 비교값을 int형 변수에 저장
				sprintf(rcvBuffer, "%d", scmp); //sprintf로 int형 문자열의 비교값을 문자열로 변환하여 rcvBuffer에 저장
			}

			write(c_socket, rcvBuffer, strlen(rcvBuffer)); //클라이언트에게 buffer의 내용을 전송함
			memset(rcvBuffer, '\0', 100); //문자열 복사 후 남은 쓰레기값의 처리를 위한 memset, 배열의 모든 자리를 널값으로 초기화
		}
		close(c_socket);
		if(strncasecmp(rcvBuffer, "kill server", 11) == 0)
			break;
	}
	close(s_socket);
	return 0;
}
