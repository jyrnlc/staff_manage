#include <stdio.h>
#include "common.h"

int sockfd, clientfd;

void do_login();
int root_menu(MSG *msg);
int do_root_search(MSG *msg);
int do_user_root_login(MSG *msg);
void show_user_msg(MSG *msg);

int main(int argc, char *argv[])
{
	struct sockaddr_in server_addr, client_addr;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	
	if(argc != 3){
		printf("usage:%s <ip> <port>\n", argv[0]);
		return -1;
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		perror("fail to socket");
		return -1;
	}
	
	memset(&server_addr, 0, addrlen);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2])); 
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	

	if(connect(sockfd, (struct sockaddr *)&server_addr, addrlen) < 0){
		perror("fail to connect");
		return -1;
	}
	
	do_login();
	
	return 0;
}

void do_login()
{
	printf("----------do_login----------\n");
	int num;
	MSG msg;
	while(1){
		printf("*************************************************************\n");
		printf("******** 1：管理员模式    2：普通用户模式    3：退出 ********\n");
		printf("*************************************************************\n");
		printf("请输入您的选择（数字）>>");
		scanf("%d", &num);
		getchar();
		switch (num){
			case 1:
				msg.msgtype = ROOT_LOGIN;
				msg.usertype = ROOT;
				break;
			case 2:
				msg.msgtype = USER_LOGIN;
				msg.usertype = USER;
				break;
			case 3:
				msg.msgtype = QUIT;
				ssize_t size = send(sockfd, &msg, sizeof(MSG), 0);
				if(size < 0){
					perror("send fail");
					exit(-1);
				}
				close(sockfd);
				exit(0);
			default:
				printf("输入有误，请重新输入！");
		}
		
		do_user_root_login(&msg);
	}
}
int do_user_root_login(MSG *msg)
{
	printf("----------do_user_root_login----------\n");
	printf("请输入用户名：");
	memset(msg->name, 0, sizeof(msg->name));
	scanf("%s", msg->name);
	getchar();
	printf("请输入密码（6位）:");
	memset(msg->passwd, 0, sizeof(msg->passwd));
	scanf("%s", msg->passwd);
	getchar();
	
	send(sockfd, msg, sizeof(MSG), 0);
	
	recv(sockfd, msg, sizeof(MSG), 0);
	
	printf("msg->msgtype:%#x\n", msg->msgtype);
	if(strncmp(msg->recvmsg, "ok", 2) == 0){
		if(msg->usertype == ROOT){
			printf("亲爱的管理员，欢迎您登陆员工管理系统！\n");
			root_menu(msg);
		}
		else if(msg->usertype == USER){
			printf("亲爱的用户，欢迎您登陆员工管理系统！\n");
		}
	}else{
		printf("登录失败！%s\n", msg->recvmsg);
		return -1;
	}
	return 0;
}

int root_menu(MSG *msg)
{
	printf("--------------root_menu-----------\n");
	int num;
	while(1){		
		printf("****************************************************************\n");
		printf("* 1.查询  2.修改 3.添加用户  4.删除用户  5.查询历史记录 6.退出 *\n");
		printf("****************************************************************\n");
		printf("请输入您的选择（数字）>>");
		scanf("%d", &num);
		getchar();
		switch(num){
			case 1:
				do_root_search(msg);
				break;
			case 2:
				//do_root_modify(msg);
				break;
			case 3:
				do_root_adduser(msg);
				break;
			case 4:
				//do_delete_user();
				break;
			case 5:
				//do_root_history();
				break;
			case 6:
				exit(0);
				break;
			default :
				printf("input err, please input again\n");
		}
	}
	return 0;
}
int do_root_search(MSG *msg)
{
	printf("--------------do_root_search-----------\n");
	int num;
	while(1){
		printf("****************************************************************\n");
		printf("******** 1：按人名查找  	2：查找所有 	3：退出 ************\n");
		printf("****************************************************************\n");
		printf("请输入您的选择（数字）>>");
		scanf("%d", &num);
		getchar();
		msg->msgtype = ROOT_SEARCH;
		switch(num){
			case 1:
				msg->flag = 1;		
				break;
			case 2:
				msg->flag = 0;
				break;
			case 3:
				goto EXIT;
		}
		if(msg->flag == 1){
			printf("请输入您要查找的用户名：");
			scanf("%s", msg->name);
			getchar();	
			send(sockfd, msg, sizeof(MSG), 0);
			recv(sockfd, msg, sizeof(MSG), 0);
			if(strncmp(msg->recvmsg, "no", 2) == 0){
				printf("user no exist\n");
				break;
			}
			show_user_msg(msg);
		}else{
			send(sockfd, msg, sizeof(MSG), 0);
			while(1){
				recv(sockfd, msg, sizeof(MSG), 0);
				if(strncmp(msg->recvmsg, "over", 4) == 0)
					break;
				show_user_msg(msg);
			}
		}
		
		EXIT:
			break;
	}
	return 0;
}
void show_user_msg(MSG *msg)
{
	printf("工号\t用户类型\t姓名\t密码\t年龄\t电话\t地址\t职位\t入职年月\t等级\t工资\n");
	printf("%s.\n",msg->recvmsg);
}
int do_root_modify(msg)
{
	printf("-----------do_root_modify-------------\n");
	return 0;
}
int do_root_adduser(MSG *msg)
{
	printf("-----------do_root_adduser-------------\n");
	return 0;
}
