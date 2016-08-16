#include "Accelerometer.h"


/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
/* Varian storage status of mode measurement */
u8 Status = MODE_2G;
volatile u8 Direction = WRITE; 
volatile u8 BYTE_TRANSMIT;
volatile u8 BYTE_ADDRESS;
volatile u8 BYTE_DATA;
volatile s8 DATA_RETURN;
volatile u8 iINDEX = 1;
volatile TestStatus Flag_Finish = FAILED;
volatile u8 Flag_ACC = 0;

extern volatile uint32_t Mul_time;
static volatile u8 i=1;


/* Initial I2C connect beetwen MCU and Accelerometer */
void I2CInit(void)
{
		I2C_InitTypeDef  I2C_InitStructure;
		/* GPIOB Periph clock enable */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2 , ENABLE);
		GPIO_InitTypeDef GPIO_InitStructure;



		/* Configure I2C2 pins: SCL and SDA ----------------------------------------*/
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11 ;//11
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

 
		
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10  ;//10
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		/* Enable I2C2  ----------------------------------------------------*/
		I2C_Cmd(I2C2, ENABLE);   

		/* I2C2 configuration ------------------------------------------------------*/
             I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
		I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
		I2C_InitStructure.I2C_OwnAddress1 = 0xA0; //I2C2_ACC_ADDRESS7
		I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
		I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
		I2C_InitStructure.I2C_ClockSpeed = ClockSpeed;
		I2C_Init(I2C2, &I2C_InitStructure);

		//I2C_CalculatePEC(I2C2, ENABLE);  
//		 I2C_CalculatePEC(I2C2, ENABLE);  
		/* Enable I2C2 event and buffer interrupts */
		//I2C_ITConfig(I2C2, I2C_IT_EVT | I2C_IT_BUF, ENABLE);
		
		/* NVIC configuration  */
		NVIC_Configuration_I2C();  
}


/* Set range of accelerometer ( 2g, 4g, 8g)  and operate in MEASUMENT status*/
void SetAccRange(u8 ModeStatus)
{
	Status = ModeStatus;
	IIC_ByteWrite(MCTL, (ModeStatus|MODE_MEAS));
}

/* Set Accelerometer operate in Stand By mode */
void SetStandByACC(void)
{
	IIC_ByteWrite(MCTL, MODE_STAND);
}


/* Set Level Threshold interrup, channel 6*/
void SetAccThresholdINT(s8 ValueThres)
{
	s8 mode = 0x40;
      /* */
//	IIC_ByteWrite(INTRST, 0x03);
	
	switch (Status){
	case MODE_2G: 
			mode =  MODE_2G|MODE_LEVEL;
	break;
	case MODE_4G:
			mode =  MODE_4G|MODE_LEVEL;
		
	break;
	case MODE_8G:
			mode =  MODE_8G|MODE_LEVEL;
	break;
	default:
	break;
	}
	ClearIntLatch();
	
	IIC_ByteWrite(CTL1, 0x02);

	IIC_ByteWrite(CTL2, 0x00);
	
	IIC_ByteWrite(LDTH, ValueThres);

	 IIC_ByteWrite(MCTL, mode);

//	IIC_ByteWrite(INTRST, 0x00);

}


/* Read X axis */
float ReadAccValueX (void)
{
      float DataReceive = 0;
	s8 temp;
	temp = IIC_ByteRead(XOUT);
	DataReceive =temp;
	DataReceive /= 64;
      
	return DataReceive;
}

/* Read Y axis */
float ReadAccValueY(void)
{
	float DataReceive = 0;
	s8 temp;
	temp = IIC_ByteRead(YOUT);//-6
      DataReceive = temp;
      DataReceive /= 64;
	  
	return DataReceive;
}

/* Read Z axis */
float ReadAccValueZ(void)
{
	float DataReceive = 0;
	s8 temp;
	//DataReceive = ConvertTypeNumber(IIC_ByteRead(ZOUT)) - 19;
	temp = IIC_ByteRead(ZOUT);
	DataReceive = temp;
	DataReceive /= 64;
      //DataReceive = Round(DataReceive);

	return DataReceive;
}


