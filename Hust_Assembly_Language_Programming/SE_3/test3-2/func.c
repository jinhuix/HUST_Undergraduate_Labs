#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#pragma warning(disable:4996)

#define OK 1
#define ERROR 0
#define N 5


typedef struct samples {
	char SAMID[9];
	long SDA, SDB, SDC, SF;
} SAMPLES;
extern char str7[20], str6, str10[20];
extern SAMPLES buf[5];
extern SAMPLES midf[5];

/*��¼����*/
int login(void)
{
	char user_name[20];
	char password[20];
	printf("�������û����� ");
	scanf("%s", user_name);
	if (strcmp(user_name, "xujinhui")) {
		printf("�û�������\n");
		return ERROR;
	}
	printf("���������룺   ");
	scanf("%s", password);
	if (strcmp(password, "U202011675")) {
		printf("�������\n");
		return ERROR;
	}
	printf("��½�ɹ�!\n");
	return OK;
}

/*չʾ�˵�����*/
void main_menu_print(void)
{
	system("pause");
	system("cls");
	printf("                   ����˵�\n");
	printf("1.����ɼ�����  2.���㲢��ӡmidf����  3.�˳�����\n");
	printf("%s", str7);
	return;
}

/*���벢����buf�ĵ�һ������*/
void add_data(char* buff)
{
	printf("�����븲��������Ϣ��\n");
	scanf("%s %d %d %d", buff, buff+9, buff+13, buff+17);
	printf("%s", str6);
	return;
}


