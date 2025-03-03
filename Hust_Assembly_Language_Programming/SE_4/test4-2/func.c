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
extern char str7[20], str6[20], str10[20], bname[9], bpass[11];
extern SAMPLES buf[5];
extern SAMPLES midf[5];


/*登录界面*/
int login(void)
{
	char user_name[20];
	char password[20];
	printf("请输入用户名： ");
	scanf("%s", user_name);
	if (strcmp(user_name, bname)) {
		printf("用户名错误！\n");
		return ERROR;
	}
	printf("请输入密码：   ");
	scanf("%s", password);

	password[0] = 'H' ^ password[0];
	password[2] = 'A' ^ password[2];
	password[4] = 'O' ^ password[4];
	password[6] = 'E' ^ password[6];

	if (strcmp(password, bpass)) {
		printf("密码错误\n");
		return ERROR;
	}
	printf("登陆成功!\n");
	return OK;
}

/*展示菜单界面*/
void main_menu_print(void)
{
	system("pause");
	system("cls");
	printf("                   程序菜单\n");
	printf("1.输入采集数据  2.计算并打印midf内容  3.退出程序\n");
	printf("%s", str7);
	return;
}

/*输入并覆盖buf的第一条数据*/
void add_data(char* buff)
{
	printf("请输入覆盖数据信息：\n");
	scanf("%s %d %d %d", buff, buff + 9, buff + 13, buff + 17);
	printf("%s", str6);
	return;
}


