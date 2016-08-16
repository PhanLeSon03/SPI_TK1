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

#include <memory.h>
#include <getopt.h>
//https://github.com/NeuroRoboticTech/JetduinoDrivers.git
#include "SimpleGPIO.h"
#define CS_SOFTWARE_CONTROL 1
#define BUFF_SIZE 20
#define SPI_SPEED 6000000
int devHandle;
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
       printf(" %02d", Rev[iBuff]);
    }
    printf("\n");
}
        
int main (void)
{
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
#if CS_SOFTWARE_CONTROL==1
    //Lower chip select
    gpio_set_value(57, false);
#endif

    //SPI_Send(devHandle, outBuff, inBuff, 20);
    for (int i=0; i< BUFF_SIZE; i++)
    {
        write(devHandle, outBuff+i, 1);
    }
#if CS_SOFTWARE_CONTROL==1
    //Raise chip select again
    gpio_set_value(57, true);
#endif

    close(devHandle);

    return 0;

}

