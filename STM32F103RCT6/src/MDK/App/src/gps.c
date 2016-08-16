/******************************************************************************
*File name	: gps.c
*Author		: 
*Version		: 1.0
*Date			: 06/18/2010
*Description: This file contain driver for NAVMAN GPS module, extract GPS information and
*				process GPS data
******************************************************************************/


#include "stm32f10x.h"
#include "gps.h"

const u8 GPS_MONTH_NAME[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
const u8 GPS_DIRECTION_NAME[8][3] = {"S", "SW", "W", "NW", "N", "NE", "E", "SE"};
const u16 GPS_TIME_FIX = 3*60 + 1;

extern u8 GPSTxBuffer[GPS_BUFFER_SIZE], GPSRxBuffer[GPS_BUFFER_SIZE];
extern __O bool GPSEnd;

//__O u8 TimingDebounce;  --> 080213 sop1hc: comment out
__O FunctionalState GPSDMS, GPSMetric;
__O float GPSSpeed = 0.0f;
__O bool FlagTimeReport, FlagDistanceReport, FlagOldPosition, GPSFix = FALSE;
__O u16 GPS60Second, GPSTimeFixCounter;


static u32 GPSTimeReport, GPSTimeReportCounter;
static float GPSDistanceReport;

typedef struct
{
	u8 hour;
	u8 minute;
	u8 second;
} GPSTimeStructure;

GPSTimeStructure GPSTime;

typedef struct
{
	u8 date;
	u8 month;
	u16 year;
} GPSDateStructure;

GPSDateStructure GPSDate={1,1,2010};

GPSPositionStruture GPSPosition={0.0f, 0.0f}, GPSPositionOld={0.0f, 0.0f};


static float GPS_Str2Float(char *str);
static void GPS_DD2DMS(char *str ,float floatvalue);
static void GPS_MessageID_Cmd(u8 value, FunctionalState newstate);


//********** Copy from stm32f10x_it.c **********************************************
extern __O u8 TimingDebounce;
__O u8 GPSTxIndex, GPSDataCmd;

// 140213 sop1hc: u8 CMDRxData[CMD_BUFFER_SIZE], CMDTxData[CMD_BUFFER_SIZE];
u8 GPSTxBuffer[GPS_BUFFER_SIZE], GPSRxBuffer[GPS_BUFFER_SIZE];
// 140213 sop1hc : static __O u8 CMDRxIndex, CMDTxIndex, GPSTxIndex, GPSRxIndex;
static __O u8 GPSTxIndex, GPSRxIndex;  //140213 sop1hc

// 140213 sop1hc : __O bool CMDEnd, GPSRun, GPSEnd;
__O bool GPSRun, GPSEnd; // 140213 sop1hc

//*****************************************************************************
//Description : Initialize GPS
//
//Parameter	: none
//
//Return			: none
//
//Note:			: none
//
//*****************************************************************************
void GPS_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOC, ENABLE);

	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPS_Pin_Wakeup|GPS_Pin_Preset;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	USART_InitTypeDef USART_InitStructure;

	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

 	USART_Cmd(USART1, ENABLE);

 	///////////GPIO_ResetBits(GPS_GPIO, GPS_Pin_Wakeup);

	//for(u32 i = 12000000; i != 0; i--);
	
	///////////GPS_MessageID_Control();
	//GPS_MessageID_Cmd(GPS_RMC, ENABLE);

	///////////GPS_CoordDMS_Cmd(ENABLE);
	///////////GPS_Metric_Cmd(ENABLE);
}

//*****************************************************************************
//Description : Toggle GPS power
//
//Parameter	: none
//
//Return			: none
//
//Note:			: none
//
//*****************************************************************************
void GPS_OnOff(void)
{
	GPIO_SetBits(GPS_GPIO, GPS_Pin_Wakeup);
	for(int i=1000; i != 0; i--);
	GPIO_ResetBits(GPS_GPIO, GPS_Pin_Wakeup);
}

