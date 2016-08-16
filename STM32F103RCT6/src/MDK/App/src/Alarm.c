#include "Alarm.h"

extern volatile u8 Flag_ACC;
extern uint32_t AlternateLanguage;
u8 Flag_AlarmAuto = 0,Flag_AlarmOn = 0;
void  Alarm_On( void )
{
	Flag_AlarmAuto = 0;
	Flag_AlarmOn = 1;
	SetAccThresholdINT(0x2F);
	SendMassageAlarm(turned_on_idx);
      Flag_ACC = 0;
      while((!Flag_ACC)|0)  // TODO: ignition --> ignore alarm	
      	{
		if (Flag_AlarmAuto == 1) // Turn on Alarm Auto
		{
			break;
		}
      	}
	if(Flag_AlarmAuto != 1)
	{
		ClearIntLatch();			
		if (1)// TODO: security shiled --> GPS  
		{
			SendMassageAlarm_Detect();
		}
		Flag_ACC = 0;
		
		//NVIC_Configuration_ACC_INT_DIS();
		SetStandByACC();
	}
}

void Alarm_Off (void)
{
	//NVIC_Configuration_ACC_INT_DIS();
	SetStandByACC();
	SendMassageAlarm(turned_off_idx);
}

void Alarm_Auto(void)
{
u8 Flag_Timing = 0;
Flag_AlarmOn = 0;
/* Announce the rountine using Alarm_Auto function */
Flag_AlarmAuto = 1;

Flag_ACC = 0;
SetAccRange(MODE_2G);
SetOffsetZ();
SetOffsetY();
SetOffsetX();
 while(Flag_AlarmOn==0)
 {
	/* Detect Stop motioning */
      if ((DetectStop())|(Flag_Timing == 1))
      	{
      		if (Flag_Timing == 0)
      		{
      			SerialPutString("\r\nStart Timer 10s\r\n", 0);
		      /* Delay 15 minute */
			tmDelayms(10000);//10s
			Flag_ACC =0;
			SetAccThresholdINT(0x2F);	
			ClearIntLatch();	
			Flag_Timing = 1;
			SerialPutString("\r\nStop Timer 10s\r\n", 0);
      		}
		if (Flag_ACC == 1) // If have motion
		{
			ClearIntLatch();		
			SendMassageAlarm_Detect();
			Flag_ACC = 0;
			Flag_Timing = 0;
		}	
      	}	
 }		
		
}


void NVIC_Configuration_ACC_INT_DIS(void)
{
  /* Enable the EXTI Interrupt */
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         :                
*******************************************************************************/
void SendMassageAlarm(sentence_idx_t Phase)
{
	uint8_t str[200],def,abc;
	/*==============Respon note to user====================*/
	sprintf(str,"%s","Tramigo: ");
	def = strlen(str);
	GetCommand(str + def, AlternateLanguage, Alarm_index);
	def =strlen(str);
	*(str + def)= ',';
	*(str + def+1)= ' ';
	GetSentence(str + def + 2, AlternateLanguage, reporting_is_idx);
	def = strlen(str);
	*(str +def)= ' ';
	GetSentence(str +def +1, AlternateLanguage, Phase);
	def =strlen(str);
	*(str + def)= ',';
	*(str + def+1)= ' ';
	GetSentence(str+def+2, AlternateLanguage, Jan_idx);	

	SerialPutString("\r\n===============Message Alarm=================\r\n",0);
	SerialPutString(str,0);
	//SerialPutString("\r\n",0);
	//SerialPutString(String2,0);
	SerialPutString("\r\n===========================================\r\n",0);
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : 
* Return         :                
*******************************************************************************/
void SendMassageAlarm_Detect(void)
{
	uint8_t str[200],def;
	/*==============Respon note to user====================*/
	sprintf(str,"%s","Tramigo: ");
	def = strlen(str);
	GetSentence(str + def, AlternateLanguage, motion_alarm_idx);
	def = strlen(str);
	*(str + def)= ' ';
	GetSentence(str + def + 1, AlternateLanguage, detected_idx);
	def = strlen(str);
	*(str + def)= ',';
	*(str + def +1)= ' ';
	GetSentence(str + def + 2, AlternateLanguage, parked_idx);
	def = strlen(str);
	*(str + def)= ',';
	*(str + def+1)= ' ';
	GetSentence(str + def + 2, AlternateLanguage, place_idx);

	SerialPutString("\r\n===============Message Alarm Detected=================\r\n",0);
	SerialPutString(str,0);
	//SerialPutString("\r\n",0);
	//SerialPutString(String2,0);
	SerialPutString("\r\n==================================================\r\n",0);
}






















