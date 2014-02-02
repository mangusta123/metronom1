/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT "3490" // the port client will be connecting to
#define MAXDATASIZE 100 // max number of bytes we can get at once

char inputstr[30]; 
char *ch;

int fsig = 6;
int ssig = 8;

char fs[2];
char ss[2];
char tm[5];
char ms[5];
int  tempv;


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
if (sa->sa_family == AF_INET) {
return &(((struct sockaddr_in*)sa)->sin_addr);
}
return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
int sockfd, numbytes;
char buf[MAXDATASIZE];
struct addrinfo hints, *servinfo, *p;
int rv;
char s[INET6_ADDRSTRLEN];

if (argc != 2) {
fprintf(stderr,"usage: client hostname\n");
exit(1);
}

memset(&hints, 0, sizeof hints);
hints.ai_family = AF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;
if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
return 1;
}
// loop through all the results and connect to the first we can
for(p = servinfo; p != NULL; p = p->ai_next) {
if ((sockfd = socket(p->ai_family, p->ai_socktype,
p->ai_protocol)) == -1) {
perror("client: socket");
continue;
}
if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
close(sockfd);
perror("client: connect");
continue;
}
break;
}
if (p == NULL) {
fprintf(stderr, "client: failed to connect\n");

return 2;
}
inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
s, sizeof s);
printf("client: connecting to %s\n", s);
freeaddrinfo(servinfo); // all done with this structure

do
{

printf("Enter a command: \n");

scanf(" %[^\n]", inputstr);    // get input string with whitespaces

ch = strtok(inputstr, " ");    // get first token up to first whitespace

if(strcmp(ch, "tempo")==0)

       {
          ch = strtok(NULL, " ");        

if(ch!=NULL)  {

        if(atoi(ch)<30 || atoi(ch)>180)
            
           printf("wrong tempo value: %d. should be between 30 and 180\n", atoi(ch));

      else 
           {  sprintf(tm, "%d", atoi(ch));                       

              printf("Got: %s\n", inputstr); 

              strcat(inputstr, " ");
              strcat(inputstr, tm);

              if (send(sockfd, inputstr, strlen(inputstr), 0) == -1)
              perror("send");
              
           }

              } 
else
      printf("no tempo value specified. please specify tempo within range [30..180]\n");


       }

else if(strcmp(ch, "time-signature")==0)

       {
          ch = strtok(NULL, " /");



if(ch!=NULL)
      
       {
        

        fsig = atoi(ch);

        ch = strtok(NULL, " /");

       

        ssig = atoi(ch);


     if(fsig==2 && ssig==4)

        {
        
         printf("Got: %s\n", inputstr);
         
         sprintf(fs, "%d", fsig);         
         sprintf(ss, "%d", ssig);

         strcat(inputstr, " ");         
         strcat(inputstr, fs);
         strcat(inputstr, "/");
         strcat(inputstr, ss);

         if (send(sockfd, inputstr, 18, 0) == -1)
         perror("send");

        }
      

else if(fsig==3 && ssig==4)
       
        {
         
         printf("Got: %s\n", inputstr);

         sprintf(fs, "%d", fsig);         
         sprintf(ss, "%d", ssig);

         strcat(inputstr, " ");         
         strcat(inputstr, fs);
         strcat(inputstr, "/");
         strcat(inputstr, ss);

         if (send(sockfd, inputstr, 18, 0) == -1)
         perror("send");

        }


else if(fsig==4 && ssig==4)

        {
         printf("Got: %s\n", inputstr);
         
         sprintf(fs, "%d", fsig);         
         sprintf(ss, "%d", ssig);

         strcat(inputstr, " ");         
         strcat(inputstr, fs);
         strcat(inputstr, "/");
         strcat(inputstr, ss);

         if (send(sockfd, inputstr, 18, 0) == -1)
         perror("send");

        }

else if(fsig==6 && ssig==8)

        {
         printf("Got: %s\n", inputstr);

         sprintf(fs, "%d", fsig);         
         sprintf(ss, "%d", ssig);

         strcat(inputstr, " ");         
         strcat(inputstr, fs);
         strcat(inputstr, "/");
         strcat(inputstr, ss); 

         if (send(sockfd, inputstr, 18, 0) == -1)
         perror("send");

        }
        

else 
      printf("wrong signature: %d/%d. please specify one of these: 2/4 3/4 4/4 6/8\n", fsig, ssig);


       }

else printf("no signature value specified. please specify one of these: 2/4 3/4 4/4 6/8\n");


      }

else if(strcmp(ch, "start")==0)

       {

       ch = strtok(NULL, " "); 

if(ch==NULL)

       printf("please specify number of measures\n");
      
else {
       
       sprintf(ms, "%d", atoi(ch));       
  
       printf("Got: %s\n", inputstr);  

       strcat(inputstr, " ");
       strcat(inputstr, ms);
  
       if (send(sockfd, inputstr, strlen(inputstr), 0) == -1)
       perror("send");

     }
       
 
       }


}while(strcmp(inputstr, "exit")!=0);

       if (send(sockfd, "exit", 4, 0) == -1)
       perror("send");



//if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
//perror("recv");
//exit(1);
//}

//buf[numbytes] = '\0';
//printf("client: received '%s'\n",buf);
close(sockfd);
return 0;
}

