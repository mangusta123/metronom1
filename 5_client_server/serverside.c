/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>


#define PORT "3490" // the port users will be connecting to
#define BACKLOG 10  // how many pending connections queue will hold
#define MAXDATASIZE 100 

int i,j;
char *ch;

int fsig = 6;
int ssig = 8;

int loopnum = 6;                // default value

int measures = 0;

int sleepvalue = 125000;        // default value


int P[] = {7, 1, 1, 3, 1, 1};   // default value                       


                                // = {7, 1, 1, 3, 1, 1};    this is for signature 6/8    loopnum = 6

                                // = {7, 1};                this is for signature 2/4    loopnum = 2                 

                                // = {7, 1, 1};             this is for signature 3/4    loopnum = 3

                                // = {7, 1, 3, 1};          this is for signature 4/4    loopnum = 4
 

char  wr_buf[16];

void sigchld_handler(int s)
{
while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
if (sa->sa_family == AF_INET) {
return &(((struct sockaddr_in*)sa)->sin_addr);
}
return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int main(int argc, char **argv)
{
int sockfd, new_fd, numbytes; // listen on sock_fd, new connection on new_fd
char buf[MAXDATASIZE];

int fd;
int rc = 0;
int wc = 0;

struct addrinfo hints, *servinfo, *p;
struct sockaddr_storage their_addr; // connector's address information
socklen_t sin_size;
struct sigaction sa;
int yes=1;
char s[INET6_ADDRSTRLEN];
int rv;
memset(&hints, 0, sizeof hints);

hints.ai_family = AF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;
hints.ai_flags = AI_PASSIVE; // use my IP
if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
return 1;
}

// loop through all the results and bind to the first we can
for(p = servinfo; p != NULL; p = p->ai_next) {
if ((sockfd = socket(p->ai_family, p->ai_socktype,
p->ai_protocol)) == -1) {
perror("server: socket");
continue;
}
if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
sizeof(int)) == -1) {
perror("setsockopt");
exit(1);
}
if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
close(sockfd);
perror("server: bind");
continue;
}
break;
}


if (p == NULL) {
fprintf(stderr, "server: failed to bind\n");
return 2;
}

freeaddrinfo(servinfo); // all done with this structure

if (listen(sockfd, BACKLOG) == -1) {
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

printf("server: waiting for connections...\n");

printf("%s: entered\n", argv[0]);

// Open the device

fd = open("/dev/use_hellodev2", O_RDWR);

if (fd == -1) {
perror("open failed");
rc = fd;
exit(-1);
}

printf("%s: open successful\n", argv[0]);



while(1) { // main accept() loop
sin_size = sizeof their_addr;
new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
if (new_fd == -1) {
perror("accept");
continue;
}

inet_ntop(their_addr.ss_family,
get_in_addr((struct sockaddr *)&their_addr),
s, sizeof s);
printf("server: got connection from %s\n", s);

if (!fork()) { // this is the child process

close(sockfd); // child doesn't need the listener

do
{

for(i=0; i<MAXDATASIZE; i++)
buf[i] = 0;

if ((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1) {
perror("recv");
exit(1);
}

buf[numbytes] = '\0';
printf("server: received '%s'\n",buf);


//here do actions based on received command

ch = strtok(buf, " "); 

if(strcmp(ch, "tempo")==0)

   {
    ch = strtok(NULL, " "); 
 
    sleepvalue = 1000*1000*120/(atoi(ch)*ssig);

//    sleepvalue = 1000*1000*30/atoi(ch);
   
    
//tempo - beats per 60 sec

//duration of on/off period is the same 

//beats per second = tempo / 60
    
//seconds per beat = 60 / tempo sec

//beat = on + off together

//on period = off period = 60 / tempo / 2 sec

//


   
   }//end tempo

else if(strcmp(ch, "time-signature")==0)
   {

     ch = strtok(NULL, " /");
     fsig = atoi(ch);
     ch = strtok(NULL, " /");
     ssig = atoi(ch);

     if(fsig==2 && ssig==4)

        {
        
         loopnum = 2;

         P[0] = 7;
         P[1] = 1; 

        }
      

else if(fsig==3 && ssig==4)
       
        {
         
         loopnum = 3;

         P[0] = 7;
         P[1] = 1;
         P[2] = 1;

        }


else if(fsig==4 && ssig==4)

        {
          
         loopnum = 4;
        
         P[0] = 7;
         P[1] = 1;
         P[2] = 3;
         P[3] = 1;
        
        }

else if(fsig==6 && ssig==8)

        {

         loopnum = 6;
                 
         P[0] = 7;
         P[1] = 1;
         P[2] = 1;
         P[3] = 3;
         P[4] = 1;
         P[5] = 1;

        }      

   }//end time-signature

else if(strcmp(ch, "start")==0)

   {
   
    ch = strtok(NULL, " "); 
  
    measures = atoi(ch);
    
   for(j=0; j<measures; j++){

   for(i=0; i<loopnum; i++){


    switch (P[i])

     {
       case 7:
              wr_buf[0] = '7';
              write(fd, wr_buf, 0);

              printf("strong\n");

              break;

       case 3:
              wr_buf[0] = '3';
              write(fd, wr_buf, 0);
              
              printf("medium\n");

              break;
       
       case 1:
              wr_buf[0] = '1';
              write(fd, wr_buf, 0);
            
              printf("weak\n");

              break;

     }
    

     //sleep here
	usleep(sleepvalue);
     wr_buf[0] = '0';
     write(fd, wr_buf, 0);
     printf("off\n");   

	usleep(sleepvalue);
     //sleep here
     

}//end i
}//end repeat j
    

   }//end start


}while(strcmp(buf, "exit")!=0);


close(new_fd);
exit(0);
}
close(new_fd); // parent doesn't need this
}

close(fd);
return 0;
}



