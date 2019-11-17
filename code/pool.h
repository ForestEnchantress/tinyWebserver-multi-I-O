#ifndef POOL_H_
#define POOL_H_ 
#include"csapp.h"
typedef struct{
	int maxfd;
	fd_set read_set;
	fd_set ready_set;
	int nready;
	int maxi;
	int clientfd[FD_SETSIZE];
}pool;
int byte_cnt=0;
void init_pool(int listenfd,pool* p);
void add_client(int connfd,pool* p);
void check_clients(pool* p);

#include"pool.c"
#endif
