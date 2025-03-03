
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
	//fd_set rfds;	//用于检查socket是否有数据到来的的文件描述符
	//fd_set wfds;	//用于检查socket是否可以发送的文件描述符
	bool flag;	//用于检查是否出错
	bool first_connetion = true;

	printf("----------------------初始化----------------------\n");
	flag = WSAStartup(0x0202, &wsaData);	//完成初始化
	//检查初始化是否成功
	if (flag) {
		printf("Winsock  startup failed with error!\n");
	}
	//检查版本号是否支持
	else if (wsaData.wVersion != 0x0202) {
		printf("Winsock version is not correct!\n");
	}
	else {
		printf("Winsock  startup Ok!\n");
	}


	printf("\n-------------配置ip地址&端口号&主目录-------------\n");
	//创建socket服务，使用Internet地址，流类型，TCP协议
	SOCKET srvSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (srvSocket != INVALID_SOCKET)
		printf("Socket create Ok!\n\n");
	else
		printf("Socket create Error!\n\n");
	//配置ip和端口号
	int port = 0;
	char inAddr[20] = "";
	//这里放一个检查是否合法
	printf("请输入端口号：");
	scanf("%d", &port);
	while (!portLegalCheck(port)) {
		printf("请输入合法的端口号：");
		scanf("%d", &port);
	}
	printf("请输入ip地址：");
	scanf("%s", inAddr);
	while (!ipLegalCheck(inAddr)) {
		printf("请输入合法的ip地址：");
		scanf("%s", inAddr);
	}
	char filename[50] = "";
	printf("请输入主目录：");
	scanf("%s", filename);


	struct sockaddr_in srvAddr;
	srvAddr.sin_family = AF_INET;//与socket的协议一样为ipv4
	srvAddr.sin_port = htons(port);//将端口号80换成网络可以读懂的存储方式，网络大端，电脑小端
	srvAddr.sin_addr.s_addr = inet_addr(inAddr);//监听地址

	//绑定
	flag = bind(srvSocket, &srvAddr, sizeof(srvAddr));
	if (flag != SOCKET_ERROR)
		printf("Socket bind Ok!\n");
	else
		printf("Socket bind Error!\n");
	//监听，等待队列长度5
	flag = listen(srvSocket, 5);
	if (flag != SOCKET_ERROR)
		printf("Socket listen Ok!\n");
	else
		printf("Socket listen Error!\n");


	printf("\n-----------------响应客户端请求-----------------\n");
	struct sockaddr_in clientAddr;
	int addrLen = sizeof(clientAddr);
	while (true)
	{
		//等待连接并生成会话SOCKET
		SOCKET sessionSocket = accept(srvSocket, &clientAddr, &addrLen);
		if (sessionSocket != INVALID_SOCKET)
			printf("Socket listen one client request!\n");
		else
			printf("连接失败！");
		//打印客户端ip地址和端口号
		printf("客户端的IP和端口号是：%s:%u\n", inet_ntoa(clientAddr.sin_addr), htons(clientAddr.sin_port));

		//开始进行会话
		char recvBuf[1024] = "";
		flag = recv(sessionSocket, recvBuf, 1024, 0);
		if (flag != SOCKET_ERROR)
			printf("接收数据成功！\n");
		printf("%s共接收到%d字节数据\n", recvBuf, strlen(recvBuf));

		//解析客户端请求
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

		
		printf("文件名：%s\n", fname);
		char path[50] = "";
		strcpy(path, filename);
		strcat(path, fname);	//得到绝对路径path
		printf("路径：%s\n", path);
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
		perror(message);	////把一个描述性错误消息输出到标准错误 stderr
		WSAGetLastError();
		getchar();
		return false;
	}
	return true;
}


/**
 * @brief 向客户端发送响应报文
 *
 * @param path 主目录
 * @param socket
 * @param filename	请求的文件路径
 */
