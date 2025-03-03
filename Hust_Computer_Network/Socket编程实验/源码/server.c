
#include<stdio.h>
#include<Winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996)
#define bool int
#define true 1
#define false 0

bool isError(int returnData, int errorData, char* message);
void sendFile(SOCKET socket, char* filename);
char* getFileExtention(char* path);
void sendHead(char* path, SOCKET socket, char* filename);
bool ipLegalCheck(char* ip);
bool portLegalCheck(int port);

/*
int main()
{
	WSADATA wsaData;
	//fd_set rfds;	//���ڼ��socket�Ƿ������ݵ����ĵ��ļ�������
	//fd_set wfds;	//���ڼ��socket�Ƿ���Է��͵��ļ�������
	bool flag;	//���ڼ���Ƿ����
	bool first_connetion = true;

	printf("----------------------��ʼ��----------------------\n");
	flag = WSAStartup(0x0202, &wsaData);	//��ɳ�ʼ��
	//����ʼ���Ƿ�ɹ�
	if (flag) {
		printf("Winsock  startup failed with error!\n");
	}
	//���汾���Ƿ�֧��
	else if (wsaData.wVersion != 0x0202) {
		printf("Winsock version is not correct!\n");
	}
	else {
		printf("Winsock  startup Ok!\n");
	}


	printf("\n-------------����ip��ַ&�˿ں�&��Ŀ¼-------------\n");
	//����socket����ʹ��Internet��ַ�������ͣ�TCPЭ��
	SOCKET srvSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (srvSocket != INVALID_SOCKET)
		printf("Socket create Ok!\n\n");
	else
		printf("Socket create Error!\n\n");
	//����ip�Ͷ˿ں�
	int port = 0;
	char inAddr[20] = "";
	//�����һ������Ƿ�Ϸ�
	printf("������˿ںţ�");
	scanf("%d", &port);
	while (!portLegalCheck(port)) {
		printf("������Ϸ��Ķ˿ںţ�");
		scanf("%d", &port);
	}
	printf("������ip��ַ��");
	scanf("%s", inAddr);
	while (!ipLegalCheck(inAddr)) {
		printf("������Ϸ���ip��ַ��");
		scanf("%s", inAddr);
	}
	char filename[50] = "";
	printf("��������Ŀ¼��");
	scanf("%s", filename);


	struct sockaddr_in srvAddr;
	srvAddr.sin_family = AF_INET;//��socket��Э��һ��Ϊipv4
	srvAddr.sin_port = htons(port);//���˿ں�80����������Զ����Ĵ洢��ʽ�������ˣ�����С��
	srvAddr.sin_addr.s_addr = inet_addr(inAddr);//������ַ

	//��
	flag = bind(srvSocket, &srvAddr, sizeof(srvAddr));
	if (flag != SOCKET_ERROR)
		printf("Socket bind Ok!\n");
	else
		printf("Socket bind Error!\n");
	//�������ȴ����г���5
	flag = listen(srvSocket, 5);
	if (flag != SOCKET_ERROR)
		printf("Socket listen Ok!\n");
	else
		printf("Socket listen Error!\n");


	printf("\n-----------------��Ӧ�ͻ�������-----------------\n");
	struct sockaddr_in clientAddr;
	int addrLen = sizeof(clientAddr);
	while (true)
	{
		//�ȴ����Ӳ����ɻỰSOCKET
		SOCKET sessionSocket = accept(srvSocket, &clientAddr, &addrLen);
		if (sessionSocket != INVALID_SOCKET)
			printf("Socket listen one client request!\n");
		else
			printf("����ʧ�ܣ�");
		//��ӡ�ͻ���ip��ַ�Ͷ˿ں�
		printf("�ͻ��˵�IP�Ͷ˿ں��ǣ�%s:%u\n", inet_ntoa(clientAddr.sin_addr), htons(clientAddr.sin_port));

		//��ʼ���лỰ
		char recvBuf[1024] = "";
		flag = recv(sessionSocket, recvBuf, 1024, 0);
		if (flag != SOCKET_ERROR)
			printf("�������ݳɹ���\n");
		printf("%s�����յ�%d�ֽ�����\n", recvBuf, strlen(recvBuf));

		//�����ͻ�������
		int i = 0, j = 0;
		char fname[15] = "";
		while (recvBuf[i] != '/')
			i++;
		while (recvBuf[i + 1] != ' ')
		{
			fname[j] = recvBuf[i + 1];
			i++;
			j++;
		}
		fname[j] = '\0';

		
		printf("�ļ�����%s\n", fname);
		char path[50] = "";
		strcpy(path, filename);
		strcat(path, fname);	//�õ�����·��path
		printf("·����%s\n", path);
		sendHead(path, sessionSocket, path);
		sendFile(sessionSocket, path);
		closesocket(sessionSocket);
	}

	closesocket(srvSocket);
	WSACleanup();

	getchar();
	return 0;
}
*/



