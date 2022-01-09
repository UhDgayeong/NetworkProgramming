/*
 파일명 : file_client4.c
 기  능 : ftp 와 비슷하게 만들기. get, put, dir, quit 구현
 컴파일 : cc -o file_client4 file_client4.c
 사용법 : file_client4 [host IP] [port]
*/
#ifdef WIN32
#include <winsock.h>
#include <signal.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#else
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#ifdef WIN32
WSADATA wsadata;
int	main_socket;

void exit_callback(int sig)
{
	closesocket(main_socket);
	WSACleanup();
	exit(0);
}

void init_winsock()
{
	WORD sversion;
	u_long iMode = 1;

	// winsock 사용을 위해 필수적임
	signal(SIGINT, exit_callback);
	sversion = MAKEWORD(1, 1);
	WSAStartup(sversion, &wsadata);
}
#endif

#define ECHO_SERVER "127.0.0.1"
#define ECHO_PORT "30000"
#define BUF_LEN 128

int main(int argc, char* argv[]) {
	int s, n, len_in, len_out;
	struct sockaddr_in server_addr;
	char* ip_addr = ECHO_SERVER, * port_no = ECHO_PORT;
	char buf[BUF_LEN + 1] = { 0 };
	char buf2[1000] = { 0 };


	if (argc == 3) {
		ip_addr = argv[1];
		port_no = argv[2];
	}
#ifdef WIN32
	printf("Windows : ");
	init_winsock();
#else // Linux
	printf("Linux : ");
#endif 

	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		printf("can't create socket\n");
		exit(0);
	}
#ifdef WIN32
	main_socket = s;
#endif 



	/* echo 서버의 소켓주소 구조체 작성 */
	memset((char*)&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip_addr);
	server_addr.sin_port = htons(atoi(port_no));

	// 파일명 입력
	FILE* fp;
	char filename[BUF_LEN] = "data.txt"; // data file 예

	/* 연결요청 */
	printf("Connecting %s %s\n", ip_addr, port_no);

	if (connect(s, (struct sockaddr*)&server_addr,
		sizeof(server_addr)) < 0) {
		printf("can't connect.\n");
		exit(0);
	}

	while (1) {
		int n, filesize = 0;
		char command[BUF_LEN + 1] = { 0 };
		memset(filename, 0, BUF_LEN);

		printf("\nfile_client4> ");
		if (fgets(buf, BUF_LEN, stdin)) { //명령어 입력받기
			len_out = BUF_LEN;
			buf[BUF_LEN] = '\0';
		}
		else {
			printf("fgets error\n");
			exit(0);
		}

		sscanf(buf, "%s %s", command, filename);

		if (strcmp(command, "put") == 0) {
			if ((fp = fopen(filename, "rb")) == NULL) {
				printf("Can't open file %s\n", filename);
				break;
			}
			fseek(fp, 0, 2);
			filesize = ftell(fp);
			rewind(fp);

			sprintf(buf, "%s %s %d", command, filename, filesize);
			if (send(s, buf, len_out, 0) < 0) {
				printf("send error\n");
				break;
			}

			memset(buf2, 0, 1000);
			n = fread(buf2, 1, filesize, fp);
			buf2[filesize] = '\0';

			printf("Sending %s %d bytes.\n", filename, filesize);
			if (send(s, buf2, n, 0) <= 0) {
				printf("send error\n");
				break;
			}
			printf("File %s %d bytes transferred.\n", filename, filesize);
		}
		else if (strcmp(command, "get") == 0) {
			if (send(s, buf, len_out, 0) < 0) {
				printf("send error\n");
				break;
			}

			if (recv(s, buf, BUF_LEN, 0) < 0) {
				printf("recv error\n");
				break;
			}
			sscanf(buf, "%s %d", filename, &filesize);

			if ((fp = fopen(filename, "wb")) == NULL) {
				printf("file open error\n");
				break;
			}

			printf("Receiving %s %d bytes.\n", filename, filesize);
			memset(buf2, 0, 1000);
			if ((n = recv(s, buf2, filesize, 0)) < 0) { //파일 받아서
				printf("recv error\n");
				break;
			}
			printf("\nFile %s %d bytes received.\n", filename, filesize);


			if (fwrite(buf2, n, 1, fp) <= 0) { //파일 쓰기
				printf("fwrite error\n");
				break;
			}

		}
		else if (strcmp(command, "quit") == 0) {
			send(s, buf, len_out, 0);
			printf("Client end.\n");
			exit(0);
		}
		else if (strcmp(command, "dir") == 0) {
			if (send(s, buf, len_out, 0) < 0) { //dir을 send
				printf("send error\n");
				break;
			}
			
			while (1) {
				memset(buf, 0, BUF_LEN + 1);
				if (recv(s, buf, BUF_LEN, 0) < 0) { //dir내용 받기
					printf("recv error\n");
					break;
				}
				if (strncmp(buf, "-EOF-", 5) == 0) {
					break;
				}
				printf(buf);
			}
			printf("\n");
		}
		else if (strcmp(command, "ldir") == 0) {
			system("dir");
		}
		else if (command[0] == '!') {
			system(command + 1);
		}

	}
#ifdef WIN32
	closesocket(s);
#else
	close(s);
#endif
	return(0);
}
