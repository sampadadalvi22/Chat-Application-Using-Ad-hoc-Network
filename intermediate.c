/*
 * server.c
 */

#include <stdio.h>
#include <stdlib.h>

// Time function, sockets, htons... file stat
#include <sys/time.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>

// File function and bzero
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>

/* Size of the buffer used to send the file in several blocks
 */
#define BUFFERT 1024
#define PORT 3006


/* Declaration of functions*/
int duration (struct timeval *start,struct timeval *stop, struct timeval *delta);
int create_server_socket (int port);

struct sockaddr_in sock_serv,clt;

int create_client_socket (int port, char* ipaddr){
    int l;
	int sfd_c;
    
	sfd_c = socket(AF_INET,SOCK_DGRAM,0);
	if (sfd_c == -1){
        perror("socket fail");
        return EXIT_FAILURE;
	}
    
    //preparation of the address of the destination socket
	l=sizeof(struct sockaddr_in);
	bzero(&sock_serv,l);
	
	sock_serv.sin_family=AF_INET;
	sock_serv.sin_port=htons(port);
    if (inet_pton(AF_INET,ipaddr,&sock_serv.sin_addr)==0){
		printf("Invalid IP adress\n");
		return EXIT_FAILURE;
	}
    
    return sfd_c;
}

int main (int argc, char**argv){
    //descriptor
	int fd, sfd_s,sfd_c;
    
	char buf[BUFFERT];
	off_t count=0, n; // long type
	char filename[256];
    unsigned int l=sizeof(struct sockaddr_in);
	
    // Variable for the date
	time_t intps;
	struct tm* tmi;
    
	if (argc != 3){
		printf("Error usage : %s <ip_serv><port_serv>\n",argv[0]);
		return EXIT_FAILURE;
	}
    
    sfd_s = create_server_socket(atoi(argv[2]));
	//sfd_c=create_client_socket(atoi(argv[2]),argv[1]);
    
	intps = time(NULL);
	tmi = localtime(&intps);
	bzero(filename,256);

        //creating file name
	sprintf(filename,"clt.%d.%d.%d.%d.%d.%d",tmi->tm_mday,tmi->tm_mon+1,1900+tmi->tm_year,tmi->tm_hour,tmi->tm_min,tmi->tm_sec);
	printf("Creating the output file : %s\n",filename);
    
	//open file
	if((fd=open(filename,O_CREAT|O_WRONLY|O_TRUNC,0600))==-1){
		perror("open fail");
		return EXIT_FAILURE;
	}
    
	//preparation of the send
	bzero(&buf,BUFFERT);
   		n=recvfrom(sfd_s,&buf,BUFFERT,0,(struct sockaddr *)&clt,&l);
	//n=read(fd,buf,BUFFERT);
	while(n){
		printf("%lld of data received \n",n);
		if(n==-1){
			perror("read fails");
			return EXIT_FAILURE;
		}
		count+=n;
		write(fd,buf,n);
			sfd_c=create_client_socket(atoi(argv[2]),argv[1]);
			if(!(sendto(sfd_c,buf,n,0,(struct sockaddr*)&sock_serv,l)))
			{
				while(sendto(sfd_c,buf,n,0,(struct sockaddr*)&sock_serv,l))
				{
					break;
				}
			}
			close(sfd_c);
		//fprintf(stdout,"----\n%s\n----\n",buf);
		bzero(buf,BUFFERT);
        n=recvfrom(sfd_s,&buf,BUFFERT,0,(struct sockaddr *)&clt,&l);
	}
    
	printf("Number of bytes transferred: %lld \n",count);
    
    close(sfd_s);
    close(fd);
	return EXIT_SUCCESS;
}

/* Function allowing the calculation of the duration of the sending*/
int duration (struct timeval *start,struct timeval *stop,struct timeval *delta)
{
    suseconds_t microstart, microstop, microdelta;
    
    microstart = (suseconds_t) (100000*(start->tv_sec))+ start->tv_usec;
    microstop = (suseconds_t) (100000*(stop->tv_sec))+ stop->tv_usec;
    microdelta = microstop - microstart;
    
    delta->tv_usec = microdelta%100000;
    delta->tv_sec = (time_t)(microdelta/100000);
    
    if((*delta).tv_sec < 0 || (*delta).tv_usec < 0)
        return -1;
    else
        return 0;
}

/* Function allowing the creation of a socket and its attachment to the system
 * Returns a file descriptor in the process descriptor table
 * bind allows its definition in the system
 */
int create_server_socket (int port){
    int l;
	int sfd_s;
    
	sfd_s = socket(AF_INET,SOCK_DGRAM,0);
	if (sfd_s == -1){
        perror("socket fail");
        return EXIT_FAILURE;
	}
    
    //preparation of the address of the destination socket
	l=sizeof(struct sockaddr_in);
	bzero(&sock_serv,l);
	
	sock_serv.sin_family=AF_INET;
	sock_serv.sin_port=htons(port);
	sock_serv.sin_addr.s_addr=htonl(INADDR_ANY);
    
	//Assign an identity to the socket
	if(bind(sfd_s,(struct sockaddr*)&sock_serv,l)==-1){
		perror("bind fail");
		return EXIT_FAILURE;
	}
    
    
    return sfd_s;
}
