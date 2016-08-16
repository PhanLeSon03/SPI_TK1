#include "Alarm.h"

extern volatile u8 Flag_ACC;
extern uint32_t AlternateLanguage;
u8 Flag_AlarmAuto = 0,Flag_AlarmOn = 0;
enum {alarm_init, alarm_wait, alarm_end, alarm_exit,check_new_command};

u8 alarm_on_status = alarm_init;
u8 Alarm_On( void )
{
	switch(alarm_on_status){
		case alarm_init:
			Flag_AlarmAuto = 0;
			Flag_AlarmOn = 1;
			SetAccThresholdINT(0x2F);
			SendMassageAlarm(turned_on_idx);
		      Flag_ACC = 0;
			alarm_on_status = alarm_wait;
			break;
		case alarm_wait:
		      if((!Flag_ACC)|0)  // TODO: ignition --> ignore alarm	
		      	{
				if (Flag_AlarmAuto == 1) // Turn on Alarm Auto
				{
					alarm_on_status = alarm_exit;
				}
				else
				{
					alarm_on_status = alarm_wait;
				}
		      	}
			else
			{
				alarm_on_status = alarm_end;
			}
			break;
		case alarm_end:
			
			ClearIntLatch();			
			if (1)// TODO: security shiled --> GPS  
			{
				SendMassageAlarm_Detect();
			}
			Flag_ACC = 0;
			SetStandByACC();
			alarm_on_status = alarm_exit;
			break;
		case check_new_command:
			
			alarm_on_status = alarm_wait;
			break;
		case alarm_exit:
		default:
			break;
	}
	return alarm_on_status;
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
SetAccThresholdINT(0x2F);
 while(Flag_AlarmOn==0)
 {
	/* Detect Stop motioning */
      if ((Flag_ACC == 0)|(Flag_Timing == 1))
      	{
      		if (Flag_Timing == 0)
      		{
      			SerialPutString("\r\nStart Timer 10s\r\n", 0);
		      /* Delay 15 minute */
			tmDelayms(10000);//10s
			Flag_ACC =0;
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























