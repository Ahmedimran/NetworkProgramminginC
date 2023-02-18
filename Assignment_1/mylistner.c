/* ** server.c -- a stream socket server demo */ 
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


#define MYPORT 4500  
#define BACKLOG 10  
#define MAXDATASIZE 100 

int Socket(int,int,int);
struct sockaddr_in Filladdr();
void Bind(int, struct sockaddr_in);
void Listen(int);
void SetSocket(int);

//void Send(int,char[]);

void RecivePacket(int);
void sendpacket(int fd,char ip[],char port[]);
void Recivepacket(int fd,char ip[],char port[]);
void sendIDmsg(int fd);
void Recivemsgidrep(int fd,char ip[],char port[]);
void SendTerm(int fd);

int WriteLogFile(char *data);
void Display(char[]);


int main(void) 
{ 
	int sockfd, new_fd;  
	struct sockaddr_in their_addr;  
	int sin_size;
	char Port[16];
	char Ip[32];

	sockfd = Socket(AF_INET,SOCK_STREAM, 0);	
	SetSocket(sockfd);
	Bind(sockfd,Filladdr());
	Listen(sockfd);

	while (1) 
	{  
	     sin_size = sizeof(struct sockaddr_in);
             if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr,
							&sin_size))== -1)
             {
               	perror("accept");
               	continue;
             }
            printf("------------------------------------------\n");
            printf("server: got connection from %s\n",
                                               inet_ntoa(their_addr.sin_addr));
            snprintf(Ip,sizeof(Ip),"Client :%s ",
                                               inet_ntoa(their_addr.sin_addr));
            snprintf(Port,sizeof(Port)," Port %d",htons(their_addr.sin_port));

           
           RecivePacket(new_fd);
	   printf("\n------------------jj-----------------------\n");	
	  	
	   sendpacket(new_fd,Ip,Port);
	   printf("\n-----------------------------------------\n");		

            Recivepacket(new_fd,Ip,Port);
 	    printf("\n-----------------------------------------\n");		

            sendIDmsg(new_fd);
	    printf("\n-----------------------------------------\n");		

	   Recivemsgidrep(new_fd,Ip,Port);
	   printf("\n-----------------------------------------\n");		

	   SendTerm(new_fd);

           RecivePacket(new_fd);            
		      
	    close(new_fd); 
	} //end while loop

  close(sockfd); 

  return 0; 
} //end main


int Socket(int family,int type,int protocol)
{
     int des;
     if ((des = socket(family, type, protocol)) == -1)
     {
        perror("socket");
        exit(1);
     }
   return des;
}

struct sockaddr_in Filladdr()
{
   struct sockaddr_in m_addr;                              
   m_addr.sin_family = AF_INET;
   m_addr.sin_port = htons(MYPORT);
   m_addr.sin_addr.s_addr = INADDR_ANY;
   memset(&(m_addr.sin_zero),'\0', 8);
   return m_addr;                                                        
}
 
void Bind(int socket, struct sockaddr_in my_addr)
{
 if(bind(socket, (struct sockaddr *)&my_addr,sizeof(struct sockaddr)) ==-1)
  {
     perror("bind");
     exit(1);
  }                                                                       
}                                                                                                 
void Listen(int fd)
{
  if (listen(fd,BACKLOG) == -1)
  {
    perror("listen");
    exit(1);
  }                                                                            
}

void SetSocket(int socket)
{
  int yes=1;
  if (setsockopt(socket,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1)
  {
     perror("setsockopt");
     exit(1);
  }                                                                      
}                                                                                         
/////////////////////////////Socket API End Here//////////////////////////////
////////////////////////////User Define Functions Start Here/////////////////
void sendpacket(int fd,char IP[],char port[])
{
  char buffer[50];
  char *msg1="Yes WelCome:";
	strcpy(buffer,msg1);
	strcat(buffer,IP);
	strcat(buffer,port);

  int len,byt_send;
  len=strlen(buffer);
 byt_send=send(fd,buffer,len,0);
 if(byt_send==-1)
   {
     perror("msg1 send error");
     exit(1);
     
   }

}

void RecivePacket(int socket)
{
        int size;
        char buf[MAXDATASIZE];
        if ((size=recv(socket, buf,MAXDATASIZE, 0)) == -1)
        {
                perror("recv");
                exit(1);
        }
         buf[size] = '\0';
	 WriteLogFile(buf);
         Display(buf);
                                                            
}

void Recivepacket(int socket,char IP[],char port[])
{
        int size;
	char *mesg="FirstDataMessage:";
	char buffer[120];
        char buf[MAXDATASIZE];
        if ((size=recv(socket, buf,MAXDATASIZE, 0)) == -1)
        {
                perror("recv");
                exit(1);
        }

         buf[size] = '\0';
	strcpy(buffer,mesg);
	strcat(buffer,IP);
	strcat(buffer,port);
	strcat(buffer,buf);
	 WriteLogFile(buffer);
         Display(buffer);
                                                            
}


/////////////////////////////////////////
void sendIDmsg(int fd)
{
  char *fmsg="IdMsg:Your first data message is recived Please Send name and ID:";
  int len,bytlen;
  len=strlen(fmsg);
  bytlen=send(fd,fmsg,len,0);
  if(bytlen==-1)
    {
      perror("Error in formal message send");
      exit(1);
    }

}

void Recivemsgidrep(int fd,char IP[],char port[])
{
  char *fmsg="IDmsgRep:";
  int len,bytlen;
   char buffer[150];
	char buf[MAXDATASIZE];
  len=strlen(fmsg);

  bytlen=recv(fd,buf,MAXDATASIZE,0);
  if(bytlen==-1)
    {
      perror("Error in formal message send");
      exit(1);
    }
	strcpy(buffer,fmsg);
	strcat(buffer,IP);
	strcat(buffer,port);
	strcat(buffer,buf);
	WriteLogFile(buffer);
	Display(buffer);

}

void SendTerm(int fd)
{
  	char *msgT="TermMsg:Your name and Id logon Listner,Comm Terminating";
        int size;
        if ((size=send(fd, msgT,strlen(msgT), 0)) == -1)
        {
                perror("Error Termination Sending");
                exit(1);
        }
         printf("\nConnection is Terminated::....\n");
	 exit(0);
                         

}

/////////////////////////////////Write Log File/////////////////////////////

void Display(char data[])
{
	printf("%s\n",data);
}


int WriteLogFile(char *Buffer)

{

  FILE *stream;
  int RetVal;
  if( (stream = fopen( "TalkerMsgLog.txt", "a+t" )) == NULL )
{
      perror("Log File Could not be opened");
      return 0;
  }

  else

	{
       
       if ((RetVal = fwrite( Buffer , sizeof( Buffer ), strlen(Buffer) , stream) <=0 )){
             perror("Could not write string to Log file \n");
             return 0;
       }

       fclose( stream );   // Closing the file Descriptor
       return (1);
  }           
  }

/*void WriteLogFile(char data[])
{
  FILE *file;
  if((file=fopen("TalkerMsgLog.txt","a+"))==NULL)
    perror("file");
  else
  {
   fseek(file,0,SEEK_END);
   fputs(data,file); 
  } 
  fclose(file);  
}*/
