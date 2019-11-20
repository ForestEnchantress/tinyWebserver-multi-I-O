#include"csapp.h"
#include"tiny.h"
#include<sys/epoll.h>
#define EPOLL_SIZE 50
int mainpid;
struct epoll_event *events;
void handle_SIGCHLD(int sig){
	while(waitpid(-1,NULL,WNOHANG)>0);
}
void handle_SIGPIPE(int sig){
	if(getpid()!=mainpid) {fprintf(stderr,"client disconnect when child execute dynamic file\n");
		exit(0);}//若在子进程中执行动态文件时客户端断连，则子进程退出
}

int main(int argc,char** argv){
	int listenfd,connfd,epfd,fd,i,n,event_cnt;
	struct epoll_event event;
	char host[MAXLINE],port[MAXLINE],buf[MAXLINE];
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
	epfd=epoll_create(EPOLL_SIZE);
	events=malloc(sizeof(struct epoll_event)*EPOLL_SIZE);
	event.data.fd=listenfd;
	event.events=EPOLLIN;
	epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&event);

	while(1){
		event_cnt=epoll_wait(epfd,events,EPOLL_SIZE,-1);
		if(event_cnt==-1){
			puts("epoll_wait error");
			break;
		}
		for(i=0;i<event_cnt;i++){
			fd=events[i].data.fd;
			if(fd==listenfd){
				clientlen=sizeof(clientaddr);
				connfd=Accept(listenfd,(SA*)&clientaddr,&clientlen);
				getnameinfo((SA*)&clientaddr,clientlen,host,MAXLINE,port,MAXLINE,0);
				printf("connected client:IP %s port %s\n",host,port);
				event.data.fd=connfd;
				event.events=EPOLLIN;
				epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&event);
			}else{
				doit(fd);
				close(fd);
				epoll_ctl(epfd,EPOLL_CTL_DEL,fd,0);	
			}
		}
	}
	Free(events);
	printf("close all!\n");
	close(epfd),close(listenfd);
	return 0;
}

