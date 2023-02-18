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


///////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    int sockfd, len,num,snum;
    char buf[MAXDATA];
        char *sbuf="";
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

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) 
    {
        perror("socket");
        exit(1);
    }

    their_addr.sin_family = AF_INET;    // host byte order 
    their_addr.sin_port = htons(PORT);  // short, network byte order 
    their_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(their_addr.sin_zero), '\0', 8);  // zero the rest of the struct 
    
  	/* if((snum=sendto(sockfd,sbuf,MAXDATA,0,(struct sockaddr*)&their_addr,sizeof(struct sockaddr))==-1)
   	{
    	 perror("Sento error");
    	 exit(1);
  	 }*/

	len=sizeof(struct sockaddr);
	if((num=recvfrom(sockfd,buf,MAXDATA,0,(struct sockaddr *)&their_addr,&len))==-1);
	{
	perror("rec error");
	}
	printf("\n%s\n",buf);

    close(sockfd);

    return(0);
}