//*****************************************************************************
//Description : Set peripheral reset pin high, GPS power off
//
//Parameter	: none
//
//Return			: none
//
//Note:			: none
//
//*****************************************************************************
void PeripheralReset_On(void)
{
	 GPIO_SetBits(GPS_GPIO, GPS_Pin_Preset);
}
//*****************************************************************************
//Description : Clear peripheral reset pin low, GPS power on
//
//Parameter	: none
//
//Return			: none
//
//Note:			: none
//
//*****************************************************************************
void PeripheralReset_Off(void)
{
	GPIO_ResetBits(GPS_GPIO, GPS_Pin_Preset);
}

//*****************************************************************************
//Description : Calculate checksum for string, add checksum and other control character at the
//					end of string
//Parameter	: String needed calulate checksum
//
//Return			: none
//
//Note:			: none
//
//*****************************************************************************
void GPS_Checksum(u8 *str)
{
	u8 tmp, value = 0;

	if(*str== '$')
	{
		str++;
		while(*str)
			value ^= *str++;
		//add "*" char
		*str++ = '*';
		//add checksum
		tmp = value >> 4;
		*str++ = (tmp > 9) ? tmp - 10 + 'A' : tmp + '0';
		tmp = value & 0x0F;
		*str++ = (tmp > 9) ? tmp - 10 + 'A' : tmp + '0';
		//add crlf and null
		*str++ = '\r';
		*str++ = '\n';
		*str = '\0';
	}
}

//*****************************************************************************
//Description : Control GPS message
//
//Parameter	: GPS message ID, enable or disable
//
//Return			: none
//
//Note:			: none
//
//*****************************************************************************
static void GPS_MessageID_Cmd(u8 value, FunctionalState newstate)
{
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	
	sprintf((char *)GPSTxBuffer, "$PSRF103,0%d,00,0%d,01", value, newstate);
		
	GPS_Checksum(GPSTxBuffer);
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);	

}

//*****************************************************************************
//Description : Control all GPS messages
//
//Parameter	: none
//
//Return			: none
//
//Note:			: none
//
//*****************************************************************************
void GPS_MessageID_Control()
{
#if 1
	tmDelayms(1000);
 	GPS_MessageID_Cmd(GPS_GGA, DISABLE);
	//tmDelayms(1000);
 	GPS_MessageID_Cmd(GPS_GLL, DISABLE);
	//tmDelayms(1000);
 	GPS_MessageID_Cmd(GPS_GSA, DISABLE);
	//tmDelayms(1000);
 	GPS_MessageID_Cmd(GPS_GSV, DISABLE);
	//tmDelayms(1000);
 	GPS_MessageID_Cmd(GPS_RMC, ENABLE);
	//tmDelayms(1000);
 	GPS_MessageID_Cmd(GPS_VTG, DISABLE);
	//tmDelayms(1000);
 	GPS_MessageID_Cmd(GPS_MSS, DISABLE);
	//tmDelayms(1000);
 	GPS_MessageID_Cmd(GPS_ZDA, DISABLE);
#else
 	GPS_MessageID_Cmd(GPS_GGA, ENABLE);
 	GPS_MessageID_Cmd(GPS_GLL, DISABLE);
 	GPS_MessageID_Cmd(GPS_GSA, ENABLE);
 	GPS_MessageID_Cmd(GPS_GSV, ENABLE);
 	GPS_MessageID_Cmd(GPS_RMC, ENABLE);
 	GPS_MessageID_Cmd(GPS_VTG, DISABLE);
 	GPS_MessageID_Cmd(GPS_MSS, DISABLE);
 	GPS_MessageID_Cmd(GPS_ZDA, DISABLE);
#endif
}