/**
 * @brief
 *
 * @param returnData
 * @param errorData
 * @param message
 * @return true
 * @return false
 */
bool isError(int returnData, int errorData, char* message) {
	if (returnData == errorData) {
		perror(message);	////��һ�������Դ�����Ϣ�������׼���� stderr
		WSAGetLastError();
		getchar();
		return false;
	}
	return true;
}


/**
 * @brief ��ͻ��˷�����Ӧ����
 *
 * @param path ��Ŀ¼
 * @param socket
 * @param filename	������ļ�·��
 */
void sendHead(char* path, SOCKET socket, char* filename) {
	char* extension = getFileExtention(path);//�����ļ�����
	char* content_type = "text/plain";
	char* body_length = "Content-Length: ";

	if (strcmp(extension, "html") == 0)
	{
		content_type = "text/html";
	}
	if (strcmp(extension, "gif") == 0)
	{
		content_type = "image/gif";
	}
	if (strcmp(extension, "jpg") == 0)
	{
		content_type = "image/jpg";
	}
	if (strcmp(extension, "png") == 0)
	{
		content_type = "image/png";
	}

	char* head = "HTTP/1.1 200 OK\r\n";//������Ӧ����ͷ����������
	char* not_find = "HTTP/1.1 404 NOT FOUND\r\n";
	int len, len1;
	char temp_1[30] = "Content-type: ";
	len = strlen(head);
	len1 = strlen(not_find);
	FILE* pfile = fopen(filename, "rb");
	if (pfile == NULL)
	{
		send(socket, not_find, len1, 0);
	}
	else if (send(socket, head, len, 0) == -1)
	{
		printf("Sending error");
		return;
	}
	if (content_type)
	{
		strcat(temp_1, content_type);
		strcat(temp_1, "\r\n");
		len = strlen(temp_1);

		if (send(socket, temp_1, len, 0) == -1)
		{
			printf("Sending error!");
			return;
		}
	}
	send(socket, "\r\n", 2, 0);

}


/**
 * @brief ��ȡ�ļ���׺
 *
 * @param path
 * @return char*
 */
char* getFileExtention(char* path)
{
	char* temp;	//���һ�γ���.����֮����ַ�
	if ((temp = strrchr(path, '.')) != NULL)
	{
		return temp + 1;
	}
	return "";
}


/**
 * @brief ��ͻ��˷����ļ�
 *
 * @param socket
 * @param filename
 */
void sendFile(SOCKET socket, char* filename)
{
	bool flag;
	FILE* file = fopen(filename, "rb");
	if (file == NULL) {
		printf("���ļ�ʧ�ܣ�\n");
		return 0;
	}
	fseek(file, 0L, SEEK_END);
	int flen = ftell(file);
	char* p = (char*)malloc(flen + 1);
	fseek(file, 0L, SEEK_SET);
	fread(p, flen, 1, file);
	send(socket, p, flen, 0);
}


/**
 * @brief ���IP�Ƿ�Ϸ�
 *
 * @param ip
 * @return true
 * @return false
 */
