/*
** server.c - a stream socket server demo
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/wait.h>
#include <signal.h>
#define MYPORT 4700 // the port users will be connecting to
#define BACKLOG 10 // how many pending connections queue will hold
#define MAXDATA 100
////////////////////////////////////////////////////////////////////////////////////
#define MSGFORMATE 0X0010

#define WELCOME_MESSAGE 0X00
#define SHELL_COMMAND 	0X11
#define CMDEXEC_SUCCESSFUL 0X12
#define TERM_SER_PROC	0X13
////////////////////////////////////////////////////////////////////////////////////
void sigchld_handler(int s)
{
while(wait(NULL) > 0);
}
//////////////////////Message Format Structures//////////////////////////////

	typedef struct
			{
			uint8_t paytype;		
			char pvalue[100];
			}plt;
	typedef struct
			{
			uint8_t mst;
			uint16_t plen;
			plt pldata;	
			
			}msgf;
//////////////////////////////////////////////////////////////////

void FillPacket(int fd,char ip[],char port[]);
void RecvPacket(int fd,char iP[]);
void SendSuc(int fd,int nu);

int main(void)
{
	int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
	struct sockaddr_in my_addr; // my address information
	struct sockaddr_in their_addr; // connector’s address information
	int sin_size;
	struct sigaction sa;
	int yes=1;
	char Port[16];
	char Ip[32];
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
		{
			perror("socket");
			exit(1);
		}
	if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) 
		{
			perror("setsockopt");
			exit(1);
		}
	my_addr.sin_family = AF_INET; // host byte order
	my_addr.sin_port = htons(MYPORT); // short, network byte order
	my_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
	memset(&(my_addr.sin_zero), '\0', 8); // zero the rest of the struct
	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr))== -1)
	 {
		perror("bind");
		exit(1);
	}
	if (listen(sockfd, BACKLOG) == -1) 
	{
		perror("listen");
		exit(1);
	}
		sa.sa_handler = sigchld_handler; // reap all dead processes
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = SA_RESTART;
		if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	while(1) 
		{ // main accept() loop
		sin_size = sizeof(struct sockaddr_in);
		if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr,&sin_size)) == -1) 
			{
			perror("accept");
			continue;
			}
		printf("server: got connection from %s\n",inet_ntoa(their_addr.sin_addr));
		
		snprintf(Ip,sizeof(Ip),"IP.%s",inet_ntoa(their_addr.sin_addr));
	
            	snprintf(Port,sizeof(Port),".%d",htons(their_addr.sin_port));
			
		if (!fork()) { // this is the child process
		close(sockfd); // child doesnot need the listener
		
		FillPacket(new_fd,Ip,Port);
		
		RecvPacket(new_fd,Ip);
		
		close(new_fd);
		exit(0);
	}
close(new_fd); // parent does not need this
}
return 0;
}

void FillPacket(int new_fd,char ip[],char port[])
	{
		
		struct sockaddr_in caddr;
		msgf obj;
		char buf[MAXDATA];
		char *msg="Welcom,Client:";
		int numbytes;
		
		strcpy(buf,msg);
		strncat(buf,ip,strlen(ip));
		strncat(buf,port,strlen(port));
		
		buf[MAXDATA]='\0';

		obj.mst=MSGFORMATE;
		obj.pldata.paytype=WELCOME_MESSAGE;
		strcpy(obj.pldata.pvalue,buf);
		obj.plen=strlen(buf);

		if (send(new_fd,&obj,sizeof(msgf),0)==-1)
		perror("Send Fill Error");
		
	}

void RecvPacket(int fd,char ip[])
	{
			msgf object;
			int a=0;
			int cmp;
			int numbytes,numby;
			char *path="/bin/";	
			char buf[15];
			if(numbytes=recv(fd,&object,sizeof(msgf),0)==-1)
			//perror("Command Recv Error");
			printf("Connection Terminated By Client......");
			
			
			
			if(object.pldata.paytype==TERM_SER_PROC)
			{
				printf("client Terminate The Connection");
				close(fd);
				exit(0);
			}
			else if(object.pldata.paytype == SHELL_COMMAND)
			{
				
			printf("\n[cmd<%s>]:%s\n",ip,&object.pldata.pvalue);
					
			strcpy(buf,path);
			strcat(buf,object.pldata.pvalue);
			
			if(!fork())
			{
			numby=execl(buf,object.pldata.pvalue,0);
			if(numby==-1)	
				{
				perror("command Execl Error");
				a++;
				
				}
			
			}
			
			}
		


		SendSuc(fd,a);
		RecvPacket(fd,ip);
		exit(0);
		
	}
	
void SendSuc(int sd,int nm)
	{
		msgf objt;
		int numb;
		char *msg="Succesfull Execution of Shell Command";
		char *imsg="Invalid Command";
		objt.mst=MSGFORMATE;
		
		
		objt.pldata.paytype=CMDEXEC_SUCCESSFUL;
		switch(nm)
		{
		case 1:
		strcpy(objt.pldata.pvalue,imsg);
		objt.plen=strlen(imsg);
		break;
		default:
		strcpy(objt.pldata.pvalue,msg);
		objt.plen=strlen(msg);
		}
		
		numb=send(sd,&objt,sizeof(msgf),0);
		if(numb==-1)
		{
		perror("SendSuc Send Error");
		exit(1);
		}
		
	}