float ReadAccValueZ10Bit (void)
{
	u8 ByteHigh,ByteLow;
	s16 Data;
	float DataReceive; 

	ByteHigh = IIC_ByteRead(ZOUTH);
	ByteLow = IIC_ByteRead(ZOUTL);

	Data = ((s16)ByteHigh<<16)|(s16)ByteLow;

	DataReceive = (float)Data/64;

	return DataReceive;
}

float ReadAccValueY10Bit (void)
{
	u8 ByteHigh,ByteLow;
	s16 Data;
	float DataReceive; 

	ByteHigh = IIC_ByteRead(YOUTH);
	ByteLow = IIC_ByteRead(YOUTL);

	Data = ((s16)ByteHigh<<16)|(s16)ByteLow;

	DataReceive = (float)Data/64;

	return DataReceive;
}

float ReadAccValueX10Bit (void)
{
	u8 ByteHigh,ByteLow;
	s16 Data;
	float DataReceive; 

	ByteHigh = IIC_ByteRead(XOUTH);
	ByteLow = IIC_ByteRead(XOUTL);

	Data = ((s16)ByteHigh<<16)|(s16)ByteLow;

	DataReceive = (float)Data/64;

	return DataReceive;
}
/* Send data from MCU to Accelerometer */



/* External interrupt by Accelerometer */

void EXTI9_5_IRQHandler(void)
{
       if(EXTI_GetITStatus(EXTI_Line6) != RESET)
       {
		/* Clear the Key Button EXTI line pending bit */
		EXTI_ClearITPendingBit(EXTI_Line6);
#if 1
		/* GPIO_SetBits(GPIOF, GPIO_Pin_8);	*/
			if (i%2)
				{
					//GPIO_WriteBit(GPIOC, GPIO_Pin_7, Bit_RESET);	
				}
			else
				{
	                //GPIO_WriteBit(GPIOC, GPIO_Pin_7, Bit_SET);	
				}
			i++;
//	 		ClearIntLatch();			
#endif			
			Flag_ACC = 1;
	
       }
		
}


/* Clear the INT1 & INT2 of accelerometer */
void ClearIntLatch(void)
{
	IIC_ByteWrite(INTRST, 0x03);
	IIC_ByteWrite(INTRST, 0x00);
}


/* Write data into specific Register of accelerometer */
void IIC_ByteWrite(u8 Address, s8 Data)
{
	
	Direction = WRITE;

	BYTE_ADDRESS = Address;

 	BYTE_DATA = Data;
	iINDEX = 1;

	/*!< While the bus is busy */
	while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY))
	{
	}
	I2C_ITConfig(I2C2, I2C_IT_EVT | I2C_IT_BUF, ENABLE);

	/* Send I2C2 START condition */
	I2C_GenerateSTART(I2C2, ENABLE);
       
	while(!Flag_Finish);

	Flag_Finish = FAILED ;

	//tmDelayms(50);

	 
}


/* Read data from specific Register of accelerometer */
s8 IIC_ByteRead(u8 Address)
{
      s8 Data;


	Direction = WRITE;

	BYTE_ADDRESS = Address;

	/*!< While the bus is busy */
	while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY))
	{
	}
	I2C_ITConfig(I2C2, I2C_IT_EVT | I2C_IT_BUF, ENABLE);
	
       /* Send I2C2 START condition */
	I2C_GenerateSTART(I2C2, ENABLE);

	while(!Flag_Finish);

	Flag_Finish = FAILED;

   
	/*-----------------------------------------------------*/  
	/*-----------------------------------------------------*/  

	/*!< While the bus is busy */
	while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY))
	{
	}
	I2C_ITConfig(I2C2, I2C_IT_EVT | I2C_IT_BUF, ENABLE);
		
	/* Read Register of Accelerometer */  
	Direction = READ;

	/* Send I2C2 START condition */
	I2C_GenerateSTART(I2C2, ENABLE);

	while(!Flag_Finish);

	Flag_Finish = FAILED;

	Data = DATA_RETURN;

       //tmDelayms(50);
	return Data;
}


/**
  * @brief  Generates I2Cx communication NACK condition.
  * @param I2Cx: where x can be 1 or 2 to select the I2C peripheral.
  * @param NewState: new state of the I2C STOP condition generation.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval : None.
  */