bool ipLegalCheck(char* ip) 
{
	int dots = 0; /*�ַ�.�ĸ���*/
	int setions = 0; /*ipÿһ�����ܺͣ�0-255��*/
	if (NULL == ip || *ip == '.') { /*�ų��������ΪNULL, ����һ���ַ�Ϊ'.'���ַ���*/
		return false;
	}

	while (*ip) {
		if (*ip == '.') {
			dots++;
			if (setions >= 0 && setions <= 255) { /*���ip�Ƿ�Ϸ�*/
				setions = 0;
				ip++;
				continue;
			}
		return false;
		}
		else if (*ip >= '0' && *ip <= '9') { /*�ж��ǲ�������*/
			setions = setions * 10 + (*ip - '0'); /*��ÿһ���ܺ�*/
		}
		else
			return false;
		ip++;
	}
	
	/*�ж�IP���һ���Ƿ�Ϸ�*/
	if (setions >= 0 && setions <= 255) {
		if (dots == 3) {
			return true;
		}
	}

	return false;
}


/**
 * @brief ���˿��Ƿ�Ϸ�
 *
 * @param ip
 * @return true
 * @return false
 */
bool portLegalCheck(int port) {
	if (port < 1 || port > 65535)
	{
		return false;
	}
	return true;
}



#include<stdio.h>
#include<Winsock2.h>
#pragma comment(lib,"ws2_32.lib")

void send_file(SOCKET s, char* filename);
int error(int backdata, int errordata, char* printword);
void send_head(char* arguments, SOCKET s, char* filename);
char* file_type_addr(char* arg);

int error(int backdata, int errordata, char* printword)
{
	if (backdata == errordata)
	{
		perror(printword);
		WSAGetLastError();
		getchar();
		return -1;
	}
	return 0;
}
int main()
{
	int isok;
	WSADATA wsdata;
	printf("----------------------��ʼ��----------------------\n");
	printf("Winsock  startup Ok!\n");	//?
	//��ʼ��winsock
	isok = WSAStartup(0x0202, &wsdata);//ȷ��socket�汾��Ϣ2.2��makeword��һ����
	error(isok, WSAEINVAL, "��ʼ��Winsockʧ��!\n");
	
	printf("\n-------------����ip��ַ&�˿ں�&��Ŀ¼-------------\n");
	//��������socket
	SOCKET ser_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//����������Э����(�˴���ipv4�����������ͣ��˴������䣩������Э�飨TCP��
	error(ser_socket, INVALID_SOCKET, "����socketʧ�ܣ�\n");
	printf("Socket create Ok!\n\n");	//?

	//����socket
	int port = 0;
	char inaddr[20] = "";
	printf("������˿ںţ�");
	scanf("%d", &port);
	while (!portLegalCheck(port)) {
		printf("������Ϸ��Ķ˿ںţ�");
		scanf("%d", &port);
	}
	printf("�����������ַ��");
	scanf("%s", inaddr);
	while (!ipLegalCheck(inaddr)) {
		printf("������Ϸ���ip��ַ��");
		scanf("%s", inaddr);
	}
	char filename[48] = "";
	printf("��������Ŀ¼��");
	scanf("%s", filename);


	struct sockaddr_in seraddr;
	seraddr.sin_family = AF_INET;//��socket��Э��һ��Ϊipv4
	seraddr.sin_port = htons(port);//���˿ں�80����������Զ����Ĵ洢��ʽ�������ˣ�����С��
	seraddr.sin_addr.s_addr = inet_addr(inaddr);//������ַ


	isok = bind(ser_socket, &seraddr, sizeof(seraddr));
	error(isok, SOCKET_ERROR, "������ʧ�ܣ�\n");
	printf("Socket bind Ok!\n");	//?

	//���õȴ�����״̬
	isok = listen(ser_socket, 5);
	error(isok, SOCKET_ERROR, "���õȴ�����״̬ʧ�ܣ�\n");
	printf("Socket listen Ok!\n");	//?

	printf("\n-----------------��Ӧ�ͻ�������-----------------\n");
	struct sockaddr_in claddr;
	int cllen = sizeof(claddr);
	while (1)
	{
		//�ȴ����Ӳ����ɻỰsocket
		SOCKET cli_socket = accept(ser_socket, &claddr, &cllen);//˭���ͻ��ˣ�������
		error(cli_socket, INVALID_SOCKET, "����ʧ�ܣ�\n");


		printf("�ͻ��˵�IP�Ͷ˿ں��ǣ�%s:%u\n", inet_ntoa(claddr.sin_addr), htons(claddr.sin_port));
		//���лỰ
		char recvdata[1024] = "";
		isok = recv(cli_socket, recvdata, 1024, 0);
		error(isok, SOCKET_ERROR, "��������ʧ�ܣ�\n");
		printf("%s�����յ�%d�ֽ�����\n", recvdata, strlen(recvdata));

		int i = 0, j = 0;
		char name[15] = " ";
		while (recvdata[i] != '/')
			i++;
		while (recvdata[i + 1] != ' ')
		{
			name[j] = recvdata[i + 1];
			i++;
			j++;
		}
		name[j] = '\0';
		printf("�ļ�����%s\n", name);	//�ļ���
		char filename2[48] = "";
		strcpy(filename2, filename);
		strcat(filename2, name);	//·��
		printf("path:%s\n", filename2);
		if (strcmp(name, "host.jpg")==0) {
			char* fobbiden = "HTTP/1.1 403 Forbidden\r\n\r\n";
			send(cli_socket, fobbiden, strlen(fobbiden), 0);
		}
		else {
			send_head(name, cli_socket, filename2);
			send_file(cli_socket, filename2);
		}
		

		closesocket(cli_socket);
	}

	closesocket(ser_socket);
	WSACleanup();

	getchar();
	return 0;
}

