/* ** client.c -- a stream socket client demo */ 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <errno.h> 
#include <string.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/time.h> 
#include <time.h>

#define PORT 4500  
#define MAXDATASIZE 100  

int Socket(int,int,int);
void Connect(int,struct sockaddr_in,socklen_t);
struct sockaddr_in Filladdr(struct hostent *);
struct hostent *CheckHostName(char *);
void CheckArgument(int);

void sendpacket(int);
void RecivePacket(int);
void sendfmsg(int);
void sendmsgidrep(int);
void ReciveTerm(int);

int WriteLogFile(char*);
void Display(char []);


int main(int argc,char *argv[]) 
{ 
      int sockfd,loop=0; 
      struct timeval tv_send, tv_recv; 
     
            
      CheckArgument(argc);
      sockfd = Socket(AF_INET, SOCK_STREAM, 0);
      Connect(sockfd,Filladdr(CheckHostName(argv[1])),sizeof(struct sockaddr) );

      printf("------------------------------------------\n");
      sendpacket(sockfd);
      printf("-------------------------------------------\n");  	

      RecivePacket(sockfd);
      printf("-------------------------------------------\n");  	

      sendfmsg(sockfd);
      printf("-------------------------------------------\n");  	

      RecivePacket(sockfd);
      printf("-------------------------------------------\n");  	

      sendmsgidrep(sockfd);
      printf("-------------------------------------------\n");  	

      ReciveTerm(sockfd);
      printf("-------------------------------------------\n");  	

           
 
   close(sockfd); 
  return 0; 
}

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

void Connect(int fd,struct sockaddr_in servaddr ,socklen_t addrlen)
{
     if(connect(fd, (struct sockaddr *)&servaddr,addrlen) == -1)
      {
                perror("connect");
                exit(1);
      }
}

struct sockaddr_in Filladdr(struct hostent *he1)
{
  
        struct sockaddr_in their_addr;
        their_addr.sin_family = AF_INET;
        their_addr.sin_port = htons(PORT);
        their_addr.sin_addr = *((struct in_addr *)he1->h_addr);
        memset(&(their_addr.sin_zero),'\0', 8);                          
        return their_addr;
}

struct hostent *CheckHostName(char *name)
{
        struct hostent *he;
        if ((he=gethostbyname(name)) == NULL)
        {
                perror("gethostbyname");
                exit(1);
        }
        return he;
}


void CheckArgument(int arg)
{
        if (arg != 2)
        {
                fprintf(stderr,"usage: client hostname Port\n");
                exit(1);
        }                                                                 
}

////////////////////////////system arguments end here////////////////////////////////
/////////////////////////////User Define Functions are here////////////////////////
void sendpacket(int fd)
{
  char *msg1="Is there some listner exisist on this port";
  int len,byt_send;
  len=strlen(msg1);
 byt_send=send(fd,msg1,len,0);
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


/////////////////////////////////////////
void sendfmsg(int fd)
{
  char *fmsg="This is first Formal data message from talker";
  int len,bytlen;
  len=strlen(fmsg);
  bytlen=send(fd,fmsg,len,0);
  if(bytlen==-1)
    {
      perror("Error in formal message send");
      exit(1);
    }

}

void sendmsgidrep(int fd)
{
  char *fmsg="IDmsgRep:Imran Ahmed Roll#F-06-072";
  int len,bytlen;
  len=strlen(fmsg);
  bytlen=send(fd,fmsg,len,0);
  if(bytlen==-1)
    {
      perror("Error in formal message send");
      exit(1);
    }

}

void ReciveTerm(int fd)
{
  
        int size;
        char buf[MAXDATASIZE];
        if ((size=recv(fd, buf,MAXDATASIZE, 0)) == -1)
        {
                perror("recv");
                exit(1);
        }
         buf[size] = '\0';
	 WriteLogFile(buf);
         printf("\nConnection is Terminated::....\n");
	 exit(0);
                         

}

/////////////////////////////////Write Log File/////////////////////////////

void Display(char data[])
{
	printf("%s\n",data);
}


int WriteLogFile(char*Buffer)

{

  FILE *stream;
  int RetVal;
  if( (stream = fopen( "ListenerMsgLog.txt", "a+t" )) == NULL )
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


/*  void WriteLogFile(char data[])
{
  FILE *file;
  if((file=fopen("ListnerMsgLog.txt","a+"))==NULL)
    perror("file");
  else
  {
   fseek(file,0,SEEK_END);
   fputs(data,file); 
  } 
  fclose(file);  
}*/

