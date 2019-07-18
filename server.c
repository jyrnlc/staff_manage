#include <stdio.h>
#include "common.h"

sqlite3 *db;
int sockfd, clientfd;

void process_client_request( MSG *msg);
int process_user_root_login( MSG *msg);
void process_client_request( MSG *msg);
int process_user_search_request(MSG *msg);

int main(int argc, char *argv[])
{
	MSG msg;
	char *errmsg;
	char buf[128] = {0};
	struct sockaddr_in server_addr, client_addr;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	
	if(argc != 3){
		printf("usage:%s <ip> <port>\n", argv[0]);
		return -1;
	}
	
	if(sqlite3_open(STAFF_DATABASE, &db) != SQLITE_OK){
		printf("fail to sqlite3 open err %s\n", sqlite3_errmsg(db));
		return -1;
	}
	sprintf(buf, "create table usrinfo(staffno integer,usertype integer,name text,passwd text,age integer,phone text,addr text,date text,level integer,salary REAL);");
	if(sqlite3_exec(db, buf, NULL, NULL, &errmsg) !=  SQLITE_OK)
    {
		printf("sqlite3 create:%s\n", errmsg);
    }else{
		printf("sqlite3 create usrinfo success\n");
	}
	
	//创建套接字
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		perror("fail to socket");
		return -1;
	}
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2])); 
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	//绑定
	if(bind(sockfd, (struct sockaddr *)&server_addr, addrlen) < 0){
		perror("fail to bind");
		return -1;
	}
	
	//监听
	if(listen(sockfd, 5) < 0){
		perror("fail to listen");
		return -1;
	}
	
	fd_set rfds, temfds;
	int maxfd = sockfd;
	FD_ZERO(&rfds);
	FD_SET(0, &rfds);
	FD_SET(sockfd, &rfds);
	
	while(1){
		temfds = rfds;
		if(select(maxfd+1, &temfds, NULL, NULL, NULL) < 0){
			perror("fail to select");
			return -1;
		}
		if(FD_ISSET(sockfd, &temfds) == 1){
			//等待连接
			clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &addrlen);
			if(clientfd < 0){
				perror("fail to accept");
				return -1;
			}
			printf("client ip : %s.\n",inet_ntoa(client_addr.sin_addr));
			FD_SET(clientfd, &rfds);
			maxfd = maxfd > clientfd ? maxfd : clientfd;
		}else{
			ssize_t size = recv(clientfd, &msg, sizeof(MSG), 0);
			if(size < 0){
				perror("recv fail");
				return -1;
			}else if(size == 0){
				printf("peer exit\n");
				exit(0);
			}
			else{
				process_client_request(&msg);
			}
		}
	}
		
	return 0;
}
void process_client_request(MSG *msg)
{
	printf("--------------process_client_request------------\n");
	printf("msg->msgtype:%#x\n", msg->msgtype);
	switch(msg->msgtype)
	{
		case ROOT_LOGIN:
		case USER_LOGIN:
			process_user_root_login(msg);
			break;
		case ROOT_SEARCH:
			process_root_search_request(msg);
			break;
		case QUIT:
			exit(0);
	}
}
int process_user_root_login(MSG *msg)
{
	printf("-----------process_user_root_login-------\n");
	
	char *errmsg;
	char buf[128] = {0};
	char **resultp;
	int nrow, ncolumn;
	memset(msg->recvmsg, 0, sizeof(msg->recvmsg));
	
	sprintf(buf, "select * from usrinfo where name = '%s' and passwd = '%s';", msg->name, msg->passwd);
	if(sqlite3_get_table(db, buf, &resultp, &nrow, &ncolumn, &errmsg) != 0){
		printf("sqlite3_get_table err:%s\n", errmsg);
	}else{
		if(nrow == 0){
			strcpy(msg->recvmsg, "username or passwd err");
			send(clientfd, msg, sizeof(MSG), 0);
		}else{
			strcpy(msg->recvmsg, "ok");
			send(clientfd, msg, sizeof(MSG), 0);
		}
	}
	return 0;
}
int process_root_search_request(MSG *msg)
{
	printf("-----------process_user_search_request-------\n");
	char **resultp, **p;
	int nrow, ncloum;
	char *errmsg;
	char buf[128] = {0}, temp[32] = {0};
	int i, j;
	
	memset(msg->recvmsg, 0, sizeof(msg->recvmsg));
	if(msg->flag == 1){
		sprintf(buf, "select * from usrinfo where name = '%s';", msg->name);		
	}else{
		sprintf(buf, "select * from usrinfo;");
	}
	
	if (sqlite3_get_table(db, buf, &resultp, &nrow, &ncloum, &errmsg) != SQLITE_OK) {
		printf("select err:%s\n", errmsg);
	}
	p = resultp + ncloum;
	for (i = 0; i < nrow; i++) {
		for (j = 0; j < ncloum; j++) {
			sprintf(temp, "%s	", p[(i*ncloum)+j]);
			strcat(msg->recvmsg, temp);
		}
		printf("server:%s\n", msg->recvmsg);
		send(clientfd, msg, sizeof(MSG), 0);
	}
	
	if(nrow == 0){
		strcpy(msg->recvmsg, "no");
		send(clientfd, msg, sizeof(MSG), 0);
	}	
	if(msg->flag != 1){			
		strcpy(msg->recvmsg, "over");
		send(clientfd, msg, sizeof(MSG), 0);
	}

	return 0;
}







