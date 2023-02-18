/////////////////////////////////////Imran Ahmed///////////////////////////////////////////////
/////////////////////////////////////F-06-072///////////////////////////////////////////////
/////////////////////////////////////Assignment#3////////////////////////////////////////////



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT 5900 // the port client will be connecting to 

#define MAXDATA 100 // max number of bytes we can get at once



#define MSGFORMATE	     0x0010

#define WELCOME_MESSAGE      0x00
#define REQUEST_FOR_USERNAME 0x11
#define USERNAME	     0x12	
#define REQUEST_FOR_PASSWORD 0x13
#define PASSWORD	     0x14
#define LOGIN_SUCCESS	     0x15	
#define LOGIN_FAILURE	     0x16
#define CMD_EXEC	     0x17	
#define CMD_EXEC_SUCCESS     0x18
#define CMD_RESULT	     0x19
#define CMD_RESULT_END	     0x20	

///////////////////////////////////////////////////////////////////////////////////

void RecvPacket(int fd);
void RecvRPacket(int fd);
void SendUName(int fd);
void SendUPass(int fd);
void SendCommand(int fd);
void RecvSuc(int fd);
void RResult(int fd);
void Ecmdr(int fd);
//void SendPacket(int fd);

///////////////////////////////////////////////////////////////////////////////////
		typedef struct
			{
			uint8_t paytype;		
			char pvalue[2240];
			}plt;

		typedef struct
			{
			uint8_t mst;
			uint16_t plen;
			plt pldata;	
			
			}msgf;
/////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    int sockfd, numbytes, fnsize=0;
    char buf[MAXDATA];
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
	
	
	RecvPacket(sockfd);//wellcome message  ck
	
	RecvPacket(sockfd);//username request and send username  ck

	RecvPacket(sockfd);//password request and send password ck

	RecvPacket(sockfd);//recv login successfull or failuiar
	
	SendCommand(sockfd);// send users commands
	
	
    close(sockfd);

    return(0);
}

void RecvPacket(int ssock)
	{
		msgf object;
		int num;
		char buf[MAXDATA];
		if((num=recv(ssock,&object,sizeof(object),0))==-1)
		{
		perror("Recive Error");
		}
	
		
		if (object.mst==MSGFORMATE)
		{
			
			if(object.pldata.paytype == WELCOME_MESSAGE)
			{
			printf("\n%s\n",object.pldata.pvalue);
			}
			
			else if (object.pldata.paytype == REQUEST_FOR_USERNAME)
			{
			SendUName(ssock);
			}
			else if(object.pldata.paytype==REQUEST_FOR_PASSWORD)
			{
			SendUPass(ssock);
			
			}
			
			else if(object.pldata.paytype==LOGIN_SUCCESS)
			{
			printf("\n------------%s----------\n",object.pldata.pvalue);
			}
			else if(object.pldata.paytype == LOGIN_FAILURE)
			{
			printf("\n------------%s----------\n",object.pldata.pvalue);
			exit(0);
			//close(ssock);
			
			}
			
		}
		

	}
	
void SendUName(int unfd)	
	{
		msgf uname;
		int numb;
		char username[20];
		

		printf("\n-Please Enter Your User Name-]:");

		scanf("%s",username);
			
		uname.mst=MSGFORMATE;
		uname.pldata.paytype=USERNAME;
		strcpy(uname.pldata.pvalue,username);
		uname.plen=strlen(username);
		if(numb=send(unfd,&uname,sizeof(msgf),0)==-1)
		{
		perror("Username Send Error");
		exit(1);
		}		
	}
	
	
void SendUPass(int unfd)	
	{
		msgf upass;
		int numb;
		char upas[10];
		
		printf("\n-Please Enter Your Password-]:");
		
		scanf("%s",upas);
		upass.mst=MSGFORMATE;
		upass.pldata.paytype=PASSWORD;
		strcpy(upass.pldata.pvalue,upas);
		upass.plen=strlen(upas);
		
		if(numb=send(unfd,&upass,sizeof(upass),0)==-1)
		{
		perror("User Password Send Error");
		exit(1);
		}		
	}
	
//////////////////////////////////////Writte command here////////////////////////////////
void SendCommand(int sfd)
	{
		msgf obj;
		char cmd[15];
		int num,or;
		
		

		printf("\n-------Enter The Command---]:");
		
		
		scanf("%s",cmd);
		
		obj.mst=MSGFORMATE;
		obj.plen=strlen(cmd);
		obj.pldata.paytype=CMD_EXEC;
		strcpy(obj.pldata.pvalue,cmd);
		
		num=send(sfd,&obj,sizeof(msgf),0);
		if(num==-1)
		{
		perror("Command.send Error");
		exit(1);
		}
		
		
		RecvSuc(sfd);
		RResult(sfd);
		Ecmdr(sfd);
		SendCommand(sfd);

	}


void RecvSuc(int sffd)
{
		msgf sucobj;
		int num;
		char buf[MAXDATA];
		
		if((num=recv(sffd,&sucobj,sizeof(msgf),0))==-1)
		perror("Recive Error");
		
			
		if (sucobj.mst==MSGFORMATE)
		{
					
			printf("---Command Execute SuccessFully----\n");
			
		}

}

void RResult(int rfd)
{
		msgf robj;
		int num;
		char buf[1024];
		
		if((num=recv(rfd,&robj,sizeof(msgf),0))==-1)
		perror("Recive Error");
		
			
		if (robj.mst==MSGFORMATE)
		{
			if(robj.pldata.paytype==CMD_RESULT)
			{
			printf("\n-------Result show here----------\n");
			printf("\n%s\n",&robj.pldata.pvalue);
			}

			
		}
	}

void Ecmdr(int endfd)
{
		msgf eobj;
		int num;
		char buf[MAXDATA];
		
		if((num=recv(endfd,&eobj,sizeof(msgf),0))==-1)
		perror("Recive Error");

		if (eobj.mst==MSGFORMATE)
		{
			if(eobj.pldata.paytype==CMD_RESULT_END)
			{
					
			printf("\n----------Command Ended------------\n");
			}
		}
}





