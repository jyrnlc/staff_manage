
#ifndef _COMMON_H_
#define _COMMON_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<sqlite3.h>
#include<sys/wait.h>
#include<signal.h>
#include<time.h>
#include<pthread.h>
#include<sys/stat.h>
#include<sqlite3.h>

#define STAFF_DATABASE "staff_info_excel.db"

#define ROOT_LOGIN 0x1
#define USER_LOGIN 0x2
#define ROOT 0x3
#define USER 0x4
#define QUIT 0x5
#define ROOT_SEARCH 0x6
#define USER_SEARCH 0x7
#define ROOT_ADDUSER 0x8

typedef struct {
	int num;
	int usertype;
	char name[32];
	char passwd[8];
	int age;
	char tel[18];
	char addr[32];
	char time[32];
	int grade;
	double salary;
}staff_info;

typedef struct{
	char msgtype;
	char usertype;
	char name[32];
	char passwd[8];
	char recvmsg[128];
	int flag;
	staff_info info;
}MSG;

#endif

