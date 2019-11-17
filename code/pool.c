#include"pool.h"
#include"tiny.h"

void init_pool(int listenfd,pool* p){
	int i;
	p->maxi=-1;
	for(i=0;i<FD_SETSIZE;i++)
		p->clientfd[i]=-1;

	p->maxfd=listenfd;
	FD_ZERO(&p->read_set);
	FD_SET(listenfd,&p->read_set);
}
void add_client(int connfd,pool* p){
	int i;
	p->nready--;
	for(i=0;i<FD_SETSIZE;i++)
		if(p->clientfd[i]<0){
			p->clientfd[i]=connfd;

			FD_SET(connfd,&p->read_set);

			if(connfd>p->maxfd)
				p->maxfd=connfd;
			if(i>p->maxi)
				p->maxi=i;
			break;
		}
	if(i==FD_SETSIZE)
		app_error("add_client error:Too many clients");
}
void check_clients(pool* p){
	int i,connfd;

	for(i=0;(i<=p->maxi)&&(p->nready>0);i++){
		connfd=p->clientfd[i];
		if((connfd>0)&&(FD_ISSET(connfd,&p->ready_set))){
			p->nready--;
			doit(connfd);
			Close(connfd);
			FD_CLR(connfd,&p->read_set);
			p->clientfd[i]=-1;
		}
	}
}

