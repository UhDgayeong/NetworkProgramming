/*
파일명 : file_server4.c
기  능 : FTP 명령어 구현 put, get, dir, ldir, !cmd
컴파일 : cc -o file_server4 file_server4.c
사용법 : file_server4 [port]
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

#define BUF_LEN 128
#define file_SERVER "0.0.0.0"
#define file_PORT "30000"

int main(int argc, char* argv[]) {
	struct sockaddr_in server_addr, client_addr;
	int server_fd, client_fd;			/* 소켓번호 */
	int len, msg_size;
	char buf[BUF_LEN + 1];
	char buf2[1000] = { 0 };
	unsigned int set = 1;
	char* ip_addr = file_SERVER, * port_no = file_PORT;

	if (argc == 2) {
		port_no = argv[1];
	}
#ifdef WIN32
	printf("Windows : ");
	init_winsock();
#else
	printf("Linux : ");
#endif
	/* 소켓 생성 */
	if ((server_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Server: Can't open stream socket.");
		exit(0);
	}
#ifdef WIN32
	main_socket = server_fd;
#endif

	printf("file_server4 waiting connection..\n");
	printf("server_fd = %d\n", server_fd);
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&set, sizeof(set));

	/* server_addr을 '\0'으로 초기화 */
	memset((char*)&server_addr, 0, sizeof(server_addr));
	/* server_addr 세팅 */
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(atoi(port_no));

	/* bind() 호출 */
	if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		printf("Server: Can't bind local address.\n");
		exit(0);
	}

	/* 소켓을 수동 대기모드로 세팅 */
	listen(server_fd, 5);

	/* iterative  file 서비스 수행 */
	printf("Server : waiting connection request.\n");
	len = sizeof(client_addr);

	while (1) {
		/* 연결요청을 기다림 */
		client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &len);
		if (client_fd < 0) {
			printf("Server: accept failed.\n");
			exit(0);
		}

		printf("Client connected from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		printf("client_fd = %d\n", client_fd);

		while (1) {
			char filename[BUF_LEN + 1] = { 0 };
			char command[BUF_LEN + 1] = { 0 };
			int filesize = 0;
			FILE* fp;
			int n;

			memset(buf, 0, BUF_LEN + 1);
			printf("\nWaiting client command\n");
			msg_size = recv(client_fd, buf, BUF_LEN, 0);
			if (msg_size <= 0) {
				printf("recv error\n");
				break;
			}
			buf[msg_size] = '\0'; // 문자열 끝에 NULL를 추가하기 위함
			sscanf(buf, "%s %s %d", command, filename, &filesize);
			printf("Received %d %s\n", msg_size, buf);

			if (strcmp(command, "put") == 0) {
				if ((fp = fopen(filename, "wb")) == NULL) {
					printf("file open error\n");
					exit(0);
				}
				n = recv(client_fd, buf2, filesize, 0);
				printf("Receiving %s %d bytes.\n", filename, filesize);
				if (fwrite(buf2, n, 1, fp) <= 0) {
					printf("fwrite error\n");
					break;
				}
				printf("\nFile %s %d bytes received.\n", filename, filesize);
			}

			else if (strcmp(command, "get") == 0) {
				if ((fp = fopen(filename, "rb")) == NULL) {
					printf("Can't open file %s\n", filename);
					exit(0);
				}
				fseek(fp, 0, 2);
				filesize = ftell(fp);
				rewind(fp);

				memset(buf, 0, BUF_LEN + 1);
				sprintf(buf, "%s %d", filename, filesize);
				if (send(client_fd, buf, BUF_LEN, 0) < 0) {
					printf("send error\n");
				}

				memset(buf2, 0, 1000);
				n = fread(buf2, 1, filesize, fp); //파일 읽어서
				buf2[filesize] = '\0';

				printf("Sending file %s %d bytes.\n", filename, filesize);
				if (send(client_fd, buf2, n, 0) <= 0) { //파일 전송
					printf("send error\n");
				}
				printf("File %s %d bytes sent.\n", filename, filesize);
			}

			else if (strcmp(command, "quit") == 0) {
				printf("Session finished..\n");
				break;
			}

			else if (strcmp(command, "dir") == 0) {
#ifdef WIN32
				fp = _popen("dir", "r");
#else
				fp = popen("ls -l", "r");
#endif
				memset(buf, 0, BUF_LEN + 1);
				printf("\nSending directory listing.\n");
				while (1) {
					n = fread(buf, 1, 128, fp); //읽어서
					if (n <= 0)
						break;
					send(client_fd, buf, 128, 0); //보내기
				}
				//memset(buf, 0, BUF_LEN + 1);
				strcpy(buf, "-EOF-");
				send(client_fd, buf, 128, 0);
			}

		}
#ifdef WIN32
		closesocket(client_fd);
#else
		close(client_fd);
#endif

	}
#ifdef WIN32
	closesocket(server_fd);
#else
	close(server_fd);
#endif	
	return(0);
}