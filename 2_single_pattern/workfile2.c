
#include <stdio.h>
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


int P[] = {7, 1, 1, 3, 1, 1};

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


for(j=0; j<16; j++){

for(i=0; i<6; i++){


    switch (P[i])

     {
       case 7:
              wr_buf[0] = '7';
              write(fd, wr_buf, 0);

              break;

       case 3:
              wr_buf[0] = '3';
              write(fd, wr_buf, 0);
             
              break;
       
       case 1:
              wr_buf[0] = '1';
              write(fd, wr_buf, 0);
            
              break;

     }
    

     //sleep here
	usleep(125000);
     wr_buf[0] = '0';
     write(fd, wr_buf, 0);
	usleep(125000);
     //sleep here
     

}//end i
}//end repeat j

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