void sendHead(char* path, SOCKET socket, char* filename) {
	char* extension = getFileExtention(path);//解析文件类型
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

	char* head = "HTTP/1.1 200 OK\r\n";//构造响应报文头部，并发送
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
 * @brief 获取文件后缀
 *
 * @param path
 * @return char*
 */
char* getFileExtention(char* path)
{
	char* temp;	//最后一次出现.及其之后的字符
	if ((temp = strrchr(path, '.')) != NULL)
	{
		return temp + 1;
	}
	return "";
}


/**
 * @brief 向客户端发送文件
 *
 * @param socket
 * @param filename
 */
void sendFile(SOCKET socket, char* filename)
{
	bool flag;
	FILE* file = fopen(filename, "rb");
	if (file == NULL) {
		printf("打开文件失败！\n");
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
 * @brief 检查IP是否合法
 *
 * @param ip
 * @return true
 * @return false
 */
bool ipLegalCheck(char* ip) 
{
	int dots = 0; /*字符.的个数*/
	int setions = 0; /*ip每一部分总和（0-255）*/
	if (NULL == ip || *ip == '.') { /*排除输入参数为NULL, 或者一个字符为'.'的字符串*/
		return false;
	}

	while (*ip) {
		if (*ip == '.') {
			dots++;
			if (setions >= 0 && setions <= 255) { /*检查ip是否合法*/
				setions = 0;
				ip++;
				continue;
			}
		return false;
		}
		else if (*ip >= '0' && *ip <= '9') { /*判断是不是数字*/
			setions = setions * 10 + (*ip - '0'); /*求每一段总和*/
		}
		else
			return false;
		ip++;
	}
	
	/*判断IP最后一段是否合法*/
	if (setions >= 0 && setions <= 255) {
		if (dots == 3) {
			return true;
		}
	}

	return false;
}


/**
 * @brief 检查端口是否合法
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
	printf("----------------------初始化----------------------\n");
	printf("Winsock  startup Ok!\n");	//?
	//初始化winsock
	isok = WSAStartup(0x0202, &wsdata);//确定socket版本信息2.2，makeword做一个字
	error(isok, WSAEINVAL, "初始化Winsock失败!\n");
	
	printf("\n-------------配置ip地址&端口号&主目录-------------\n");
	//创建监听socket
	SOCKET ser_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//三个参数：协议族(此处是ipv4），传输类型（此处流传输），传输协议（TCP）
	error(ser_socket, INVALID_SOCKET, "创建socket失败！\n");
	printf("Socket create Ok!\n\n");	//?

	//监听socket
	int port = 0;
	char inaddr[20] = "";
	printf("请输入端口号：");
	scanf("%d", &port);
	while (!portLegalCheck(port)) {
		printf("请输入合法的端口号：");
		scanf("%d", &port);
	}
	printf("请输入监听地址：");
	scanf("%s", inaddr);
	while (!ipLegalCheck(inaddr)) {
		printf("请输入合法的ip地址：");
		scanf("%s", inaddr);
	}
	char filename[48] = "";
	printf("请输入主目录：");
	scanf("%s", filename);


	struct sockaddr_in seraddr;
	seraddr.sin_family = AF_INET;//与socket的协议一样为ipv4
	seraddr.sin_port = htons(port);//将端口号80换成网络可以读懂的存储方式，网络大端，电脑小端
	seraddr.sin_addr.s_addr = inet_addr(inaddr);//监听地址


	isok = bind(ser_socket, &seraddr, sizeof(seraddr));
	error(isok, SOCKET_ERROR, "监听绑定失败！\n");
	printf("Socket bind Ok!\n");	//?

	//设置等待连接状态
	isok = listen(ser_socket, 5);
	error(isok, SOCKET_ERROR, "设置等待连接状态失败！\n");
	printf("Socket listen Ok!\n");	//?

	printf("\n-----------------响应客户端请求-----------------\n");
	struct sockaddr_in claddr;
	int cllen = sizeof(claddr);
	while (1)
	{
		//等待连接并生成会话socket
		SOCKET cli_socket = accept(ser_socket, &claddr, &cllen);//谁（客户端）连进来
		error(cli_socket, INVALID_SOCKET, "连接失败！\n");


		printf("客户端的IP和端口号是：%s:%u\n", inet_ntoa(claddr.sin_addr), htons(claddr.sin_port));
		//进行会话
		char recvdata[1024] = "";
		isok = recv(cli_socket, recvdata, 1024, 0);
		error(isok, SOCKET_ERROR, "接受数据失败！\n");
		printf("%s共接收到%d字节数据\n", recvdata, strlen(recvdata));

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
		printf("文件名：%s\n", name);	//文件名
		char filename2[48] = "";
		strcpy(filename2, filename);
		strcat(filename2, name);	//路径
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
	char* extension = file_type_addr(arguments);//解析文件类型
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

	char* head = "HTTP/1.1 200 OK\r\n";//构造响应报文头部，并发送
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

char* file_type_addr(char* arg)//定位文件名后缀位置
{
	char* temp;
	if ((temp = strrchr(arg, '.')) != NULL)
	{
		return temp + 1;
	}
	return "";
}

void send_file(SOCKET s, char* filename)//发送文件给客户端
{
	int ok;
	FILE* pfile = fopen(filename, "rb");
	if (pfile == NULL)
	{
		printf("打开文件失败！\n");
		return 0;
	}
	fseek(pfile, 0L, SEEK_END);
	int flen = ftell(pfile);
	char* p = (char*)malloc(flen + 1);
	fseek(pfile, 0L, SEEK_SET);
	fread(p, flen, 1, pfile);
	send(s, p, flen, 0);
}