void I2C2_EV_IRQHandler(void)
{
 switch (I2C_GetLastEvent(I2C2))
  {
    case I2C_EVENT_MASTER_MODE_SELECT:                 /* EV5 */
	      if(Direction == WRITE)
	      {
	        /* Master Transmitter ----------------------------------------------*/
	        /* Send slave Address for write */
	        I2C_Send7bitAddress(I2C2, I2C2_ACC_ADDRESS7, I2C_Direction_Transmitter);		
	      }
	      else
	      {
	        /* Master Receiver -------------------------------------------------*/
	        /* Send slave Address for read */
	        I2C_Send7bitAddress(I2C2, I2C2_ACC_ADDRESS7, I2C_Direction_Receiver);
	       
	      }
		 
	      break;
        
    /* Master Transmitter --------------------------------------------------*/
    /* Test on I2C1 EV6 and first EV8 and clear them */
    case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:   
	      /* Send the first data */	
	      I2C_SendData(I2C2, BYTE_ADDRESS); 
	      break;

    /* Test on I2C1 EV8 and clear it */
   case I2C_EVENT_MASTER_BYTE_TRANSMITTING:  /* Without BTF, EV8 */     
	         /* Transmit I2C2 data */	
		if (Direction == WRITE)
			{
				if (iINDEX%2)
					{
				         		I2C_SendData(I2C2, BYTE_DATA);
					}
				else
					{
				  		I2C_ITConfig(I2C2, I2C_IT_BUF, DISABLE);

					}
				iINDEX++;
			}
		else
			{
				I2C_ITConfig(I2C2, I2C_IT_BUF, DISABLE);
			}
	     break;

    case I2C_EVENT_MASTER_BYTE_TRANSMITTED: /* With BTF EV8-2 */
		
		I2C_ITConfig(I2C2, I2C_IT_EVT, DISABLE);
		/* I2C2 Re-START Condition */
//		I2C_GenerateSTART(I2C2, ENABLE);	
		/* Send I2C1 STOP Condition */
		I2C_GenerateSTOP(I2C2, ENABLE);

		iINDEX = 1;
	
		Flag_Finish = PASSED;
		
		break;

    /* Master Receiver -------------------------------------------------------*/
    case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED:
		/* Disable I2C1 acknowledgement */
		I2C_AcknowledgeConfig(I2C2, DISABLE);
		/* Send I2C1 STOP Condition */
		I2C_GenerateSTOP(I2C2, ENABLE);
             break;

   /* Test on I2C2 EV7 and clear it */
   case I2C_EVENT_MASTER_BYTE_RECEIVED:
   		DATA_RETURN = I2C_ReceiveData (I2C2);
             /* Disable I2C1 acknowledgement */
             //I2C_AcknowledgeConfig(I2C2, ENABLE);
	       Flag_Finish = PASSED;
             break;

    default:
	GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_RESET);		
      break;
  }  
  

}

void I2C2_ER_IRQHandler(void)
{
		/* Check on I2C2 AF flag and clear it */
		//GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_RESET);	
		
		if (I2C_GetITStatus(I2C2, I2C_IT_AF)) 
		{
			I2C_ClearITPendingBit(I2C2, I2C_IT_AF);
			//GPIO_WriteBit(GPIOC, GPIO_Pin_7, Bit_RESET);	
 		 }
}

/* Function config Even Interupt of Error Interrupt of I2C2 */
void NVIC_Configuration_I2C (void)
{
  
 
  NVIC_InitTypeDef  NVIC_InitStructure;
  /* Configure and enable I2C2 event interrupt -------------------------------*/  
  NVIC_InitStructure.NVIC_IRQChannel = I2C2_EV_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;// 1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Configure and enable I2C2 error interrupt -------------------------------*/  
  NVIC_InitStructure.NVIC_IRQChannel = I2C2_ER_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;// 2
  NVIC_Init(&NVIC_InitStructure);
}

