/*
** server.c - a stream socket server demo
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
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
#include <time.h>

#define MYPORT 5900 // the port users will be connecting to
#define BACKLOG 10 // how many pending connections queue will hold
#define MAXDATA 100
////////////////////////////////////////////////////////////////////////////////////
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


////////////////////////////////////////////////////////////////////////////////////
//int max(int fd,int ufd);
void Load(void);
void FillPacket(int fd,char ip[],char port[]);
void RecvPacket(int fd);
void SendRequest(int fd);

void CheckU(int fd,char data[]);
void CheckP(int fd,char data[]);
void Success(int fd);
void Fail(int fd);
void Command(int fd,char ip[]);
void SendS(int fd);
void FillPacketP(int fd);

void ReadFromFile(int fd);
void Results(int fd,char data[]);

void Ecmd(int fd);


char cname[20];
char ipp[32];
//////////////////////Message Format Structures//////////////////////////////

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
	typedef struct 
			{
			char UserName[20];
			char Password[10];
			
			}TelenetUserList;
	TelenetUserList TELobj[10];	
//////////////////////////////////////////////////////////////////


void sigchld_handler(int s)
{
while(wait(NULL) > 0);
}

int main(void)
{
	int listenfd,connfd,udpfd,nready,maxfdp1; // listen on sock_fd, new connection on new_fd
	struct sockaddr_in my_addr; // my address information
	struct sockaddr_in their_addr; // connectors address information
	pid_t childpid;
	int sin_size;
	fd_set rset;
	ssize_t n;
	socklen_t len;
	struct sigaction sa;
	int on=1;
	int numbytes;

	char Port[16];
	char rbuf[2];
	char Ip[32];
	char tbuf[MAXDATA];
	time_t ticks;
	Load();
///////////////////////////////TCP socket////////////////////////////////////////
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
		{
			perror("TCP socket Error");
			exit(1);
		}
		
	my_addr.sin_family = AF_INET; // host byte order
	my_addr.sin_port = htons(MYPORT); // short, network byte order
	my_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
	memset(&(my_addr.sin_zero), '\0', 8); // zero the rest of the struct
		
	if (setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(int)) == -1) 
		{
			perror("setsockopt");
			exit(1);
		}
	if (bind(listenfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr))== -1)
		{
		perror("bind");
		exit(1);
		}
	if(listen(listenfd, BACKLOG) == -1)
		{
		perror("listen");
		exit(1);
		}
///////////////////////////////UDP socket////////////////////////////////////////
	if ((udpfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
		{
			perror("UDP socket Error");
			exit(1);
		}
		

	my_addr.sin_family = AF_INET; // host byte order
	my_addr.sin_port = htons(MYPORT); // short, network byte order
	my_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
	memset(&(my_addr.sin_zero), '\0', 8); // zero the rest of the struct
	
	if(bind(udpfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr))== -1)
	 {
		perror("bind");
		exit(1);
	}
	
		sa.sa_handler = sigchld_handler; // reap all dead processes
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = SA_RESTART;
		if (sigaction(SIGCHLD, &sa, NULL) == -1) 
		{
			perror("sigaction");
			exit(1);
		}
	
		FD_ZERO(&rset);
		
///////////////////////////////*Select Function call Here///////////////////////////////////////
while(1) 
    { 
    	FD_SET(listenfd,&rset);
    	FD_SET(udpfd,&rset);
    	
    	if ((nready=select(FD_SETSIZE,&rset,NULL,NULL,NULL))<0)
    	{
    	if(errno==EINTR)
    	continue;
    	else
	  perror("Select Error");
	  printf("-----------------------");
    	}  
    	if(FD_ISSET(listenfd,&rset))
    	{
    	
       		len=sizeof(their_addr);
    		if((connfd = accept(listenfd, (struct sockaddr *)&their_addr,&len)) ==-1)
 		{
			  perror("accept ka error");
                  	  continue;       
		 }
		printf("server: got connection from %s\n",inet_ntoa(their_addr.sin_addr));
		snprintf(Ip,sizeof(Ip),"IP.%s",inet_ntoa(their_addr.sin_addr));
		snprintf(Port,sizeof(Port),".%d",htons(their_addr.sin_port));
		strcpy(ipp,Ip);
	
	if(!fork())
		{
		close(listenfd);
		
		
		FillPacket(connfd,Ip,Port);	//ck
		SendRequest(connfd);///send request for username ck
		
		RecvPacket(connfd);/////Receive username and send Request for password ck
		
		RecvPacket(connfd);
		
		Command(connfd,Ip);
		
		
		exit(0);			
		}
		close(connfd);               
    	}
    	
    	
	if(FD_ISSET(udpfd,&rset))
	    	{
	    	/*len=sizeof(struct sockaddr);
	    	
		if((numbytes=recvfrom(udpfd,rbuf,MAXDATA,0,(struct sockaddr *)&their_addr,&len)==-1)
		{
		perror("rfrom error");
		exit(1);
		}
		printf("We revform");
	    	*/
	    	ticks=time(NULL);

    		strcpy(tbuf,ctime(&ticks));
		    	
	
	if((numbytes=sendto(udpfd,tbuf,sizeof(tbuf),0,(struct sockaddr *)&their_addr,sizeof(struct sockaddr)))==-1)
    		{
    		perror("Sendto Erro");
    		exit(1);
    		}
    		
    		close(udpfd);
    		}
    	
   } 
  
