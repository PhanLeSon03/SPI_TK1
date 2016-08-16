#include "Trip.h"
//float Velocity = 0,ACC,AX=0,AY=0,AZ=0,Velocity_X=0,Velocity_Y=0,Velocity_Z=0;
//float AX_OLD =0, AY_OLD=0, AZ_OLD=0, Velocity_X_OLD=0, Velocity_Y_OLD=0, Velocity_Z_OLD=0;
//float DistanceX, DistanceY, DistanceZ,Distance;
float Distance;
u32 ts= 0;
u8 Flag_TripStart = 0;
u8 Flag_TripStop = 1;
extern bool GPSEnd;
extern bool GPSFix; 
extern GPSPositionStruture GPSPosition, GPSPositionOld;
__O u8 Flag_Idle = 0,Flag_Detect = 0,Flag_Motion = 0;
__O u16 Count_ts = 0;
/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/

float GetSpeed(void)
{
#if 0
	/*=== Calculation velocity (using GPS or ACC) ==== */
	Velocity = sqrt(Velocity_X*Velocity_X + Velocity_Y*Velocity_Y + Velocity_Z*Velocity_Z); // Calculation follow Acceleromater 	
	/*=== Return Value ======================== */

	return Velocity;
#endif
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/

float GetDistance(void)
{
#if 0
	float Distance;
	/*==== using GPS ==== */

	/*====using Accelerometer ==============================================*/
	Distance = sqrt(DistanceX*DistanceX + DistanceY*DistanceY + DistanceZ*DistanceZ);
	return Distance;
#endif
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/
void DetectTripStart(float Dis, float Velocity)
{
	s8 Flag_Ignition;
	float Speed,Distance;
	u8 temp[100];
	
	/*====== Ignition on =========*/
	Flag_Ignition = GetIgnitionStatus();
		
	/*====== Get Speed =========*/
	Speed = Velocity*3600/1000;
	sprintf(temp,"speed = %0.3f",Speed);
	SerialPutString(temp, 0);
	SerialPutString("\r\n",0);
	
	/*====== Get Distance =======*/
	Distance = Dis;
	sprintf(temp,"Distance = %0.3f",Distance);
	SerialPutString(temp, 0);
	SerialPutString("\r\n",0);
	
	if((Flag_Ignition == 1)&(Speed > 6)&(Distance > 300)) // Condition of Trip start
	{
		Flag_TripStart = 1;
		Flag_TripStop = 0;
		ReportTripStart();
		ts = 0;
		
	}
	else if ((Flag_Ignition == 0)&(Speed > 3)&(Distance > 300))
	{
		Flag_TripStart = 1;
		Flag_TripStop = 0;
		ReportTripStart();
		ts = 0;
		
	}
	
}	

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/
void DetectTripStop(void)
{
	s8 Flag_Ignition;
	float Speed;
      /*===== Get flag of ignition =======*/
	Flag_Ignition = GetIgnitionStatus();
	/*===== Check idle 15 min ========*/
	if ((Flag_Idle == 1)||(Flag_Ignition == 0))
	{
		/*===== Report ================*/	  
		Flag_TripStart = 0;
		Flag_TripStop = 1;
		Flag_Idle = 0;
		Distance = 0;
		ReportTripStop();
	}
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/
void ReportTripStart(void)
{
	SerialPutString("Trip Start\r\n",0);
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/
void ReportTripStop(void)
{
	SerialPutString("Trip Stop\r\n",0);
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/
int8_t DetectIdle(void)
{
	return Flag_Idle;
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/
int8_t GetIgnitionStatus(void)
{
	return 1;
}

void initTimer3 (void)
{
	uint16_t TimerPeriod;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	/* Compute the value to be set in ARR regiter to generate signal frequency at  */
	TimerPeriod =  72 - 1;

	/* TIM1 configuration */
	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;          
	TIM_TimeBaseStructure.TIM_Prescaler = 999;       
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0000;

	/* Enable the TIM1 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
	//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	/* Clear TIM1 update pending flag */
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	
}

void TIM3_IRQHandler(void)
{
	/* Clear TIM1 COM pending bit */
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	Count_ts++;
	
	//if (Count_ts%100 ==0)
	//{
	//	Flag_Filter = 1; //0.1 s is set this flag
	//}
	
	if (Count_ts == 1000)
	{
		//GPIO_WriteBit(GPIOC, GPIO_Pin_7, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_7)));
		ts++;
		Count_ts = 0;
		
		if (Flag_Motion == 1)
		{
			ts = 0;
		}
		if ((ts == 60*1)&&(Flag_TripStart == 1)&&(!Flag_Motion))
		{
			Flag_Idle = 1; // Detect idle state
			ts = 0;
		}
		
		Flag_Detect = 1;
	}
	
	
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/
void Trip_On(void)
{
	float Speed;
	/* Enable TIM1  Update interrupt */
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
	 /* TIM1 counter enable */
	 TIM_Cmd(TIM3, ENABLE);
	 
	 //GPSEnd = FALSE;
	 //while(GPSEnd != TRUE);
	 GPS_Data_Process();
	 GPS_SavePosition();
	 while(1)
	 {
		
		if (Flag_Detect == 1) // T =1s --> detect trip
		{
			//Update new value of GPS and Accelerometer
			Speed = NewSpeed();
			
			// whether or  not have motion
			if ((Speed <= 3)||(Distance <=300))
			{
				Flag_Motion = 0;
			}
			else
			{
				Flag_Motion = 1;
			}

			// if car in Stop state
			if (Flag_TripStop == 1)
			{
				DetectTripStart(Distance,Speed);
			}

			// if car in Start state
			if(Flag_TripStart == 1)
			{
				DetectTripStop();
			}
			
			Flag_Detect = 0;
		}
	}
}


/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/
void Trip_Off(void)
{
	/*DISABLE TIM1 Update interrupt*/
	  TIM_ITConfig(TIM1, TIM_IT_Update, DISABLE);
	  
	  /* TIM1 counter DISABLE */
	  TIM_Cmd(TIM1, DISABLE);	
}
/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/
void Trip_Now(void)
{
	if (Flag_TripStart == 1)
	{
		ReportTripStart();
	}
	if (Flag_TripStop == 1)
	{
		ReportTripStop();
	}
}
/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/
u8 UpdateVelocity(void)
{
#if 0
	float A_XYZ;
	u8 temp[200];
	     
	sprintf(temp,"AX = %0.3f, AY = %0.3f, AZ = %0.3f",AX,AY,AZ);
	SerialPutString(temp, 0);
	SerialPutString("\r\n",0);

	Velocity_X += (AX+AX_OLD)/2;
	Velocity_Y += (AY+AY_OLD)/2;
	Velocity_Z +=(AZ+AZ_OLD)/2;

	sprintf(temp,"VX = %0.3f, VY = %0.3f, VZ = %0.3f",Velocity_X,Velocity_Y,Velocity_Z);
	SerialPutString(temp, 0);
	SerialPutString("\r\n",0);
	AX_OLD = AX;
	AY_OLD = AY;
	AZ_OLD = AZ;
	
	DistanceX += (Velocity_X + Velocity_X_OLD)/2;
	DistanceY += (Velocity_Y + Velocity_Y_OLD)/2;
	DistanceZ += (Velocity_Z + Velocity_Z_OLD)/2;
	sprintf(temp,"X = %0.3f, Y = %0.3f, Z = %0.3f",DistanceX,DistanceY,DistanceZ);
	SerialPutString(temp, 0);
	SerialPutString("\r\n\n",0);
	Velocity_X_OLD = Velocity_X;
	Velocity_Y_OLD = Velocity_Y;
	Velocity_Z_OLD = Velocity_Z;

	A_XYZ = sqrt(AX*AX + AY*AY + AZ*AZ);

	if (A_XYZ>=4)
	{
		return 1; // have motion
	}
	else
	{
		return 0; // don't have motion
	}
#endif
}

float *Filter(float Acc, float Pos)
{
	float MeasNoise = 10*0.3048; // Position measurement noise (m) --> get parameter from measurement
	float AccNoise = 0.2*0.3048;  // Accelerations noise (m/s2)          --> get parameter from measuremnet
	// element of matrix A
	float A_00 = 1;
	float A_01 = T; // T =0.1s
	float A_10 = 0;
	float A_11 = 1;

	float B_00 = T*T/2;
	float B_10 = T;

	float C_00 = 1;
	float C_01 = 0;

	static float X[2] ={0,0};
	
	float Sz;                                                      // covariance of measurement noise
	float Sw_00, Sw_01, Sw_10, Sw_11;     // covarian of process noise
	static float P_00, P_01, P_10, P_11;              // covarian of state
	float Inn,y,s;

	float K_00; // gain of predict
	float K_10; // gain of predict

       // Temp variable
	float AP_00,AP_01,AP_10,AP_11;
	float APAT_00, APAT_01, APAT_10, APAT_11;
	float KCPAT_00,KCPAT_01,KCPAT_10,KCPAT_11;

	// measurement noise
	Sz = MeasNoise*MeasNoise;
	// process noise
	Sw_00 = AccNoise*AccNoise*T*T*T*T/4;
	Sw_01 = AccNoise*AccNoise*T*T*T/2;
	Sw_10 = AccNoise*AccNoise*T*T*T/2;
	Sw_11 = AccNoise*AccNoise*T*T;
	P_00 = Sw_00;
	P_01 = Sw_01;
	P_10 = Sw_10;
	P_11 = Sw_11;

	// Position get from GPS
       y = Pos;
	
	// extrapolate the mosts recent state estimate to the present time.
	// x = a*x + b*u 
	X[0] = A_00*X[0] + A_01*X[1] + B_00*Acc;
	X[1] = A_10*X[0] + A_11*X[1] + B_10*Acc;
	// Form the innovation vector
	Inn = y - X[0];
	
	// compute the covariance of the Inovation
	// s = c*P*c' + Sz;
	 s = P_00 + Sz;
	
	// Form the kalman gain matix
	// K = a*P*c'*inv(s);
	K_00 = (A_00*P_00 + A_01*P_10)/s;
	K_10 = (A_10*P_00 + A_11*P_10)/s;
	
	// Update state estimate
	// xhat = xhat + k*Inn;
       X[0] = X[0] + K_00*Inn; 
       X[1] = X[1] + K_10*Inn;
	// Compute the covariance of the estimate error
	// P = a*P*a' - a*P*c'*inv(s)*c*P*a' + Sw;
	AP_00 = P_00 + A_01*P_10;;
	AP_01 = P_01 + A_01*P_11;
	AP_10 = P_10;
	AP_11 = P_11;
	APAT_00 = AP_00 + (AP_01 * A_01);
	APAT_01 = AP_01;
	APAT_10 = AP_10 + (AP_11 * A_01);
	APAT_11 = AP_11;
	KCPAT_00 = (K_00 * P_00) + (K_00 * P_01) * A_01;
	KCPAT_01 = (K_00 * P_01);
	KCPAT_10 = (K_10 * P_00) + (K_10 * P_01) * A_01;
	KCPAT_11 = (K_10 * P_01);
	P_00 = APAT_00 - KCPAT_00 + Sw_00;
	P_01 = APAT_01 - KCPAT_01 + Sw_01;
	P_10 = APAT_10 - KCPAT_10 + Sw_10;
	P_11 = APAT_11 - KCPAT_11 + Sw_11;
	
	return X;	
}

float NewSpeed(void)
{
	float Dis,AX,AY,A,Speed;
	float *X;
	int8_t temp[100];

	GPS_Data_Process();
      Dis = GPS_Distance(GPSPositionOld, GPSPosition);
	sprintf(temp,"Dis_GPS = %0.3f",Dis);
	SerialPutString(temp, 0);
	SerialPutString("\r\n",0);
	GPS_SavePosition();
      AX = ReadAccValueX()*G;
      AY = ReadAccValueY()*G;
      A = sqrt(AX*AX + AY*AY) ;
	 sprintf(temp,"ACC = %0.3f",A);
	SerialPutString(temp, 0);
	SerialPutString("\r\n",0);
      X= Filter(A, Dis);
      Distance += *X;
      Speed = *(++X);
      return Speed;
}



