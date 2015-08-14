#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXDATASIZE 1024
#define PORT 1111
#define BACKLOG 5


void process(int connectfd){
    char buf[MAXDATASIZE];
    int num=0;
    while(0<(num=read(connectfd,buf,sizeof(buf)))){
        buf[num]='\0';
        if(0==strcmp(buf,"bye"))
            break;
        int i=0;
        puts(buf);
        while(buf[i]){
            if(buf[i]>='a'&&buf[i]<='z')
                buf[i]=buf[i]-'a'+'A';
            i++;
        }
        write(connectfd,buf,strlen(buf));
	printf("pid = %ld \n",(long)getpid());
    }
    close(connectfd);
}

int main(){
	int listenfd,connectfd;
	struct sockaddr_in server;
	struct sockaddr_in client;
	int len;
	pid_t pid;

	if(-1==(listenfd=socket(AF_INET,SOCK_STREAM,0))){
	    perror("create socket error");
	    exit(1);
	}

	int opt=SO_REUSEADDR;
	setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

	bzero(&server,sizeof(server));

	server.sin_family=AF_INET;
	server.sin_port=htons(PORT);
	server.sin_addr.s_addr=htonl(INADDR_ANY);

	if(-1==bind(listenfd,(struct sockaddr *)&server,sizeof(struct sockaddr ))){
	    perror("bind error\n");
	    exit(1);
	}

	if(-1==listen(listenfd,BACKLOG)){
	    perror("listen error\n");
	    exit(1);
	}

	len=sizeof(client);

	while(1){
		printf("before accept\n");
		if(-1==(connectfd=accept(listenfd,(struct sockaddr *)&client,&len))){
			perror("connect error\n");
			exit(1);
		}
		printf("after accept\n");
		if((pid=fork())<0){
			perror("fork error\n");
			exit(1);
		}
		else if(0==pid){
			close(listenfd);
			printf("Got a connection from %s. \n",inet_ntoa(client.sin_addr));
			process(connectfd);
			
			exit(0);
		}else{
			sleep(1);
		}
		printf("pid = %ld \n",(long)getpid());
	}
	
	close(listenfd);
	return 0;
}