/* Application Function detect value of Accelerometer when having dangerous situation */ 
void DetectACC(s8 ValueThreshold, u8 TimeDetect,float *maxX,float *minX, float *maxY, float *minY, float *maxZ, float *minZ)
{

	float x, y, z;
	u8 REG_STATUS;
	//u32  Count;
	//Count = (TimeDetect*1250);
	*maxX = 0;
	*minX  = 0;
	*maxY = 0;
	*minY  = 0;
	*maxZ = 0;
	*minZ  = 0;
	/* Initial:  External Interrupt of Accelerometer(Pin PC6)---------------------------------*/
	/*               Timer1                                                                                                   */
	/*               I2C2                                                                                                      */
	/* ----------------------------------------------------------------------------*/
	EXTI_ButtonInit();
	tmInit();  
	I2CInit();
	
	/*-----------------------------------------------------------------------------*/

	SetAccThresholdINT(ValueThreshold);	
	/*------------------------ wating for External Interrupt---------------------------- */
	while (Flag_ACC == 0)
		{
		//GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_SET);	
		}
	Flag_ACC = 0; 
	

	/*------------------------ clear flag interrupt in accelerometer ----------------------*/
//	IIC_ByteWrite(INTRST, 0x03);
	ClearIntLatch();
	
	/*---- Read value of X, Y, Z after detect level threshold during the specific timer-----------*/
	/*------------------------------- Start timer 1-----------------------------------*/
	Mul_time = TimeDetect*1000;
	tmStart();
	/* --------------Set Mode Measurement------------------------------- */
	SetAccRange(MODE_2G);
	
	/* -------------- Find the max, min value of X, Y, Z axis of accelerometer --- */
   	while(Mul_time)
   	{
		   REG_STATUS = IIC_ByteRead(STATUS); 
		   REG_STATUS |= 0xFE;
		   if (REG_STATUS == 0xFF)
		   {
				   /* Read Value */
				   x = ReadAccValueX ();
				        
				   y = ReadAccValueY ();
				        
				   z = ReadAccValueZ ();

				   if (x <= *minX )  *minX  = x;
				   if (x >= *maxY)  *maxX = x;

				   if ( y <= *minY)  *minY  = y;
				   if ( y >= *maxY) *maxY = y;

				   if ( z<= *minZ)   *minZ  = z;
				   if ( z>= *maxZ)  *maxZ = z;
		   
		   }
		   
   	}
/* -----------------------turn off timer1-------------------------------- */
tmStop();
/*------------------------Enable external interrupt of accelerometer -------- */
//IIC_ByteWrite(INTRST, 0x00);
//GPIO_WriteBit(GPIOC, GPIO_Pin_8, Bit_SET);	
}

/* Function set offset allow X axis */
void SetOffsetX ( void )
{
	s8 ValueMeasX = 0x01;;
	float temp=0;
	u8 iCount = 0;
	
	for(iCount =0; iCount < 200; iCount++)
	{
		/* Read Value Accelerometer allow X axis */
		ValueMeasX = IIC_ByteRead(XOUT);
             temp+=ValueMeasX;
		tmDelayms(4);	
	}
	temp = temp*3/400;
	ValueMeasX = (s8)temp;
	ValueMeasX =  -ValueMeasX;
	if (ValueMeasX < 0)
		{
			IIC_ByteWrite(XOFFH,0xFF);
			ValueMeasX = (ValueMeasX << 1) | 0x01;
			IIC_ByteWrite(XOFFL,ValueMeasX);
		}
	else
		{
			IIC_ByteWrite(XOFFH,0x00);
			ValueMeasX = (ValueMeasX << 1) | 0x01;
			IIC_ByteWrite(XOFFL,ValueMeasX);
		}
					
		
}

/* Function set offset allow Y axis */
void SetOffsetY ( void )
{
	s8 ValueMeasY ;
	float temp=0;
	u8 iCount = 0;
	for(iCount =0; iCount < 128; iCount++)
	{
		/* Read Value Accelerometer allow X axis */
		ValueMeasY = IIC_ByteRead(YOUT);
		temp+=ValueMeasY;
		tmDelayms(4);	
	}
	temp = temp/128-1;
	ValueMeasY = (s8)temp ;
	ValueMeasY =  -ValueMeasY;
	if (ValueMeasY < 0)
		{
			IIC_ByteWrite(YOFFH,0xFF);
			ValueMeasY = (ValueMeasY << 1) | 0x01;
			IIC_ByteWrite(YOFFL,ValueMeasY);
		}
	else
		{
			IIC_ByteWrite(YOFFH,0x00);
			ValueMeasY = (ValueMeasY << 1)|0x01;
			IIC_ByteWrite(YOFFL,ValueMeasY);
		}	
}

