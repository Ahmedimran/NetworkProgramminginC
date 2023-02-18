#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 4700 // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once


///////////////////////////////////////////////////////////////////////////////////
#define MSGFORMATE 0X0010

#define WELCOME_MESSAGE 0X00
#define SHELL_COMMAND 	0X11
#define CMDEXEC_SUCCESSFUL 0X12
#define TERM_SER_PROC	0X13
///////////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////////////
void RecvPacket(int fd);
void SendPacket(int fd);

int main(int argc, char *argv[])
{
    int sockfd, numbytes, fnsize=0;
    char buf[MAXDATASIZE];
    struct hostent *he;

    struct sockaddr_in their_addr; // connector's address information 

    if (argc != 2) 
    {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

    if ((he=gethostbyname(argv[1])) == NULL)
     {  // get the host info 
        perror("gethostbyname");
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
    {
        perror("socket");
        exit(1);
    }

    their_addr.sin_family = AF_INET;    // host byte order 
    their_addr.sin_port = htons(PORT);  // short, network byte order 
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    memset(&(their_addr.sin_zero), '\0', 8);  // zero the rest of the struct 

    if (connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1)
     {
        perror("connect");
        exit(1);
    }

	RecvPacket(sockfd);
	SendPacket(sockfd);
    	//RecvPacket(sockfd);


    close(sockfd);

    return 0;
}

void RecvPacket(int ssock)
	{
		msgf object;
		int num;
		char buf[MAXDATASIZE];
		if(num=recv(ssock,&object,sizeof(msgf),0)==-1)
		{
		perror("Recive Error");
		}
		if (object.mst==MSGFORMATE)
		{
			if(object.pldata.paytype==WELCOME_MESSAGE)
			{
			printf("%s\n",object.pldata.pvalue);
			}
			else if(object.pldata.paytype==CMDEXEC_SUCCESSFUL)
			{
			printf("%s\n",object.pldata.pvalue);
			}
		
		}
		
		
	}

void SendPacket(int sfd)
	{
		char cmd[15];
		int num,or;
		char *ch="e";
		msgf obj;
		printf("\n----------------------------------------");
		printf("\n---------Enter The Command--------------");
		printf("\n-----Type-e-to-Terminate-Connection---\n");
		
		scanf("%s",&cmd);
		if(cmd==ch)
		{
		obj.mst=MSGFORMATE;
		obj.plen=strlen(cmd);
		obj.pldata.paytype=TERM_SER_PROC;
		strcpy(obj.pldata.pvalue,cmd);
		num=send(sfd,&obj,sizeof(msgf),0);
		if(num==-1)
		{
		perror("C.send Error");
		exit(1);
		}
		close(sfd);
		}
		else
		{
		obj.mst=MSGFORMATE;
		obj.plen=strlen(cmd);
		obj.pldata.paytype=SHELL_COMMAND;
		strcpy(obj.pldata.pvalue,cmd);
		num=send(sfd,&obj,sizeof(msgf),0);
		if(num==-1)
		{
		perror("C.send Error");
		exit(1);
		}
		
		
		RecvPacket(sfd);
		SendPacket(sfd);
		}
	}









