/*
���ϸ� : wecho_server.c
��  �� : echo ���񽺸� �����ϴ� ����
������ : cc -o wecho_server wecho_server.c
���� : wecho_server [port]
*/
#include <winsock.h>
#include <signal.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

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

	// winsock ����� ���� �ʼ�����
	signal(SIGINT, exit_callback);
	sversion = MAKEWORD(1, 1);
	WSAStartup(sversion, &wsadata);
}

#define BUF_LEN 128
#define ECHO_SERVER "0.0.0.0"
#define ECHO_PORT "30000"

int main(int argc, char* argv[]) {
	struct sockaddr_in server_addr, client_addr;
	int server_fd, client_fd;			/* ���Ϲ�ȣ */
	int len, msg_size, code;
	char buf[BUF_LEN + 1];
	unsigned int set = 1;
	char* ip_addr = ECHO_SERVER, * port_no = ECHO_PORT;
	char right_id[20] = { "hansung" };
	char right_password[20] = { "computer" };
	char login_id[20] = { 0 };


	if (argc == 2) {
		port_no = argv[1];
	}

	init_winsock();

	/* ���� ���� */
	if ((server_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Server: Can't open stream socket.");
		exit(0);
	}
	main_socket = server_fd;

	printf("echo_server5 waiting connection..\n");
	printf("server_fd = %d\n", server_fd);
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&set, sizeof(set));

	/* server_addr�� '\0'���� �ʱ�ȭ */
	memset((char*)&server_addr, 0, sizeof(server_addr));
	/* server_addr ���� */
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(atoi(port_no));

	/* bind() ȣ�� */
	if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		printf("Server: Can't bind local address.\n");
		exit(0);
	}

	/* ������ ���� ������ ���� */
	listen(server_fd, 5);

	/* iterative  echo ���� ���� */
	printf("Server : waiting connection request.\n");
	len = sizeof(client_addr);

	while (1) {
		char welcome[BUF_LEN + 1];
		sprintf(welcome, "%s", "Welcome to Server!!");

		/* �����û�� ��ٸ� */
		client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &len);
		if (client_fd < 0) {
			printf("Server: accept failed.\n");
			exit(0);
		}

		printf("Client connected from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		printf("client_fd = %d\n", client_fd);
		//Welcome to Server!!
		msg_size = send(client_fd, welcome, BUF_LEN, 0);
		printf("Sending len=%d : %s\n", msg_size, welcome);
		
		while (1) { //�α���. "id password" �����
			char id[20] = { 0 };
			char password[20] = { 0 };
			char* p;
			int i, k;

			msg_size = recv(client_fd, buf, BUF_LEN, 0);
			buf[msg_size] = '\0';

			p = buf;
			for (i = 0; i < 20; i++) {
				if (*p != ' ') {
					id[i] = *p++;
				}
				else {
					p++;
					break;
				}
			}
			for (k = 0; k < 20; k++) {
				if (*p == '\0')
					break;
				password[k] = *p++;
			}
			printf("Received id=%s pass=%s\n", id, password);
			
			if (strcmp(right_id, id) == 0) { //id ����
				if (strcmp(right_password, password) == 0) { //�α��� ����
					code = 200;
					sprintf(buf, "%d %s %s!!", code, "Welcome", right_id);
					msg_size = send(client_fd, buf, BUF_LEN, 0);
					printf("Sending len=%d : %s\n", msg_size, buf);
					strcpy(login_id, id);
					break;
				}
				else { //��� Ʋ��
					code = 402;
					sprintf(buf, "%d %s", code, "Invalid Password");
					msg_size = send(client_fd, buf, BUF_LEN, 0);
					printf("Sending len=%d : %s\n", msg_size, buf);
				}
			}
			else { //id Ʋ��
				code = 401;
				sprintf(buf, "%d %s", code, "Invalid ID");
				msg_size = send(client_fd, buf, BUF_LEN, 0);
				printf("Sending len=%d : %s\n", msg_size, buf);
			}
		}


		while (1) {
			msg_size = recv(client_fd, buf, BUF_LEN, 0);
			if (msg_size <= 0) {
				printf("recv error\n");
				break;
			}
			buf[msg_size] = '\0'; // ���ڿ� ���� NULL�� �߰��ϱ� ����

			char menu = buf[0];
			char str[BUF_LEN + 1];
			for (int i = 2; i < BUF_LEN + 1; i++) {
				str[i - 2] = buf[i];
			}
			if (menu == '4') {
				printf("Received len=%d : %c\n", msg_size, menu);
				printf("Session finished for %s.\n", login_id);
				break;
			}
			else {
				printf("Received len=%d : %c %s", msg_size, menu, str);
			}

			char* s = str;

			if (menu == '1') { // ��� ���ڿ��� �빮�ڷ� ��ȯ
				while (*s) {
					*s = toupper(*s);
					s++;
				}
			}
			else if (menu == '2') { // ��� ���ڿ��� �ҹ��ڷ� ��ȯ
				while (*s) {
					*s = tolower(*s);
					s++;
				}
			}
			else if (menu == '3') { // ��/�� ���� ��ȣ ��ȯ
				while (*s) {
					if (islower(*s))
						*s = toupper(*s);
					else
						*s = tolower(*s);
					s++;
				}
			}

			msg_size = send(client_fd, str, msg_size, 0);
			if (msg_size <= 0) {
				printf("send error\n");
				break;
			}
			printf("Sending len=%d : %s", msg_size, str);
		}
		closesocket(client_fd); // close(client_fd); �׷��� ���� �� ���ο� client�� �޴´� 
	}
	closesocket(server_fd); // close(client_fd);
	return(0);
}
