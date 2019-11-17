#include"csapp.h"
#include<math.h>
#include"pool.h"

int mainpid;
static pool p;

void handle_SIGCHLD(int sig){
	while(waitpid(-1,NULL,WNOHANG)>0);
}
void handle_SIGPIPE(int sig){
	if(getpid()!=mainpid) {fprintf(stderr,"client disconnect when child execute dynamic file\n");
		exit(0);}//若在子进程中执行动态文件时客户端断连，则子进程退出
}
int main(int argc,char** argv){
	int listenfd,connfd;
	char host[MAXLINE],port[MAXLINE];
	socklen_t clientlen;
	struct sockaddr_storage clientaddr;

	if(argc!=2){
		fprintf(stderr,"usage:%s <port>\n",argv[0]);
		exit(1);
	}
	mainpid=getpid();
	if(Signal(SIGCHLD,handle_SIGCHLD)==SIG_ERR)
		unix_error("mask SIGCHLD error");
	if(Signal(SIGPIPE,handle_SIGPIPE)==SIG_ERR)
		unix_error("mask SIGPIPE error");
	/*open server*/
	listenfd=Open_listenfd(atoi(argv[1]));
	init_pool(listenfd,&p);

	while(1){
		p.ready_set=p.read_set;
		p.nready=Select(p.maxfd+1,&p.ready_set,0,0,0);
		if(p.nready==0)
			continue;
		if(FD_ISSET(listenfd,&p.ready_set)){
			clientlen=sizeof(struct sockaddr_storage);
			connfd=Accept(listenfd,(SA*)&clientaddr,&clientlen);
			getnameinfo((SA*)&clientaddr,clientlen,host,MAXLINE,port,MAXLINE,0);
			printf("connect to host:%s,port:%s\n",host,port);
			add_client(connfd,&p);
		}
		check_clients(&p);
	}
	return 0;
}

