#include <stdio.h>
#include <string.h>

int main(){
	char buffer[100] = "strcmp hello hihi";
	char *token;
	char *str[3];
	int idx = 0;

	token = strtok(buffer, " "); //공백으로 자름
	while(token != NULL){ //널값이 될때까지 돌림
		str[idx] = token; //포인터배열에 토큰을 저장
		printf("str[%d] = %s\n", idx, str[idx]); //출력
		idx++; //인덱스 증가
		token = strtok(NULL, " "); //다음토큰을 변수에 저장
	}
	if(idx < 3)
		strcpy(buffer, "문자열 비교를 위래서는 두 문자열이 필요합니다.");
	else if(!strcmp(str[1], str[2])) //같은 문자열이면
		sprintf(buffer, "%s와 %s는 같은 문자열입니다", str[1], str[2]);
	else
		sprintf(buffer, "%s와 %s는 다른 문자열입니다", str[1], str[2]);

	printf("%s\n", buffer);
}