return 0;
}
///////////////////////////////////////////////End of Main///////////////////////////////////////////////
////////////////////////////////////Wellcome Messend Send this fun ///////////////////////////////////
void FillPacket(int tfd,char ip[],char port[])
	{
		
		
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
	
		
		numbytes=send(tfd, &obj, sizeof(msgf),0);
		if(numbytes==-1)
		{
		perror("Send Fill Error Filpack");
		exit(1);
		}
		
	}
/////////////////////////REquest for user name/////////////////////////////////
void SendRequest(int csock)
	{
		
		msgf obj1;
		int num;
		obj1.mst=MSGFORMATE;
		obj1.pldata.paytype=REQUEST_FOR_USERNAME;
		strcpy(obj1.pldata.pvalue,"");
		obj1.plen=0;
		
		if ((num=send(csock, &obj1, sizeof(obj1),0))==-1)
		{
		perror("Send Fill Error SendReq");
		exit(1);
		}
		
}
/////////////////////////////////////////REcv UserName And Password/////////////////////////
void  RecvPacket(int tcpfd)
	{
		msgf user;
//		TelenetUserList obj[]
		int num,i;
		char uname[20];
		char buff[MAXDATA];
		char upass[10];

	
		if((num=recv(tcpfd, (struct msgf*)&user, sizeof(user),0))==-1)
		{
			perror("User Name recv Error");
			exit(1);
		}
		
		
		if(user.mst==MSGFORMATE)
			{
			if(user.pldata.paytype==USERNAME)
				{
				
				strcpy(uname,user.pldata.pvalue);
				
				CheckU(tcpfd,uname);
				
				}
			if(user.pldata.paytype==PASSWORD)
				{
				
				strcpy(upass,user.pldata.pvalue);
				CheckP(tcpfd,upass);
				}
			}
	
	}
/////////////////////////////////////////UserName and Password Filling//////////////////////////////


void Load(void)
{
FILE *file;

 int n;
 int nn;

	
      if((file=fopen("UserFile.bin","r"))==NULL)
  	  printf("File Not Open\n");
  	 else
   	{
    	 while(fread(&TELobj[n],sizeof(TELobj[n]),1,file)==1)
   	  n++;
	 fclose(file);    
	}
	/*for(nn=0;nn<6;nn++)
	{
	printf("\n%s::%s",TELobj[nn].UserName,TELobj[nn].Password);
	}*/
	
}

void CheckU(int desc,char match[])
{
		
	int i=0;
	char tuser[20];
	while(i<6)
	{
	strcpy(tuser,TELobj[i].UserName);
	
	if(strcmp(tuser,match)==0)
		{
		printf("------------User Match--------------\n");
		strcpy(cname,match);
		FillPacketP(desc);
		}
	i++;
	}

	
	
}

void CheckP(int desc,char match[])
{
		
	int p=0;
	char pass[10];
	while(p<6)
	{
	strcpy(pass,TELobj[p].Password);
	if(strcmp(pass,match)==0)
		{
		printf("-----------Password Match-----------\n");

		Success(desc);/////////////command execute form here
		break;	
		}
		p++;
	}

//	Fail(desc);
}