void send_head(char* arguments, SOCKET s, char* filename)
{
	char* extension = file_type_addr(arguments);//�����ļ�����
	char* content_type = "text/plain";
	char* body_length = "Content-Length: ";

	if (strcmp(extension, "html") == 0)
	{
		content_type = "text/html";
	}
	if (strcmp(extension, "gif") == 0)
	{
		content_type = "image/gif";
	}
	if (strcmp(extension, "jpg") == 0)
	{
		content_type = "image/jpg";
	}
	if (strcmp(extension, "png") == 0)
	{
		content_type = "image/png";
	}

	char* head = "HTTP/1.1 200 OK\r\n";//������Ӧ����ͷ����������
	char* not_find = "HTTP/1.1 404 NOT FOUND\r\n";
	

	int len, len1;
	char temp_1[30] = "Content-type: ";
	len = strlen(head);
	len1 = strlen(not_find);
	FILE* pfile = fopen(filename, "rb");
	if (pfile == NULL)
	{
		send(s, not_find, len1, 0);
	}
	else if (send(s, head, len, 0) == -1)
	{
		printf("Sending error");
		return;
	}
	if (content_type)
	{
		strcat(temp_1, content_type);
		strcat(temp_1, "\r\n");
		len = strlen(temp_1);

		if (send(s, temp_1, len, 0) == -1)
		{
			printf("Sending error!");
			return;
		}
	}
	send(s, "\r\n", 2, 0);
}

char* file_type_addr(char* arg)//��λ�ļ�����׺λ��
{
	char* temp;
	if ((temp = strrchr(arg, '.')) != NULL)
	{
		return temp + 1;
	}
	return "";
}

void send_file(SOCKET s, char* filename)//�����ļ����ͻ���
{
	int ok;
	FILE* pfile = fopen(filename, "rb");
	if (pfile == NULL)
	{
		printf("���ļ�ʧ�ܣ�\n");
		return 0;
	}
	fseek(pfile, 0L, SEEK_END);
	int flen = ftell(pfile);
	char* p = (char*)malloc(flen + 1);
	fseek(pfile, 0L, SEEK_SET);
	fread(p, flen, 1, pfile);
	send(s, p, flen, 0);
}