/* Function set offset allow Z axis */
void SetOffsetZ ( void )
{
	s8 ValueMeasZ  ;
	float temp=0;
	u8 iCount = 0;
	for(iCount =0; iCount < 128; iCount++)
	{
		/* Read Value Accelerometer allow X axis */
		ValueMeasZ = IIC_ByteRead(ZOUT);
             temp+=ValueMeasZ;
		tmDelayms(4);	
	}
	temp = temp/128 + 5;
	ValueMeasZ = (s8)(temp -64);
       ValueMeasZ =  -ValueMeasZ ;
	if (ValueMeasZ < 0)
	{
		IIC_ByteWrite(ZOFFH,0xFF);
		ValueMeasZ = (ValueMeasZ << 1) | 0x01;
		IIC_ByteWrite(ZOFFL,ValueMeasZ);
	}
	else
	{
		IIC_ByteWrite(ZOFFH,0x00);
		ValueMeasZ = (ValueMeasZ << 1) | 0x01;
		IIC_ByteWrite(YOFFL,ValueMeasZ);
	}					
}

/* convert varian from unsign integer 8bit to sign integer 8bit */
s8 ConvertTypeNumber(u8 Input)
{
	u8 Sign;
	Sign = (Input | 0x7F);
	if (Sign == 0xFF)
		{
			return (-128 + (Input&0x7F));
		}
	else
		{
			return Input;
		}
}

/* REconvert varian from unsign integer 8bit to sign integer 8bit */
s8 ReConvertTypeNumber(u8 Input)
{
	u8 Sign;
	Sign = (Input | 0x7F);
	if (Sign == 0xFF)
		{
			return (256 - (Input&0x7F));
		}
	else
		{
			return Input;
		}
}

/* round 2 digit after the commas */
float Round(float Input)
{
     float Temp;
      int c =0;
	
	c  = (int)(Input*100 + 0.5);
	
	Temp = (float)c/100;
	return (float)Temp;
}


void SelfTestZAxis(FunctionalState Switch)
{
      u8 Status_MCTL; 
      Status_MCTL = IIC_ByteRead(MCTL);
	if (Switch == ENABLE)
		{
    
   			 IIC_ByteWrite(MCTL, (Status_MCTL|0x10));
		}
	else
		{
			 IIC_ByteWrite(MCTL, (Status_MCTL & 0xEF));
		}
}

/* Calculate the Angle between Accelerometer with Z axis */
float CalculateAngleZACC ( float x, float y, float z)
{
	float CosA;
	float Angle;

	CosA = z/sqrt(x*x + y*y + z*z);

	Angle = (acos(CosA)/3.1416)*180;

	return Angle;
}

/* Calculate the Angle between Accelerometer with Y axis */
float CalculateAngleYACC ( float x, float y, float z)
{
	float CosA;
	float Angle;

	CosA = y/sqrt(x*x + y*y + z*z);

	Angle = (acos(CosA)/3.1416)*180;

	return Angle;
}

/* Calculate the Angle between Accelerometer with X axis */
float CalculateAngleXACC ( float x, float y, float z)
{
	float CosA;
	float Angle;

	CosA = x/sqrt(x*x + y*y + z*z);

	Angle = (acos(CosA)/3.1416)*180;

	return Angle;
}

u8 DetectStop(void)
{
	u8 i=0;
	float x,y,z;
	//SetAccRange(MODE_2G);
	i = 0;
      while(i++<100)
      	{
      	x = ReadAccValueX();
	y = ReadAccValueY();
	z = ReadAccValueZ();
	if ((x<=0.45)&(x>=-0.45)&(y<=0.45)&(y>=-0.45))
		{	
		}
	else
		{
			return 0;
		}
	tmDelayms(10);
      	}
return 1;
}