void FillPacketP(int fd)
{
	msgf pass;
	int num;
	pass.mst=MSGFORMATE;
	pass.pldata.paytype=REQUEST_FOR_PASSWORD;
	strcpy(pass.pldata.pvalue,"");
	pass.plen=0;
	
	if(num=send(fd,&pass,sizeof(msgf),0)==-1)
	{
	perror("Pass Send Error");
	exit(1);
	}
	
}

void Success(int succ)
{
	msgf suc;
	int num;
	suc.mst=MSGFORMATE;
	suc.pldata.paytype=LOGIN_SUCCESS;
	strcpy(suc.pldata.pvalue,"Login Succes Hogia");
	suc.plen=strlen(suc.pldata.pvalue);
	
	if((num=send(succ,&suc,sizeof(msgf),0))==-1)
	{
	perror("Success Send Error");
	exit(1);
	}
	
}


void Fail(int fail)
{
	msgf fa;
	int num;
	
	fa.mst=MSGFORMATE;
	fa.pldata.paytype=LOGIN_FAILURE;
	strcpy(fa.pldata.pvalue,"Login Fail ho gia");
	fa.plen=strlen(fa.pldata.pvalue);
	
	if((num=send(fail,&fa,sizeof(msgf),0))==-1)
	{
	perror("Fail Send Error");
	exit(1);
	}

}
//////////////////////////////////////USer Command Executions////////////////////////////////////
void Command(int fd,char ip[])
{
			msgf object;
			int numbytes,numby,numb;
			char *path="/bin/";	
			char buf[15];
			char buf2[20];
			
			if(numbytes=recv(fd,&object,sizeof(msgf),0)==-1)
			perror("Command Recv Error");
			
		if(object.mst==MSGFORMATE)
		{
			
			if(object.pldata.paytype == CMD_EXEC)
			{
			printf("\n[%s<%s>]:%s\n",cname,ip,object.pldata.pvalue);
									
			strcpy(buf,path);
			strcat(buf,object.pldata.pvalue);
			
			strcpy(buf2,path);
			strcat(buf2,object.pldata.pvalue);
			strcat(buf2,">");
			strcat(buf2,"mytel.temp");
			
			system(buf2);
			
			if(!fork())
			{
			if((numby=execl(buf,object.pldata.pvalue,0))==-1)

				{
				perror("command Execl Error");
				exit(1);
				}
			}
		}
	}
	
		SendS(fd);
		ReadFromFile(fd);
		Ecmd(fd);
		
		Command(fd,ip);

}
///////////////////////////Send Notification For Successful Execution of the command///////////////////
void SendS(int sd)
	{
		msgf objt;
		int numb;
		
		objt.mst=MSGFORMATE;
		objt.pldata.paytype=CMD_EXEC_SUCCESS;
		strcpy(objt.pldata.pvalue,"");
		objt.plen=0;
		numb=send(sd,&objt,sizeof(objt),0);
		if(numb==-1)
		{
		perror("SendSuc Send Error");
		
		}
		
	}

void ReadFromFile(int fd)
{
 FILE *file;
 char ch;
 char chf[2480];

 int in=0;


   if((file=fopen("mytel.temp","r"))==NULL)
   {
      printf("File Not Open\n");
      printf("----------------------\n");
      exit(1);
   }
   else
   {
     while(!feof(file))
     {
       ch=fgetc(file);
       chf[in++]=ch;
     }
     fclose(file);
     
   }
   

	Results(fd,chf);
}

void Results(int des,char data[])
	{
	msgf res;
	int n;
	int len;
	res.mst=MSGFORMATE;
	res.pldata.paytype=CMD_RESULT;
	
	strcpy(res.pldata.pvalue,data);
	res.plen=strlen(data);
	n=send(des,&res,sizeof(msgf),0);
	if(n==-1)
	{
	perror("Result Send Error");
	exit(1);
	}
	
	
}

void Ecmd(int ecd)
{
	msgf ec;
	int n;
	ec.mst=MSGFORMATE;
	ec.pldata.paytype=CMD_RESULT_END;
	strcpy(ec.pldata.pvalue,"");
	ec.plen=0;
	n=send(ecd,&ec,sizeof(msgf),0);
	if(n==-1)
	{
	perror("Result Send Error");
	exit(1);
	}	

}