//*****************************************************************************
//Description : Extract and process GPS data
//
//Parameter	: none
//
//Return			: none
//
//Note:			: none
//
//*****************************************************************************
void GPS_Data_Process(void)
{
//$GPRMC,022312.781,A,1048.7033,N,10640.0460,E,1.02,0.10,150610,,,A*6A
	if(GPSEnd)
	{
		if(strncmp((const char *)GPSRxBuffer, "$GPRMC", 6) == 0)
		{
			if(GPSRxBuffer[18] == 'A')
			{
				char *pi, *pj, strtmp[10];
				pi = (char *)GPSRxBuffer;
				
				while(*pi++ != ',');
				//extract hour, minute, second GPS UTC time
				GPSTime.hour = (*pi++ - '0') * 10;
				GPSTime.hour += *pi++ -'0';
				GPSTime.minute = (*pi++ - '0') * 10;
				GPSTime.minute += *pi++ -'0';
				GPSTime.second = (*pi++ - '0') * 10;
				GPSTime.second += *pi++ -'0';
				
				while(*pi++ != ',');
				while(*pi++ != ',');
				//extract GPS latitude
				pj = strtmp;
				while(*pi != ',') *pj++ = *pi++;
				*pj = '\0';
				GPSPosition.latitude = GPS_Str2Float(strtmp);
				
				pi++;
				if(*pi++ == 'S')
					GPSPosition.latitude = -GPSPosition.latitude;
				//extract GPS longtitude
				pi++;
				pj = strtmp;
				while(*pi != ',') *pj++ = *pi++;
				*pj = '\0';
				GPSPosition.longitude = GPS_Str2Float(strtmp);

				pi++;
				if(*pi++ == 'W')
					GPSPosition.longitude = -GPSPosition.longitude;

				pi++;

				//GPS speed = knots * 1.852 (kmh) or knots * 1.15 (mph)
				pj = strtmp;
				while(*pi != ',') *pj++ = *pi++;
				*pj = '\0';
				GPSSpeed = atof((const char *)strtmp);
				if(GPSMetric)
					GPSSpeed *=1.852;
				else
					GPSSpeed *=1.15;

				pi++;
				while(*pi++ != ',');
				
				//extract date month year
				GPSDate.date = (*pi++ - '0') * 10;
				GPSDate.date += *pi++ -'0';
				GPSDate.month = (*pi++ - '0') * 10;
				GPSDate.month += *pi++ -'0';
				GPSDate.year = (*pi++ - '0') * 10;
				GPSDate.year += *pi++ -'0' + 2000;
				
				//GPS validate, clear old position
				GPSTimeFixCounter = 0;
				FlagOldPosition = FALSE;
				GPSFix = TRUE;
				
				//check distance report
				if(GPSDistanceReport)
				{
					if(GPS_Distance(GPSPositionOld, GPSPosition) >= GPSDistanceReport)
					{
						FlagDistanceReport = TRUE;
						GPS_SavePosition();
					}
				}

			}
			else
			{
			//set old position
				if(FlagOldPosition == FALSE)
				{
					if(++GPSTimeFixCounter == GPS_TIME_FIX)
					{
						GPSTimeFixCounter = 0;
						FlagOldPosition = TRUE;
					}
				}
			}

			//check time report
			if(++GPS60Second == 60)
			{
				GPS60Second = 0;
				if(GPSTimeReport)
				{
					if(++GPSTimeReportCounter == GPSTimeReport)
					{
						GPSTimeReportCounter = 0;
						FlagTimeReport = TRUE;
					}
				}
			}
			
		}
		
		GPSEnd = FALSE;
	}
}

//*****************************************************************************
//Description : Convert string to latitude or longitude
//
//Parameter	: String needed convertion
//
//Return			: none
//
//Note:			: none
//
//*****************************************************************************
static float GPS_Str2Float(char *str)
{
	int itmp;
	float ftmp;
	ftmp = atof((const char *)str);
	itmp = (int)ftmp/100;
	ftmp -= itmp * 100;
	return (itmp + ftmp / 60);
}

