
/*
 * Allows to set arbitrary speed for the serial device on Linux.
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>      /* Error number definitions */ 
#include "/usr/include/asm-generic/termbits.h"
#include "/usr/include/asm-generic/ioctls.h"
// #include <termios.h>    /* POSIX terminal control definitions */


int write_port(int fd, unsigned char *buf, size_t len)
{
    int num;

    num = write(fd, buf, len);
    if (num < 0)
        printf("write failed! (%s)\n", strerror(errno));

    return num;
}

int read_port(int fd, unsigned char *buf, size_t len, struct timeval *tout)
{
    fd_set inputs;
    int num, ret;
    
    num = 0 ;   

    FD_ZERO(&inputs);
    FD_SET(fd, &inputs);

    ret = select(fd+1, &inputs, (fd_set *)NULL, (fd_set *)NULL, tout);
    //printf("select = %d\n", ret);
    if (ret < 0) {
        perror("select error!!");
        return ret;
    }
    if (ret > 0) {
        if (FD_ISSET(fd, &inputs)) {
            num = read(fd, buf, len);           
        }
    }
    
    return num;
}


int main(int argc, char* argv[]) 
{
    int fd, speed;
    struct timeval tout;
    unsigned char send_ch, recv_ch;    

    if (argc != 3) {
        printf("%s device speed\n\nSet speed for a serial device.\nFor instance:\n    %s /dev/ttyUSB0 250000\n", argv[0], argv[0]);                   
        return -1;
    }

    fd = open(argv[1], O_RDWR);

    speed = atoi(argv[2]);

    struct termios2 tio;
    ioctl(fd, TCGETS2, &tio);
    tio.c_cflag &= ~CBAUD;
    tio.c_cflag |= BOTHER;
    tio.c_ispeed = speed;
    tio.c_ospeed = speed;
    int r = ioctl(fd, TCSETS2, &tio);
    // close(fd);

    if (r == 0) {
        printf("Changed successfully.\n");
    } else {
        perror("ioctl");
    }

    // Test code
    sleep(1);

    printf("Test start\n");

    send_ch = '0';
/*    for (;;){
        write_port(fd, &send_ch, 1);
        send_ch++;
        if (send_ch > 0x7f) send_ch = '0';

        tout.tv_sec = 1;
        tout.tv_usec = 100*1000;
        read_port(fd, &recv_ch, 1, &tout);
        if (recv_ch == 'X') break;

        sleep(1);
    }
*/
    printf("Sending RESET\n");
    unsigned char reset[5] = {0xd8, 0x55, 0xff, 0x55, 0xff};
    write_port(fd, reset, sizeof(reset));
    usleep(200000);
    printf("Sending SYNC\n");
    unsigned char sync[5] = {0xff, 0x55, 0xff, 0x55, 0xff};
    write_port(fd, sync, sizeof(sync));
    usleep(200000);
    printf("Reading data now\n");
    usleep(200000);
    int cnt = 0;
    for(;;){
        unsigned char data[5];
        tout.tv_sec = 0;
        tout.tv_usec = 50;
        int numRead = read_port(fd, data, sizeof(data), &tout);
        
        if(numRead > 0){
            printf("Read[%d]: ", cnt++);
            // print values:
            for(int i = 0; i < sizeof(data); ++i){
                printf("0x%02x", data[i]);
                if(i < sizeof(data) - 1) {
                    printf(",");
                } else {
                    printf("\n");
                }
            }
        }

        //usleep(50);
    }
    

    return 0;

}
