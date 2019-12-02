#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
void *do_chat(void *); //채팅 메세지를 보내는 함수
int pushClient(int, int); //새로운 클라이언트가 접속했을 때 클라이언트 정보 추가
int popClient(int); //클라이언트가 종료했을 때 클라이언트 정보 삭제
pthread_t thread;
pthread_mutex_t mutex;
#define MAX_CLIENT 10
#define CHATDATA 1024
#define INVALID_SOCK -1
#define PORT 9000
#define NAMEDATA 100

char escape[ ] = "exit";
char greeting[ ] = "Welcome to chatting room\n";
char CODE200[ ] = "Sorry No More Connection\n";

struct User{
	int list_c;
	char nickname[NAMEDATA];
};

struct User users[MAX_CLIENT];

int main(int argc, char *argv[ ])
{
    int c_socket, s_socket;
    struct sockaddr_in s_addr, c_addr;
    int len;
    int i, j, n;
    int res;
    if(pthread_mutex_init(&mutex, NULL) != 0) {
        printf("Can not create mutex\n");
        return -1;
    }
    s_socket = socket(PF_INET, SOCK_STREAM, 0);
    memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(PORT);
    if(bind(s_socket, (struct sockaddr *)&s_addr, sizeof(s_addr)) == -1) {
        printf("Can not Bind\n");
        return -1;
    }
    if(listen(s_socket, MAX_CLIENT) == -1) {
        printf("listen Fail\n");
        return -1;
    }
    for(i = 0; i < MAX_CLIENT; i++)
        users[i].list_c = INVALID_SOCK;
    while(1) {
        len = sizeof(c_addr);
        c_socket = accept(s_socket, (struct sockaddr *) &c_addr, &len);
        res = pushClient(c_socket, s_socket);
        if(res < 0) { //MAX_CLIENT만큼 이미 클라이언트가 접속해 있다면,
            write(c_socket, CODE200, strlen(CODE200));
            close(c_socket);
        } else {
            write(c_socket, greeting, strlen(greeting));
            //pthread_create with do_chat function.
			 pthread_create(&thread, NULL, do_chat, (void *)&c_socket);
        }
    }
}
void *do_chat(void *arg)
{
    int c_socket = *((int *)arg);
    char chatData[CHATDATA];
    int i, n;
    while(1) {
        memset(chatData, 0, sizeof(chatData));
        if((n = read(c_socket, chatData, sizeof(chatData))) > 0){

              if(strstr(chatData, escape) != NULL) {
                  popClient(c_socket);
                  break;
				}
			 	else if(strstr(chatData,"/w ")!= NULL) {
					//귓속말할 경우
					char *token;
					char *whisper[3];
					token = strtok(chatData, "] ");
					whisper[0] = token; // 본인 닉네임 저장
					token = strtok(NULL, " ");
					token = strtok(NULL, " ");
					whisper[1] = token; // 상대 닉네임 저장
					token = strtok(NULL, "\0");
					whisper[2] = token; // 보낼 메세지 저장

					for(i=0; i < MAX_CLIENT; i++){
						if(!strcmp(whisper[1], users[i].nickname)){
							sprintf(chatData, "%s to %s] %s", whisper[0], whisper[1], whisper[2]);
							write(users[i].list_c, chatData, strlen(chatData));
							break;
						}
					}
				} else {
					//write chatData to all clients
					for(i=0; i < MAX_CLIENT; i++){
						if(users[i].list_c != INVALID_SOCK){
							write(users[i].list_c, chatData, strlen(chatData));
						}
					}
				}
        }
    }
}

int pushClient(int c_socket, int s_socket) {
	//ADD c_socket to list_c array.
    //return -1, if list_c is full.
    //return the index of list_c which c_socket is added.
	int i;
	for(i=0; i < MAX_CLIENT; i++){
		pthread_mutex_lock(&mutex);

		if(users[i].list_c == INVALID_SOCK){
			users[i].list_c = c_socket;
			read(c_socket, users[i].nickname, sizeof(users[i].nickname));
			pthread_mutex_unlock(&mutex);
			return i;
		}
		pthread_mutex_unlock(&mutex);
	}

	if(i == MAX_CLIENT)
		return -1;
}

int popClient(int c_socket){
    //REMOVE c_socket from list_c array.
	int i;
	for(i=0; i < MAX_CLIENT; i++){
		pthread_mutex_lock(&mutex);

		if(users[i].list_c == c_socket){
			users[i].list_c = INVALID_SOCK;
			strcpy(users[i].nickname, '\0');
			pthread_mutex_unlock(&mutex);
			return 0;
		}
		pthread_mutex_unlock(&mutex);
	}
	close(c_socket);
}