//*****************************************************************************
//Description : Convert decimal degree to degree, minute, second
//
//Parameter	: String contain result, float value converted
//
//Return			: none
//
//Note:			: none
//
//*****************************************************************************
static void GPS_DD2DMS(char *str ,float floatvalue)
{
	int degree, minute, second;
	floatvalue = fabs(floatvalue);
	degree = (int)floatvalue;
	floatvalue -= degree;
	floatvalue *= 60;
	minute = (int)floatvalue;
	second = (int)((floatvalue - minute) * 60);
	sprintf(str, "%d %02d\'%02d\"", degree, minute, second);
}

//*****************************************************************************
//Description : Convert GPS position to string
//
//Parameter	: String contain result, GPS position
//
//Return			: none
//
//Note:			: none
//
//*****************************************************************************
void GPS_Position2String(char *str, GPSPositionStruture GPSPositionValue)
{
	char str1[20], str2[20], char1, char2;
	
	if(GPSFix)
	{
		if(GPSDMS){
			GPS_DD2DMS(str1, GPSPositionValue.latitude);
			char1 = (GPSPositionValue.latitude >= 0.0f) ? 'N' : 'S';
			GPS_DD2DMS(str2, GPSPositionValue.longitude);
			char2 = (GPSPositionValue.longitude >= 0.0f) ? 'E' : 'W';
			if(FlagOldPosition)
				sprintf(str, "%s%c, %s%c, old position", str1, char1, str2, char2);
			else
				sprintf(str, "%s%c, %s%c", str1, char1, str2, char2);
		}
		else
		{
			if(FlagOldPosition)
				sprintf(str, "%0.5f, %0.5f, old position", GPSPositionValue.latitude, GPSPositionValue.longitude);
			else
				sprintf(str, "%0.5f, %0.5f", GPSPositionValue.latitude, GPSPositionValue.longitude);
		}
	}
	else sprintf(str, "no GPS lock");
}

//*****************************************************************************
//Description : Calculate distance between 2 position
//
//Parameter	: Base position, target positon
//
//Return			: Distance
//
//Note:			: none
//
//*****************************************************************************
float GPS_Distance(GPSPositionStruture GPSPosition1, GPSPositionStruture GPSPosition2)
{
	float lat1, lat2, lon1, lon2, tmp;
	
	lat1 = GPSPosition1.latitude;
	lon1 = GPSPosition1.longitude;
	lat2 = GPSPosition2.latitude;
	lon2 = GPSPosition2.longitude;
	
	lat1 /= PIRAD;
	lat2 /= PIRAD;
	lon1 /= PIRAD;
	lon2 /= PIRAD;

	tmp = acos(sin(lat1) * sin(lat2) + cos(lat1) * cos(lat2) * cos(lon2 - lon1));
	if(GPSMetric)
		tmp *= 6371;
	else
		tmp *= 3959;

	return tmp;
}

//*****************************************************************************
//Description : Calculate bearing between 2 position
//
//Parameter	: Base position, target position
//
//Return			: Degree
//
//Note:			: none
//
//*****************************************************************************
float GPS_Bearing(GPSPositionStruture GPSPosition1, GPSPositionStruture GPSPosition2)
{
	float lat1, lat2, lon1, lon2;
	
	lat1 = GPSPosition1.latitude;
	lon1 = GPSPosition1.longitude;
	lat2 = GPSPosition2.latitude;
	lon2 = GPSPosition2.longitude;
	
	lat1 /= PIRAD;
	lat2 /= PIRAD;
	lon1 /= PIRAD;
	lon2 /= PIRAD;
	
	return atan2(sin(lon2 - lon1) * cos(lat2), cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(lon2 - lon1)) * PIRAD;
}

