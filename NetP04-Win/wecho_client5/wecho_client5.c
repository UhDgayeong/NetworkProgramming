/*
 ���ϸ� : echo_client.c
 ��  �� : echo ���񽺸� �䱸�ϴ� TCP(������) Ŭ���̾�Ʈ
 ������ : cc -o echo_client echo_client.c
 ���� : echo_client [host] [port]
*/
#include <winsock.h>
#include <signal.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>

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

#define ECHO_SERVER "127.0.0.1"
#define ECHO_PORT "30000"
#define BUF_LEN 128

int main(int argc, char* argv[]) {
	int s, n, i, len_in, len_out;
	struct sockaddr_in server_addr;
	char* ip_addr = ECHO_SERVER, * port_no = ECHO_PORT;
	char buf[BUF_LEN + 1] = { 0 };
	char buf2[BUF_LEN + 1] = { 0 };

	if (argc == 3) {
		ip_addr = argv[1];
		port_no = argv[2];
	}

	init_winsock();

	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		printf("can't create socket\n");
		exit(0);
	}
	main_socket = s;


	/* echo ������ �����ּ� ����ü �ۼ� */
	memset((char*)&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip_addr);
	server_addr.sin_port = htons(atoi(port_no));

	/* �����û */
	printf("Connecting %s %s\n", ip_addr, port_no);

	if (connect(s, (struct sockaddr*)&server_addr,
		sizeof(server_addr)) < 0) {
		printf("can't connect.\n");
		exit(0);
	}
	//Welcome to Server!!
	n = recv(s, buf, BUF_LEN, 0);
	buf[n] = '\0'; // ���ڿ� ���� NULL �߰�
	printf("Received %d bytes : %s\n", n, buf);

	char id[20] = { 0 };
	char password[20] = { 0 };
	while (1) { //�α���
		char str[BUF_LEN] = { 0 };
		char code[4] = { 0 };

		printf("ID : ");
		scanf("%s", id);
		printf("Password : ");
		scanf("%s", password);

		sprintf(buf, "%s %s", id, password);
		send(s, buf, BUF_LEN, 0);

		n = recv(s, buf, BUF_LEN, 0);

		for (i = 0; i < BUF_LEN; i++) {
			str[i] = buf[i + 3];
		}

		for (i = 0; i < 3; i++) {
			code[i] = buf[i];
		}
		code[3] = '\0';
		if (strcmp(code, "200") == 0) {
			printf("%s\n", str);
			break;
		}
		printf("%s\n", buf);
	}

	while (1) {
		int menu;
		printf("*** ��/�ҹ��� ��ȯ �޴��Դϴ�. ***\n");
		printf(" (1) ��� �빮�� ��ȯ\n");
		printf(" (2) ��� �ҹ��� ��ȯ\n");
		printf(" (3) ��>�� ��>�� ��ȯ\n");
		printf(" (4) ����\n");
		/* Ű���� �Է��� ���� */
		printf("�����ϼ��� : ");
		scanf("%d", &menu);

		if (menu == 4) {
			len_out = BUF_LEN;
			sprintf(buf2, "%d", menu);
			send(s, buf2, len_out, 0);
			break;
		}
		printf("Input string : ");
		fgets(buf, BUF_LEN, stdin);
		if (fgets(buf, BUF_LEN, stdin)) { // gets(buf);
			len_out = BUF_LEN;
			buf[BUF_LEN] = '\0';
		}
		else {
			printf("fgets error\n");
			exit(0);
		}

		/* echo ������ �޽��� �۽� */
		//printf("Sending len=%d : %s", len_out, buf);
		sprintf(buf2, "%d %s", menu, buf);
		if (send(s, buf2, len_out, 0) < 0) {
			printf("send error\n");
			exit(0);
		}
		//if (strcmp(buf2, "exit\n") == 0)
		//	break;
		if ((n = recv(s, buf, BUF_LEN, 0)) < 0) {
			printf("recv error\n");
			exit(0);
		}
		buf[n] = '\0'; // ���ڿ� ���� NULL �߰�
		printf("Received %d bytes : %s\n", n, buf);
	}
	closesocket(s);
	return(0);
}
