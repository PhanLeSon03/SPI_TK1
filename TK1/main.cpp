#include <iostream>
#include <linux/types.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <pthread.h>
#include <time.h>
#include <termios.h>

#include <memory.h>
#include <getopt.h>
//https://github.com/NeuroRoboticTech/JetduinoDrivers.git
#include "SimpleGPIO.h"



#define CS_SOFTWARE_CONTROL 0
#define BUFF_SIZE 20
#define SPI_SPEED 1125000

const char *byte_to_binary(int x);

int devHandle;
char stKeyPrss, stKeyPrssOld;
unsigned char inBuff[BUFF_SIZE];
unsigned char outBuff[BUFF_SIZE];

static void dumpstat(const char *name, int fd)
{
    __u8 mode, lsb, bitsr=8, bitsw;
    __u32 speed=SPI_SPEED;

    if (ioctl(fd, SPI_IOC_RD_MODE, &mode) < 0)
    {
        perror("SPI rd_mode");
        return;
    }

    if (ioctl(fd, SPI_IOC_RD_LSB_FIRST, &lsb) < 0)
    {
        perror("SPI rd_lsb_fist");
        return;
    }

    if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bitsr) < 0)
    {
        perror("SPI bits_per_word");
        return;
    }

    if (ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bitsr) < 0)
    {
         perror("SPI bits_per_word");
         return;
    }

    if (ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bitsw) < 0)
    {
        perror("SPI bits_per_word");
        return;
    }

    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) > 0)
    {
        perror("SPI write max_speed_hz");
        return;
    }

    if (ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed) < 0) 
    {
        perror("SPI max_speed_hz");
        return;
    }

    printf("%s: spi mode %d, %d bitsr, %d bitsw, %sper word, %d Hz max\n", name, mode, bitsr, bitsw, lsb ? "(lsb first) " : "", speed);
}

void SPI_Send(int dev, unsigned char *Data,unsigned char *Rev, int len)
{
    struct spi_ioc_transfer xfer[1];
    int status;
    unsigned char txBuff[32], rxBuff[32];

    memset(xfer, 0, sizeof xfer);
    memset(txBuff, 0, sizeof txBuff);
    memset(rxBuff, 0, sizeof rxBuff);

    for(int iBuff=0; iBuff < len; iBuff++)
    {
        txBuff[iBuff] = Data[iBuff];
    }

    xfer[0].tx_buf = (unsigned long)txBuff;
    xfer[0].rx_buf = (unsigned long)rxBuff;
    xfer[0].len = len;
    xfer[0].speed_hz = SPI_SPEED;
    status = ioctl(dev,SPI_IOC_MESSAGE(1), xfer);

    if (status < 0)
    {
        perror("SPI_IOC_MESSAGE");
        return;
    }

    printf("response(%2d, %2d): ", len, status);
    for (int iBuff = 0; iBuff < len; iBuff++)
    {
       Rev[iBuff] = rxBuff[iBuff];
       ////printf(" %.2x", Rev[iBuff]);
       printf(" %s", byte_to_binary(Rev[iBuff]));
    }
    printf("\n");
}

int getkey() 
{
	
    int character;
    struct termios orig_term_attr;
    struct termios new_term_attr;

    /* set the terminal to raw mode */
    tcgetattr(fileno(stdin), &orig_term_attr);
    memcpy(&new_term_attr, &orig_term_attr, sizeof(struct termios));
    new_term_attr.c_lflag &= ~(ECHO|ICANON);
    new_term_attr.c_cc[VTIME] = 0;
    new_term_attr.c_cc[VMIN] = 0;
    tcsetattr(fileno(stdin), TCSANOW, &new_term_attr);

    /* read a character from the stdin stream without blocking */
    /*   returns EOF (-1) if no character is available */
    //rewind( stdin );
    //fflush(stdin);
    character = fgetc(stdin);

    /* restore the original terminal attributes */
    tcsetattr(fileno(stdin), TCSANOW, &orig_term_attr);

    return character;
}

void * Key_Read(void *x_void_ptr)
{
	char *x_ptr = (char *)x_void_ptr;
	
	while (*x_ptr!=27)
	{
		*x_ptr = getkey();
                 //rewind(stdin);
                 //fflush(stdin);
	    //printf("Key Press: %d\n",*x_ptr);
	    usleep(20000);
	}
	
	return NULL;
}

const char *byte_to_binary(int x)
{
    static char b[9];
    b[0] = '\0';

    int z;
    for (z = 128; z > 0; z >>= 1)
    {
        strcat(b, ((x & z) == z) ? "1" : "0");
    }

    return b;
}
        
int main (void)
{
    pthread_t Key_Thread;
    printf("Testing SPI on TK1 \n");

#if CS_SOFTWARE_CONTROL==1
    printf("Exporting pin 57\r\n");
    gpio_export(57);

    gpio_set_dir(57, OUTPUT_PIN);

    //Keep the chipselect high till we want to write something
    gpio_set_value(57, true);
#endif
   
    // Initialize the buffer
    for (int i=0; i < BUFF_SIZE;i++)
    {
        outBuff[i]= 65 + i;
        inBuff[i] = 0;
        printf(" %02d", outBuff[i]);
    }
    printf("\r\n");

    //open device on /dev/spidev0.0
    if ((devHandle = open("/dev/spidev0.0",O_RDWR))<0)
    {
        printf("Error: Couldn't open SPI device 0! %d\n",devHandle);
        return 1;
    }

    dumpstat("spidev0.0",devHandle);

	if(pthread_create(&Key_Thread, NULL, Key_Read, (void *)&stKeyPrss)) 
	{
        fprintf(stderr, "Error creating thread\n");
        return 1;
    }
    
    printf("Press ESC for escape\r\n");
    printf("Press 's' for sending data\r\n");

	// 27 is the ESC key
	while (stKeyPrss!=27)
	{
       if ((stKeyPrss==115)) // s press
	   {
           
           printf("Sending data to SPI1 bus... \r\n");
#if CS_SOFTWARE_CONTROL==1
           //Lower chip select
           gpio_set_value(57, false);
#endif
           SPI_Send(devHandle, outBuff, inBuff, BUFF_SIZE);
           for (int i=0; i< BUFF_SIZE; i++)
           {
              //write(devHandle, outBuff + i, 1);
              //read(devHandle,inBuff + i,1);
              //printf(" %s", byte_to_binary(inBuff[i]));
              //usleep(100);         // on for 100us
           }
           printf("\r\n");

#if CS_SOFTWARE_CONTROL==1
          //Raise chip select again
          gpio_set_value(57, true);
#endif 
           usleep(10000); 
	   }
       else
       {
           if (0&&(stKeyPrssOld!=stKeyPrss)&&(stKeyPrss!=0)&&(stKeyPrss!=0xFF))
           {
               write(devHandle, &stKeyPrss, 1); 
               //usleep(1000);
           } 
           else 
           {
               ;
           }
       }

       stKeyPrssOld = stKeyPrss;
    }


    close(devHandle);

    return 0;

}