//*****************************************************************************
//Description : Convert degree to cardival
//
//Parameter	: String contain result, degree
//
//Return			: none
//
//Note:			: none
//
//*****************************************************************************
void GPS_Direction(char *str, float fvalue)
{
	if(GPSFix)
	{
		fvalue = (fvalue + 180 +22.5) /45;
		sprintf(str, "%s", GPS_DIRECTION_NAME[(int)floor(fvalue) % 8]);
	}
	else
		sprintf(str, "N");
}

//*****************************************************************************
//Description : Save current position to old position
//
//Parameter	: none
//
//Return			: none
//
//Note:			: none
//
//*****************************************************************************
void GPS_SavePosition()
{
	GPSPositionOld.latitude = GPSPosition.latitude;
	GPSPositionOld.longitude = GPSPosition.longitude;
}

//*****************************************************************************
//Description : Select coordinate are displayed in degree, minute, second format or decimal
//				degree format
//Parameter	: ENABLE of DISABLE
//
//Return			: none
//
//Note:			: If enable degree, minute, second is selected
//
//*****************************************************************************
void GPS_CoordDMS_Cmd(FunctionalState newstate)
{
	GPSDMS = newstate;
}

//*****************************************************************************
//Description : Select metric or imperial measurement
//
//Parameter	: ENABLE or DISABLE
//
//Return			: none
//
//Note:			: if enable metric is selected
//
//*****************************************************************************
void GPS_Metric_Cmd(FunctionalState newstate)
{
	GPSMetric = newstate;
}

//*****************************************************************************
//Description : Convert speed to string
//
//Parameter	: String contain result, speed
//
//Return			: none
//
//Note:			: none
//
//*****************************************************************************
void GPS_Speed2String(char *str, float fvalue)
{
	if(GPSMetric)
		sprintf(str, "%0.2f kmh", fvalue);
	else
		sprintf(str, "%0.2f mph", fvalue);
}

//*****************************************************************************
//Description : Convert distance to string
//
//Parameter	: String contain result, distance
//
//Return			: none
//
//Note:			: none
//
//*****************************************************************************
void GPS_Distance2String(char *str, float fvalue)
{
	if(GPSFix)
	{
		if(GPSMetric)
			sprintf(str, "%0.2f km", fvalue);
		else
			sprintf(str, "%0.2f mi", fvalue);
	}
	else sprintf(str, "no GPS lock");
}

//*****************************************************************************
//Description : Convert time to string
//
//Parameter	: String contain result
//
//Return			: none
//
//Note:			: Time and date of GPS UTC time
//
//*****************************************************************************
void GPS_Time2String(char *str)
{
	sprintf(str,"%02d:%02d %s %d", GPSTime.hour, GPSTime.minute, GPS_MONTH_NAME[GPSDate.month-1], GPSDate.date);
}

//*****************************************************************************
//Description : Set automatic time report
//
//Parameter	: Time in minute
//
//Return			: none
//
//Note:			: none
//
//*****************************************************************************
void GPS_SetTimeReport(u32 timevalue)
{
	GPSTimeReport = timevalue;
	GPS60Second = 0;
	GPSTimeReportCounter = 0;
}

//*****************************************************************************
//Description : Set automatic distance report
//
//Parameter	: Distance in km
//
//Return			: none
//
//Note:			: none
//
//*****************************************************************************
void GPS_SetDistanceReport(float distancevalue)
{
	GPSDistanceReport = distancevalue;
	GPS_SavePosition();
}

