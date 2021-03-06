
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv)
{
/* Our file descriptor */

int fd;
int rc = 0;
int wc = 0;

int i = 0;
int j = 0;


int fsig = 6;
int ssig = 8;

char inputstr[30];              // sufficient buffer size for input string

char *ch;                       // tokenizer


int loopnum = 6;                // default value

int tempovalue = 120;

int sleepvalue = 125000;        // default value

int P[] = {7, 1, 1, 3, 1, 1};   // default value                       


                                // = {7, 1, 1, 3, 1, 1};    this is for signature 6/8    loopnum = 6

                                // = {7, 1};                this is for signature 2/4    loopnum = 2                 

                                // = {7, 1, 1};             this is for signature 3/4    loopnum = 3

                                // = {7, 1, 3, 1};          this is for signature 4/4    loopnum = 4
                                

char *rd_buf[16];
char  wr_buf[16];


printf("%s: entered\n", argv[0]);

// Open the device

fd = open("/dev/use_hellodev2", O_RDWR);

if (fd == -1) {
perror("open failed");
rc = fd;
exit(-1);
}

printf("%s: open successful\n", argv[0]);



while(1) {

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
           {  sleepvalue = 1000*1000*120/(atoi(ch)*ssig);
                            
              tempovalue = atoi(ch);


              printf("Got: %s\n", inputstr); 
           }

              } 
else
      printf("no tempo value specified. please specify tempo within range [30..180]\n");


       }//end if tempo



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
        
         loopnum = 2;

         P[0] = 7;
         P[1] = 1; printf("Got: %s\n", inputstr);

         //ioctl

           ioctl(fd, 0, 224);

        }
      

else if(fsig==3 && ssig==4)
       
        {
         
         loopnum = 3;

         P[0] = 7;
         P[1] = 1;
         P[2] = 1;printf("Got: %s\n", inputstr);

         //ioctl
           
           ioctl(fd, 0, 234);

        }


else if(fsig==4 && ssig==4)

        {
          
         loopnum = 4;
        
         P[0] = 7;
         P[1] = 1;
         P[2] = 3;
         P[3] = 1;printf("Got: %s\n", inputstr);

         //ioctl

           ioctl(fd, 0, 244);
 
        }

else if(fsig==6 && ssig==8)

        {

         loopnum = 6;
                 
         P[0] = 7;
         P[1] = 1;
         P[2] = 1;
         P[3] = 3;
         P[4] = 1;
         P[5] = 1;printf("Got: %s\n", inputstr);

         //ioctl

          ioctl(fd, 0, 268);

        }
        

else 
      printf("wrong signature: %d/%d. please specify one of these: 2/4 3/4 4/4 6/8\n", fsig, ssig);


       }

else printf("no signature value specified. please specify one of these: 2/4 3/4 4/4 6/8\n");


      }//end if time-signature



else if(strcmp(ch, "start")==0)

      {

      //ioctl
        
        ioctl(fd, 0, tempovalue);


      }//end if start



else if(strcmp(ch, "stop")==0)

      {

      //ioctl

        ioctl(fd, 0, 300);        


      }//end if stop




else if(strcmp(ch, "exit")==0)    ////DO NOT INPUT THIS COMMAND WHEN TIMER HAS BEEN STOPPED
                                  ////THIS COMMAND BREAKS THE LOOP AND DEVICE FILE IS CLOSED
      {                           ////MEANWHILE THE TIMER
       break;                     ////STOPPING THE TIMER WHICH HAS ALREADY BEEN STOPPED RAISES EXCEPTION

      }//end if exit



}



/*
// Issue a write 
wc = write(fd, wr_buf, 0);

if (wc == -1) {
perror("read failed");
close(fd);
exit(-1);
}

printf("%s: write: returning %d bytes!\n", argv[0], wc);
*/

// Close device
close(fd);
return 0;


}
