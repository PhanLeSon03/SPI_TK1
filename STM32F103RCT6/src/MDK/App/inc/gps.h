/******************************************************************************
*File name	: gps.h
*Author		: 
*Version		: 1.0
*Date			: 06/18/2010
*Description: This file contain all the function prototypes for gps driver
******************************************************************************/

#ifndef __GPS_H
#define __GPS_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "stm32f10x_exti.h"
#include "stm32f10x_tim.h"
#include "Common.h"

#define GPS_GPIO GPIOC
#define GPS_Pin_Wakeup GPIO_Pin_9
#define GPS_Pin_Preset GPIO_Pin_11
#define GPS_GGA	((u8)0x00)
#define GPS_GLL	((u8)0x01)
#define GPS_GSA ((u8)0x02)
#define GPS_GSV	((u8)0x03)
#define GPS_RMC	((u8)0x04)
#define GPS_VTG	((u8)0x05)
#define GPS_MSS	((u8)0x06)
#define GPS_ZDA	((u8)0x08)

#define RAD 3.14/180;//0.017441;
#define R 6371;
#define PIRAD 57.329578;//180/PI

#define DAILY 1440;
#define WEEKLY 10080;

#define EXTI_TIMING_DEBOUNCE 10
#define CMD_BUFFER_SIZE 100
#define GPS_BUFFER_SIZE 100

typedef struct
{
	float latitude;
	float longitude;
} GPSPositionStruture;

void GPS_Init(void);
void GPS_OnOff(void);
void PeripheralReset_On(void);
void PeripheralReset_Off(void);
void GPS_Checksum(u8 *str);
void GPS_MessageID_Control();
void GPS_Data_Process(void);
void GPS_CoordDMS_Cmd(FunctionalState newstate);
void GPS_Metric_Cmd(FunctionalState newstate);
void GPS_Speed2String(char *str, float fvalue);
void GPS_Distance2String(char *str, float fvalue);
void GPS_Time2String(char *str);
void GPS_SetTimeReport(u32 timevalue);
void GPS_SetDistanceReport(float distancevalue);
void GPS_Report(char *str);
void GPS_Position2String(char *str, GPSPositionStruture GPSPositionValue);
void GPS_SavePosition();
float GPS_Distance(GPSPositionStruture GPSPosition1, GPSPositionStruture GPSPosition2);
float GPS_Bearing(GPSPositionStruture GPSPosition1, GPSPositionStruture GPSPosition2);
void GPS_Direction(char *str, float fvalue);

#endif