//*****************************************************************************
//Description : Send report
//
//Parameter	: String from database
//
//Return			: none
//
//Note:			: none
//
//*****************************************************************************
void GPS_Report(char *str)
{
//Chi Hoa Prison,District 10,Ho Chi Minh,VN,P,10.77717,106.66884\n
	char *pi, *pj, st[10], strtmp[200];
	GPSPositionStruture position;
	
	pi = (char *)str;
	pj = strtmp;
	while(*pi != ',') *pj++ = *pi++;
	*pj++ = *pi++;
	while(*pi != ',') *pj++ = *pi++;
	*pj++ = *pi++;
	while(*pi != ',') *pj++ = *pi++;
	*pj++ = *pi++;
	while(*pi != ',') *pj++ = *pi++;
	*pj++ = *pi++;
	while(*pi != ',') *pj++ = *pi++;
	*pj = '\0';
	
	pi++;
	pj = st;
	while(*pi != ',') *pj++ = *pi++;
	*pj = '\0';
	position.latitude = atof(st);
	
	pi++;
	pj = st;
	while(*pi != 0) *pj++ = *pi++;
	*pj = '\0';
	position.longitude = atof(st);

	char sdistance[10];
	GPS_Metric_Cmd(ENABLE);
	GPS_Distance2String(sdistance, GPS_Distance(GPSPosition, position));

	char sdirection[4];
	GPS_Direction(sdirection, GPS_Bearing(GPSPosition, position));

	char sspeed[10];
	GPS_Speed2String(sspeed, GPSSpeed);

	char stime[20];
	GPS_Time2String(stime);

	char sposition[40];
	GPS_CoordDMS_Cmd(DISABLE);
	GPS_Position2String(sposition, GPSPosition);
	
	sprintf(strtmp, "%s, cach day %s huong %s, %s, %s, %s", strtmp, sdistance, sdirection, sspeed, stime, sposition);
	Send2PC((u8 *)strtmp);
}
/********************** Copy from stm32f10x_it.c ***********************************/
void SysTick_Handler(void)
{
	TimingDelay_Decrement();
}

void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line10) != RESET)
	{
		//sop1hc:190513 -->PC8 -->LED 2 
		//GPIO_WriteBit(GPIOC, GPIO_Pin_8, (BitAction)(1 - GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8)));
		
		EXTI_ClearITPendingBit(EXTI_Line10);
		
		EXTI_InitTypeDef EXTI_InitStructure;
		
		EXTI_InitStructure.EXTI_Line = EXTI_Line10;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
		EXTI_InitStructure.EXTI_LineCmd = DISABLE;
		EXTI_Init(&EXTI_InitStructure);
		
		TimingDebounce = EXTI_TIMING_DEBOUNCE;
	}
}

void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		u8 tmp = USART_ReceiveData(USART1);
		if(GPSFix)
		{
			//USART_SendData(USART2, tmp);
			//SerialPutString(&tmp,1);
		}
		else
		{
			if(tmp == '\n')
			{
				GPSEnd = TRUE;
				GPSRxBuffer[--GPSRxIndex] = 0;
				GPSRxIndex = 0;
			}
			else
			{
				GPSRxBuffer[GPSRxIndex++] = tmp;
				if(GPSRxIndex == GPS_BUFFER_SIZE)
				{
					GPSRxIndex = 0;
				}
			}
		}
	}

	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
	{
		USART_SendData(USART1, GPSTxBuffer[GPSTxIndex++]);
		if(GPSTxBuffer[GPSTxIndex] == '\0')
		{
			USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
			GPSTxIndex = 0;
		}
	}
}

/* 140213 :sop1hc
void USART2_IRQHandler(void)
{
	
	if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
	{
		USART_SendData(USART2, CMDTxData[CMDTxIndex++]);
		if(CMDTxData[CMDTxIndex] == 0x00)
		{
			USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
			CMDTxIndex = 0;
		}
	}

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		u8 tmp =	USART_ReceiveData(USART2);
		if(tmp == '\n')
		{
			CMDEnd = TRUE;
			CMDRxData[--CMDRxIndex] = 0;
			CMDRxIndex = 0;
		}
		else
		{
			CMDRxData[CMDRxIndex++] = tmp;
			if(CMDRxIndex == CMD_BUFFER_SIZE)
			{
				CMDRxIndex = 0;
			}
		}
	}
}

*/

void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET);
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	//GPIO_WriteBit(GPIOC, GPIO_Pin_7, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_7)));
}

